#pragma once
#include "Common.h"

namespace AstralRaytracer
{
	namespace Random
	{
		static uint32 pcgHash(uint32 input)
		{
			uint32 state= input * 747796405u + 2891336453u;
			uint32 word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
			return (word >> 22u) ^ word;
		}

		static float32 randomFloat(uint32& seed)
		{
			constexpr float32 oneOverFloatLimit= 1.0f / std::numeric_limits<uint32>::max();

			seed= pcgHash(seed);
			return (float32)seed * oneOverFloatLimit;
		}

		static glm::vec3 unitSphere(uint32& seed)
		{
			return glm::normalize(glm::vec3(
					randomFloat(seed) * 2.0f - 1.0f, randomFloat(seed) * 2.0f - 1.0f,
					randomFloat(seed) * 2.0f - 1.0f
			));
		}

		static glm::vec3 unitHemiSphere(uint32& seed, const glm::vec3& normal)
		{
			glm::vec3 on_unit_sphere= unitSphere(seed);
			if(glm::dot(on_unit_sphere, normal) > 0.0f) // In the same hemisphere as the normal
			{
				return on_unit_sphere;
			}
			else
			{
				return -on_unit_sphere;
			}
		}
	} // namespace Random
} // namespace AstralRaytracer