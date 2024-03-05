#include "Utils/TextureData.h"

namespace AstralRaytracer
{
	template<typename T, uint32 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(): m_width(0), m_height(0)
	{
	}

	template<typename T, uint32 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(uint32 width, uint32 height)
	{
		resize(width, height);
	}

	template<typename T, uint32 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(
			uint32                width,
			uint32                height,
			const std::vector<T>& data
	)
	{
		resize(width, height);
		setTextureData(data);
	}

	template<typename T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::resize(uint32 width, uint32 height)
	{
		m_width = width;
		m_height= height;
		m_data.resize(width * height * ComponentCount);
	}

	template<typename T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTextureData(const std::vector<T>& data)
	{
		ASTRAL_ASSERTM(m_width * m_height * ComponentCount == data.size(), "Mismatched data size");
		m_data= data;
	}

	template<typename T, uint32 ComponentCount>
	const std::vector<T>& TextureData<T, ComponentCount>::getTextureData() const
	{
		return m_data;
	}

	template<typename T, uint32 ComponentCount>
	uint32 TextureData<T, ComponentCount>::getWidth() const noexcept
	{
		return m_width;
	}

	template<typename T, uint32 ComponentCount>
	uint32 TextureData<T, ComponentCount>::getHeight() const noexcept
	{
		return m_height;
	}

	template<typename T, uint32 ComponentCount>
	constexpr uint32 TextureData<T, ComponentCount>::getComponentCount() const noexcept
	{
		return ComponentCount;
	}

	template<typename T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTexelColorAtPixelIndex(
			uint32                                            index,
			const glm::vec<ComponentCount, T, glm::defaultp>& rgb
	)
	{
		ASTRAL_ASSERTM(index <= (m_data.size() - ComponentCount), "Invalid index");

		m_data[index]    = rgb.r;
		m_data[index + 1]= rgb.g;
		m_data[index + 2]= rgb.b;
	}

	template<typename T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTexelColor(
			uint32                                            x,
			uint32                                            y,
			const glm::vec<ComponentCount, T, glm::defaultp>& rgb
	)
	{
		const uint32 index= (m_width * y + x) * ComponentCount;
		setTexelColorAtPixelIndex(index, rgb);
	}

	template<typename T, uint32 ComponentCount>
	glm::vec<ComponentCount, T, glm::defaultp>
	TextureData<T, ComponentCount>::getTexelColor(uint32 x, uint32 y) const
	{
		const uint32 index= (m_width * y + x) * ComponentCount;

		glm::vec<ComponentCount, T, glm::defaultp> out;

		for(uint32 compIndex= 0; compIndex < ComponentCount; ++compIndex)
		{
			out[compIndex]= m_data[index + compIndex];
		}

		return out;
	}

	template<typename T, uint32 ComponentCount>
	glm::vec<ComponentCount, T, glm::defaultp>
	TextureData<T, ComponentCount>::getTexelColor(float32 u, float32 v) const
	{
		const int32 xIndex= u * (m_width - 1);
		const int32 yIndex= v * (m_height - 1);

		const uint32 modXIndex= glm::abs(xIndex) % m_width;
		const uint32 modYIndex= glm::abs(yIndex) % m_height;

		return getTexelColor(modXIndex, modYIndex);
	}

	template class TextureData<uint8, 3>;
	template class TextureData<uint8, 4>;
	template class TextureData<float32, 3>;
	template class TextureData<float32, 4>;

} // namespace AstralRaytracer