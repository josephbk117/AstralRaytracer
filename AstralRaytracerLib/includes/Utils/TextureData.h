#pragma once
#include "ColourData.h"
#include "Common.h"

#include <vector>

namespace AstralRaytracer
{
	enum class SamplingType : uint32
	{
		INVALID,
		MIN,
		REPEAT= MIN,
		CLAMP_TO_EDGE,
		MIRRORED_REPEAT,
		CLAMP_TO_BORDER,
		MAX= CLAMP_TO_BORDER
	};

	template<typename T>
	concept ArithMeticType= std::is_arithmetic_v<T>;

	template<ArithMeticType T, uint32 ComponentCount>
	class TextureData
	{
		private:
			std::vector<T> m_data;
			uint32         m_width;
			uint32         m_height;
		public:
			TextureData();
			TextureData(uint32 width, uint32 height);
			TextureData(uint32 width, uint32 height, const std::vector<T>& data);
			~TextureData()= default;
			void resize(uint32 width, uint32 height);
			void setTextureData(const std::vector<T>& data);

			[[nodiscard]]
			const std::vector<T>& getTextureData() const;
			[[nodiscard]]
			uint32 getWidth() const noexcept;
			[[nodiscard]]
			uint32 getHeight() const noexcept;
			[[nodiscard]]
			constexpr uint32 getComponentCount() const noexcept;

			void setTexelColorAtPixelIndex(
					uint32                                            index,
					const glm::vec<ComponentCount, T, glm::defaultp>& rgb
			);

			void setTexelColor(uint32 x, uint32 y, const glm::vec<ComponentCount, T, glm::defaultp>& rgb);
			[[nodiscard]]
			glm::vec<ComponentCount, T, glm::defaultp> getTexelColor(uint32 x, uint32 y) const;
			[[nodiscard]]
			glm::vec<ComponentCount, T, glm::defaultp>
			getTexelColor(float32 u, float32 v, SamplingType samplingType) const;
	};

	using TextureDataRGB  = TextureData<uint8, 3>;
	using TextureDataRGBA = TextureData<uint8, 4>;
	using TextureDataRGBF = TextureData<float32, 3>;
	using TextureDataRGBAF= TextureData<float32, 4>;
} // namespace AstralRaytracer