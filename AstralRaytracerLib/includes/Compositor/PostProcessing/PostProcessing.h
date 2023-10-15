#pragma once
#include "../DrawingPanel.h"
#include "../RenderTexture.h"
#include "Utils/ShaderProgram.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class PostProcessing
	{
		public:
			PostProcessing()= default;

			virtual ~PostProcessing();

			virtual const std::string& getName() const= 0;

			virtual void init()= 0;

			virtual void processImage(
					DrawingPanel&        drawPanel,
					const RenderTexture& renderTexture,
					gl::GLuint           imageTexture
			) const= 0;

			void setUniform(const std::string& name, std::any data);

			std::any getUniform(const std::string& name) const;

			const ShaderProgram& getShader() const;
		protected:
			virtual const char* const getVertexShaderSrcCode() const;
			virtual const char* const getFragmentShaderSrcCode() const= 0;

			ShaderProgram m_shaderProgram;
	};

} // namespace AstralRaytracer