#include "Raytracer/Traceable/TriangleTraceable.h"

namespace AstralRaytracer
{
	TriangleTraceable::TriangleTraceable()
	{
		TriangleTraceable(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
											glm::vec3(0.0f, 0.0f, 1.0f));
	}

	TriangleTraceable::TriangleTraceable(const glm::vec3& vertexA, const glm::vec3& vertexB,
																			 const glm::vec3& vertexC)
			: m_vertexA(vertexA), m_vertexB(vertexB), m_vertexC(vertexC)
	{
		m_normal= glm::normalize(glm::cross(m_vertexC - m_vertexA, m_vertexB - m_vertexA));
		m_vAvB  = m_vertexB - m_vertexA;
		m_vAvC  = m_vertexC - m_vertexA;
	}

	float32 TriangleTraceable::getTriangleDistance() const { return glm::dot(m_normal, m_vertexA); }

	bool TriangleTraceable::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
		const glm::vec3& adjustedOrigin(rayIn.worldSpacePosition - m_position);

		const glm::vec3 pvec= glm::cross(rayIn.direction, m_vAvC);
		const float32   det = glm::dot(m_vAvB, pvec);

		const float32 kEpsilon= 0.0001f;
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

		hitInfo.materialIndex            = m_materialIndex;
		hitInfo.hitDistance              = t;
		hitInfo.worldSpaceNormal         = -m_normal;
		hitInfo.rayOut.worldSpacePosition= rayIn.worldSpacePosition + (rayIn.direction * t);
		hitInfo.rayOut.direction         = glm::normalize(glm::reflect(rayIn.direction, -m_normal));

		return true;
	}
} // namespace AstralRaytracer