#include "Utils/TextureData.h"

namespace AstralRaytracer
{
	template<ArithmeticType T, uint32 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(Resolution resolution)
	{
		resize(resolution);
	}

	template<ArithmeticType T, uint32 ComponentCount>
	TextureData<T, ComponentCount>::TextureData(Resolution resolution, const std::vector<T>& data)
	{
		resize(resolution);
		setTextureData(data);
	}

	template<ArithmeticType T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::resize(Resolution resolution)
	{
		m_resolution= resolution;
		m_data.resize(m_resolution.x * m_resolution.y * ComponentCount);
	}

	template<ArithmeticType T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTextureData(const std::vector<T>& data)
	{
		ASTRAL_ASSERTM(
				m_resolution.x * m_resolution.y * ComponentCount == data.size(), "Mismatched data size"
		);
		m_data= data;
	}

	template<ArithmeticType T, uint32 ComponentCount>
	const std::vector<T>& TextureData<T, ComponentCount>::getTextureData() const noexcept
	{
		return m_data;
	}

	template<ArithmeticType T, uint32 ComponentCount>
	int32 TextureData<T, ComponentCount>::getWidth() const noexcept
	{
		return m_resolution.x;
	}

	template<ArithmeticType T, uint32 ComponentCount>
	int32 TextureData<T, ComponentCount>::getHeight() const noexcept
	{
		return m_resolution.y;
	}

	template<ArithmeticType T, uint32 ComponentCount>
	constexpr uint32 TextureData<T, ComponentCount>::getComponentCount() const noexcept
	{
		return ComponentCount;
	}

	template<ArithmeticType T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTexelColorAtPixelIndex(
			uint32                                            index,
			const glm::vec<ComponentCount, T, glm::defaultp>& color
	)
	{
		ASTRAL_ASSERTM(index <= (m_data.size() - ComponentCount), "Invalid index");
		std::memcpy(&m_data[index], &color, ComponentCount * sizeof(T));
	}

	template<ArithmeticType T, uint32 ComponentCount>
	void TextureData<T, ComponentCount>::setTexelColor(
			uint32                                            x,
			uint32                                            y,
			const glm::vec<ComponentCount, T, glm::defaultp>& color
	)
	{
		const uint32 index= (m_resolution.x * y + x) * ComponentCount;
		setTexelColorAtPixelIndex(index, color);
	}

	template<ArithmeticType T, uint32 ComponentCount>
	glm::vec<ComponentCount, T, glm::defaultp>
	TextureData<T, ComponentCount>::getTexelColor(int32 x, int32 y) const
	{
		const uint32 index= (m_resolution.x * y + x) * ComponentCount;
		ASTRAL_ASSERTM(index <= (m_data.size() - ComponentCount), "Invalid index");

		glm::vec<ComponentCount, T, glm::defaultp> out;
		std::memcpy(&out[0], &m_data[index], ComponentCount * sizeof(T));

		return out;
	}

	template<ArithmeticType T, uint32 ComponentCount>
	glm::vec<ComponentCount, T, glm::defaultp>
	TextureData<T, ComponentCount>::getTexelColor(float32 u, float32 v, SamplingType samplingType)
			const
	{
		const int32 xIndex= u * (m_resolution.x - 1);
		const int32 yIndex= v * (m_resolution.y - 1);

		uint32 finalXIndex= 0;
		uint32 finalYIndex= 0;

		switch(samplingType)
		{
			case SamplingType::REPEAT:
				{
					finalXIndex= glm::abs(xIndex) % m_resolution.x;
					finalYIndex= glm::abs(yIndex) % m_resolution.y;
				}
				break;
			case SamplingType::MIRRORED_REPEAT:
				{
					finalXIndex= glm::abs(xIndex) % (2 * m_resolution.x);
					if(finalXIndex >= m_resolution.x)
					{
						finalXIndex= 2 * m_resolution.x - finalXIndex - 1;
					}
					finalYIndex= glm::abs(yIndex) % (2 * m_resolution.y);
					if(finalYIndex >= m_resolution.y)
					{
						finalYIndex= 2 * m_resolution.y - finalYIndex - 1;
					}
				}
				break;
			case SamplingType::CLAMP_TO_EDGE:
				{
					finalXIndex= glm::clamp(xIndex, 0, static_cast<int32>(m_resolution.x - 1));
					finalYIndex= glm::clamp(yIndex, 0, static_cast<int32>(m_resolution.y - 1));
				}
				break;
			case SamplingType::CLAMP_TO_BORDER:
				{
					if(xIndex < 0 || xIndex >= m_resolution.x || yIndex < 0 || yIndex >= m_resolution.y)
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