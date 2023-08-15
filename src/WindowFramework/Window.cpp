#include "Window.h"

namespace AstralRaytracer
{
	const std::string& Window::getName() const { return m_name; }

	void Window::initialize()
	{
		int width= 300, height= 300;

		if(!glfwInit())
		{
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindow= glfwCreateWindow(width, height, m_name.c_str(), nullptr, nullptr);

		glfwSetWindowSizeCallback(glfwWindow, windowSizeCallback);

		if(!glfwWindow)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(glfwWindow);
		glbinding::initialize(glfwGetProcAddress);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
		ImGui_ImplOpenGL3_Init("#version 460 core");

		gl::glViewport(0, 0, width, height);
	}

	void Window::windowSizeCallback(GLFWwindow* window, int32 width, int32 height)
	{
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
} // namespace AstralRaytracer