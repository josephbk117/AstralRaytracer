#pragma once
#include "Material.h"
#include "TextureData.h"
#include "Traceable/Traceable.h"
#include "Utils/AssetManager.h"

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace AstralRaytracer
{
	class Scene
	{
		public:
		Scene();

		[[nodiscard]] bool hasSceneLoaded() const;
		void addTraceable(std::unique_ptr<Traceable>&& traceable);
		void addMaterial(const Material& material);
		void addTexture(TextureData&& texture);
		void serialize(const AssetManager& assetManager, const std::filesystem::path& path);
		void deserialize(AssetManager& assetManager, const std::filesystem::path& path);

		std::vector<std::unique_ptr<Traceable>> m_sceneTraceables;
		std::vector<Material>                   m_materials;
		std::vector<TextureData>                m_textures;

		private:
		static std::string defaultEmptyName;
	};
} // namespace AstralRaytracer