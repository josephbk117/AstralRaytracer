#pragma once
#include "Common.h"
#include "Raytracer/ColourData.h"

#include <yaml-cpp/yaml.h>
namespace AstralRaytracer
{
	namespace Serialization
	{
		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec3);
		YAML::Emitter& operator<<(YAML::Emitter& out, const ColourData& colourData);

		class Serializable
		{
			public:
			Serializable()                              = default;
			virtual void serialize(YAML::Emitter& out)  = 0;
			virtual void deserialize(YAML::Emitter& out)= 0;
		};
	} // namespace Serialization
} // namespace AstralRaytracer