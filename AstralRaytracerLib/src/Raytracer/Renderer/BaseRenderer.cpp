#include "Raytracer/Renderer/BaseRenderer.h"

namespace AstralRaytracer
{

void BaseRenderer::renderStart(const Scene &scene, const Camera &cam)
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
void BaseRenderer::renderEnd()
{
    if (m_renderingThread.joinable())
    {
        m_renderingThread.join();
    }
}
glm::vec3 BaseRenderer::getRayDirectionFromNormalizedCoord(const glm::vec2 &coord, const glm::mat4 &inverseProjection,
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

void BaseRenderer::findClosestHit(HitInfo &closestHitInfo, const Scene &scene, const CoOrd3DF &rayOrigin,
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

TextureId BaseRenderer::getTextureId() const
{
    return m_textureId;
}

void BaseRenderer::setBounceCount(uint32 count)
{
    m_BounceCount = count;
}

uint32 BaseRenderer::getBounceCount() const
{
    return m_BounceCount;
}

uint32 BaseRenderer::getFrameIndex() const
{
    return m_frameIndex;
}

BaseRenderer::RendererState BaseRenderer::getState() const
{
    return m_state.load();
}

void BaseRenderer::setState(RendererState renderState)
{
    m_state.store(renderState);
}
} // namespace AstralRaytracer