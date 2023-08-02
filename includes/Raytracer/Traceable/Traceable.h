#pragma once
#include "Raytracer/Ray.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Traceable
	{
		public:
		virtual bool trace(const Ray& rayIn, HitInfo& hitInfo)= 0;
	};
} // namespace AstralRaytracer