#pragma once

#include "../Camera.h"
#include "../Scene.h"
#include "Utils/Common.h"
#include "Utils/ShaderProgram.h"
#include "Utils/TextureData.h"
#include <atomic>

namespace AstralRaytracer
{

class BaseRenderer
{
  public:
    enum class RendererState : uint8
    {
        NOT_STARTED,
        STARTED,
        DONE
    };

    virtual ~BaseRenderer(){};
    virtual void initialize() = 0;
    virtual bool onRenderComplete() = 0;
    virtual bool onResize(const Resolution &resolution) = 0;
    virtual void resetFrameIndex() = 0;

    void renderStart(const Scene &scene, const Camera &cam);
    void renderEnd();

    glm::vec3 getRayDirectionFromNormalizedCoord(const glm::vec2 &coord, const glm::mat4 &inverseProjection,
                                                 const glm::mat4 &inverseView) const;

    void findClosestHit(HitInfo &closestHitInfo, const Scene &scene, const CoOrd3DF &rayOrigin,
                        const Direction3D &rayDir) const;

    TextureId getTextureId() const;
    void setBounceCount(uint32 count);
    uint32 getBounceCount() const;
    uint32 getFrameIndex() const;

  protected:
    virtual void render(const Scene &scene, const Camera &cam) = 0;

    RendererState getState() const;
    void setState(RendererState renderState);

    std::thread m_renderingThread;
    std::atomic<RendererState> m_state{RendererState::NOT_STARTED};

    float32 m_maxLuminance = 0;
    std::vector<uint32> m_rayIterator;
    uint32 m_textureId = 0;
    uint32 m_frameIndex = 1;
    uint32 m_BounceCount = 4;
};
} // namespace AstralRaytracer