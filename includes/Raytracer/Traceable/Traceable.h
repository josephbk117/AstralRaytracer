#pragma once
#include "Raytracer/Ray.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Traceable
	{
		public:
		Traceable(): m_position(0.0f) {}
		virtual ~Traceable(){};
		virtual void             setPosition(const glm::vec3& position) { m_position= position; };
		virtual const glm::vec3& getPosition() const { return m_position; }
		virtual void   setMaterialIndex(uint32 materialIndex) { m_materialIndex= materialIndex; };
		virtual uint32 getMaterialIndex() const { return m_materialIndex; }
		virtual bool   trace(const Ray& rayIn, HitInfo& hitInfo) const= 0;

		protected:
		glm::vec3 m_position;
		uint32    m_materialIndex= 0; // points to default material
	};
} // namespace AstralRaytracer