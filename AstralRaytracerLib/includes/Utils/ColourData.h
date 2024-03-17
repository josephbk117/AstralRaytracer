#pragma once
#include "Common.h"

namespace AstralRaytracer
{
struct ColourData
{
  private:
    glm::vec4 m_colour{1.0f, 1.0f, 1.0f, 1.0f};

  public:
    ColourData() = default;

    ColourData(float32 r, float32 g, float32 b, float32 a)
    {
        m_colour = glm::vec4(r, g, b, a);
    }

    ColourData(const glm::vec4 &normalized)
    {
        m_colour = normalized;
    }

    void setColour_8_Bit(const glm::vec4 &colour)
    {
        int32 r = glm::clamp((int32)colour.r, 0, 255);
        int32 g = glm::clamp((int32)colour.g, 0, 255);
        int32 b = glm::clamp((int32)colour.b, 0, 255);
        int32 a = glm::clamp((int32)colour.a, 0, 255);
        m_colour = glm::vec4(r, g, b, a);
    }

    void setColour_8_Bit(uint8 r, uint8 g, uint8 b, uint8 a)
    {
        float32 _r = r / 255.0f;
        float32 _g = g / 255.0f;
        float32 _b = b / 255.0f;
        float32 _a = a / 255.0f;
        m_colour = glm::vec4(_r, _g, _b, _a);
    }

    void setColour_32_bit(const glm::vec4 &colour)
    {
        m_colour = colour;
    }

    void setColour_32_bit(float32 r, float32 g, float32 b, float32 a)
    {
        m_colour = glm::vec4(r, g, b, a);
    }

    [[nodiscard]] const glm::vec4 &getColour_32_bit() const
    {
        return m_colour;
    }

    [[nodiscard]] glm::u8vec4 getColour_8_BitClamped() const
    {
        const glm::vec4 &clamped = glm::min(m_colour, glm::vec4(1.0f)) * 255.0f;
        return glm::u8vec4(clamped.r, clamped.g, clamped.b, clamped.a);
    }
};

namespace Colors
{
const ColourData Red(1.0f, 0.0f, 0.0f, 1.0f);
const ColourData Green(0.0f, 1.0f, 0.0f, 1.0f);
const ColourData Blue(0.0f, 0.0f, 1.0f, 1.0f);
const ColourData Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const ColourData Pink(1.0f, 0.25f, 1.0f, 1.0f);
const ColourData White(1.0f, 1.0f, 1.0f, 1.0f);
const ColourData Black(0.0f, 0.0f, 0.0f, 1.0f);
} // namespace Colors
} // namespace AstralRaytracer