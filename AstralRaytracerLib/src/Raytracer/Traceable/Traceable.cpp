#include "Raytracer/Traceable/Traceable.h"

namespace AstralRaytracer
{
	Traceable::Traceable(const AABB& boundingBox): m_initialBoundingBox(boundingBox), m_boundingBox(boundingBox) { }

	const glm::vec3& Traceable::getPosition() const { return m_transform.getPosition(); }

	float32 Traceable::getRotation() const { return m_transform.getRotation(); }

	const glm::vec3& Traceable::getScale() const { return m_transform.getScale(); }

	const glm::mat4& Traceable::getTransformMatrix() const { return m_transform.getMatrix(); }

	Transform& Traceable::getTransform() { return m_transform; }

	uint32 Traceable::getMaterialIndex() const { return m_materialIndex; }

	void Traceable::serialize(AssetManager& assetManager, YAML::Emitter& out) const
	{
		using namespace Serialization;
		out << YAML::Key << "Transform" << YAML::Value << m_transform;
		out << YAML::Key << "Material" << YAML::Value << m_materialIndex;
	}

	void Traceable::deserialize(AssetManager& assetManager, YAML::Node& node)
	{
		m_transform    = node["Transform"].as<Transform>();
		m_materialIndex= node["Material"].as<uint32>();
	}
} // namespace AstralRaytracer