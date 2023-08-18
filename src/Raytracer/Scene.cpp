#include "Raytracer/Scene.h"

namespace AstralRaytracer
{
	std::string Scene::defaultEmptyName= "";

	Scene::Scene() { m_materials.push_back(Material()); }

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

	const std::string& Scene::getTraceableName(uint32 traceableIndex)
	{
		auto it= m_traceableNameMap.find(traceableIndex);
		if(it != m_traceableNameMap.end())
		{
			return it->second;
		}

		return defaultEmptyName;
	}

	const std::string& Scene::getMaterialName(uint32 materialIndex)
	{
		auto it= m_materialNameMap.find(materialIndex);
		if(it != m_materialNameMap.end())
		{
			return it->second;
		}

		return defaultEmptyName;
	}

} // namespace AstralRaytracer