#include "Compositor/PostProcessing/BilateralFilterPostProcess.h"

namespace AstralRaytracer
{
	void BilateralFilterPostProcess::init()
	{
		m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
		m_shaderProgram.linkShaders();

		m_shaderProgram.use();
		m_shaderProgram.setUniformValue("spatialSigma", 0.001f, 0.001f, 1.0f);
		m_shaderProgram.setUniformValue("intensitySigma", 0.001f, 0.001f, 1.0f);
		m_shaderProgram.unuse();
	}

	void BilateralFilterPostProcess::processImage(
			DrawingPanel&        drawPanel,
			const RenderTexture& renderTexture,
			gl::GLuint           imageTexture
	) const
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
		return R"SHADER(

				#version 330 core
				in vec2 textureUV;
				out vec4 color;
				uniform sampler2D inputTexture;
				uniform float spatialSigma;   // Spatial standard deviation
				uniform float intensitySigma; // Intensity standard deviation

				const int KERNEL_SIZE = 9;

				void main()
				{
					vec2 texelSize = 1.0 / textureSize(inputTexture, 0);
					vec4 centerColor = texture(inputTexture, textureUV);

					vec3 sum = vec3(0.0);
					float weightSum = 0.0;

					for (int i = 0; i < KERNEL_SIZE; i++)
					{
						for (int j = 0; j < KERNEL_SIZE; j++)
						{
							vec2 offset = vec2(i - KERNEL_SIZE / 2, j - KERNEL_SIZE / 2) * texelSize;
							vec4 sampleColor = texture(inputTexture, textureUV + offset);

							vec3 colorDiff = sampleColor.rgb - centerColor.rgb;
							float spatialWeight = exp(-(length(offset) * length(offset)) / (2.0 * spatialSigma * spatialSigma));
							float intensityWeight = exp(-(dot(colorDiff, colorDiff)) / (2.0 * intensitySigma * intensitySigma));

							float weight = spatialWeight * intensityWeight;
							sum += sampleColor.rgb * weight;
							weightSum += weight;
						}
					}

					color = vec4(sum / weightSum, centerColor.a);
				}
				)SHADER";
	}

	const std::string& BilateralFilterPostProcess::getName() const
	{
		static const std::string name("Bilateral Filter");
		return name;
	}
} // namespace AstralRaytracer