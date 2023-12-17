#include "Raytracer/Camera.h"

#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace AstralRaytracer
{

	Camera::Camera(float32 fov, float32 nearClip, float32 farClip)
			: m_vFov(fov), m_nearClip(nearClip), m_farClip(farClip)
	{
		m_direction        = glm::vec3(0.0f, 0.0f, -1.0f);
		m_position         = glm::vec3(0.0f, 1.0f, 3.0f);
		m_lastMousePosition= glm::vec2(0.0f);
		m_resolution       = { 1, 1 };

		recalculateView();
		recalculateProjection(m_resolution);
	}

	void Camera::update(const glm::u32vec2& resolution)
	{
		m_resolution= resolution;
		recalculateView();
		recalculateProjection(resolution);
	}

	void Camera::moveForward(float32 units) { m_position+= m_direction * units; }

	void Camera::moveRight(float32 units)
	{
		const glm::vec3  upDir(0.0f, 1.0f, 0.0f);
		const glm::vec3& rightDir= glm::cross(m_direction, upDir);
		m_position+= rightDir * units;
	}

	void Camera::moveUp(float32 units) { m_position+= glm::vec3(0.0f, 1.0f, 0.0f) * units; }

	void Camera::rotate(glm::vec2 rot)
	{
		const float32 rotSpeed= 0.5f;

		const float32 pitchDelta= rot.y * rotSpeed;
		const float32 yawDelta  = rot.x * rotSpeed;

		const glm::vec3  upDir(0.0f, 1.0f, 0.0f);
		const glm::vec3& rightDir= glm::cross(m_direction, upDir);

		glm::quat q= glm::normalize(
				glm::cross(glm::angleAxis(-pitchDelta, rightDir), glm::angleAxis(-yawDelta, upDir))
		);

		m_direction= glm::rotate(q, m_direction);
	}

	void Camera::recalculateView()
	{
		m_view       = glm::lookAt(m_position, m_position + m_direction, glm::vec3(0.0f, 1.0f, 0.0f));
		m_inverseView= glm::inverse(m_view);
	}

	void Camera::recalculateProjection(const glm::u32vec2& resolution)
	{
		const float32 fovRadian= glm::radians(m_vFov);
		m_projection           = glm::perspective(
        fovRadian, (float32)resolution.x / (float32)resolution.y, m_nearClip, m_farClip
    );
		m_inverseProjection= glm::inverse(m_projection);
	}

} // namespace AstralRaytracer