#pragma once

#include "GenericRenderer.h"

namespace AstralRaytracer
{
class DepthRenderer : public RendererRF
{
  public:
    DepthRenderer();
    virtual ~DepthRenderer();

  protected:
    virtual void render(const Scene &scene, const Camera &cam) override;
    virtual glm::vec1 perPixel(uint32 &seedVal, const Scene &scene, CoOrd3DF &rayOrigin,
                               Direction3D &rayDir) const override;
};
} // namespace AstralRaytracer