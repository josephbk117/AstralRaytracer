#include "Raytracer/PostProcessing/PostProcessing.h"

namespace AstralRaytracer
{
	const char* const PostProcessing::getVertexShaderSrcCode() const
	{
		return ShaderLiterals::VertexShader;
	}
} // namespace AstralRaytracer