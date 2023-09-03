#pragma once
#include "TriangleTraceable.h"
#include "Utils/Common.h"

#include <vector>

namespace AstralRaytracer
{
	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	class StaticMesh: public Traceable
	{
		public:
		StaticMesh()= default;
		StaticMesh(const std::vector<TriangleTraceable>& triangles, const AABB& boundingBox)
				: m_triangles(triangles), m_boundingBox(boundingBox)
		{
		}
		bool trace(const Ray& rayIn, HitInfo& hitInfo) const override;
		void setPosition(const glm::vec3& position) override;
		void setMaterialIndex(uint32 materialIndex) override;

		private:
		bool intersectsBoundingBox(const Ray& rayIn) const;

		AABB m_boundingBox;
		std::vector<TriangleTraceable> m_triangles;
	};
} // namespace AstralRaytracer