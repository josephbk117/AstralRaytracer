#include "Raytracer/Scene.h"

namespace AstralRaytracer
{
	// const AstralRaytracer::Traceable* const Scene::getTraceableInScene(uint32 index)
	//{
	//	return m_sceneTraceables.at(index).get();
	// }

	Scene::Scene() { m_materials.push_back(Material()); }

} // namespace AstralRaytracer