#pragma once
#include "Raytracer/Material.h"
#include "Raytracer/Traceable/Traceable.h"
#include "Raytracer/Transform.h"
#include "WindowFramework/Window.h"

#include <filesystem>

namespace AstralRaytracer
{
	namespace UI
	{
		namespace fs = std::filesystem;

		bool displayXYZFloat(glm::vec3& vec3, const std::string& label);

		bool displayTransform(Traceable& traceable);
	} // namespace UI
} // namespace AstralRaytracer