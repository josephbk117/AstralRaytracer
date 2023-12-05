#include "Compositor/PostProcessing/HorizontalGaussBlurPostProcess.h"

const std::string& AstralRaytracer::HorizontalGaussBlurPostProcessing::getName() const
{
	static const std::string name("Horizontal Gauss Blur");
	return name;
}

void AstralRaytracer::HorizontalGaussBlurPostProcessing::init() 
{
	m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
	m_shaderProgram.linkShaders();

	m_shaderProgram.use();
	m_shaderProgram.setUniformValue("blurSize", 11, 1, 15);
	m_shaderProgram.unuse();
}

void AstralRaytracer::HorizontalGaussBlurPostProcessing::processImage(
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

const char* const
AstralRaytracer::HorizontalGaussBlurPostProcessing::getFragmentShaderSrcCode() const
{
	return R"SHADER(
						#version 330 core
						in vec2				textureUV;
						in vec3				worldPos;
						out vec4			color;
						uniform sampler2D	inputTexture0;
						uniform int			blurSize;

						void main()
						{
							const int maxKernelSize = 15;
							int kernelSize = min(blurSize, maxKernelSize);

							vec2 texelSize = 1.0 / textureSize(inputTexture0, 0);

							// Compute Gaussian weights
							float sigma = float(kernelSize) / texelSize.x;
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

							// Gaussian kernel offsets
							vec2 offsets[maxKernelSize];
							for (int i = 0; i < kernelSize; ++i)
							{
							    offsets[i] = vec2(float(i) - float(kernelSize - 1) / 2.0, 0.0);
							}

							vec3 blur = vec3(0.0);
							for (int i = 0; i < kernelSize; ++i)
							{
							    vec2 sampleCoord = textureUV + offsets[i] * texelSize;
							    sampleCoord.x = clamp(sampleCoord.x, 0.0, 1.0); // Clamp X-coordinate

							    blur += texture(inputTexture0, sampleCoord).rgb * weights[i];
							}

							color = vec4(blur, 1.0);
						}
				)SHADER";
}
