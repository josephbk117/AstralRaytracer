#include "WindowFramework/Input.h"

namespace AstralRaytracer
{
	glm::vec2 Input::getMousePosition(const Window& window)
	{
		double xPos;
		double yPos;
		glfwGetCursorPos(window.m_glfwWindow, &xPos, &yPos);
		return { xPos, yPos };
	}

	void Input::setCursorMode(const Window& window, CursorMode cursorMode)
	{
		glfwSetInputMode(window.m_glfwWindow, GLFW_CURSOR, (int32)cursorMode);
	}

	bool Input::isMouseButtonDown(const Window& window, MouseButtonIndex mouseButtonIndex)
	{
		const int32 newState= glfwGetMouseButton(window.m_glfwWindow, (int32)mouseButtonIndex);
		return newState == GLFW_PRESS;
	}

	bool Input::isMouseButtonUp(const Window& window, MouseButtonIndex mouseButtonIndex)
	{
		const int32 newState= glfwGetMouseButton(window.m_glfwWindow, (int32)mouseButtonIndex);
		return newState == GLFW_RELEASE;
	}

	bool Input::isKeyDown(const Window& window, InputKey inputKey)
	{
		const int32 state= glfwGetKey(window.m_glfwWindow, (int32)inputKey);
		return state == GLFW_PRESS;
	}

	float64 Input::getTimeSinceStart() { return glfwGetTime(); }
} // namespace AstralRaytracer