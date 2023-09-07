#include "Raytracer/Material.h"

namespace AstralRaytracer
{

	glm::vec3 Material::getEmission() const { return emission.getColour_32_bit() * emissionStrength; }

	void Material::serialize(YAML::Emitter& out)
	{
		using namespace Serialization;

		out << YAML::Key << "Albedo" << YAML::Value << albedo;
		out << YAML::Key << "Emission" << YAML::Value << emission;
		out << YAML::Key << "EmissionStrength" << YAML::Value << emissionStrength;
		out << YAML::Key << "Roughness" << YAML::Value << roughness;
		out << YAML::Key << "Texture" << YAML::Value << texture;
	}

	void Material::deserialize(YAML::Emitter& out) {}

} // namespace AstralRaytracer