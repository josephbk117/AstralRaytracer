#include "WindowFramework/Input.h"

GLFWwindow* AstralRaytracer::Input::glfwWindow= nullptr;

namespace AstralRaytracer
{
	void Input::initialize(const Window& window) { glfwWindow= window.m_glfwWindow; }

	glm::vec2 Input::getMousePosition()
	{
		double xPos;
		double yPos;
		glfwGetCursorPos(glfwWindow, &xPos, &yPos);
		return {xPos, yPos};
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

	bool Input::isMouseButtonUp(MouseButtonIndex mouseButtonIndex)
	{
		int32 newState= glfwGetMouseButton(glfwWindow, (int32)mouseButtonIndex);
		return newState == GLFW_RELEASE;
	}

	bool Input::isKeyDown(InputKey inputKey)
	{
		int32 state= glfwGetKey(glfwWindow, (int32)inputKey);
		return state == GLFW_PRESS;
	}

	float64 Input::getTimeSinceStart() { return glfwGetTime(); }
} // namespace AstralRaytracer