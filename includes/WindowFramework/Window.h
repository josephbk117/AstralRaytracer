#pragma once
#include "Utils/Common.h"

#define GLFW_INCLUDE_NONE

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include <string>



namespace AstralRaytracer
{
    class Window
    {
        public:
        friend class Input;

        Window():m_name{"default"}{}
        Window(const std::string name):m_name{name}{}
        const std::string& getName() const;
        bool shouldWindowClose() const;

        void initialize();
        void shutdown();
        void swapBuffers();
        void pollEvents();
        void startUI();
        void endUI();

        private:
        GLFWwindow* glfwWindow = nullptr;
        std::string m_name;

	    static void windowSizeCallback(GLFWwindow* window, int32 width, int32 height);
    };
}