#pragma once
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Camera
	{
		public:
			Camera(
					float32 focalLength,
					float32 sensorHeight,
					float32 focusDistance,
					float32 apertureDiameter,
					float32 nearClip= 0.01f,
					float32 farClip = 100.0f
			);
			void update(const Resolution& resolution);

			void moveForward(float32 units);
			void moveRight(float32 units);
			void moveUp(float32 units);
			void rotate(glm::vec2 rot);

			const Resolution& getResolution() const { return m_resolution; }

			const glm::vec3& getPosition() const { return m_position; }

			const glm::vec3& getForwardDirection() const { return m_forwardDir; }

			void setFarClipDistance(float32 farClip) { m_farClip= farClip; }

			float32 getFarClip() const { return m_farClip; }

			void setNearClipDistance(float32 nearClip) { m_nearClip= nearClip; }

			float32 getNearClip() const { return m_nearClip; }

			void setSensorHeight(float32 sensorHeight) { m_sensorHeight= sensorHeight; }

			float32 getSensorHeight() const { return m_sensorHeight; }

			void setFocalLength(float32 focalLength) { m_focalLength= focalLength; }

			float32 getFocalLength() const { return m_focalLength; }

			void setVerticalFov(float32 vFov, float32 sensorHeight);

			float32 getVerticalFov() const;

			void setFStop(float32 fStop);

			float32 getFStop() const;

			void setAperture(float32 apertureSize) { m_apertureDiameter= apertureSize; }

			float32 getApertureDiameter() const { return m_apertureDiameter; }

			void setFocusDistance(float32 focusDistance) { m_focusDistance= focusDistance; }

			float32 getFocusDistance() const { return m_focusDistance; }

			const Direction3D& getRight() const;

			const Direction3D& getUp() const;

			const glm::mat4& getView() const { return m_view; }

			const glm::mat4& getProjection() const { return m_projection; }

			const glm::mat4& getInverseProjection() const { return m_inverseProjection; };

			const glm::mat4& getInverseView() const { return m_inverseView; };
		private:
			inline void recalculateView();
			inline void recalculateProjection(const Resolution& resolution);

			Resolution m_resolution{ 1, 1 };

			glm::mat4 m_projection       = glm::identity<glm::mat4>();
			glm::mat4 m_view             = glm::identity<glm::mat4>();
			glm::mat4 m_inverseProjection= glm::identity<glm::mat4>();
			glm::mat4 m_inverseView      = glm::identity<glm::mat4>();

			CoOrd3DF m_position{ 0.0f };
			Direction3D m_forwardDir{ 0.0f };
			Direction3D m_right{ 0.0f };
			Direction3D m_up{ 0.0f };

			float32 m_focalLength     = 0.0f;
			float32 m_sensorHeight    = 0.0f;
			float32 m_focusDistance   = 0.0f;
			float32 m_apertureDiameter= 0.0f;
			float32 m_nearClip        = 0.0f;
			float32 m_farClip         = 0.0f;

			CoOrd2DF m_lastMousePosition{ 0.0f };
	};
} // namespace AstralRaytracer