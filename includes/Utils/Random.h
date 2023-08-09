#pragma once
#include "Common.h"

namespace AstralRaytracer
{
	static uint32 pcgHash(uint32 input)
	{
		uint state= input * 747796405u + 2891336453u;
		uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float32 randomFloat(uint32& seed)
	{
		seed= pcgHash(seed);
		return seed / (float32)std::numeric_limits<uint32>()::max();
	}

	static glm::vec3 unitSphere(uint32& seed)
	{
		return glm::vec3(randomFloat(seed) * 2.0f - 1.0f, randomFloat(seed) * 2.0f - 1.0f, randomFloat(seed) *  2.0f - 1.0f);
	}

} // namespace AstralRaytracer