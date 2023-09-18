#pragma once
#include "Raytracer/Material.h"
#include "Raytracer/TextureData.h"
#include "Raytracer/Traceable/Traceable.h"
#include "Serialization.h"

#include <filesystem>
#include <optional>
#include <unordered_map>
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

		[[nodiscard]] TextureData LoadTextureAsset(const fs::path& path, const std::string& name);
		[[nodiscard]] bool        LoadMaterialAsset(const fs::path& path, const std::string& name,
																								Material& outMaterial);
		[[nodiscard]] std::unique_ptr<Traceable> LoadTraceableAsset(const fs::path& path);
		void SaveMaterialAsset(const fs::path& folderPath, const std::string& name, const Material& material);
		void SaveMaterialAsset(const std::string& name, const Material& material);

		void SaveTraceableAsset(const std::string& name, const std::unique_ptr<Traceable>& traceable);

		[[nodiscard]] std::optional<NameAndPath> getNameAndPathOfTexture(uint32 id) const;
		[[nodiscard]] std::optional<NameAndPath> getNameAndPathOfMaterial(uint32 id) const;
		[[nodiscard]] std::optional<NameAndPath> getNameAndPathOfTraceable(uint32 id) const;

		private:
		std::unordered_map<uint32, NameAndPath> m_traceableNameAndPathMap;
		std::unordered_map<uint32, NameAndPath> m_materialNameAndPathMap;
		std::unordered_map<uint32, NameAndPath> m_textureNameAndPathMap;

		private:
		uuids::uuid generateUUID();
		std::string generateUUIDasString();

		std::mt19937 m_randomNumGenerator;
	};
} // namespace AstralRaytracer