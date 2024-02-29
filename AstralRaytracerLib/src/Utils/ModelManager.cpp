#include "Utils/ModelManager.h"
// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION

#include <glm/glm.hpp>
#include <tinygltf/tiny_gltf.h>

namespace AstralRaytracer
{

	AstralRaytracer::StaticMesh ModelManager::getStaticMeshFromGLTF(const std::filesystem::path& path)
	{
		tinygltf::Model model;

		std::string error;
		std::string warning;

		tinygltf::TinyGLTF loader;

		loader.LoadASCIIFromFile(&model, &error, &warning, path.string());

		if(!warning.empty())
		{
			ASTRAL_LOG_WARN("Loading mesh warning :{}",warning);
		}

		if(!error.empty())
		{
			ASTRAL_LOG_ERROR("Loading mesh error :{}", error);
		}

		const tinygltf::Accessor& accessor=
				model.accessors[model.meshes.at(0).primitives.at(0).attributes["POSITION"]];
		const tinygltf::BufferView& bufferView= model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer&     buffer    = model.buffers[bufferView.buffer];

		std::vector<glm::vec3> vertexPositions;

		const float32* positions=
				reinterpret_cast<const float32*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
		for(size_t i= 0; i < accessor.count; ++i)
		{
			vertexPositions.push_back(
					glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2])
			);
		}

		const tinygltf::Accessor& indexAccessor=
				model.accessors[model.meshes.at(0).primitives.at(0).indices];
		const tinygltf::BufferView& indexBufferView= model.bufferViews[indexAccessor.bufferView];
		const tinygltf::Buffer&     indexBuffer    = model.buffers[indexBufferView.buffer];

		std::vector<TriangleTraceable> triangles;

		const int16* meshIndices= reinterpret_cast<const int16*>(
				&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]
		);
		for(size_t i= 0; i < indexAccessor.count; i+= 3)
		{
			triangles.push_back(TriangleTraceable(
					vertexPositions[meshIndices[i]], vertexPositions[meshIndices[i + 1]],
					vertexPositions[meshIndices[i + 2]]
			));
		}

		AABB boundingBox{};
		boundingBox.min= glm::vec3(accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
		boundingBox.max= glm::vec3(accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);

		return StaticMesh(triangles, boundingBox, path.string());
	}

} // namespace AstralRaytracer