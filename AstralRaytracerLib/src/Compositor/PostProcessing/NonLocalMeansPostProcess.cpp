#include "Compositor/PostProcessing/NonLocalMeansPostProcess.h"

const std::string& AstralRaytracer::NonLocalMeansPostProcessing::getName() const
{
	static const std::string name("Non-Local Means");
	return name;
}

void AstralRaytracer::NonLocalMeansPostProcessing::init() 
{
	m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
	m_shaderProgram.linkShaders();

	m_shaderProgram.use();
	m_shaderProgram.setUniformValue("searchRadius", 5, 3, 10);
	m_shaderProgram.setUniformValue("patchRadius", 1.0f, 0.5f, 2.0f);
	m_shaderProgram.setUniformValue("noiseLevel", 0.1f, 0.05f, 0.2f);
	m_shaderProgram.unuse();
}

void AstralRaytracer::NonLocalMeansPostProcessing::processImage(
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

const char* const AstralRaytracer::NonLocalMeansPostProcessing::getFragmentShaderSrcCode() const
{
	return R"SHADER(
						#version 330 core
						in vec2				textureUV;
						in vec3				worldPos;
						out vec4			color;
						uniform sampler2D	inputTexture;
						uniform int			searchRadius; //Adjust the search radius as needed
						uniform float		patchRadius; //Adjust the patch radius as needed
						uniform float		noiseLevel; //Adjust the noise level as needed

						void main()
						{
							vec2 uv = textureUV;
							vec3 resultColor = vec3(0.0);
							float totalWeight = 0.0;

							for (float i = -searchRadius; i <= searchRadius; i++) {
							    for (float j = -searchRadius; j <= searchRadius; j++) {
							        vec2 offset = vec2(i, j) / textureSize(inputTexture, 0);

							        // Calculate similarity measure based on patch comparison
							        float weight = exp(-dot(texture(inputTexture, uv).rgb - texture(inputTexture, uv + offset).rgb, texture(inputTexture, uv).rgb - texture(inputTexture, uv + offset).rgb) / (2.0 * noiseLevel * noiseLevel));

							        // Accumulate weighted color values
							        resultColor += weight * texture(inputTexture, uv + offset).rgb;
							        totalWeight += weight;
							    }
							}

							 // Normalize the result by dividing by the total weight
							resultColor /= totalWeight;

							color = vec4(resultColor, 1.0);
						}
				)SHADER";
}
