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

		void      render(const Scene& scene, const Camera& cam);
		glm::vec3 perPixel(const uint32 bounceCount, uint32& seedVal, const Scene& scene,
											 glm::vec3& rayOrigin, glm::vec3& rayDir);

		uintptr getTextureId() const { return m_textureId; }
		bool    onResize(uint32 width, uint32 height);
		void    resetFrameIndex();

		private:
		TextureData            m_texData;
		std::vector<float32>   m_accumlatedColorData;
		std::vector<glm::vec3> m_cachedRayDirections;
		std::vector<uint32>    m_rayIterator;
		uint32                 m_textureId;
		uint32                 m_frameIndex= 1;
	};
} // namespace AstralRaytracer