#include "Raytracer/Scene.h"

#include <fstream>

namespace AstralRaytracer
{
	std::string Scene::defaultEmptyName= "";

	Scene::Scene()
	{
		addMaterial(Material(), "Default Material");
		TextureData defaultTexData(1, 1, 3);
		defaultTexData.setTexelColorAtPixelIndex(0, glm::u8vec3(255, 255, 255));
		addTexture(std::move(defaultTexData), "Default Texture");
	}

	void Scene::addTraceable(std::unique_ptr<Traceable>&& traceable, const std::string& name)
	{
		m_sceneTraceables.push_back(std::move(traceable));
		m_traceableNameMap.insert({m_sceneTraceables.size() - 1, name});
	}

	void Scene::addMaterial(const Material& material, const std::string& name)
	{
		m_materials.push_back(material);
		m_materialNameMap.insert({m_materials.size() - 1, name});
	}

	void Scene::addTexture(TextureData&& texture, const std::string& name)
	{
		m_textures.push_back(std::move(texture));
		m_textureNameMap.insert({m_textures.size() - 1, name});
	}

	void Scene::serialize(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "New";

		// Textures
		out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;

		out << YAML::BeginMap;
		for(uint32 textureIndex= 0; textureIndex < m_textures.size(); ++textureIndex)
		{
			out << YAML::Key << textureIndex << YAML::Value << m_textureNameMap[textureIndex];
		}
		out << YAML::EndMap << YAML::EndSeq;

		// Materials
		out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;

		out << YAML::BeginMap;
		for(uint32 matIndex= 0; matIndex < m_materials.size(); ++matIndex)
		{
			out << YAML::Key << matIndex << YAML::Value << m_materialNameMap[matIndex];
		}
		out << YAML::EndMap << YAML::EndSeq;

		// Trace-ables
		out << YAML::Key << "Traceables" << YAML::Value << YAML::BeginSeq;

		out << YAML::BeginMap;
		for(uint32 traceableIndex= 0; traceableIndex < m_sceneTraceables.size(); ++traceableIndex)
		{
			out << YAML::Key << traceableIndex << YAML::Value << m_traceableNameMap[traceableIndex];
		}
		out << YAML::EndMap << YAML::EndSeq;

		const std::filesystem::path outputPath= std::filesystem::current_path().string() + path.string();

		std::filesystem::create_directory(outputPath.parent_path());

		std::ofstream ofs(outputPath);
		ofs << out.c_str();
		ofs.close();
	}

	const std::string& Scene::getTraceableName(uint32 traceableIndex) const
	{
		auto it= m_traceableNameMap.find(traceableIndex);
		if(it != m_traceableNameMap.end())
		{
			return it->second;
		}

		return defaultEmptyName;
	}

	const std::string& Scene::getMaterialName(uint32 materialIndex) const
	{
		auto it= m_materialNameMap.find(materialIndex);
		if(it != m_materialNameMap.end())
		{
			return it->second;
		}

		return defaultEmptyName;
	}

} // namespace AstralRaytracer