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

		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec4)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << vec4.x << vec4.y << vec4.z << vec4.w << YAML::EndSeq;
			return out;
		}

		YAML::Emitter& operator<<(YAML::Emitter& out, const ColourData& colourData)
		{
			out << colourData.getColour_32_bit();
			return out;
		}

		YAML::Emitter& operator<<(YAML::Emitter& out, const Transform& transform)
		{
			const glm::vec3& pos  = transform.getPosition();
			const float32    rot  = transform.getRotation();
			const glm::vec3& scale= transform.getScale();

			out << YAML::Flow;
			out << YAML::BeginSeq << pos.x << pos.y << pos.z << rot << scale.x << scale.y << scale.z
					<< YAML::EndSeq;
			return out;
		}

		const uuids::uuid& Serializable::getUUID() const { return m_id; }

	} // namespace Serialization
} // namespace AstralRaytracer