#include "Raytracer/PostProcessing/PostProcessing.h"

namespace AstralRaytracer
{
	void PostProcessing::setUniform(const std::string& name, std::any data)
	{
		m_shaderProgram.use();
		m_shaderProgram.setUniformValue(name, data);
		m_shaderProgram.unuse();
	}

	std::any PostProcessing::getUniform(const std::string& name) const
	{
		return m_shaderProgram.getUniformData().at(name).data;
	}

	const ShaderProgram& PostProcessing::getShader() const { return m_shaderProgram; }
	const char* const    PostProcessing::getVertexShaderSrcCode() const
	{
		return ShaderLiterals::VertexShader;
	}
} // namespace AstralRaytracer