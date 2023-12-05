#include "Compositor/PostProcessing/AddPostProcess.h"

const std::string& AstralRaytracer::AddPostProcessing::getName() const
{
	static const std::string name("Add");
	return name;
}

void AstralRaytracer::AddPostProcessing::init()
{
	m_shaderProgram.compileShadersFromSrcCode(getVertexShaderSrcCode(), getFragmentShaderSrcCode());
	m_shaderProgram.linkShaders();

	m_shaderProgram.use();
	m_shaderProgram.unuse();
}

void AstralRaytracer::AddPostProcessing::processImage(
		DrawingPanel&        drawPanel,
		const RenderTexture& renderTexture,
		gl::GLuint           imageTexture
) const
{
	renderTexture.bind();
	gl::glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

	m_shaderProgram.use();
	drawPanel.setTextureID(m_mainColorTexture, 0);
	drawPanel.setTextureID(imageTexture, 1);

	m_shaderProgram.applyShaderInt(
			m_shaderProgram.getUniformLocation("inputTexture0"), 0
	);
	m_shaderProgram.applyShaderInt(
			m_shaderProgram.getUniformLocation("inputTexture1"), 1
	);

	drawPanel.draw();
	m_shaderProgram.unuse();

	renderTexture.unbind();
}

const char* const AstralRaytracer::AddPostProcessing::getFragmentShaderSrcCode() const
{
	return R"SHADER(		
			#version 330 core
			in vec2 textureUV;
			out vec4 color;
			uniform sampler2D inputTexture0;
			uniform sampler2D inputTexture1;

			void main()
			{
				vec4 texColor0 = texture(inputTexture0, textureUV);
				vec4 texColor1 = texture(inputTexture1, textureUV);

				vec4 output = texColor0 + texColor1;

				color = vec4(output.rgb, 1.0);
			})SHADER";
}
