#include "WindowFramework/Input.h"

namespace AstralRaytracer
{
	const GLFWwindow* Input::glfwWindow= nullptr;

	void Input::initialize(const Window& window) { glfwWindow= window.m_glfwWindow; }

	glm::vec2 Input::getMousePosition()
	{
		double xPos;
		double yPos;
		glfwGetCursorPos(const_cast<GLFWwindow*>(glfwWindow), &xPos, &yPos);
		return { xPos, yPos };
	}

	void Input::setCursorMode(CursorMode cursorMode)
	{
		glfwSetInputMode(const_cast<GLFWwindow*>(glfwWindow), GLFW_CURSOR, (int32)cursorMode);
	}

	bool Input::isMouseButtonDown(MouseButtonIndex mouseButtonIndex)
	{
		int32 newState=
				glfwGetMouseButton(const_cast<GLFWwindow*>(glfwWindow), (int32)mouseButtonIndex);
		return newState == GLFW_PRESS;
	}

	bool Input::isMouseButtonUp(MouseButtonIndex mouseButtonIndex)
	{
		int32 newState=
				glfwGetMouseButton(const_cast<GLFWwindow*>(glfwWindow), (int32)mouseButtonIndex);
		return newState == GLFW_RELEASE;
	}

	bool Input::isKeyDown(InputKey inputKey)
	{
		int32 state= glfwGetKey(const_cast<GLFWwindow*>(glfwWindow), (int32)inputKey);
		return state == GLFW_PRESS;
	}

	float64 Input::getTimeSinceStart() { return glfwGetTime(); }
} // namespace AstralRaytracer