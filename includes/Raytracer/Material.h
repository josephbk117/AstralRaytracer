#pragma once
#include "ColourData.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	struct Material
	{
		ColourData albedo          = Colors::Pink;
		ColourData emission        = Colors::White;
		float32    emissionStrength= 0.0f;
		float32    roughness       = 0.5;
		uint32     texture         = 0;

		glm::vec3 getEmission() const { return emission.getColour_32_bit() * emissionStrength; }
	};
} // namespace AstralRaytracer