#include "Raytracer/Traceable/SphereTraceable.h"

namespace AstralRaytracer
{
	bool SphereTraceable::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
		const glm::vec3& adjustedOrigin(rayIn.origin - m_position);

		const float32 a= glm::dot(rayIn.direction, rayIn.direction);
		const float32 b= 2.0f * glm::dot(adjustedOrigin, rayIn.direction);
		const float32 c= glm::dot(adjustedOrigin, adjustedOrigin) - m_radius * m_radius;

		const float32 discriminant= b * b - 4.0f * a * c;

		if(discriminant >= 0.0f)
		{
			float32 closestHit= (-b - glm::sqrt(discriminant)) / (2.0f * a);
			//float32 t0      = (-b + glm::sqrt(discriminant)) / (2.0f * a); // second hit

			glm::vec3 hitPoint= adjustedOrigin + rayIn.direction * closestHit;
			glm::vec3 normal  = glm::normalize(hitPoint);

			glm::vec3 lightDir= glm::normalize(glm::vec3(-1, -1, -1));

			float32 d= (glm::dot(normal, -lightDir) + 1.0f) * 0.5f;

			hitInfo.materialIndex = m_materialIndex;
			hitInfo.hitDistance     = closestHit;
			hitInfo.normal = normal;
			hitInfo.rayOut.origin   = hitPoint;
			hitInfo.rayOut.direction= normal;
			return true;
		}

		return false;
	}
} // namespace AstralRaytracer