#include <iostream>
#include <cstring>

#define GLFW_INCLUDE_NONE

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"
#include "Raytracer/TextureData.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/DrawingPanel.h"
#include "Raytracer/ShaderProgram.h"

void windowSizeCallback(GLFWwindow* window, int width, int height);

int main()
{
    std::cout<<"Hello World!";
    Window window("new window test");

	glm::vec2(0.1f, 0.5f);

    std::cout<<"\n window name :"<<window.getName()<<"\nMax window count "<<WindowUtils::GetMaxWindowsAllowed();

    GLFWwindow* glfwWindow = nullptr;
    int width = 300, height = 300;

    if(!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindow = glfwCreateWindow(width, height, "Custom Window", nullptr, nullptr);

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
	ImGui_ImplOpenGL3_Init("#version 330 core");

    gl::glViewport(0,0,width,height);

	//----------------

	const int NX = 380;
	const int NY = 190;
	const int NS = 40;

	unsigned char *data = new unsigned char[NX * NY * 3];

	std::memset(data, 100, NX * NY);

	{ // Destructors need to be called before context is removed
		TextureData texData;
		texData.setTextureData(data, NX, NY, 3);
		unsigned int textureId = TextureManager::loadTextureFromData(texData, false);

		DrawingPanel drawingPanel;
		drawingPanel.init(1, 1);
		drawingPanel.setTextureID(textureId);
		drawingPanel.getTransform()->setPosition(0, 0);
		drawingPanel.getTransform()->setScale(glm::vec2(1, 1));
		drawingPanel.getTransform()->update();

		ShaderProgram shader;
		shader.compileShaders("resources/simpleDisplay.vs", "resources/simpleDisplay.fs");
		shader.linkShaders();
		shader.use();

		int drawingPanelModelMatrix = shader.getUniformLocation("model");

		//----------------

		while(!glfwWindowShouldClose(glfwWindow))
		{
			gl::glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			gl::glClear(gl::GL_COLOR_BUFFER_BIT);

			shader.use();
			shader.applyShaderUniformMatrix(drawingPanelModelMatrix, drawingPanel.getTransform()->getMatrix());
			drawingPanel.draw();

			gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
			gl::glTexSubImage2D(gl::GL_TEXTURE_2D, 0, 0, 0, NX, NY, TextureManager::getTextureFormatFromData(3), gl::GL_UNSIGNED_BYTE, data);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			bool isOpen = true;
			ImGui::ShowMetricsWindow(&isOpen);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(glfwWindow);
			glfwPollEvents();
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	gl::glViewport(0, 0, width, height);
}