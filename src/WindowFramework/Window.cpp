#include "Window.h"

#include "Raytracer/TextureManager.h"

#include <iostream>

namespace AstralRaytracer
{
	Window*            Window::m_instance= nullptr;
	const std::string& Window::getName() const { return m_name; }

	void Window::initialize()
	{
		assert(m_instance == nullptr);
		m_instance= this;

		m_resolution= {500, 500};

		if(!glfwInit())
		{
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindow=
				glfwCreateWindow(m_resolution.first, m_resolution.second, m_name.c_str(), nullptr, nullptr);

		if(!glfwWindow)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetWindowSizeCallback(glfwWindow, windowSizeCallback);
		glfwMakeContextCurrent(glfwWindow);

		TextureData iconTexData=
				TextureManager::loadTextureDataFromFile("resources/icons/astralraytracer.png", 4);

		GLFWimage image[1];
		image[0].width = iconTexData.getWidth();
		image[0].height= iconTexData.getHeight();
		image[0].pixels= const_cast<uint8*>(iconTexData.getTextureData().data());
		glfwSetWindowIcon(glfwWindow, 1, image);

		glbinding::initialize(glfwGetProcAddress);

		gl::glEnable(gl::GL_DEBUG_OUTPUT);
		gl::glEnable(gl::GL_DEBUG_OUTPUT_SYNCHRONOUS);
		gl::glDebugMessageCallback(debugCallback, nullptr);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
		ImGui_ImplOpenGL3_Init("#version 420 core");

		ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/fonts/ABeeZee-Regular.ttf", 16.0f);

		gl::glViewport(0, 0, m_resolution.first, m_resolution.second);
	}

	void Window::windowSizeCallback(GLFWwindow* window, int32 width, int32 height)
	{
		m_instance->m_resolution.first = width;
		m_instance->m_resolution.second= height;
		gl::glViewport(0, 0, width, height);
	}

	bool Window::shouldWindowClose() const { return glfwWindowShouldClose(glfwWindow); }

	void Window::shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwTerminate();
	}

	void Window::swapBuffers() { glfwSwapBuffers(glfwWindow); }

	void Window::pollEvents() { glfwPollEvents(); }

	void Window::startUI()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Window::endUI()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GL_APIENTRY Window::debugCallback(gl::GLenum source, gl::GLenum type, gl::GLuint id,
																				 gl::GLenum severity, gl::GLsizei length,
																				 const gl::GLchar* message, const void* userParam)
	{
		std::cout << "\n---------------------opengl-callback-start------------" << std::endl;
		std::cout << "message: " << message << std::endl;
		std::cout << "type: ";
		switch(type)
		{
			case gl::GL_DEBUG_TYPE_ERROR: std::cout << "ERROR"; break;
			case gl::GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "DEPRECATED_BEHAVIOR"; break;
			case gl::GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "UNDEFINED_BEHAVIOR"; break;
			case gl::GL_DEBUG_TYPE_PORTABILITY: std::cout << "PORTABILITY"; break;
			case gl::GL_DEBUG_TYPE_PERFORMANCE: std::cout << "PERFORMANCE"; break;
			case gl::GL_DEBUG_TYPE_OTHER: std::cout << "OTHER"; break;
		}
		std::cout << std::endl;

		std::cout << "id: " << id << std::endl;
		std::cout << "severity: ";
		switch(severity)
		{
			case gl::GL_DEBUG_SEVERITY_LOW: std::cout << "LOW"; break;
			case gl::GL_DEBUG_SEVERITY_MEDIUM: std::cout << "MEDIUM"; break;
			case gl::GL_DEBUG_SEVERITY_HIGH: std::cout << "HIGH"; break;
		}
		std::cout << std::endl;
		std::cout << "---------------------opengl-callback-end--------------" << std::endl;
	}
} // namespace AstralRaytracer