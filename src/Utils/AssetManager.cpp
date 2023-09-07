#include "Utils/AssetManager.h"

#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
namespace AstralRaytracer
{

	TextureData AssetManager::LoadTextureAsset(const std::filesystem::path& path,
																						 const std::string&           name)
	{
		static uint32 textureCount= 0;
		NameAndPath   nameAndPath = {name, path.string()};
		m_textureNameAndPathMap.emplace(textureCount, nameAndPath);
		textureCount++;
		return TextureManager::loadTextureDataFromFile(path);
	}

	Material AssetManager::LoadMaterialAsset(const std::filesystem::path& path,
																					 const std::string&           name)
	{
		static uint32 matCount   = 0;
		NameAndPath   nameAndPath= {name, path.string()};
		m_materialNameAndPathMap.emplace(matCount, nameAndPath);
		matCount++;
		return Material();
	}

	std::unique_ptr<AstralRaytracer::Traceable>
	AssetManager::LoadTraceableAsset(const std::filesystem::path& path, const std::string& name)
	{
		static uint32 traceableCount= 0;
		NameAndPath   nameAndPath   = {name, path.string()};
		m_traceableNameAndPathMap.emplace(traceableCount, nameAndPath);
		traceableCount++;
		return std::make_unique<AstralRaytracer::SphereTraceable>();
	}

	std::optional<AssetManager::NameAndPath> AssetManager::getNameAndPathOfTexture(uint32 id) const
	{
		auto it= m_textureNameAndPathMap.find(id);
		if(it != m_textureNameAndPathMap.end())
		{
			return it->second;
		}

		return std::nullopt;
	}

	std::optional<AssetManager::NameAndPath> AssetManager::getNameAndPathOfMaterial(uint32 id) const
	{
		auto it= m_materialNameAndPathMap.find(id);
		if(it != m_materialNameAndPathMap.end())
		{
			return it->second;
		}

		return std::nullopt;
	}

} // namespace AstralRaytracer