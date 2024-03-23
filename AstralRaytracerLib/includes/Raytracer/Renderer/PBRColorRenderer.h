#pragma once

#include "GenericRenderer.h"

namespace AstralRaytracer
{
class PBRColorRenderer : public RendererRGBF
{
  public:
    PBRColorRenderer();
    virtual ~PBRColorRenderer();

  protected:
    virtual void render(const Scene &scene, const Camera &cam) override;
    virtual glm::vec3 perPixel(uint32 &seedVal, const Scene &scene, CoOrd3DF &rayOrigin,
                               Direction3D &rayDir) const override;
};
} // namespace AstralRaytracer