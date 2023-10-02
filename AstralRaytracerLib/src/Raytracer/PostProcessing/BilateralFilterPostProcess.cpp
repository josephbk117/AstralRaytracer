#include "Raytracer/PostProcessing/BilateralFilterPostProcess.h"

namespace AstralRaytracer
{
	void BilateralFilterPostProcess::init()
	{
		m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
		m_shaderProgram.linkShaders();

		m_shaderProgram.use();
		m_shaderProgram.setUniformValue("spatialSigma", 10.0f);
		m_shaderProgram.setUniformValue("rangeSigma", 0.1f);
		m_shaderProgram.unuse();
	}
	void BilateralFilterPostProcess::processImage(DrawingPanel&        drawPanel,
																								const RenderTexture& renderTexture,
																								gl::GLuint           imageTexture)
	{
		renderTexture.bind();
		gl::glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

		m_shaderProgram.use();
		drawPanel.setTextureID(imageTexture);
		drawPanel.draw();
		m_shaderProgram.unuse();

		renderTexture.unbind();
	}
	const char* const BilateralFilterPostProcess::getFragmentShaderSrcCode() const
	{
		return ShaderLiterals::BilateralFilterFS;
	}
	const std::string& BilateralFilterPostProcess::getName() const
	{
		static const std::string name("Bilateral Filter");
		return name;
	}
} // namespace AstralRaytracer