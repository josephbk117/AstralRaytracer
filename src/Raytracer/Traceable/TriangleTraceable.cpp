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
		const float32    a= glm::dot(rayIn.direction, m_normal);

		if(a == 0.0f)
		{
			return false;
		}

		const float32 b= glm::dot(m_normal, adjustedOrigin + m_normal - getTriangleDistance()); // TODO
		const float32 distanceToPlane= -1.0f * b / a;

		const glm::vec3 intersect= rayIn.direction * distanceToPlane + adjustedOrigin;

		const glm::vec3 CA= m_vertexC - m_vertexA;
		const glm::vec3 IA= intersect - m_vertexA;

		const float32 test1= glm::dot(glm::cross(CA, IA), m_normal);

		const glm::vec3 BC= m_vertexB - m_vertexC;
		const glm::vec3 IC= intersect - m_vertexC;

		const float32 test2= glm::dot(glm::cross(BC, IC), m_normal);

		const glm::vec3 AB= m_vertexA - m_vertexB;
		const glm::vec3 IB= intersect - m_vertexB;

		const float32 test3= glm::dot(glm::cross(AB, IB), m_normal);

		if(test1 >= 0.0f && test2 >= 0.0f && test3 >= 0.0f)
		{
			hitInfo.materialIndex   = m_materialIndex;
			hitInfo.hitDistance     = distanceToPlane;
			hitInfo.normal          = -m_normal;
			hitInfo.rayOut.origin   = intersect;
			hitInfo.rayOut.direction= m_normal;

			return true;
		}

		return false;
	}
} // namespace AstralRaytracer