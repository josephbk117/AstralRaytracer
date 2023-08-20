#pragma once
#include "ColourData.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	struct Material
	{
		ColourData albedo   = Colors::Pink;
		float32    roughness= 0.5;
		uint32     texture  = 0;
	};
} // namespace AstralRaytracer