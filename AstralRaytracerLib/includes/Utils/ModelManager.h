#include "Common.h"
#include "Raytracer/Traceable/StaticMesh.h"

#include <filesystem>

namespace AstralRaytracer
{
	class ModelManager
	{
		public:
			[[nodiscard]]
			static StaticMesh getStaticMeshFromGLTF(const std::filesystem::path& path);
	};
} // namespace AstralRaytracer