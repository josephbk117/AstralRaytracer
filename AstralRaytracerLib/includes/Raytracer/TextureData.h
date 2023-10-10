#pragma once
#include "ColourData.h"
#include "Utils/Common.h"

#include <array>
#include <vector>

template<typename T, uint8 ComponentCount>
class TextureData
{
	private:
		std::vector<T> m_data;
		uint32         m_width;
		uint32         m_height;
	public:
		TextureData();
		TextureData(uint32 width, uint32 height);
		~TextureData()= default;
		void                  resize(uint32 width, uint32 height);
		void                  setTextureData(const std::vector<T>& data);
		const std::vector<T>& getTextureData() const;
		uint32                getWidth() const noexcept;
		uint32                getHeight() const noexcept;
		constexpr uint8       getComponentCount() const noexcept;
		void
		setTexelColorAtPixelIndex(uint32 index, const glm::vec<ComponentCount, T, glm::defaultp>& rgb);
		void setTexelColor(const std::array<T, ComponentCount>& texel, uint32 x, uint32 y);
		glm::vec<ComponentCount, T, glm::defaultp> getTexelColor(uint32 x, uint32 y) const;
		glm::vec<ComponentCount, T, glm::defaultp> getTexelColor(float32 u, float32 v) const;
};

using TextureDataRGB  = TextureData<uint8, 3>;
using TextureDataRGBA = TextureData<uint8, 4>;
using TextureDataRGBF = TextureData<float32, 3>;
using TextureDataRGBAF= TextureData<float32, 4>;