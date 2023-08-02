#include "Raytracer/Traceable/SphereTraceable.h"

namespace AstralRaytracer
{
	bool SphereTraceable::trace(const Ray& rayIn, HitInfo& hitInfo)
	{
		const float32 radius = 0.5f;

		const float32 a = glm::dot(rayIn.direction, rayIn.direction);
		const float32 b = 2.0f * glm::dot(rayIn.origin, rayIn.direction);
		const float32 c = glm::dot(rayIn.origin, rayIn.origin) - radius * radius;

		const float32 discriminant = b * b - 4.0f * a * c;

		if (discriminant >= 0.0f)
		{
			//float32 t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			float32 closesrT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

			glm::vec3 hitPoint = rayIn.origin + rayIn.direction * closesrT;
			glm::vec3 normal = glm::normalize(hitPoint);

			glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

			float32 d = (glm::dot(normal, -lightDir) + 1.0f) * 0.5f;

			hitInfo.rayOut.origin = hitPoint;
			hitInfo.rayOut.direction = normal;

			hitInfo.colorData = ColourData(d,d,d);

			return true;
		}

		return false;
	}
}