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
                [this, &inverseProjection, &inverseView, &oneOverBounceCount, &xAxisPixelCount, &scene, &cam,
                 &oneOverFrameIndex, &oneOverXAxisPixelCount, &oneOverImageHeight](uint32 index) {
                    uint32 seedVal = index * m_frameIndex;

                    const size_t pixelAccessIndex = static_cast<size_t>(index);

                    const float32 randFloat1 = Random::randomFloatSymmetric(seedVal);
                    const float32 randFloat2 = Random::randomFloatSymmetric(seedVal);

                    const float32 xIndex = (pixelAccessIndex % xAxisPixelCount) + randFloat1;
                    const float32 yIndex = (pixelAccessIndex * oneOverXAxisPixelCount) + randFloat2;

                    const CoOrd2DF coOrd{xIndex * oneOverXAxisPixelCount, yIndex * oneOverImageHeight};

                    CoOrd3DF rayOrigin = cam.getPosition();

                    Direction3D rayDir = getRayDirectionFromNormalizedCoord(coOrd, inverseProjection, inverseView);

                    const glm::vec1 outColor = perPixel(seedVal, scene, rayOrigin, rayDir) * oneOverBounceCount;

                    float &channelVal = m_accumulatedColorData[pixelAccessIndex];
                    channelVal += outColor[0];
                    const float32 finalColor = channelVal * oneOverFrameIndex;

                    m_texData.setTexelColorAtPixelIndex(pixelAccessIndex, glm::vec1(finalColor));
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