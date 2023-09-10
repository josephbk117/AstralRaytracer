#pragma once
#include "Raytracer/Ray.h"
#include "Raytracer/Transform.h"
#include "Utils/Common.h"
#include "Utils/Serialization.h"

namespace AstralRaytracer
{
	class Traceable : public Serialization::Serializable
	{
		public:
		Traceable()= default;
		virtual ~Traceable(){};
		virtual void setPosition(const glm::vec3& position) { m_transform.setPosition(position); };
		virtual const glm::vec3& getPosition() const { return m_transform.getPosition(); }
		virtual const glm::mat4& getTransformMatrix() const { return m_transform.getMatrix(); }
		virtual void   setMaterialIndex(uint32 materialIndex) { m_materialIndex= materialIndex; };
		virtual uint32 getMaterialIndex() const { return m_materialIndex; }
		virtual bool   trace(const Ray& rayIn, HitInfo& hitInfo) const= 0;

		virtual void serialize(YAML::Emitter& out) const override;
		virtual void deserialize(YAML::Node& node) override;

		protected:
		Transform m_transform;
		uint32    m_materialIndex= 0; // points to default material
	};
} // namespace AstralRaytracer