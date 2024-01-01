#pragma once
#include "Raytracer/Material.h"
#include "Raytracer/Traceable/Traceable.h"
#include "Serialization.h"
#include "Utils/TextureData.h"

#include <filesystem>
#include <optional>
#include <unordered_map>
#include <uuid.h>

namespace AstralRaytracer
{
	namespace fs= std::filesystem;

	class AssetManager
	{
		public:
			struct NameAndPath
			{
					std::string assetName;
					std::string relativePath;
			};

			AssetManager();

			bool loadProject(const fs::path& absolutePath);
			void clearAndResetCachedData();

			[[nodiscard]]
			TextureDataRGBF loadTextureAsset(const fs::path& path, const std::string& name);
			[[nodiscard]]
			bool loadMaterialAsset(const fs::path& path, const std::string& name, Material& outMaterial);
			[[nodiscard]]
			std::unique_ptr<Traceable> loadTraceableAsset(const fs::path& path);
			void                       saveMaterialAsset(
																const fs::path&    folderPath,
																const std::string& name,
																const Material&    material
														);
			void saveMaterialAsset(const std::string& name, const Material& material);

			void SaveTraceableAsset(const std::string& name, const std::unique_ptr<Traceable>& traceable);

			[[nodiscard]]
			std::optional<NameAndPath> getNameAndPathOfTexture(uint32 id) const;
			[[nodiscard]]
			std::optional<NameAndPath> getNameAndPathOfMaterial(uint32 id) const;
			[[nodiscard]]
			std::optional<NameAndPath> getNameAndPathOfTraceable(uint32 id) const;

			const std::string& getCurrentRelativePath() const { return m_currentRelativePath; }

			const std::string& getDefaultSceneRelativePath() const { return m_defaultScenePath; }

			std::string getDefaultSceneAbsolutePath() const;

			const std::string& getCurrentProjectName() const { return m_currentProjectName; }

			[[nodiscard]]
			bool isProjectOpen() const;
		private:
			uint32 textureCount  = 0;
			uint32 matCount      = 1;
			uint32 traceableCount= 0;

			std::unordered_map<uint32, NameAndPath> m_traceableNameAndPathMap;
			std::unordered_map<uint32, NameAndPath> m_materialNameAndPathMap;
			std::unordered_map<uint32, NameAndPath> m_textureNameAndPathMap;
			std::mt19937                            m_randomNumGenerator;

			std::string m_currentProjectName;
			std::string m_currentRelativePath= "../../../../";
			std::string m_defaultScenePath;

			uuids::uuid generateUUID();
			std::string generateUUIDasString();
	};
} // namespace AstralRaytracer