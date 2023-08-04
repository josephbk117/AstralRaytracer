#include "Raytracer/ModelManager.h"
// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION

#include <iostream>
#include <tiny_gltf.h>

namespace AstralRaytracer
{

	AstralRaytracer::StaticMesh ModelManager::getStaticMeshFromGLTF(const std::filesystem::path& path)
	{
		tinygltf::Model model;
		std::string     error, warning;

		tinygltf::TinyGLTF loader;

		loader.LoadASCIIFromFile(&model, &error, &warning, path.string());

		if(!warning.empty())
		{
			std::cout << "\nWarning: " << warning << "\n";
		}

		if(!error.empty())
		{
			std::cout << "\Error: " << error << "\n";
		}

		const tinygltf::BufferView& bufferView= model.bufferViews[0];
		const tinygltf::Buffer& buffer= model.buffers[bufferView.buffer];
		std::cout << "bufferview.target " << bufferView.target << std::endl;

		StaticMesh staticMesh;
		std::vector<TriangleTraceable> triangles;
		
		for(uint32 index= 0; index < buffer.data.size(); index += 12) 
		{
			float32 x= buffer.data.data()[index];
			float32 y= buffer.data.data()[index + sizeof(float32)];
			float32 z= buffer.data.data()[index + sizeof(float32) * 2];

			triangles.push_back(TriangleTraceable(x,y,z));
		}
	}

} // namespace AstralRaytracer