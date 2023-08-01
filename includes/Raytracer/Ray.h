#pragma once
#include <glm.hpp>
#include "ColourData.h"

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

struct HitInfo
{
	Ray rayOut;
	ColourData colorData;
};