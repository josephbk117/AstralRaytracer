#include "Compositor/PostProcessing/NoAlphaPassThroughPostProcess.h"

void AstralRaytracer::NoAlphaPassThroughPostProcessing::init()
{
    m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
    m_shaderProgram.linkShaders();

    m_shaderProgram.use();
    m_shaderProgram.unuse();
}

void AstralRaytracer::NoAlphaPassThroughPostProcessing::processImage(DrawingPanel &drawPanel,
                                                                     const RenderTexture &renderTexture,
                                                                     gl::GLuint imageTexture) const
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

const char *const AstralRaytracer::NoAlphaPassThroughPostProcessing::getFragmentShaderSrcCode() const
{
    return R"SHADER(
						#version 330 core
						in vec2				textureUV;
						in vec3				worldPos;
						out vec4			color;
						uniform sampler2D	inputTexture0;

						void main()
						{
							// Sample the texture
							vec3 texColor = texture(inputTexture0, textureUV).rgb;
							color = vec4(texColor, 1.0);
						}
				)SHADER";
}
