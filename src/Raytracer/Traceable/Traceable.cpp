#include "Raytracer/Traceable/Traceable.h"

namespace AstralRaytracer
{
	void Traceable::serialize(YAML::Emitter& out) const
	{
		using namespace Serialization;
		out << YAML::Key << "Transform" << YAML::Value << m_transform;
		out << YAML::Key << "Material" << YAML::Value << m_materialIndex;
	}

	void Traceable::deserialize(YAML::Node& node)
	{
		m_transform    = node["Transform"].as<Transform>();
		m_materialIndex= node["Material"].as<uint32>();
	}
} // namespace AstralRaytracer