#include "Raytracer/Renderer/GenericRenderer.h"
#include "Utils/TextureManager.h"

namespace AstralRaytracer
{
template <ArithmeticType T, uint32 ComponentCount> GenericRenderer<T, ComponentCount>::GenericRenderer()
{
}

template <ArithmeticType T, uint32 ComponentCount> GenericRenderer<T, ComponentCount>::~GenericRenderer()
{
    renderEnd();
}

template <ArithmeticType T, uint32 ComponentCount> void GenericRenderer<T, ComponentCount>::initialize()
{
    const Resolution initialRes(16, 16);

    m_texData.resize(initialRes);
    m_textureId = TextureManager::loadTextureFromTextureData<T, ComponentCount>(m_texData, false);

    resize(initialRes);
}

template <ArithmeticType T, uint32 ComponentCount> bool GenericRenderer<T, ComponentCount>::onRenderComplete()
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
void GenericRenderer<T, ComponentCount>::resize(const Resolution &resolution)
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
bool GenericRenderer<T, ComponentCount>::onResize(const Resolution &resolution)
{
    if (m_texData.getResolution() == resolution)
    {
        return false;
    }

    resize(resolution);

    return true;
}

template <ArithmeticType T, uint32 ComponentCount> void GenericRenderer<T, ComponentCount>::resetFrameIndex()
{
    m_frameIndex = 1;

    const Resolution &texRes = m_texData.getResolution();

    const size_t numBytes = m_texData.getComponentCount() * sizeof(float32);
    const size_t bufferSize = numBytes * texRes.x * texRes.y;

    std::memset(m_accumulatedColorData.data(), 0, bufferSize);
}

template class GenericRenderer<uint8, 3>;
template class GenericRenderer<uint8, 4>;
template class GenericRenderer<float32, 1>;
template class GenericRenderer<float32, 3>;
template class GenericRenderer<float32, 4>;
} // namespace AstralRaytracer