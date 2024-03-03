#include "Utils/TextureData.h"

namespace AstralRaytracer
{
	template<typename T, uint8 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(): m_width(0), m_height(0)
	{
	}

	template<typename T, uint8 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(uint32 width, uint32 height)
	{
		m_width = width;
		m_height= height;
		m_data.resize(width * height * ComponentCount);
	}

	template<typename T, uint8 ComponentCount>
	void TextureData<T, ComponentCount>::resize(uint32 width, uint32 height)
	{
		m_width = width;
		m_height= height;
		m_data.resize(width * height * ComponentCount);
	}

	template<typename T, uint8 ComponentCount>
	void TextureData<T, ComponentCount>::setTextureData(const std::vector<T>& data)
	{
		ASTRAL_ASSERTM(
				m_width * m_height * ComponentCount == data.size(),
				"Can't set texture data, invalid input data size"
		);

		m_data= data;
	}

	template<typename T, uint8 ComponentCount>
	const std::vector<T>& TextureData<T, ComponentCount>::getTextureData() const
	{
		return m_data;
	}

	template<typename T, uint8 ComponentCount>
	uint32 TextureData<T, ComponentCount>::getWidth() const noexcept
	{
		return m_width;
	}

	template<typename T, uint8 ComponentCount>
	uint32 TextureData<T, ComponentCount>::getHeight() const noexcept
	{
		return m_height;
	}

	template<typename T, uint8 ComponentCount>
	constexpr uint8 TextureData<T, ComponentCount>::getComponentCount() const noexcept
	{
		return ComponentCount;
	}

	template<typename T, uint8 ComponentCount>
	void TextureData<T, ComponentCount>::setTexelColorAtPixelIndex(
			uint32                                            index,
			const glm::vec<ComponentCount, T, glm::defaultp>& rgb
	)
	{
		ASTRAL_ASSERTM(
				index <= (m_data.size() - ComponentCount), "Can't set color at pixel index, invalid range"
		);

		m_data[index]    = rgb.r;
		m_data[index + 1]= rgb.g;
		m_data[index + 2]= rgb.b;
	}

	template<typename T, uint8 ComponentCount>
	void TextureData<T, ComponentCount>::setTexelColor(
			const std::array<T, ComponentCount>& texel,
			uint32                               x,
			uint32                               y
	)
	{
		const int32 i= ((float32)m_width * (float32)y + (float32)x) * 4.0f;

		m_data[i]    = texel[0];
		m_data[i + 1]= texel[1];
		m_data[i + 2]= texel[2];
	}

	template<typename T, uint8 ComponentCount>
	glm::vec<ComponentCount, T, glm::defaultp>
	TextureData<T, ComponentCount>::getTexelColor(uint32 x, uint32 y) const
	{
		const uint32                               index= (m_width * y + x) * ComponentCount;
		glm::vec<ComponentCount, T, glm::defaultp> out;

		for(uint32 compIndex= 0; compIndex < ComponentCount; ++compIndex)
		{
			out[compIndex]= m_data[index + compIndex];
		}

		return out;
	}

	template<typename T, uint8 ComponentCount>
	glm::vec<ComponentCount, T, glm::defaultp>
	TextureData<T, ComponentCount>::getTexelColor(float32 u, float32 v) const
	{
		const uint32 xIndex= glm::abs(static_cast<uint32>(u * m_width) + m_width) % m_width;
		const uint32 yIndex= glm::abs(static_cast<uint32>(v * m_height) + m_height) % m_height;

		return getTexelColor(xIndex, yIndex);
	}

	template class TextureData<uint8, 3>;
	template class TextureData<uint8, 4>;
	template class TextureData<float32, 3>;
	template class TextureData<float32, 4>;

} // namespace AstralRaytracer