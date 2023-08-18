#include "Window.h"

namespace AstralRaytracer
{
	Window*            Window::m_instance= nullptr;
	const std::string& Window::getName() const { return m_name; }

	void Window::initialize()
	{
		assert(m_instance == nullptr);
		m_instance = this;

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
		glbinding::initialize(glfwGetProcAddress);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
		ImGui_ImplOpenGL3_Init("#version 420 core");

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
} // namespace AstralRaytracer