#include "Raytracer/Material.h"

namespace AstralRaytracer
{

	glm::vec3 Material::getEmission() const { return emission.getColour_32_bit() * emissionStrength; }

	void Material::serialize(YAML::Emitter& out) const
	{
		using namespace Serialization;

		out << YAML::Key << "Albedo" << YAML::Value << albedo;
		out << YAML::Key << "Emission" << YAML::Value << emission;
		out << YAML::Key << "EmissionStrength" << YAML::Value << emissionStrength;
		out << YAML::Key << "Roughness" << YAML::Value << roughness;
		out << YAML::Key << "Texture" << YAML::Value << texture;
	}

	void Material::deserialize(YAML::Node& node)
	{
		m_id            = uuids::uuid::from_string(node["UUID"].as<std::string>()).value();
		albedo          = node["Albedo"].as<ColourData>();
		emission        = node["Emission"].as<ColourData>();
		emissionStrength= node["EmissionStrength"].as<float32>();
		roughness       = node["Roughness"].as<float32>();
		texture         = node["Texture"].as<uint32>();
	}

} // namespace AstralRaytracer