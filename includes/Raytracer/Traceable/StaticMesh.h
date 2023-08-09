#pragma once
#include "TriangleTraceable.h"
#include "Utils/Common.h"

#include <vector>

namespace AstralRaytracer
{
	class StaticMesh: public Traceable
	{
		public:
		StaticMesh()= default;
		StaticMesh(const std::vector<TriangleTraceable>& triangles): m_triangles(triangles) {}
		bool trace(const Ray& rayIn, HitInfo& hitInfo) const override;
		void setPosition(const glm::vec3& position) override;
		void setMaterialIndex(uint32 materialIndex) override;

		private:
		std::vector<TriangleTraceable> m_triangles;
	};
} // namespace AstralRaytracer