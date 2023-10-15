#pragma once
#include "Material.h"
#include "Utils/TextureData.h"
#include "Utils/AssetManager.h"

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace AstralRaytracer
{
	class Traceable;
	class PostProcessing;
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	class Scene
	{
		public:
			Scene();
			~Scene();

			[[nodiscard]]
			bool hasSceneLoaded() const;
			void addTraceable(std::unique_ptr<Traceable>&& traceable);
			void addMaterial(const Material& material);
			void addTexture(TextureDataRGBF&& texture);
			void addPostProcessing(std::unique_ptr<PostProcessing>&& postProcessing);
			void serialize(const AssetManager& assetManager, const std::filesystem::path& path);
			void deserialize(AssetManager& assetManager, const std::filesystem::path& path);
			void unload();

			std::vector<std::unique_ptr<Traceable>>      m_sceneTraceables;
			std::vector<Material>                        m_materials;
			std::vector<TextureDataRGBF>                 m_textures;
			std::vector<std::unique_ptr<PostProcessing>> m_postProcessingStack;
	};
} // namespace AstralRaytracer