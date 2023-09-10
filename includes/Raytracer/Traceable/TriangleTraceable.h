#pragma once

#include "Traceable.h"

namespace AstralRaytracer
{
	class TriangleTraceable: public Traceable
	{
		public:
		TriangleTraceable();
		TriangleTraceable(const glm::vec3& vertexA, const glm::vec3& vertexB, const glm::vec3& vertexC);
		float32 getTriangleDistance() const;
		bool    trace(const Ray& rayIn, HitInfo& hitInfo) const override;

		void serialize(YAML::Emitter& out) const override;
		void deserialize(YAML::Node& node) override;

		private:
		glm::vec3 m_vertexA, m_vertexB, m_vertexC;
		glm::vec3 m_normal;
		glm::vec3 m_vAvB;
		glm::vec3 m_vAvC;
	};

} // namespace AstralRaytracer