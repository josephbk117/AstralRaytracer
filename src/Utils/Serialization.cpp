#include "Utils/Serialization.h"

namespace AstralRaytracer
{
	namespace Serialization
	{
		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec3)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << vec3.x << vec3.y << vec3.z << YAML::EndSeq;
			return out;
		}

		YAML::Emitter& operator<<(YAML::Emitter& out, const ColourData& colourData)
		{
			out << colourData.getColour_32_bit();
			return out;
		}

		YAML::Emitter& operator<<(YAML::Emitter& out, const Transform& transform)
		{
			out << transform.getPosition() << transform.getRotation() << transform.getScale();
			return out;
		}

	} // namespace Serialization
} // namespace AstralRaytracer