#include "Raytracer/Camera.h"

//#include "WindowFramework/Input.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace AstralRaytracer
{

	Camera::Camera(float32 fov, float32 nearClip, float32 farClip)
			: m_fov(fov), m_nearClip(nearClip), m_farClip(farClip)
	{
		m_direction        = glm::vec3(0.0f, 0.0f, -1.0f);
		m_position         = glm::vec3(0.0f, 1.0f, 3.0f);
		m_lastMousePosition= glm::vec2(0.0f);

		recalculateView();
		m_resolution= {1, 1};
		recalculateProjection(m_resolution);
	}

	bool Camera::update(float32 deltaTime, const glm::u32vec2& resolution)
	{
		//if(m_lastMousePosition == glm::vec2(0.0f))
		//{
		//	m_lastMousePosition= Input::getMousePosition();
		//	return false;
		//}
		//const glm::vec2& mousePos= Input::getMousePosition();
		//const glm::vec2  delta   = (mousePos - m_lastMousePosition);
		//m_lastMousePosition      = mousePos;

		//bool forceRecalculate= m_resolution != resolution;

		//if(!Input::isMouseButtonDown(MouseButtonIndex::MOUSE_BUTTON_RIGHT) && !forceRecalculate)
		//{
		//	Input::setCursorMode(CursorMode::NORMAL);
		//	return false;
		//}

		//bool moved= false;

		//if(!forceRecalculate)
		//{
		//	Input::setCursorMode(CursorMode::CAPTURED);

		//	const glm::vec3 upDir(0.0f, 1.0f, 0.0f);
		//	glm::vec3       rightDir= glm::cross(m_direction, upDir);

		//	float32 moveSpeed = deltaTime;
		//	if(Input::isKeyDown(InputKey::LEFT_SHIFT)) 
		//	{
		//		moveSpeed *= 5.0f;
		//	}

		//	if(Input::isKeyDown(InputKey::W))
		//	{
		//		m_position+= m_direction * moveSpeed;
		//		moved= true;
		//	}

		//	if(Input::isKeyDown(InputKey::S))
		//	{
		//		m_position-= m_direction * moveSpeed;
		//		moved= true;
		//	}

		//	if(Input::isKeyDown(InputKey::A))
		//	{
		//		m_position-= rightDir * moveSpeed;
		//		moved= true;
		//	}

		//	if(Input::isKeyDown(InputKey::D))
		//	{
		//		m_position+= rightDir * moveSpeed;
		//		moved= true;
		//	}

		//	if(Input::isKeyDown(InputKey::Q))
		//	{
		//		m_position+= glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed;
		//		moved= true;
		//	}

		//	if(Input::isKeyDown(InputKey::E))
		//	{
		//		m_position-= glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed;
		//		moved= true;
		//	}

		//	// Rotation

		//	if(delta.x != 0.0f || delta.y != 0.0f)
		//	{
		//		const float32 rotSpeed= 0.5f;

		//		const float32 pitchDelta= delta.y * deltaTime * rotSpeed;
		//		const float32 yawDelta  = delta.x * deltaTime * rotSpeed;

		//		glm::quat q=
		//				glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDir),
		//																	glm::angleAxis(-yawDelta, glm::vec3(0.0f, 1.0f, 0.0f))));

		//		m_direction= glm::rotate(q, m_direction);

		//		moved= true;
		//	}
		//}
		//if(moved || forceRecalculate)
		//{
		//	m_resolution= resolution;
		//	recalculateView();
		//	recalculateProjection(resolution);
		//}

		//return moved || forceRecalculate;

		return true;
	}

	void Camera::recalculateView()
	{
		m_view       = glm::lookAt(m_position, m_position + m_direction, glm::vec3(0.0f, 1.0f, 0.0f));
		m_inverseView= glm::inverse(m_view);
	}

	void Camera::recalculateProjection(const glm::u32vec2& resolution)
	{
		m_projection       = glm::perspectiveFov(glm::radians(m_fov), (float32)resolution.x,
																						 (float32)resolution.y, m_nearClip, m_farClip);
		m_inverseProjection= glm::inverse(m_projection);
	}

} // namespace AstralRaytracer