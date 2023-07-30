#include "Raytracer/TextureData.h"
TextureData::TextureData()
{
	data = nullptr;
	width = 0;
	height = 0;
	componentCount = 0;
}

void TextureData::setTextureData(uint8 * data, uint32 width, uint32 height, uint8 componentCount)
{
	this->data = data;
	data = nullptr;
	this->width = width;
	this->height = height;
	this->componentCount = componentCount;
}

uint8 * TextureData::getTextureData()
{
	return data;
}

const uint32 TextureData::getWidth() noexcept
{
	return width;
}

const uint32 TextureData::getHeight() noexcept
{
	return height;
}

uint8 TextureData::getComponentCount() noexcept
{
	return componentCount;
}

void TextureData::setTexelColor(int32 r, int32 g, int32 b, uint32 x, uint32 y)
{
	x = glm::clamp(x, 0u, width);
	y = glm::clamp(y, 0u, height);
	int32 i = ((float32)width * (float32)y + (float32)x) * 4.0f;
	r = glm::clamp(r, 0, 255);
	g = glm::clamp(g, 0, 255);
	b = glm::clamp(b, 0, 255);

	data[i] = r;
	data[i + 1] = g;
	data[i + 2] = b;
}

void TextureData::setTexelColor(ColourData & colourData, uint32 x, uint32 y)
{
	x = glm::clamp(x, 0u, width);
	y = glm::clamp(y, 0u, height);
	int32 i = ((float32)width * (float32)y + (float32)x) * 4.0f;
	data[i] = colourData.getColour_8_Bit().r;
	data[i + 1] = colourData.getColour_8_Bit().g;
	data[i + 2] = colourData.getColour_8_Bit().b;
}

ColourData TextureData::getTexelColor(uint32 x, uint32 y)
{
	int32 i = ((float32)width * (float32)y + (float32)x) * 3.0f;
	ColourData colData;
	colData.setColour_8_Bit(data[i], data[i + 1], data[i + 2]);
	return colData;
}

TextureData::~TextureData()
{
	delete[] data;
}