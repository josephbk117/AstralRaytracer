#pragma once
#include "Utils/Common.h"
#include <glm.hpp>
struct ColourData
{
private:
	glm::vec3 colour;
public:
	ColourData()
	{
		colour = glm::vec3(1.0f, 1.0f, 1.0f);
	}
	ColourData(float32 r, float32 g, float32 b)
	{
		colour = glm::vec3(r, g, b);
	}
	void setColour_8_Bit(const glm::vec4& colour)
	{
		int32 r = glm::clamp((int32)colour.r, 0, 255);
		int32 g = glm::clamp((int32)colour.g, 0, 255);
		int32 b = glm::clamp((int32)colour.b, 0, 255);
		this->colour = glm::vec3(r, g, b);
	}
	void setColour_8_Bit(uint8 r, uint8 g, uint8 b)
	{
		float32 _r = r / 255.0f;
		float32 _g = g / 255.0f;
		float32 _b = b / 255.0f;
		this->colour = glm::vec3(_r, _g, _b);
	}
	void setColour_32_bit(const glm::vec3& colour)
	{
		this->colour = colour;
	}
	void setColour_32_bit(float32 r, float32 g, float32 b)
	{
		this->colour = glm::vec3(r, g, b);
	}
	glm::vec3 getColourIn_0_1_Range()
	{
		return colour;
	}
	glm::vec3 getColour_8_Bit()
	{
		uint8 rV = (uint8)(colour.r * 255.0f);
		uint8 gV = (uint8)(colour.g * 255.0f);
		uint8 bV = (uint8)(colour.b * 255.0f);
		return glm::vec3(rV, gV, bV);
	}
};