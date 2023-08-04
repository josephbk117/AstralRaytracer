#include "Utils/Common.h"
#include "Traceable/StaticMesh.h"
#include <filesystem>

namespace AstralRaytracer
{
	class ModelManager
	{
		static StaticMesh getStaticMeshFromGLTF(const std::filesystem::path& path);
	};
} // namespace AstralRaytracer