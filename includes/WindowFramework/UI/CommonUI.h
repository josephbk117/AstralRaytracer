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

		struct AppStateInfo
		{
			glm::u32vec2 rendererSize{500, 500};
			glm::u32vec2 rendererResolution{500, 500};
			UiBounds     uiBounds;
			float32      resolutionScale    = 50.0f;
			bool         isSceneDirty       = false;
			bool         canSelectObjects   = false;
			uint32       selectedObjectIndex= 0;
		};

		bool displayXYZFloat(glm::vec3& vec3, const std::string& label);

		bool displayTransform(Traceable& traceable);
	} // namespace UI
} // namespace AstralRaytracer