#include "Raytracer/Traceable/SphereTraceable.h"

namespace AstralRaytracer
{
	bool SphereTraceable::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
		const glm::vec3& adjustedOrigin(rayIn.worldSpacePosition - m_transform.getPosition());

		const float32 a= glm::dot(rayIn.direction, rayIn.direction);
		const float32 b= 2.0f * glm::dot(adjustedOrigin, rayIn.direction);
		const float32 c= glm::dot(adjustedOrigin, adjustedOrigin) - m_radius * m_radius;

		const float32 discriminant= b * b - 4.0f * a * c;

		if(discriminant >= 0.0f)
		{
			float32 closestHit= (-b - glm::sqrt(discriminant)) / (2.0f * a);
			// float32 t0      = (-b + glm::sqrt(discriminant)) / (2.0f * a); // second hit

			if(closestHit < 0.0f)
			{
				return false;
			}

			hitInfo.worldSpacePosition= adjustedOrigin + rayIn.direction * closestHit;
			hitInfo.worldSpaceNormal  = glm::normalize(hitInfo.worldSpacePosition);
			hitInfo.worldSpacePosition+= m_transform.getPosition();

			hitInfo.materialIndex= m_materialIndex;
			hitInfo.hitDistance  = closestHit;
			return true;
		}

		return false;
	}
} // namespace AstralRaytracer