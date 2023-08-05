#include "Raytracer/Traceable/StaticMesh.h"

namespace AstralRaytracer
{

	bool StaticMesh::trace(const Ray& rayIn, HitInfo& hitInfo) const
	{
		HitInfo test_hitInfo;
		HitInfo closestHitInfo;
		for(uint32 triIndex= 0; triIndex < m_triangles.size(); ++triIndex)
		{
			if(m_triangles[triIndex].trace({rayIn.origin, rayIn.direction}, test_hitInfo))
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

} // namespace AstralRaytracer