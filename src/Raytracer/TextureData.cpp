#include "Raytracer/TextureData.h"
TextureData::TextureData(uint32 width, uint32 height, uint8 componentCount)
{
	m_width = width;
	m_height = height;
	m_componentCount = componentCount;

	assertm(m_componentCount <= 4, "Invalid Component count");

	m_data.resize(width * height * componentCount);
}

void TextureData::setTextureData(const std::vector<uint8>& data)
{
	m_data = data;
}

const std::vector<uint8>& TextureData::getTextureData()
{
	return m_data;
}

const uint32 TextureData::getWidth() noexcept
{
	return m_width;
}

const uint32 TextureData::getHeight() noexcept
{
	return m_height;
}

uint8 TextureData::getComponentCount() noexcept
{
	return m_componentCount;
}

void TextureData::setTexelColorAtPixelIndex(uint32 index, int32 r, int32 g, int32 b)
{
	assertm(index <= (m_data.size() - m_componentCount), "Can't set color at pixel index, invalid range");

	m_data[index] = r;
	m_data[index+1] = g;
	m_data[index+2] = b;
}

void TextureData::setTexelColor(int32 r, int32 g, int32 b, uint32 x, uint32 y)
{
	int32 i = ((float32)m_width * (float32)y + (float32)x) * 4.0f;

	m_data[i] = r;
	m_data[i + 1] = g;
	m_data[i + 2] = b;
}

void TextureData::setTexelColor(ColourData & colourData, uint32 x, uint32 y)
{
	int32 i = ((float32)m_width * (float32)y + (float32)x) * 4.0f;
	m_data[i] = colourData.getColour_8_Bit().r;
	m_data[i + 1] = colourData.getColour_8_Bit().g;
	m_data[i + 2] = colourData.getColour_8_Bit().b;
}

ColourData TextureData::getTexelColor(uint32 x, uint32 y)
{
	int32 i = ((float32)m_width * (float32)y + (float32)x) * 3.0f;
	ColourData colData;
	colData.setColour_8_Bit(m_data[i], m_data[i + 1], m_data[i + 2]);
	return colData;
}