#pragma once
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Camera
	{
		public:
		Camera(float32 fov, float32 nearClip, float32 farClip);
		bool update(float32 deltaTime, const glm::u32vec2& resolution);

		const glm::u32vec2& getResolution() const { return m_resolution; }
		const glm::vec3&    getPosition() const { return m_position; }
		const glm::vec3&    getDirection() const { return m_direction; }
		const glm::mat4&    getView() const { return m_view; }
		const glm::mat4&    getProjection() const { return m_projection; }
		const glm::mat4&    getInverseProjection() const { return m_inverseProjection; };
		const glm::mat4&    getInverseView() const { return m_inverseView; };

		private:
		void recalculateView();
		void recalculateProjection(const glm::u32vec2& resolution);

		glm::u32vec2 m_resolution{500, 500};

		glm::mat4 m_projection{1.0f};
		glm::mat4 m_view{1.0f};
		glm::mat4 m_inverseProjection{1.0f};
		glm::mat4 m_inverseView{1.0f};

		glm::vec3 m_position{0.0f};
		glm::vec3 m_direction{0.0f};

		float32 m_fov;
		float32 m_nearClip;
		float32 m_farClip;

		glm::vec2 m_lastMousePosition;
	};
} // namespace AstralRaytracer