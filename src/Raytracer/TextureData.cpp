#include "Raytracer/TextureData.h"

TextureData::TextureData(): m_width(0), m_height(0), m_componentCount(0) {}

TextureData::TextureData(uint32 width, uint32 height, uint8 componentCount)
{
	m_width         = width;
	m_height        = height;
	m_componentCount= componentCount;

	assertm(m_componentCount <= 4, "Invalid Component count");

	m_data.resize(width * height * componentCount);
}

void TextureData::resize(uint32 width, uint32 height)
{
	if(m_width == width && m_height == height)
	{
		return;
	}

	m_width = width;
	m_height= height;

	m_data.resize(width * height * m_componentCount);
}

void TextureData::setTextureData(const std::vector<uint8>& data) { m_data= data; }

const std::vector<uint8>& TextureData::getTextureData() const { return m_data; }

uint32 TextureData::getWidth() const noexcept { return m_width; }

uint32 TextureData::getHeight() const noexcept { return m_height; }

uint8 TextureData::getComponentCount() const noexcept { return m_componentCount; }

void TextureData::setTexelColorAtPixelIndex(uint32 index, const glm::u8vec3& rgb)
{
	assertm(index <= (m_data.size() - m_componentCount),
					"Can't set color at pixel index, invalid range");
	m_data[index]    = rgb.r;
	m_data[index + 1]= rgb.g;
	m_data[index + 2]= rgb.b;
}


void TextureData::setTexelColor(int32 r, int32 g, int32 b, uint32 x, uint32 y)
{
	const int32 i= ((float32)m_width * (float32)y + (float32)x) * 4.0f;

	m_data[i]    = r;
	m_data[i + 1]= g;
	m_data[i + 2]= b;
}

void TextureData::setTexelColor(ColourData& colourData, uint32 x, uint32 y)
{
	int32 i      = ((float32)m_width * (float32)y + (float32)x) * 4.0f;
	m_data[i]    = colourData.getColour_8_BitClamped().r;
	m_data[i + 1]= colourData.getColour_8_BitClamped().g;
	m_data[i + 2]= colourData.getColour_8_BitClamped().b;
}

ColourData TextureData::getTexelColor(uint32 x, uint32 y)
{
	int32      i= ((float32)m_width * (float32)y + (float32)x) * 3.0f;
	ColourData colData;
	colData.setColour_8_Bit(m_data[i], m_data[i + 1], m_data[i + 2]);
	return colData;
}