#pragma once
#include "Traceable.h"

namespace AstralRaytracer
{
	class SphereTraceable: public Traceable
	{
		public:
			SphereTraceable()= default;

			void setRadius(float32 radius) { m_radius= radius; };

			[[nodiscard]]
			float32 getRadius() const;

			virtual void setScale(const glm::vec3& scale) override;

			[[nodiscard]]
			bool trace(const Ray& rayIn, HitInfo& hitInfo) const override;

			void serialize(AssetManager& assetManager, YAML::Emitter& out) const override;
			void deserialize(AssetManager& assetManager, YAML::Node& node) override;
		private:
			float32 m_radius= 1.0f;
	};
} // namespace AstralRaytracer