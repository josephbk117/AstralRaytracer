#include "Compositor/PostProcessing/VerticalGaussBlurPostProcess.h"

const std::string& AstralRaytracer::VerticalGaussBlurPostProcessing::getName() const
{
	static const std::string name("Vertical Gauss Blur");
	return name;
}

void AstralRaytracer::VerticalGaussBlurPostProcessing::init() 
{
	m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
	m_shaderProgram.linkShaders();

	m_shaderProgram.use();
	m_shaderProgram.setUniformValue("blurSize", 11, 1, 11);
	m_shaderProgram.unuse();
}

void AstralRaytracer::VerticalGaussBlurPostProcessing::processImage(
		DrawingPanel&        drawPanel,
		const RenderTexture& renderTexture,
		gl::GLuint           imageTexture
) const
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

const char* const AstralRaytracer::VerticalGaussBlurPostProcessing::getFragmentShaderSrcCode() const
{
	return R"SHADER(
						#version 330 core
						in vec2				textureUV;
						in vec3				worldPos;
						out vec4			color;
						uniform sampler2D	textureOne;
						uniform int			blurSize;

						void main()
						{
							const int maxKernelSize = 11;
							int kernelSize = min(blurSize, maxKernelSize);

							// Compute Gaussian weights
							float sigma = float(kernelSize) / 3.0;
							float sumWeights = 0.0;
							float weights[maxKernelSize];

							for (int i = 0; i < kernelSize; ++i)
							{
								weights[i] = exp(-(float(i) * float(i)) / (2.0 * sigma * sigma));
								sumWeights += weights[i];
							}

							// Normalize weights
							for (int i = 0; i < kernelSize; ++i)
							{
								weights[i] /= sumWeights;
							}

							vec2 texelSize = 1.0 / textureSize(textureOne, 0);

							// Gaussian kernel offsets (vertical)
							vec2 offsets[maxKernelSize];
							for (int i = 0; i < kernelSize; ++i)
							{
								offsets[i] = vec2(0.0, float(i) - float(kernelSize - 1) / 2.0);
							}

							vec3 blur = vec3(0.0);
							for (int i = 0; i < kernelSize; ++i)
							{
								vec2 sampleCoord = textureUV + offsets[i] * texelSize;
								sampleCoord.y = clamp(sampleCoord.y, 0.0, 1.0); // Clamp Y-coordinate

								blur += texture(textureOne, sampleCoord).rgb * weights[i];
							}

							color = vec4(blur, 1.0);
						}
				)SHADER";
}
