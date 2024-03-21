#include "Raytracer/Renderer.h"

#include "Compositor/PostProcessing/PostProcessing.h"
#include "Raytracer/Scene.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/StaticMesh.h"
#include "Raytracer/Traceable/TriangleTraceable.h"
#include "Utils/Random.h"
#include "Utils/TextureManager.h"

#ifdef SUPPORT_STD_EXECUTION
#include <execution>
#endif

namespace AstralRaytracer
{

template <ArithmeticType T, uint32 ComponentCount> Renderer<T, ComponentCount>::Renderer()
{
}

template <ArithmeticType T, uint32 ComponentCount> Renderer<T, ComponentCount>::~Renderer()
{
    renderEnd();
}

template <ArithmeticType T, uint32 ComponentCount> void Renderer<T, ComponentCount>::initialize()
{
    const Resolution initialRes(16, 16);

    m_texData.resize(initialRes);
    m_textureId = TextureManager::loadTextureFromTextureData<T, ComponentCount>(m_texData, false);

    resize(initialRes);
}

template <ArithmeticType T, uint32 ComponentCount>
void Renderer<T, ComponentCount>::renderStart(const Scene &scene, const Camera &cam)
{
    if (getState() != RendererState::NOT_STARTED)
    {
        return;
    }

    setState(RendererState::STARTED);
    renderEnd();

    onResize(cam.getResolution());

    m_renderingThread = std::thread([this, &scene, &cam]() { this->render(scene, cam); });
}

template <ArithmeticType T, uint32 ComponentCount> void Renderer<T, ComponentCount>::renderEnd()
{
    if (m_renderingThread.joinable())
    {
        m_renderingThread.join();
    }
}

template <ArithmeticType T, uint32 ComponentCount>
void Renderer<T, ComponentCount>::render(const Scene &scene, const Camera &cam)
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

    const glm::vec3 camHorizontalDir = glm::normalize(glm::cross(cam.getUp(), cam.getForwardDirection()));

    runParallel(m_rayIterator.begin(), m_rayIterator.end(),
                [this, &inverseProjection, &inverseView, &camHorizontalDir, &oneOverBounceCount, &xAxisPixelCount,
                 &scene, &cam, &oneOverFrameIndex, &oneOverXAxisPixelCount, &oneOverImageHeight](uint32 index) {
                    uint32 seedVal = index * m_frameIndex;

                    const size_t pixelAccessIndex = static_cast<size_t>(index) * m_texData.getComponentCount();

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
                        glm::vec3 apertureOffset = camHorizontalDir * apertureSample.x + cam.getUp() * apertureSample.y;

                        rayOrigin += apertureOffset;
                    }

                    glm::vec3 rayDir = getRayDirectionFromNormalizedCoord(coOrd, inverseProjection, inverseView);

                    // Adjust ray direction
                    glm::vec3 focalPoint = cam.getPosition() + cam.getFocusDistance() * rayDir;
                    rayDir = glm::normalize(focalPoint - rayOrigin);

                    const glm::vec4 outColor = perPixel(seedVal, scene, rayOrigin, rayDir) * oneOverBounceCount;

                    T &redChannel = m_accumulatedColorData[pixelAccessIndex];
                    T &greenChannel = m_accumulatedColorData[pixelAccessIndex + 1];
                    T &blueChannel = m_accumulatedColorData[pixelAccessIndex + 2];
                    T &alphaChannel = m_accumulatedColorData[pixelAccessIndex + 3];

                    redChannel += outColor.r;
                    greenChannel += outColor.g;
                    blueChannel += outColor.b;
                    alphaChannel += outColor.a;

                    const glm::vec4 finalColorVec(redChannel, greenChannel, blueChannel, 1.0f);
                    glm::vec4 finalColorData = finalColorVec * oneOverFrameIndex;

                    m_texData.setTexelColorAtPixelIndex(pixelAccessIndex, finalColorData);
                });

    setState(RendererState::DONE);
}

template <ArithmeticType T, uint32 ComponentCount>
Renderer<T, ComponentCount>::RendererState Renderer<T, ComponentCount>::getState() const
{
    return m_state.load();
}

template <ArithmeticType T, uint32 ComponentCount> void Renderer<T, ComponentCount>::setState(RendererState renderState)
{
    m_state.store(renderState);
}

