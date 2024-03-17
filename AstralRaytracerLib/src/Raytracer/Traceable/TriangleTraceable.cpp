#include "Raytracer/Traceable/TriangleTraceable.h"

namespace AstralRaytracer
{
	TriangleTraceable::TriangleTraceable()
	{
		TriangleTraceable(
				glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)
		);
	}

	TriangleTraceable::TriangleTraceable(
			const glm::vec3& vertexA,
			const glm::vec3& vertexB,
			const glm::vec3& vertexC
	)
			: m_vertexA(vertexA), m_vertexB(vertexB), m_vertexC(vertexC)
	{
		m_InitialVertexA= m_vertexA;
		m_InitialVertexB= m_vertexB;
		m_InitialVertexC= m_vertexC;

		m_normal= glm::normalize(glm::cross(m_vertexC - m_vertexA, m_vertexB - m_vertexA));
		m_vAvB  = m_vertexB - m_vertexA;
		m_vAvC  = m_vertexC - m_vertexA;
	}

	void TriangleTraceable::setScale(const glm::vec3& scale)
	{
		m_vertexA= m_InitialVertexA * scale;
		m_vertexB= m_InitialVertexB * scale;
		m_vertexC= m_InitialVertexC * scale;

		m_normal= glm::normalize(glm::cross(m_vertexC - m_vertexA, m_vertexB - m_vertexA));
		m_vAvB  = m_vertexB - m_vertexA;
		m_vAvC  = m_vertexC - m_vertexA;
		Traceable::setScale(scale);
	}

	float32 TriangleTraceable::getTriangleDistance() const { return glm::dot(m_normal, m_vertexA); }

	bool TriangleTraceable::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
		const glm::vec3 adjustedOrigin(rayIn.worldSpacePosition - m_transform.getPosition());

		const glm::vec3 pvec= glm::cross(rayIn.direction, m_vAvC);
		const float32   det = glm::dot(m_vAvB, pvec);

		constexpr float32 kEpsilon= std::numeric_limits<float32>::epsilon();
		// if the determinant is negative, the triangle is 'back facing'
		// if the determinant is close to 0, the ray misses the triangle
		if(det < kEpsilon)
		{
			return false;
		}
		// ray and triangle are parallel if det is close to 0
		if(std::abs(det) <= kEpsilon)
		{
			return false;
		}

		const float32 invDet= 1.0f / det;

		const glm::vec3 tvec= adjustedOrigin - m_vertexA;
		const float32   u   = glm::dot(tvec, pvec) * invDet;
		if(u < 0.0f || u > 1.0f)
		{
			return false;
		}

		const glm::vec3 qvec= glm::cross(tvec, m_vAvB);
		const float32   v   = glm::dot(rayIn.direction, qvec) * invDet;
		if(v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		const float32 t= glm::dot(m_vAvC, qvec) * invDet;

		if(t <= kEpsilon)
		{
			return false;
		}

		hitInfo.materialIndex     = m_materialIndex;
		hitInfo.hitDistance       = t;
		hitInfo.worldSpaceNormal  = -m_normal;
		hitInfo.worldSpacePosition= rayIn.worldSpacePosition + (rayIn.direction * t);

		return true;
	}

	void TriangleTraceable::serialize(AssetManager& assetManager, YAML::Emitter& out) const
	{
		using namespace Serialization;
		Traceable::serialize(assetManager, out);

		out << YAML::Key << "Type" << YAML::Value << static_cast<uint32>(TraceableType::TRIANGLE);

		out << YAML::Key << "vertex A" << YAML::Value << m_vertexA;
		out << YAML::Key << "vertex B" << YAML::Value << m_vertexB;
		out << YAML::Key << "vertex C" << YAML::Value << m_vertexC;
	}

	void TriangleTraceable::deserialize(AssetManager& assetManager, YAML::Node& node)
	{
		m_vertexA= node["vertex A"].as<glm::vec3>();
		m_vertexB= node["vertex B"].as<glm::vec3>();
		m_vertexC= node["vertex C"].as<glm::vec3>();

		*this= TriangleTraceable(m_vertexA, m_vertexB, m_vertexC);
		m_id = uuids::uuid::from_string(node["UUID"].as<std::string>()).value();
		// Creates new TriangleTraceable so need to deserialize parent after
		Traceable::deserialize(assetManager, node);
	}

} // namespace AstralRaytracer