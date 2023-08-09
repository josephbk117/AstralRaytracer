#pragma once
#include "ColourData.h"

#include <glm.hpp>
#include <limits>

struct Ray
{
	glm::vec3 worldSpacePosition;
	glm::vec3 direction;
};

struct HitInfo
{
	float32   hitDistance  = std::numeric_limits<float32>::max();
	uint32    materialIndex= 0;
	glm::vec3 worldSpaceNormal;
	glm::vec3 worldSpacePosition;
};