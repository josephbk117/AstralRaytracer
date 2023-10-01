#include "Raytracer/PostProcessing/GammaCorrectionPostProcess.h"

namespace AstralRaytracer
{
	void GammaCorrectionPostProcessing::init()
	{
		m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
		m_shaderProgram.linkShaders();

		m_shaderProgram.use();
		m_shaderProgram.setUniformValue("gamma", 2.2f);
		m_shaderProgram.unuse();
	}

	void GammaCorrectionPostProcessing::processImage(DrawingPanel&        drawPanel,
																									 const RenderTexture& renderTexture,
																									 gl::GLuint           imageTexture)

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
		return ShaderLiterals::GammaCorrectionFS;
	}
} // namespace AstralRaytracer