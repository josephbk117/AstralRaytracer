#pragma once
#include "Raytracer/DrawingPanel.h"
#include "Raytracer/RenderTexture.h"
#include "Raytracer/ShaderProgram.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class PostProcessing
	{
		public:
		PostProcessing()= default;
		virtual ~PostProcessing() {}

		virtual const std::string& getName() const= 0;

		virtual void init()= 0;

		virtual void processImage(DrawingPanel& drawPanel, const RenderTexture& renderTexture,
															gl::GLuint imageTexture)= 0;

		void setUniform(const std::string& name, std::any data);

		std::any getUniform(const std::string& name) const;

		const ShaderProgram& getShader() const;

		protected:
		virtual const char* const getVertexShaderSrcCode() const;
		virtual const char* const getFragmentShaderSrcCode() const= 0;

		ShaderProgram m_shaderProgram;
	};
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	namespace ShaderLiterals
	{
		constexpr const char* VertexShader= R"SHADER(
										#version 140
										in vec2			vertexPosition;
										in vec2         texCoords;
										out vec2        textureUV;
										out vec3        worldPos;

										void main()
										{
											gl_Position  = vec4(vertexPosition.xy, 0, 1.0);
											worldPos     = gl_Position.xyz;
											textureUV    = texCoords;
										}
										)SHADER";

		constexpr const char* GammaCorrectionFS= R"SHADER(
										#version 140
										in vec2				textureUV;
										in vec3				worldPos;
										out vec4			color;
										uniform sampler2D	textureOne;
										uniform float		gamma;

										void main()
										{
											// Sample the texture
											vec3 texColor = texture(textureOne, textureUV).rgb;
											color = vec4(pow(texColor, vec3(1.0 / gamma)), 1.0);
										}
										)SHADER";

		constexpr const char* BilateralFilterFS= R"SHADER(
										#version 140
										in vec2				textureUV;
										in vec3				worldPos;
										out vec4			color;
										uniform sampler2D	textureOne;
										uniform float		spatialSigma; // Controls spatial smoothing
										uniform float		rangeSigma;   // Controls intensity smoothing

										void main()
										{
											vec4 centerPixel = texture(textureOne, textureUV);
											vec3 result = vec3(0.0);
											float weightSum = 0.0;

											for (int i = -1; i <= 1; i++)
											{
												for (int j = -1; j <= 1; j++)
												{
													vec2 offset = vec2(i, j) / textureSize(textureOne, 0);
													vec4 neighborPixel = texture(textureOne, textureUV + offset);

													float spatialWeight = exp(-(length(offset) / spatialSigma) * (length(offset) / spatialSigma));
													float rangeWeight = exp(-(length(centerPixel.rgb - neighborPixel.rgb) / rangeSigma) * (length(centerPixel.rgb - neighborPixel.rgb) / rangeSigma));

													float weight = spatialWeight * rangeWeight;
													result += weight * neighborPixel.rgb;
													weightSum += weight;
												}
											}

											result /= weightSum;
											color = vec4(result, centerPixel.a);
										}
										)SHADER";
	} // namespace ShaderLiterals
} // namespace AstralRaytracer