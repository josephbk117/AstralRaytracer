#include "Raytracer/Renderer/DepthRenderer.h"
#include "Utils/Random.h"

namespace AstralRaytracer
{
DepthRenderer::DepthRenderer()
{
}

DepthRenderer::~DepthRenderer()
{
}

void DepthRenderer::render(const Scene &scene, const Camera &cam)
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
                [this, &inverseProjection, &inverseView, &oneOverBounceCount, &xAxisPixelCount,
                 &scene, &cam, &oneOverFrameIndex, &oneOverXAxisPixelCount, &oneOverImageHeight](uint32 index) {
                    uint32 seedVal = index * m_frameIndex;

                    const uint32 texComponentData = m_texData.getComponentCount();
                    const size_t pixelAccessIndex = static_cast<size_t>(index) * texComponentData;

                    const float32 randFloat1 = Random::randomFloatSymmetric(seedVal);
                    const float32 randFloat2 = Random::randomFloatSymmetric(seedVal);

                    const float32 xIndex = (pixelAccessIndex % xAxisPixelCount) + randFloat1;
                    const float32 yIndex = (pixelAccessIndex * oneOverXAxisPixelCount) + randFloat2;

                    const glm::vec2 coOrd{xIndex * oneOverXAxisPixelCount, yIndex * oneOverImageHeight};

                    glm::vec3 rayOrigin = cam.getPosition();          

                    glm::vec3 rayDir = getRayDirectionFromNormalizedCoord(coOrd, inverseProjection, inverseView);

                    glm::vec3 focalPoint = cam.getPosition() + cam.getFocusDistance() * rayDir;
                    rayDir = glm::normalize(focalPoint - rayOrigin);

                    const glm::vec1 outColor = perPixel(seedVal, scene, rayOrigin, rayDir) * oneOverBounceCount;

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

glm::vec1 DepthRenderer::perPixel(uint32 &seedVal, const Scene &scene, CoOrd3DF &rayOrigin, Direction3D &rayDir) const
{
    HitInfo closestHitInfo;
    findClosestHit(closestHitInfo, scene, rayOrigin, rayDir);

    return glm::vec1(closestHitInfo.hitDistance);
}
} // namespace AstralRaytracer