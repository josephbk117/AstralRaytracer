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
	concept ArithmeticType= std::is_arithmetic_v<T>;

	template<ArithmeticType T, uint32 ComponentCount>
	class TextureData
	{
		private:
			std::vector<T> m_data;
			Resolution     m_resolution{ 0 };
		public:
			TextureData()= default;
			TextureData(Resolution resolution);
			TextureData(Resolution resolution, const std::vector<T>& data);
			TextureData(const TextureData&)= default;
			~TextureData()                 = default;
			void resize(Resolution resolution);
			void setTextureData(const std::vector<T>& data);

			[[nodiscard]]
			const std::vector<T>& getTextureData() const noexcept;
			[[nodiscard]]
			const Resolution& getResolution() const noexcept;
			[[nodiscard]]
			int32 getWidth() const noexcept;
			[[nodiscard]]
			int32 getHeight() const noexcept;
			[[nodiscard]]
			inline constexpr uint32 getComponentCount() const noexcept;

			void setTexelColorAtPixelIndex(
					uint32                                            index,
					const glm::vec<ComponentCount, T, glm::defaultp>& color
			);

			void
			setTexelColor(uint32 x, uint32 y, const glm::vec<ComponentCount, T, glm::defaultp>& color);
			[[nodiscard]]
			glm::vec<ComponentCount, T, glm::defaultp> getTexelColor(int32 x, int32 y) const;
			[[nodiscard]]
			glm::vec<ComponentCount, T, glm::defaultp>
			getTexelColor(float32 u, float32 v, SamplingType samplingType) const;
	};

	using TextureDataRGB  = TextureData<uint8, 3>;
	using TextureDataRGBA = TextureData<uint8, 4>;
	using TextureDataRGBF = TextureData<float32, 3>;
	using TextureDataRGBAF= TextureData<float32, 4>;
} // namespace AstralRaytracer