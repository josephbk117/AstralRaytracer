#pragma once

#include "BaseRenderer.h"

namespace AstralRaytracer
{
template <ArithmeticType T, uint32 ComponentCount> class GenericRenderer : public BaseRenderer
{
  public:
    GenericRenderer();
    virtual ~GenericRenderer();

    virtual void initialize() override;
    virtual bool onRenderComplete() override;
    virtual bool onResize(const Resolution &resolution) override;
    virtual void resetFrameIndex() override;

    const TextureData<T, ComponentCount> &getTextureData() const
    {
        return m_texData;
    }

  protected:
    virtual glm::vec<ComponentCount, T, glm::defaultp> perPixel(uint32 &seedVal, const Scene &scene,
                                                                CoOrd3DF &rayOrigin, Direction3D &rayDir) const = 0;

    void resize(const Resolution &resolution);

    TextureData<T, ComponentCount> m_texData;
    std::vector<T> m_accumulatedColorData;
};

using RendererRGB = GenericRenderer<uint8, 3>;
using RendererRGBA = GenericRenderer<uint8, 4>;
using RendererRF = GenericRenderer<float32, 1>;
using RendererRGBF = GenericRenderer<float32, 3>;
using RendererRGBAF = GenericRenderer<float32, 4>;
} // namespace AstralRaytracer