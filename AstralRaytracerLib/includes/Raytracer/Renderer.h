#include "Camera.h"
#include "DrawingPanel.h"
#include "RenderTexture.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "TextureData.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Renderer
	{
		public:
		Renderer();

		void render(const Scene& scene, const Camera& cam);

		const TextureDataRGBF& getTextureData() const { return m_texData; }

		glm::vec3 getRayDirectionFromNormalizedCoord(glm::vec2        coord,
																								 const glm::mat4& inverseProjection,
																								 const glm::mat4& inverseView) const;

		glm::vec3 perPixel(uint32& seedVal, const Scene& scene, glm::vec3& rayOrigin,
											 glm::vec3& rayDir);

		void findClosestHit(HitInfo& closestHitInfo, const Scene& scene, const glm::vec3& rayOrigin,
												const glm::vec3& rayDir) const;

		gl::GLuint getTextureId() const { return m_renderTexture.getTexture(); }
		void       setBounceCount(uint32 count) { m_BounceCount= count; }
		uint32     getBounceCount() const { return m_BounceCount; }
		uint32     getFrameIndex() const { return m_frameIndex; }
		bool       onResize(uint32 width, uint32 height);
		void       resetFrameIndex();

		private:
		TextureDataRGBF      m_texData;
		std::vector<float32> m_accumulatedColorData;
		float32              m_maxLuminance= 0;
		std::vector<uint32>  m_rayIterator;
		uint32               m_textureId;
		RenderTexture        m_renderTexture;
		DrawingPanel         m_dwPanel;
		ShaderProgram        m_shaderProgram;
		uint32               m_frameIndex = 1;
		uint32               m_BounceCount= 4;
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
										uniform mat4    model;

										void main()
										{
											gl_Position  = model * vec4(vertexPosition.xy, 0, 1.0);
											worldPos     = gl_Position.xyz;
											textureUV    = texCoords;
										}
										)SHADER";

		constexpr const char* FragmentShader= R"SHADER(
										#version 140
										in vec2				textureUV;
										in vec3				worldPos;
										out vec4			color;
										uniform sampler2D	textureOne;

										void main()
										{
											float spatialSigma = 1.0; // Controls spatial smoothing
											float rangeSigma = 1.0;   // Controls intensity smoothing

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

											float gamma = 2.2;
											color = vec4(pow(color.rgb, vec3(1.0 / gamma)), 1.0);
										}
										)SHADER";
	} // namespace ShaderLiterals
} // namespace AstralRaytracer