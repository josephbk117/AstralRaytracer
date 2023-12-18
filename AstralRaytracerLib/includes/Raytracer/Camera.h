#pragma once
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Camera
	{
		public:
			Camera(float32 fov, float32 nearClip, float32 farClip);
			void update(const glm::u32vec2& resolution);

			void moveForward(float32 units);
			void moveRight(float32 units);
			void moveUp(float32 units);
			void rotate(glm::vec2 rot);

			const glm::u32vec2& getResolution() const { return m_resolution; }

			const glm::vec3& getPosition() const { return m_position; }

			const glm::vec3& getDirection() const { return m_direction; }

			void setAperture(float32 apertureSize) { m_apertureSize= apertureSize; }

			const float32 getAperture() const { return m_apertureSize; }

			void setFocusDistance(float32 focusDistance) { m_focusDistance= focusDistance; }

			const float32 getFocusDistance() const { return m_focusDistance; }

			const glm::vec3 getRight() const;

			const glm::vec3 getUp() const;

			const glm::mat4& getView() const { return m_view; }

			const glm::mat4& getProjection() const { return m_projection; }

			const glm::mat4& getInverseProjection() const { return m_inverseProjection; };

			const glm::mat4& getInverseView() const { return m_inverseView; };
		private:
			inline void recalculateView();
			inline void recalculateProjection(const glm::u32vec2& resolution);

			glm::u32vec2 m_resolution{ 500, 500 };

			glm::mat4 m_projection       = glm::identity<glm::mat4>();
			glm::mat4 m_view             = glm::identity<glm::mat4>();
			glm::mat4 m_inverseProjection= glm::identity<glm::mat4>();
			glm::mat4 m_inverseView      = glm::identity<glm::mat4>();

			glm::vec3 m_position{ 0.0f };
			glm::vec3 m_direction{ 0.0f };

			float32 m_apertureSize = 0.2f;
			float32 m_focusDistance= 10.0f;

			float32 m_vFov;
			float32 m_nearClip;
			float32 m_farClip;

			glm::vec2 m_lastMousePosition;
	};
} // namespace AstralRaytracer