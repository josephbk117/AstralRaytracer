#include "Raytracer/PostProcessing/PostProcessing.h"

namespace AstralRaytracer
{
	const ShaderProgram& PostProcessing::getShader() const
	{
		return m_shaderProgram;
	}
	const char* const PostProcessing::getVertexShaderSrcCode() const
	{
		return ShaderLiterals::VertexShader;
	}
} // namespace AstralRaytracer