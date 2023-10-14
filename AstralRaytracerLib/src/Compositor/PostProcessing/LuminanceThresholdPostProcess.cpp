#include "Compositor/PostProcessing/LuminanceThresholdPostProcess.h"

const std::string& AstralRaytracer::LuminanceThresholdPostProcessing::getName() const
{
	static const std::string name("Luminance Threshold");
	return name;
}

void AstralRaytracer::LuminanceThresholdPostProcessing::init()
{
	m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
	m_shaderProgram.linkShaders();

	m_shaderProgram.use();
	m_shaderProgram.setUniformValue("minThreshold", 0.0f, 0.0f, 100.0f);
	m_shaderProgram.setUniformValue("maxThreshold", 100.0f, 0.0f, 100.0f);
	m_shaderProgram.unuse();
}

void AstralRaytracer::LuminanceThresholdPostProcessing::processImage(
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
AstralRaytracer::LuminanceThresholdPostProcessing::getFragmentShaderSrcCode() const
{
	return R"SHADER(		
			#version 330 core
			in vec2 textureUV;
			out vec4 color;
			uniform sampler2D inputTexture;
			uniform float minThreshold;
			uniform float maxThreshold;

			void main()
			{
				vec4 texColor = texture(inputTexture, textureUV);
				float luminance = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
				
				if(luminance < minThreshold || luminance > maxThreshold)
				{
					texColor.rgb = vec3(0.0,0.0,0.0);
				}

				color = vec4(texColor.rgb, texColor.a);
			})SHADER";
}
