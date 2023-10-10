#include "Raytracer/PostProcessing/PostProcessing.h"

namespace AstralRaytracer
{
	void PostProcessing::setUniform(const std::string& name, std::any data)
	{
		m_shaderProgram.use();
		m_shaderProgram.updateUniformData(name, data);
		m_shaderProgram.unuse();
	}

	std::any PostProcessing::getUniform(const std::string& name) const
	{
		return m_shaderProgram.getUniformData().at(name).data;
	}

	const ShaderProgram& PostProcessing::getShader() const { return m_shaderProgram; }

	const char* const PostProcessing::getVertexShaderSrcCode() const
	{
		return R"SHADER(
						#version 330 core
						in vec2			vertexPosition;
						in vec2         texCoords;
						out vec2        textureUV;
						out vec3        worldPos;

						void main()
						{
							gl_Position  = vec4(vertexPosition.xy, 0, 1.0);
							worldPos     = gl_Position.xyz;
							textureUV    = texCoords;
						}
				)SHADER";
	}
} // namespace AstralRaytracer