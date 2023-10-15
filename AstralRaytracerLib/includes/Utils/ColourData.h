#pragma once
#include "Utils/Common.h"

struct ColourData
{
	private:
		glm::vec3 m_colour;
	public:
		ColourData() { m_colour= glm::vec3(1.0f, 1.0f, 1.0f); }

		ColourData(float32 r, float32 g, float32 b) { m_colour= glm::vec3(r, g, b); }

		ColourData(const glm::vec3& normalized) { m_colour= normalized; }

		void setColour_8_Bit(const glm::vec3& colour)
		{
			int32 r = glm::clamp((int32)colour.r, 0, 255);
			int32 g = glm::clamp((int32)colour.g, 0, 255);
			int32 b = glm::clamp((int32)colour.b, 0, 255);
			m_colour= glm::vec3(r, g, b);
		}

		void setColour_8_Bit(uint8 r, uint8 g, uint8 b)
		{
			float32 _r= r / 255.0f;
			float32 _g= g / 255.0f;
			float32 _b= b / 255.0f;
			m_colour  = glm::vec3(_r, _g, _b);
		}

		void setColour_32_bit(const glm::vec3& colour) { m_colour= colour; }

		void setColour_32_bit(float32 r, float32 g, float32 b) { m_colour= glm::vec3(r, g, b); }

		glm::vec3 getColour_32_bit() const { return m_colour; }

		glm::u8vec3 getColour_8_BitClamped() const
		{
			const glm::vec3& clamped= glm::min(m_colour, glm::vec3(1.0f)) * 255.0f;
			return glm::u8vec3(clamped.r, clamped.g, clamped.b);
		}
};

namespace AstralRaytracer
{
	namespace Colors
	{
		const ColourData Red(1.0f, 0.0f, 0.0f);
		const ColourData Green(0.0f, 1.0f, 0.0f);
		const ColourData Blue(0.0f, 0.0f, 1.0f);
		const ColourData Yellow(1.0f, 1.0f, 0.0f);
		const ColourData Pink(1.0f, 0.25f, 1.0f);
		const ColourData White(1.0f, 1.0f, 1.0f);
		const ColourData Black(0.0f, 0.0f, 0.0f);
	} // namespace Colors
} // namespace AstralRaytracer