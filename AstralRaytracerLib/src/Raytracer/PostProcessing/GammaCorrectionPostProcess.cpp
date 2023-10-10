#include "Raytracer/PostProcessing/GammaCorrectionPostProcess.h"

namespace AstralRaytracer
{
	const std::string& GammaCorrectionPostProcessing::getName() const
	{
		static const std::string name("Gamma Correction");
		return name;
	}

	void GammaCorrectionPostProcessing::init()
	{
		m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
		m_shaderProgram.linkShaders();

		m_shaderProgram.use();
		m_shaderProgram.setUniformValue("gamma", 2.2f, 0.0f, 2.4f);
		m_shaderProgram.unuse();
	}

	void GammaCorrectionPostProcessing::processImage(
			DrawingPanel&        drawPanel,
			const RenderTexture& renderTexture,
			gl::GLuint           imageTexture
	)

	{
		renderTexture.bind();
		gl::glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

		m_shaderProgram.use();
		drawPanel.setTextureID(imageTexture);
		drawPanel.draw();
		m_shaderProgram.unuse();

		renderTexture.unbind();
	}

	const char* const GammaCorrectionPostProcessing::getFragmentShaderSrcCode() const
	{
		return R"SHADER(
						#version 330 core
						in vec2				textureUV;
						in vec3				worldPos;
						out vec4			color;
						uniform sampler2D	textureOne;
						uniform float		gamma;

						void main()
						{
							// Sample the texture
							vec3 texColor = texture(textureOne, textureUV).rgb;
							color = vec4(pow(texColor, vec3(1.0 / gamma)), 1.0);
						}
				)SHADER";
	}
} // namespace AstralRaytracer