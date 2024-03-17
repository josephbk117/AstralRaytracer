#include "Compositor/PostProcessing/DesaturatePostProcess.h"

namespace AstralRaytracer
{
void DesaturatePostProcessing::init()
{
    m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
    m_shaderProgram.linkShaders();

    m_shaderProgram.use();
    m_shaderProgram.setUniformValue("desaturationFactor", 0.0f, 0.0f, 1.0f);
    m_shaderProgram.unuse();
}

void DesaturatePostProcessing::processImage(DrawingPanel &drawPanel, const RenderTexture &renderTexture,
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

const char *const DesaturatePostProcessing::getFragmentShaderSrcCode() const
{
    return R"SHADER(		
			#version 330 core
			in vec2 textureUV;
			out vec4 color;
			uniform sampler2D inputTexture0;
			uniform float desaturationFactor; // Range from 0.0 (fully desaturated) to 1.0 (no desaturation)

			void main()
			{
				vec4 texColor = texture(inputTexture0, textureUV);
				float grayValue = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
				vec3 desaturatedColor = mix(texColor.rgb, vec3(grayValue), desaturationFactor);
				color = vec4(desaturatedColor, texColor.a);
			})SHADER";
}
} // namespace AstralRaytracer