#pragma once
#define GLFW_INCLUDE_NONE

#include "Raytracer/Material.h"
#include "Raytracer/Traceable/Traceable.h"
#include "Raytracer/Transform.h"

#include <GLFW/glfw3.h>
#include <filesystem>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//--PREVENT AUTO HEADER FILE ORDERING--
#include "FileDialog/ImGuiFileDialog.h"

//--PREVENT AUTO HEADER FILE ORDERING--

namespace AstralRaytracer
{
	namespace UI
	{
		namespace fs= std::filesystem;

		struct AppStateInfo
		{
				glm::u32vec2 rendererSize{ 500, 500 };
				glm::u32vec2 rendererResolution{ 500, 500 };
				ImTextureID  outputTextureId= 0;
				UiBounds     uiBounds;
				float32      resolutionScale       = 50.0f;
				bool         isSceneDirty          = false;
				bool         canSelectObjects      = false;
				uint32       selectedObjectIndex   = 0;
				bool         cameraUpdatedThisFrame= false;
		};

		ImVec2       toImVec2(const glm::vec2& vec2);
		ImVec2       toImVec2(const glm::u32vec2& vec2);
		glm::vec2    toVec2(const ImVec2& vec2);
		glm::u32vec2 toU32Vec2(const ImVec2& vec2);

		bool displayXYZFloat(glm::vec3& vec3, const std::string& label, float32 speed= 1.0f);

		bool displayTransform(Traceable& traceable);
	} // namespace UI
} // namespace AstralRaytracer