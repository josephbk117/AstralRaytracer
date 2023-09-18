#pragma once
#include "Traceable.h"

namespace AstralRaytracer
{
	class SphereTraceable: public Traceable
	{
		public:
		SphereTraceable()= default;
		void    setRadius(float32 radius) { m_radius= radius; };
		float32 getRadius() const { return m_radius; }

		virtual void setScale(const glm::vec3& scale) override;

		bool trace(const Ray& rayIn, HitInfo& hitInfo) const override;

		void serialize(YAML::Emitter& out) const override;
		void deserialize(YAML::Node& node) override;

		private:
		float32 m_radius= 1.0f;
	};
} // namespace AstralRaytracer