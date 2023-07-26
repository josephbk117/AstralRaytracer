#include <iostream>
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"
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

    glfwWindow = glfwCreateWindow(width, height, "Custom Window", nullptr, nullptr);

    if(!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    while(!glfwWindowShouldClose(glfwWindow))
    {
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}