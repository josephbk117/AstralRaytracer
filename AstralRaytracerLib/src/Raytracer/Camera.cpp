#include "Raytracer/Camera.h"

#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace AstralRaytracer
{
	Camera::Camera(
			float32 focalLength,
			float32 sensorHeight,
			float32 focusDistance,
			float32 apertureDiameter,
			float32 nearClip,
			float32 farClip
	)
			: m_focalLength(focalLength),
				m_sensorHeight(sensorHeight),
				m_focusDistance(focusDistance),
				m_apertureDiameter(apertureDiameter),
				m_nearClip(nearClip),
				m_farClip(farClip)
	{
		m_forwardDir= Direction3D(0.0f, 0.0f, -1.0f);
		m_position = CoOrd3DF(0.0f, 1.0f, 3.0f);

		rotate({ 0.0f, 0.0f });
		recalculateView();
		recalculateProjection(m_resolution);
	}

	void Camera::update(const Resolution& resolution)
	{
		m_resolution= resolution;
		recalculateView();
		recalculateProjection(resolution);
	}

	void Camera::moveForward(float32 units) { m_position+= m_forwardDir * units; }

	void Camera::moveRight(float32 units)
	{
		const Direction3D rightDir= glm::cross(m_forwardDir, MathConstants::UpDirection);
		m_position+= rightDir * units;
	}

	void Camera::moveUp(float32 units) { m_position+= MathConstants::UpDirection * units; }

	void Camera::rotate(glm::vec2 rot)
	{
		constexpr float32 rotSpeed= 0.5f;

		const float32 pitchDelta= rot.y * rotSpeed;
		const float32 yawDelta  = rot.x * rotSpeed;

		const Direction3D rightDir= glm::cross(m_forwardDir, MathConstants::UpDirection);

		glm::quat q= glm::normalize(glm::cross(
				glm::angleAxis(-pitchDelta, rightDir), glm::angleAxis(-yawDelta, MathConstants::UpDirection)
		));

		m_forwardDir= glm::rotate(q, m_forwardDir);
		m_right    = glm::normalize(glm::cross(m_forwardDir, MathConstants::UpDirection));
		m_up       = glm::normalize(glm::cross(m_right, m_forwardDir));
	}

	void Camera::setVerticalFov(float32 vFov, float32 sensorHeight)
	{
		m_focalLength= sensorHeight / (2.0f * glm::tan(vFov * 0.5f));
	}

	float32 Camera::getVerticalFov() const
	{
		return 2.0f * glm::atan(m_sensorHeight / (2.0f * m_focalLength));
	}

	void Camera::setFStop(float32 fStop) { m_apertureDiameter= m_focalLength / fStop; }

	float32 Camera::getFStop() const { return m_focalLength / m_apertureDiameter; }

	const Direction3D& Camera::getRight() const { return m_right; }

	const Direction3D& Camera::getUp() const { return m_up; }

	void Camera::recalculateView()
	{
		m_view       = glm::lookAt(m_position, m_position + m_forwardDir, MathConstants::UpDirection);
		m_inverseView= glm::inverse(m_view);
	}

	void Camera::recalculateProjection(const Resolution& resolution)
	{
		const float32 aspectRatio= static_cast<float32>(resolution.x) / resolution.y;

		m_projection       = glm::perspective(getVerticalFov(), aspectRatio, m_nearClip, m_farClip);
		m_inverseProjection= glm::inverse(m_projection);
	}

} // namespace AstralRaytracer