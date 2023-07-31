#include "Raytracer/Camera.h"
#include "WindowFramework/Input.h"
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/rotate_vector.hpp>

namespace AstralRaytracer
{

	Camera::Camera(float32 fov, float32 nearClip, float32 farClip) : m_fov(fov), m_nearClip(nearClip), m_farClip(farClip)
	{
		m_direction = {0.0f, 0.0f, -1.0f};
		m_position = {0.0f, 0.0f, 3.0f};
	}

	void Camera::update(float32 deltaTime)
	{
		const glm::vec2& mousePos = Input::getMousePosition();
		const glm::vec2 delta = (mousePos - m_lastMousePosition);
		m_lastMousePosition = mousePos;

		if (!Input::isMouseButtonDown(MouseButtonIndex::MOUSE_BUTTON_RIGHT))
		{
			Input::setCursorMode(CursorMode::NORMAL);
			return;
		}

		Input::setCursorMode(CursorMode::CAPTURED);

		bool moved = false;

		constexpr glm::vec3 upDir(0.0f, 1.0f, 0.0f);
		glm::vec3 rightDir = glm::cross(m_direction, upDir);


		if (Input::isKeyDown(InputKey::W))
		{
			m_position += m_direction * deltaTime;
			moved = true;
		}

		//Rotation

		if(delta.x != 0.0f || delta.y != 0.0f)
		{
			float32 pitchDelta	= delta.y;
			float32 yawDelta	= delta.x;

			glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDir), glm::angleAxis(-yawDelta, glm::vec3(0.0f, 1.0f, 0.0f))));

			m_direction = glm::rotate(q, m_direction);

			moved = true;
		}

		if (moved)
		{
			recalculateView();
			recalculateProjection();
		}
	}

	void Camera::recalculateView()
	{
		m_view = glm::translate(glm::mat4(1.0f), m_position);
		m_view = glm::lookAt(m_position, m_position + m_direction, glm::vec3(0.0f, 1.0f, 0.0f));
		m_inverseView = glm::inverse(m_view);
	}

	void Camera::recalculateProjection()
	{
		m_projection = glm::perspectiveFov(glm::radians(m_fov), 500.0f, 500.0f, m_nearClip, m_farClip);
		m_inverseProjection = glm::inverse(m_projection);
	}

}