template <ArithmeticType T, uint32 ComponentCount> bool Renderer<T, ComponentCount>::onRenderComplete()
{
    if (m_state != RendererState::DONE)
    {
        return false;
    }

    m_state = RendererState::NOT_STARTED;

    TextureManager::updateTexture(m_texData, m_textureId);
    ++m_frameIndex;

    return true;
}

template <ArithmeticType T, uint32 ComponentCount>
glm::vec3 Renderer<T, ComponentCount>::getRayDirectionFromNormalizedCoord(const glm::vec2 &coord,
                                                                          const glm::mat4 &inverseProjection,
                                                                          const glm::mat4 &inverseView) const
{
    // Pre-calculate the multiplication factor for the coord adjustment
    const glm::vec2 adjustedCoord = (coord * 2.0f) - 1.0f;

    // Perform the matrix multiplication and normalization in a single step
    const glm::vec4 target = inverseProjection * glm::vec4(adjustedCoord.x, adjustedCoord.y, 1.0f, 1.0f);
    const glm::vec3 targetNormalized = glm::normalize(glm::vec3(target) / target.w);

    // Perform the final matrix multiplication
    return glm::vec3(inverseView * glm::vec4(targetNormalized, 0.0f));
}

template <ArithmeticType T, uint32 ComponentCount>
void Renderer<T, ComponentCount>::resize(const Resolution &resolution)
{
    m_texData.resize(resolution);

    const size_t newSizePixelCount = static_cast<size_t>(resolution.x) * resolution.y;
    m_accumulatedColorData.resize(newSizePixelCount * m_texData.getComponentCount());
    resetFrameIndex();

    m_rayIterator.resize(newSizePixelCount);
    std::iota(m_rayIterator.begin(), m_rayIterator.end(), 0);

    TextureManager::resizeTexture(m_texData, m_textureId);
}

template <ArithmeticType T, uint32 ComponentCount>
bool Renderer<T, ComponentCount>::onResize(const Resolution &resolution)
{
    if (m_texData.getResolution() == resolution)
    {
        return false;
    }

    resize(resolution);

    return true;
}

template <ArithmeticType T, uint32 ComponentCount> void Renderer<T, ComponentCount>::resetFrameIndex()
{
    m_frameIndex = 1;

    const Resolution &texRes = m_texData.getResolution();

    const size_t numBytes = m_texData.getComponentCount() * sizeof(float32);
    const size_t bufferSize = numBytes * texRes.x * texRes.y;

    std::memset(m_accumulatedColorData.data(), 0, bufferSize);
}

template <ArithmeticType T, uint32 ComponentCount>
glm::vec4 Renderer<T, ComponentCount>::perPixel(uint32 &seedVal, const Scene &scene, CoOrd3DF &rayOrigin,
                                                Direction3D &rayDir) const
{
    glm::vec3 light(0.0f);
    glm::vec3 contribution(1.0f);

    constexpr float32 kEpsilon = std::numeric_limits<float32>::epsilon();

    const TextureDataRGBF &skyTexData = scene.m_textures[scene.m_textures.size() - 1];

    HitInfo initialClosestHitInfo;
    findClosestHit(initialClosestHitInfo, scene, rayOrigin, rayDir);

    const float32 depth = initialClosestHitInfo.hitDistance;

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

    return glm::vec4(light.r, light.g, light.b, depth);
}

template <ArithmeticType T, uint32 ComponentCount>
void Renderer<T, ComponentCount>::findClosestHit(HitInfo &closestHitInfo, const Scene &scene, const CoOrd3DF &rayOrigin,
                                                 const Direction3D &rayDir) const
{
    // Initialize closest hit distance to a large value
    closestHitInfo.hitDistance = std::numeric_limits<float32>::max();

    for (uint32 traceableIndex = 0; traceableIndex < scene.m_sceneTraceables.size(); ++traceableIndex)
    {
        HitInfo hitInfo;
        if (scene.m_sceneTraceables[traceableIndex]->trace({rayOrigin, rayDir}, hitInfo) &&
            hitInfo.hitDistance < closestHitInfo.hitDistance)
        {
            hitInfo.objectIndex = traceableIndex;
            closestHitInfo = hitInfo;
        }
    }
}

template class Renderer<uint8, 3>;
template class Renderer<uint8, 4>;
template class Renderer<float32, 3>;
template class Renderer<float32, 4>;

} // namespace AstralRaytracer