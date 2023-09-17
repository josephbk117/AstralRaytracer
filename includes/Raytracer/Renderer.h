#include "Camera.h"
#include "Scene.h"
#include "TextureData.h"
#include "Utils/Common.h"

namespace AstralRaytracer
{
	class Renderer
	{
		public:
		Renderer();

		void render(const Scene& scene, const Camera& cam);

		const TextureData& getTextureData() const { return m_texData; }

		glm::vec3 getRayDirectionFromNormalizedCoord(glm::vec2        coord,
																								 const glm::mat4& inverseProjection,
																								 const glm::mat4& inverseView);

		glm::vec3 perPixel(uint32& seedVal, const Scene& scene, glm::vec3& rayOrigin,
											 glm::vec3& rayDir);

		void findClosestHit(HitInfo& closestHitInfo, const Scene& scene, const glm::vec3& rayOrigin,
												const glm::vec3& rayDir);

		uintptr getTextureId() const { return m_textureId; }
		void    setBounceCount(uint32 count) { m_BounceCount= count; }
		uint32  getBounceCount() const { return m_BounceCount; }
		bool    onResize(uint32 width, uint32 height);
		void    resetFrameIndex();

		private:
		TextureData          m_texData;
		std::vector<float32> m_accumlatedColorData;
		std::vector<uint32>  m_rayIterator;
		uint32               m_textureId;
		uint32               m_frameIndex = 1;
		uint32               m_BounceCount= 4;
	};
} // namespace AstralRaytracer