#pragma once
#include "Utils/Common.h"

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

namespace AstralRaytracer
{
	class Window
	{
		public:
		friend class Input;

		Window(): m_name{"default"} {}
		Window(const std::string name): m_name{name} {}
		Window(const Window&)           = delete;
		Window& operator=(const Window&)= delete;
		~Window(){};
		const std::string& getName() const;
		bool               shouldWindowClose() const;

		void initialize();
		void shutdown();
		void swapBuffers();
		void pollEvents();
		void startUI();
		void endUI();

		const std::pair<uint32, uint32>& getResolution() const { return m_resolution; };

		private:
		static Window* m_instance;

		GLFWwindow*               glfwWindow= nullptr;
		std::string               m_name;
		std::pair<uint32, uint32> m_resolution{0, 0};

		static void windowSizeCallback(GLFWwindow* window, int32 width, int32 height);
	};
} // namespace AstralRaytracer