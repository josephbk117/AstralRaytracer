#pragma once
#include "Traceable.h"

namespace AstralRaytracer
{
	class SphereTraceable: public Traceable
	{
		public:
		SphereTraceable() = default;
		void setRadius(float32 radius) { m_radius = radius; };
		float32 getRadius() const { return m_radius; }
		bool trace(const Ray& rayIn, HitInfo& hitInfo) const override;

		private:
		float32 m_radius = 1.0f;
	};
} // namespace AstralRaytracer