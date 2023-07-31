#pragma once
#include <vector>
#include "Utils/Common.h"
#include "ColourData.h"
class TextureData
{
private:
	std::vector<uint8> m_data;
	uint32 m_width;
	uint32 m_height;
	uint8 m_componentCount;
public:
	TextureData();
	TextureData(uint32 width, uint32 height, uint8 componentCount);
    ~TextureData() = default;
	void setTextureData(const std::vector<uint8>& data);
	const std::vector<uint8>& getTextureData() const;
	uint32 getWidth() const noexcept;
	uint32 getHeight() const noexcept;
	uint8 getComponentCount() const noexcept;
	void setTexelColorAtPixelIndex(uint32 index, int32 r , int32 g, int32 b);
	void setTexelColor(int32 r, int32 g, int32 b, uint32 x, uint32 y);
	void setTexelColor(ColourData& colourData, uint32 x, uint32 y);
	ColourData getTexelColor(uint32 x, uint32 y);
};
