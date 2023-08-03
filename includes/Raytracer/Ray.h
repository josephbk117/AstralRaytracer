#pragma once
#include "ColourData.h"
#include <limits>

#include <glm.hpp>

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

struct HitInfo
{
	float32    hitDistance= std::numeric_limits<float32>::max();
	Ray        rayOut;
	ColourData colorData;
};