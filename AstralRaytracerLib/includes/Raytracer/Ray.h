#pragma once
#include "Utils/Common.h"

struct Ray
{
		glm::vec3 worldSpacePosition;
		glm::vec3 direction;
};

struct HitInfo
{
		float32   hitDistance  = std::numeric_limits<float32>::max();
		uint32    objectIndex  = 0;
		uint32    materialIndex= 0;
		glm::vec3 worldSpaceNormal;
		glm::vec3 worldSpacePosition;

		bool isValid() const
		{
			return hitDistance > 0.0f && hitDistance < std::numeric_limits<float32>::max();
		}
};