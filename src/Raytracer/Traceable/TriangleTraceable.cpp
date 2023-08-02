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

	bool TriangleTraceable::trace(const Ray& rayIn, HitInfo& hitInfo)
	{
		float32 a= glm::dot(rayIn.direction, m_normal);

		if(a == 0.0f)
		{
			return false;
		}

		float32 b= glm::dot(m_normal, rayIn.origin + m_normal - getTriangleDistance()); // TODO
		float32 distanceToPlane= -1 * b / a;

		glm::vec3 intersect= rayIn.direction * distanceToPlane + rayIn.origin;

		glm::vec3 CA= m_vertexC - m_vertexA;
		glm::vec3 IA= intersect - m_vertexA;

		float32 test1= glm::dot(glm::cross(CA, IA), m_normal);

		glm::vec3 BC= m_vertexB - m_vertexC;
		glm::vec3 IC= intersect - m_vertexC;

		float32 test2= glm::dot(glm::cross(BC, IC), m_normal);

		glm::vec3 AB= m_vertexA - m_vertexB;
		glm::vec3 IB= intersect - m_vertexB;

		float32 test3= glm::dot(glm::cross(AB, IB), m_normal);

		if(test1 >= 0.0f && test2 >= 0.0f && test3 >= 0.0f)
		{
			hitInfo.rayOut.origin   = intersect;
			hitInfo.rayOut.direction= m_normal;

			hitInfo.colorData= ColourData(1.0f, 0.0f, 0.0f);
			return true;
		}

		return false;
	}
} // namespace AstralRaytracer