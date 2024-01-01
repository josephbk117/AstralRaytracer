#include "Camera.h"
#include "Scene.h"
#include "Utils/Common.h"
#include "Utils/ShaderProgram.h"
#include "Utils/TextureData.h"

namespace AstralRaytracer
{
	class Renderer
	{
		public:
			Renderer();
			~Renderer();

			void render(const Scene& scene, const Camera& cam);

			const TextureDataRGBF& getTextureData() const { return m_texData; }

			inline glm::vec3 getRayDirectionFromNormalizedCoord(
					glm::vec2        coord,
					const glm::mat4& inverseProjection,
					const glm::mat4& inverseView
			) const;

			glm::vec3
			perPixel(uint32& seedVal, const Scene& scene, glm::vec3& rayOrigin, glm::vec3& rayDir) const;

			void findClosestHit(
					HitInfo&         closestHitInfo,
					const Scene&     scene,
					const glm::vec3& rayOrigin,
					const glm::vec3& rayDir
			) const;

			gl::GLuint getTextureId() const { return m_textureId; }

			void setBounceCount(uint32 count) { m_BounceCount= count; }

			uint32 getBounceCount() const { return m_BounceCount; }

			uint32 getFrameIndex() const { return m_frameIndex; }

			bool onResize(uint32 width, uint32 height);
			void resetFrameIndex();
		private:
			TextureDataRGBF      m_texData;
			std::vector<float32> m_accumulatedColorData;
			float32              m_maxLuminance= 0;
			std::vector<uint32>  m_rayIterator;
			uint32               m_textureId  = 0;
			uint32               m_frameIndex = 1;
			uint32               m_BounceCount= 4;
	};
} // namespace AstralRaytracer