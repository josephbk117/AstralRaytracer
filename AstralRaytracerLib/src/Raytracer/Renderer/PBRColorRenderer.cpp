#include "Raytracer/Renderer/PBRColorRenderer.h"
#include "Utils/Random.h"

namespace AstralRaytracer
{

PBRColorRenderer::PBRColorRenderer()
{
}

PBRColorRenderer::~PBRColorRenderer()
{
}

void PBRColorRenderer::render(const Scene &scene, const Camera &cam)
{
    if (getState() != RendererState::STARTED)
    {
        return;
    }

    const Resolution &texRes = m_texData.getResolution();

    const uint32 xAxisPixelCount = texRes.x * m_texData.getComponentCount();
    const float32 imageHeight = texRes.y;
    const glm::mat4 &inverseView = cam.getInverseView();
    const glm::mat4 &inverseProjection = cam.getInverseProjection();

    const float32 oneOverBounceCount = 1.0f / m_BounceCount;
    const float32 oneOverFrameIndex = 1.0f / m_frameIndex;
    const float32 oneOverXAxisPixelCount = 1.0f / xAxisPixelCount;
    const float32 oneOverImageHeight = 1.0f / imageHeight;

    runParallel(m_rayIterator.begin(), m_rayIterator.end(),
                [this, &inverseProjection, &inverseView, &oneOverBounceCount, &xAxisPixelCount, &scene, &cam,
                 &oneOverFrameIndex, &oneOverXAxisPixelCount, &oneOverImageHeight](uint32 index) {
                    uint32 seedVal = index * m_frameIndex;

                    const uint32 texComponentData = m_texData.getComponentCount();
                    const size_t pixelAccessIndex = static_cast<size_t>(index) * texComponentData;

                    const float32 randFloat1 = Random::randomFloatSymmetric(seedVal);
                    const float32 randFloat2 = Random::randomFloatSymmetric(seedVal);

                    const float32 xIndex = (pixelAccessIndex % xAxisPixelCount) + randFloat1;
                    const float32 yIndex = (pixelAccessIndex * oneOverXAxisPixelCount) + randFloat2;

                    const glm::vec2 coOrd{xIndex * oneOverXAxisPixelCount, yIndex * oneOverImageHeight};

                    glm::vec3 rayOrigin = cam.getPosition();

                    // Depth of Field: Adjust the ray origin based on aperture size
                    if (cam.getApertureDiameter() > 0.0f)
                    {
                        glm::vec3 apertureSample = Random::unitDisk(seedVal) * cam.getApertureDiameter();
                        glm::vec3 apertureOffset =
                            cam.getHorizontalDir() * apertureSample.x + cam.getUp() * apertureSample.y;

                        rayOrigin += apertureOffset;
                    }

                    glm::vec3 rayDir = getRayDirectionFromNormalizedCoord(coOrd, inverseProjection, inverseView);

                    // Adjust ray direction
                    glm::vec3 focalPoint = cam.getPosition() + cam.getFocusDistance() * rayDir;
                    rayDir = glm::normalize(focalPoint - rayOrigin);

                    const glm::vec3 outColor = perPixel(seedVal, scene, rayOrigin, rayDir) * oneOverBounceCount;

                    glm::vec3 finalColorVec{};
                    for (uint32 channelIndex = 0; channelIndex < texComponentData; ++channelIndex)
                    {
                        float &channelVal = m_accumulatedColorData[pixelAccessIndex + channelIndex];
                        channelVal += outColor[channelIndex];
                        finalColorVec[channelIndex] = channelVal * oneOverFrameIndex;
                    }

                    m_texData.setTexelColorAtPixelIndex(pixelAccessIndex, finalColorVec);
                });

    setState(RendererState::DONE);
}

glm::vec3 PBRColorRenderer::perPixel(uint32 &seedVal, const Scene &scene, CoOrd3DF &rayOrigin,
                                     Direction3D &rayDir) const
{
    glm::vec3 light(0.0f);
    glm::vec3 contribution(1.0f);

    constexpr float32 kEpsilon = std::numeric_limits<float32>::epsilon();

    const TextureDataRGBF &skyTexData = scene.m_textures[scene.m_textures.size() - 1];

    for (uint32 bounceIndex = 0; bounceIndex < m_BounceCount; ++bounceIndex)
    {
        seedVal += bounceIndex;
        HitInfo closestHitInfo;

        findClosestHit(closestHitInfo, scene, rayOrigin, rayDir);

        if (closestHitInfo.isValid())
        {
            const Material &mat = scene.m_materials[closestHitInfo.materialIndex];

            const TextureDataRGBF &texData = scene.m_textures[mat.texture];

            glm::vec3 colorData =
                texData.getTexelColor(closestHitInfo.worldSpacePosition.x * 0.1f,
                                      closestHitInfo.worldSpacePosition.z * 0.1f, SamplingType::MIRRORED_REPEAT);

            const glm::vec4 &albedo = mat.albedo.getColour_32_bit();

            contribution *= glm::vec3(albedo.r, albedo.g, albedo.b) * colorData;
            light += mat.getEmission() * colorData;

            // Avoid self-intersection by moving the ray origin slightly
            rayOrigin = closestHitInfo.worldSpacePosition + closestHitInfo.worldSpaceNormal * kEpsilon;

            // Compute the new ray direction (reflection)
            const glm::vec3 roughnessOffset =
                mat.roughness * Random::unitHemiSphere(seedVal, closestHitInfo.worldSpaceNormal);
            rayDir = glm::reflect(rayDir, closestHitInfo.worldSpaceNormal + roughnessOffset);
        }
        else // No hit, Render sky
        {
            constexpr float32 PI = std::numbers::pi;
            const float32 theta = glm::acos(-rayDir.y);        // Polar angle (latitude)
            const float32 phi = glm::atan(rayDir.z, rayDir.x); // Azimuthal angle (longitude)

            const float32 u = (phi + PI) / (2.0f * PI); // Map longitude to U
            const float32 v = 1.0f - (theta / PI);      // Map latitude to V

            // No intersection; add background color

            const glm::vec3 &skyColor = skyTexData.getTexelColor(u, v, SamplingType::CLAMP_TO_EDGE);
            light += skyColor * contribution;
        }
    }

    return light;
}
} // namespace AstralRaytracer