#pragma once
#include "Raytracer/Ray.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Traceable
	{
		public:
		Traceable(): m_position(0.0f) {}
		virtual void setPosition (const glm::vec3& position) { m_position = position; };
		virtual const glm::vec3& getPosition() const { return m_position; }
		virtual bool trace(const Ray& rayIn, HitInfo& hitInfo) const= 0;

		protected:
		glm::vec3 m_position;
	};
} // namespace AstralRaytracer