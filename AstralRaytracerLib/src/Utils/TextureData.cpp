#include "Utils/TextureData.h"

namespace AstralRaytracer
{
	template<ArithMeticType T, uint32 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(uint32 width, uint32 height)
	{
		resize(width, height);
	}

	template<ArithMeticType T, uint32 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(
			uint32                width,
			uint32                height,
			const std::vector<T>& data
	)
	{
		resize(width, height);
		setTextureData(data);
	}

	template<ArithMeticType T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::resize(uint32 width, uint32 height)
	{
		m_width = width;
		m_height= height;
		m_data.resize(width * height * ComponentCount);
	}

	template<ArithMeticType T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTextureData(const std::vector<T>& data)
	{
		ASTRAL_ASSERTM(m_width * m_height * ComponentCount == data.size(), "Mismatched data size");
		m_data= data;
	}

	template<ArithMeticType T, uint32 ComponentCount>
	const std::vector<T>& TextureData<T, ComponentCount>::getTextureData() const noexcept
	{
		return m_data;
	}

	template<ArithMeticType T, uint32 ComponentCount>
	uint32 TextureData<T, ComponentCount>::getWidth() const noexcept
	{
		return m_width;
	}

	template<ArithMeticType T, uint32 ComponentCount>
	uint32 TextureData<T, ComponentCount>::getHeight() const noexcept
	{
		return m_height;
	}

	template<ArithMeticType T, uint32 ComponentCount>
	constexpr uint32 TextureData<T, ComponentCount>::getComponentCount() const noexcept
	{
		return ComponentCount;
	}

	template<ArithMeticType T, uint32 ComponentCount>
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

	template<ArithMeticType T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTexelColor(
			uint32                                            x,
			uint32                                            y,
			const glm::vec<ComponentCount, T, glm::defaultp>& rgb
	)
	{
		const uint32 index= (m_width * y + x) * ComponentCount;
		setTexelColorAtPixelIndex(index, rgb);
	}

	template<ArithMeticType T, uint32 ComponentCount>
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

	template<ArithMeticType T, uint32 ComponentCount>
	glm::vec<ComponentCount, T, glm::defaultp>
	TextureData<T, ComponentCount>::getTexelColor(float32 u, float32 v, SamplingType samplingType)
			const
	{
		const int32 xIndex= u * (m_width - 1);
		const int32 yIndex= v * (m_height - 1);

		uint32 finalXIndex= 0;
		uint32 finalYIndex= 0;

		switch(samplingType)
		{
			case SamplingType::REPEAT:
				{
					finalXIndex= glm::abs(xIndex) % m_width;
					finalYIndex= glm::abs(yIndex) % m_height;
				}
				break;
			case SamplingType::MIRRORED_REPEAT:
				{
					finalXIndex= glm::abs(xIndex) % (2 * m_width);
					if(finalXIndex >= m_width)
					{
						finalXIndex= 2 * m_width - finalXIndex - 1;
					}
					finalYIndex= glm::abs(yIndex) % (2 * m_height);
					if(finalYIndex >= m_height)
					{
						finalYIndex= 2 * m_height - finalYIndex - 1;
					}
				}
				break;
			case SamplingType::CLAMP_TO_EDGE:
				{
					finalXIndex= glm::clamp(xIndex, 0, static_cast<int32>(m_width - 1));
					finalYIndex= glm::clamp(yIndex, 0, static_cast<int32>(m_height - 1));
				}
				break;
			case SamplingType::CLAMP_TO_BORDER:
				{
					if(xIndex < 0 || xIndex >= m_width || yIndex < 0 || yIndex >= m_height)
					{
						return glm::vec<ComponentCount, T, glm::defaultp>(0);
					}
					finalXIndex= xIndex;
					finalYIndex= yIndex;
				}
				break;
			default: break;
		}

		return getTexelColor(finalXIndex, finalYIndex);
	}

	template class TextureData<uint8, 3>;
	template class TextureData<uint8, 4>;
	template class TextureData<float32, 3>;
	template class TextureData<float32, 4>;

} // namespace AstralRaytracer