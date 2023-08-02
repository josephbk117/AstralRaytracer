#include "Input.h"

GLFWwindow* AstralRaytracer::Input::glfwWindow= nullptr;

namespace AstralRaytracer
{
	void Input::initialize(const Window& window) { glfwWindow= window.glfwWindow; }

	glm::vec2 Input::getMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		return {xpos, ypos};
	}

	void Input::setCursorMode(CursorMode cursorMode)
	{
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, (int32)cursorMode);
	}

	bool Input::isMouseButtonDown(MouseButtonIndex mouseButtonIndex)
	{
		int32 newState= glfwGetMouseButton(glfwWindow, (int32)mouseButtonIndex);
		return newState == GLFW_PRESS;
	}

	bool Input::isKeyDown(InputKey inputKey)
	{
		int32 state= glfwGetKey(glfwWindow, (int32)inputKey);
		return state == GLFW_PRESS;
	}

	float64 Input::getTimeSinceStart() { return glfwGetTime(); }
} // namespace AstralRaytracer