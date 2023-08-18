#pragma once
#include "Material.h"
#include "Traceable/Traceable.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace AstralRaytracer
{
	class Scene
	{
		public:
		Scene();

		void addTraceable(std::unique_ptr<Traceable>&& traceable, const std::string& name);
		void addMaterial(const Material& material, const std::string& name);
		const std::string& getTraceableName(uint32 traceableIndex);
		const std::string& getMaterialName(uint32 materialIndex);

		std::vector<std::unique_ptr<Traceable>> m_sceneTraceables;
		std::vector<Material>                   m_materials;

		private:
		std::unordered_map<uint32, std::string> m_traceableNameMap;
		std::unordered_map<uint32, std::string> m_materialNameMap;

		static std::string defaultEmptyName;
	};
} // namespace AstralRaytracer