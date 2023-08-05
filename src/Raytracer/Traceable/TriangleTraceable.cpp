#include "Raytracer/Traceable/TriangleTraceable.h"

namespace AstralRaytracer
{
	TriangleTraceable::TriangleTraceable()
			: m_vertexA(1.0f, 0.0f, 0.0f), m_vertexB(0.0f, 1.0f, 0.0f), m_vertexC(0.0f, 0.0f, 1.0f)
	{
		m_normal= glm::normalize(glm::cross(m_vertexC - m_vertexA, m_vertexB - m_vertexA));
	}

	TriangleTraceable::TriangleTraceable(const glm::vec3& vertexA, const glm::vec3& vertexB,
																			 const glm::vec3& vertexC)
			: m_vertexA(vertexA), m_vertexB(vertexB), m_vertexC(vertexC)
	{
		m_normal= glm::normalize(glm::cross(m_vertexC - m_vertexA, m_vertexB - m_vertexA));
	}

	float32 TriangleTraceable::getTriangleDistance() const { return glm::dot(m_normal, m_vertexA); }

	bool TriangleTraceable::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
		const glm::vec3& adjustedOrigin(rayIn.origin - m_position);

		glm::vec3 v0v1= m_vertexB - m_vertexA;
		glm::vec3 v0v2= m_vertexC - m_vertexA;
		glm::vec3 pvec= glm::cross(rayIn.direction, v0v2);
		float32   det = glm::dot(v0v1, pvec);

		const float32 kEpsilon= 0.0f;
		// if the determinant is negative, the triangle is 'back facing'
		// if the determinant is close to 0, the ray misses the triangle
		if(det < kEpsilon)
			return false;
		// ray and triangle are parallel if det is close to 0
		if(fabs(det) < kEpsilon)
			return false;
		float32 invDet= 1 / det;

		glm::vec3 tvec= adjustedOrigin - m_vertexA;
		float32   u   = glm::dot(tvec, pvec) * invDet;
		if(u < 0 || u > 1)
			return false;

		glm::vec3 qvec= glm::cross(tvec, v0v1);
		float32   v   = glm::dot(rayIn.direction, qvec) * invDet;
		if(v < 0 || u + v > 1)
			return false;

		float32 t= glm::dot(v0v2, qvec) * invDet;

		hitInfo.materialIndex   = m_materialIndex;
		hitInfo.hitDistance     = t;
		hitInfo.normal          = -m_normal;
		hitInfo.rayOut.origin   = rayIn.direction * t;
		hitInfo.rayOut.direction= m_normal;

		return true;
	}
} // namespace AstralRaytracer