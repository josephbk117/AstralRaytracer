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
	class AssetManager
	{
		public:
		struct NameAndPath
		{
			std::string assetName;
			std::string relativePath;
		};

		[[nodiscard]] TextureData                LoadTextureAsset(const std::filesystem::path& path,
																															const std::string&           name);
		[[nodiscard]] Material                   LoadMaterialAsset(const std::filesystem::path& path,
																															 const std::string&           name);
		[[nodiscard]] std::unique_ptr<Traceable> LoadTraceableAsset(const std::filesystem::path& path,
																																const std::string&           name);
		[[nodiscard]] std::optional<NameAndPath> getNameAndPathOfTexture(uint32 id) const;
		[[nodiscard]] std::optional<NameAndPath> getNameAndPathOfMaterial(uint32 id) const;

		private:
		std::unordered_map<uint32, NameAndPath> m_traceableNameAndPathMap;
		std::unordered_map<uint32, NameAndPath> m_materialNameAndPathMap;
		std::unordered_map<uint32, NameAndPath> m_textureNameAndPathMap;
	};
} // namespace AstralRaytracer