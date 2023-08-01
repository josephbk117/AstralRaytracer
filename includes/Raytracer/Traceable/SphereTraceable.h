#pragma once
#include "Traceable.h"

namespace AstralRaytracer
{
	class SphereTraceable : public Traceable
	{
		public:
		bool trace(const Ray& rayIn, HitInfo& hitInfo) override;
	};
}