#pragma once
#include "ColourData.h"

#include <glm.hpp>

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

struct HitInfo
{
	Ray        rayOut;
	ColourData colorData;
};