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
			enum class RendererState
			{
				NOT_STARTED,
				STARTED,
				PROCESSING,
				EXIT,
				DONE
			};

			Renderer();
			~Renderer();

			void initialize();

			void renderStart(const Scene& scene, const Camera& cam);
			void renderEnd();
			bool onRenderComplete();

			const TextureDataRGBF& getTextureData() const { return m_texData; }

			glm::vec3 getRayDirectionFromNormalizedCoord(
					const glm::vec2& coord,
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
			void render(const Scene& scene, const Camera& cam);

			std::thread   m_renderingThread;
			RendererState m_state= RendererState::NOT_STARTED;

			TextureDataRGBF      m_texData;
			std::vector<float32> m_accumulatedColorData;
			float32              m_maxLuminance= 0;
			std::vector<uint32>  m_rayIterator;
			uint32               m_textureId  = 0;
			uint32               m_frameIndex = 1;
			uint32               m_BounceCount= 4;
	};
} // namespace AstralRaytracer