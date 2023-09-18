#include "Traceable/StaticMesh.h"
#include "Utils/Common.h"

#include <filesystem>

namespace AstralRaytracer
{
	class ModelManager
	{
		public:
		static StaticMesh getStaticMeshFromGLTF(const std::filesystem::path& path);
	};
} // namespace AstralRaytracer