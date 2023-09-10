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

		void addTraceable(std::unique_ptr<Traceable>&& traceable, const std::string& name);
		void addMaterial(const Material& material, const std::string& name);
		void addTexture(TextureData&& texture);
		void serialize(const AssetManager& assetManager, const std::filesystem::path& path);
		void deserialize(AssetManager& assetManager, const std::filesystem::path& path);
		const std::string& getTraceableName(uint32 traceableIndex) const;
		const std::string& getMaterialName(uint32 materialIndex) const;

		std::vector<std::unique_ptr<Traceable>> m_sceneTraceables;
		std::vector<Material>                   m_materials;
		std::vector<TextureData>                m_textures;

		private:
		std::unordered_map<uint32, std::string> m_traceableNameMap;
		std::unordered_map<uint32, std::string> m_materialNameMap;

		static std::string defaultEmptyName;
	};
} // namespace AstralRaytracer