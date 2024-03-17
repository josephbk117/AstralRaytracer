#pragma once
#include "TriangleTraceable.h"
#include "Utils/Common.h"

#include <filesystem>
#include <vector>

namespace AstralRaytracer
{
	class StaticMesh: public Traceable
	{
		public:
			StaticMesh()= default;
			StaticMesh(const std::filesystem::path& meshFilePath);

			StaticMesh(const std::vector<TriangleTraceable>& triangles, const AABB& boundingBox)
					: m_triangles(triangles), Traceable(boundingBox)
			{
			}

			StaticMesh(
					const std::vector<TriangleTraceable>& triangles,
					const AABB&                           boundingBox,
					const std::string&                    srcFilePath
			)
					: m_triangles(triangles), Traceable(boundingBox), m_sourceMeshFilePath(srcFilePath)
			{
			}

			[[nodiscard]]
			const std::string& getSourceMeshFilePath() const;
			[[nodiscard]]
			bool         trace(const Ray& rayIn, HitInfo& hitInfo) const override;
			virtual void setPosition(const glm::vec3& position) override;
			virtual void setScale(const glm::vec3& scale) override;

			void setMaterialIndex(uint32 materialIndex) override;

			void serialize(AssetManager& assetManager, YAML::Emitter& out) const override;
			void deserialize(AssetManager& assetManager, YAML::Node& node) override;
		private:
			[[nodiscard]]
			bool intersectsBoundingBox(const Ray& rayIn) const;

			std::string                    m_sourceMeshFilePath;
			std::vector<TriangleTraceable> m_triangles;
	};
} // namespace AstralRaytracer