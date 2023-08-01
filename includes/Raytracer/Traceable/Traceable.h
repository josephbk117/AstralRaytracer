#pragma once
#include "Utils/Common.h"
#include "Raytracer/Ray.h"

namespace AstralRaytracer
{
	class Traceable
	{
		public:
		virtual bool trace(const Ray& rayIn, HitInfo& hitInfo) = 0;
	};
}