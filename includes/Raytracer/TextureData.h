#pragma once
#include "Utils/Common.h"
#include "ColourData.h"
class TextureData
{
private:
	uint8* data;
	uint32 width;
	uint32 height;
	uint8 componentCount;
public:
	TextureData();
    ~TextureData();
	void setTextureData(uint8* data, uint32 width, uint32 height, uint8 componentCount);
	uint8* getTextureData();
	const uint32 getWidth()noexcept;
	const uint32 getHeight()noexcept;
	uint8 getComponentCount()noexcept;
	void setTexelColor(int32 r, int32 g, int32 b, uint32 x, uint32 y);
	void setTexelColor(ColourData& colourData, uint32 x, uint32 y);
	ColourData getTexelColor(uint32 x, uint32 y);
};
