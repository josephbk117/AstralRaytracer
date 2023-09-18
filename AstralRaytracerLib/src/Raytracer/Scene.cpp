#include "Raytracer/Scene.h"

#include <fstream>
#include <iostream>

namespace AstralRaytracer
{
	std::string Scene::defaultEmptyName= "";

	Scene::Scene()
	{
		addMaterial(Material());
		TextureData defaultTexData(1, 1, 3);
		defaultTexData.setTexelColorAtPixelIndex(0, glm::u8vec3(255, 255, 255));
		addTexture(std::move(defaultTexData));
	}

	bool Scene::hasSceneLoaded() const { return m_sceneTraceables.size() > 0; }

	void Scene::addTraceable(std::unique_ptr<Traceable>&& traceable)
	{
		m_sceneTraceables.push_back(std::move(traceable));
	}

	void Scene::addMaterial(const Material& material) { m_materials.push_back(material); }

	void Scene::addTexture(TextureData&& texture) { m_textures.push_back(std::move(texture)); }

	void Scene::serialize(const AssetManager& assetManager, const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "New";

		// Textures
		out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;

		for(uint32 textureIndex= 0; textureIndex < m_textures.size(); ++textureIndex)
		{
			std::optional<AssetManager::NameAndPath> nameAndPath=
					assetManager.getNameAndPathOfTexture(textureIndex);
			if(nameAndPath.has_value())
			{
				out << YAML::BeginMap;

				out << YAML::Key << nameAndPath.value().assetName << YAML::Value
						<< nameAndPath.value().relativePath;
				out << YAML::EndMap;
			}
		}
		out << YAML::EndSeq;

		// Materials
		out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;

		for(uint32 matIndex= 0; matIndex < m_materials.size(); ++matIndex)
		{
			std::optional<AssetManager::NameAndPath> nameAndPath=
					assetManager.getNameAndPathOfMaterial(matIndex);
			if(nameAndPath.has_value())
			{
				out << YAML::BeginMap;

				out << YAML::Key << nameAndPath.value().assetName << YAML::Value
						<< nameAndPath.value().relativePath;
				out << YAML::EndMap;
			}
		}

		out << YAML::EndSeq;

		// Trace-ables
		out << YAML::Key << "Traceables" << YAML::Value << YAML::BeginSeq;

		for(uint32 traceableIndex= 0; traceableIndex < m_sceneTraceables.size(); ++traceableIndex)
		{
			std::optional<AssetManager::NameAndPath> nameAndPath=
					assetManager.getNameAndPathOfTraceable(traceableIndex);
			if(nameAndPath.has_value())
			{
				out << YAML::BeginMap;

				out << YAML::Key << nameAndPath.value().assetName << YAML::Value
						<< nameAndPath.value().relativePath;

				out << YAML::EndMap;
			}
		}
		out << YAML::EndSeq;

		const std::filesystem::path outputPath=
				std::filesystem::current_path().string() + path.string();

		std::filesystem::create_directory(outputPath.parent_path());

		std::ofstream ofs(outputPath);
		ofs << out.c_str();
		ofs.close();
	}

	void Scene::deserialize(AssetManager& assetManager, const std::filesystem::path& path)
	{
		std::ifstream stream(path);
		YAML::Node    data= YAML::Load(stream);

		if(!data["Scene"])
		{
			return;
		}

		std::cout << "Scene " << data["Scene"].as<std::string>();

		const auto& textures= data["Textures"];

		std::cout << "\nNumber of textures " << textures.size();

		for(uint32 texIndex= 0; texIndex < textures.size(); ++texIndex)
		{
			const auto& tex= textures[texIndex];
			for(auto magic: tex)
			{
				addTexture(assetManager.LoadTextureAsset(magic.second.as<std::string>(),
																								 magic.first.as<std::string>()));
				std::cout << "\n Texture at index : " << texIndex << " : " << magic.first.as<std::string>();
			}
		}

		const auto& materials= data["Materials"];
		std::cout << "\nNumber of materials " << materials.size();

		for(uint32 matIndex= 0; matIndex < materials.size(); ++matIndex)
		{
			const auto& mat= materials[matIndex];
			for(auto magic: mat)
			{
				Material materialNew;
				assetManager.LoadMaterialAsset(magic.second.as<std::string>(),
																			 magic.first.as<std::string>(), materialNew);
				addMaterial(materialNew);

				std::cout << "\n Material at index : " << matIndex << " : "
									<< magic.first.as<std::string>();
			}
		}

		const auto& traceables= data["Traceables"];
		std::cout << "\nNumber of traceables " << traceables.size();

		for(uint32 traceIndex= 0; traceIndex < traceables.size(); ++traceIndex)
		{
			const auto& traceable= traceables[traceIndex];
			for(auto magic: traceable)
			{
				addTraceable(assetManager.LoadTraceableAsset(magic.second.as<std::string>()));
			}
		}
	}

} // namespace AstralRaytracer