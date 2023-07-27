#include <iostream>
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"
#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include "GLFW/glfw3.h"


int main()
{
    std::cout<<"Hello World!";
    Window window("new window test");

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

    if(!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfwWindow);
    glbinding::initialize(glfwGetProcAddress);

    gl::glViewport(0,0,width,height);

    while(!glfwWindowShouldClose(glfwWindow))
    {
		gl::glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		gl::glClear(gl::GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}