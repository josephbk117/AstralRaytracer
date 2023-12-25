#pragma once
#include "TriangleTraceable.h"
#include "Utils/Common.h"

#include <filesystem>
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
		StaticMesh(const std::filesystem::path& meshFilePath);
		StaticMesh(const std::vector<TriangleTraceable>& triangles, const AABB& boundingBox)
				: m_triangles(triangles), m_boundingBox(boundingBox)
		{
			m_initialBoundingBox= boundingBox;
		}
		StaticMesh(const std::vector<TriangleTraceable>& triangles, const AABB& boundingBox,
							 const std::string& srcFilePath)
				: m_triangles(triangles), m_boundingBox(boundingBox), m_sourceMeshFilePath(srcFilePath)
		{
			m_initialBoundingBox= boundingBox;
		}

		const std::string& getSourceMeshFilePath() const;
		bool               trace(const Ray& rayIn, HitInfo& hitInfo) const override;
		virtual void       setPosition(const glm::vec3& position) override;
		virtual void       setScale(const glm::vec3& scale) override;

		void setMaterialIndex(uint32 materialIndex) override;

		void serialize(AssetManager& assetManager, YAML::Emitter& out) const override;
		void deserialize(AssetManager& assetManager, YAML::Node& node) override;

		private:
		bool intersectsBoundingBox(const Ray& rayIn) const;

		std::string                    m_sourceMeshFilePath;
		AABB                           m_boundingBox;
		AABB                           m_initialBoundingBox;
		std::vector<TriangleTraceable> m_triangles;
	};
} // namespace AstralRaytracer