#include "Raytracer/Traceable/StaticMesh.h"
#include "Common.h"

#include <filesystem>

namespace AstralRaytracer
{
	class ModelManager
	{
		public:
			static StaticMesh getStaticMeshFromGLTF(const std::filesystem::path& path);
	};
} // namespace AstralRaytracer