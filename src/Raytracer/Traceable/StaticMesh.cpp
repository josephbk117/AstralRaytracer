#include "Raytracer/Traceable/StaticMesh.h"

namespace AstralRaytracer
{

	bool StaticMesh::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
		if(!intersectsBoundingBox(rayIn))
		{
			return false;
		}

		HitInfo closestHitInfo;
		for(uint32 triIndex= 0; triIndex < m_triangles.size(); ++triIndex)
		{
			HitInfo test_hitInfo;
			if(m_triangles[triIndex].trace({rayIn.worldSpacePosition, rayIn.direction}, test_hitInfo))
			{
				if(test_hitInfo.hitDistance < closestHitInfo.hitDistance)
				{
					closestHitInfo= test_hitInfo;
				}
			}
		}
		if(closestHitInfo.hitDistance > 0.0f &&
			 closestHitInfo.hitDistance < std::numeric_limits<float32>::max())
		{
			hitInfo= closestHitInfo;
			return true;
		}
		return false;
	}

	void StaticMesh::setPosition(const glm::vec3& position)
	{
		for(uint32 triIndex= 0; triIndex < m_triangles.size(); ++triIndex)
		{
			m_triangles[triIndex].setPosition(position);
		}

		Traceable::setPosition(position);
	}

	void StaticMesh::setMaterialIndex(uint32 materialIndex)
	{
		for(uint32 triIndex= 0; triIndex < m_triangles.size(); ++triIndex)
		{
			m_triangles[triIndex].setMaterialIndex(materialIndex);
		}
	}

	bool StaticMesh::intersectsBoundingBox(const Ray& rayIn) const
	{
		const float32 dirFracX= 1.0f / rayIn.direction.x;
		const float32 dirFracY= 1.0f / rayIn.direction.y;
		const float32 dirFracZ= 1.0f / rayIn.direction.z;
		// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
		// r.org is origin of ray

		const glm::vec3& lb= m_transform.getPosition() + m_boundingBox.min;
		const glm::vec3& rt= m_transform.getPosition() + m_boundingBox.max;

		const float32 t1= (lb.x - rayIn.worldSpacePosition.x) * dirFracX;
		const float32 t2= (rt.x - rayIn.worldSpacePosition.x) * dirFracX;
		const float32 t3= (lb.y - rayIn.worldSpacePosition.y) * dirFracY;
		const float32 t4= (rt.y - rayIn.worldSpacePosition.y) * dirFracY;
		const float32 t5= (lb.z - rayIn.worldSpacePosition.z) * dirFracZ;
		const float32 t6= (rt.z - rayIn.worldSpacePosition.z) * dirFracZ;

		const float32 tmax= glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if(tmax < 0.0f)
		{
			return false;
		}

		const float32 tmin= glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));

		// if tmin > tmax, ray doesn't intersect AABB
		if(tmin > tmax)
		{
			return false;
		}

		return true;
	}

} // namespace AstralRaytracer