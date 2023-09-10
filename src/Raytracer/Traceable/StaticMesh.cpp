#include "Raytracer/Traceable/StaticMesh.h"

#include "Raytracer/ModelManager.h"

namespace AstralRaytracer
{

	StaticMesh::StaticMesh(const std::filesystem::path& meshFilePath)
	{
		m_sourceMeshFilePath= meshFilePath.string();

		*this= ModelManager::getStaticMeshFromGLTF(meshFilePath);
	}

	const std::string& StaticMesh::getSourceMeshFilePath() const { return m_sourceMeshFilePath; }

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

	void StaticMesh::setScale(const glm::vec3& scale)
	{
		for(uint32 triIndex= 0; triIndex < m_triangles.size(); ++triIndex)
		{
			m_triangles[triIndex].setScale(scale);
		}

		m_boundingBox.max= m_initialBoundingBox.max * scale;
		m_boundingBox.min= m_initialBoundingBox.min * scale;

		Traceable::setScale(scale);
	}

	void StaticMesh::setMaterialIndex(uint32 materialIndex)
	{
		for(uint32 triIndex= 0; triIndex < m_triangles.size(); ++triIndex)
		{
			m_triangles[triIndex].setMaterialIndex(materialIndex);
		}
	}

	void StaticMesh::serialize(YAML::Emitter& out) const
	{
		using namespace Serialization;
		Traceable::serialize(out);
		out << YAML::Key << "Type" << YAML::Value << static_cast<uint32>(TraceableType::STATIC_MESH);
		out << YAML::Key << "Source Path" << YAML::Value << m_sourceMeshFilePath;
	}

	void StaticMesh::deserialize(YAML::Node& node)
	{
		*this= StaticMesh(node["Source Path"].as<std::string>());
		// Creates new StaticMesh so need to deserialize parent after
		Traceable::deserialize(node);
		setMaterialIndex(m_materialIndex);
		setPosition(m_transform.getPosition());
	}

	bool StaticMesh::intersectsBoundingBox(const Ray& rayIn) const
	{
		const glm::vec3& direction         = rayIn.direction;
		const glm::vec3& worldSpacePosition= rayIn.worldSpacePosition;
		const glm::vec3& lb                = m_transform.getPosition() + m_boundingBox.min;
		const glm::vec3& rt                = m_transform.getPosition() + m_boundingBox.max;

		const float32 dirFracX= 1.0f / direction.x;
		const float32 dirFracY= 1.0f / direction.y;
		const float32 dirFracZ= 1.0f / direction.z;

		const float32 t1= (lb.x - worldSpacePosition.x) * dirFracX;
		const float32 t2= (rt.x - worldSpacePosition.x) * dirFracX;
		const float32 t3= (lb.y - worldSpacePosition.y) * dirFracY;
		const float32 t4= (rt.y - worldSpacePosition.y) * dirFracY;
		const float32 t5= (lb.z - worldSpacePosition.z) * dirFracZ;
		const float32 t6= (rt.z - worldSpacePosition.z) * dirFracZ;

		const float32 tmax= glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

		if(tmax < 0.0f)
		{
			return false;
		}

		const float32 tmin= glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));

		if(tmin > tmax)
		{
			return false;
		}

		return true;
	}

} // namespace AstralRaytracer