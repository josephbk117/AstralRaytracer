#include "Raytracer/Traceable/StaticMesh.h"

namespace AstralRaytracer
{

	bool StaticMesh::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
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

} // namespace AstralRaytracer