#include "ColourData.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	struct Material
	{
		ColourData albedo   = Colors::Pink;
		float32    roughness= 0.5;
	};
} // namespace AstralRaytracer