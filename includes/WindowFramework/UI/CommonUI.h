#pragma once
#include "Raytracer/Traceable/Traceable.h"
#include "Raytracer/Transform.h"
#include "WindowFramework/Window.h"
namespace AstralRaytracer
{
	namespace UI
	{
		bool displayXYZFloat(glm::vec3& vec3, const std::string& label);

		bool displayTransform(Traceable& traceable);
	} // namespace UI
} // namespace AstralRaytracer