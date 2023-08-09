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

		void   render(const Scene& scene, const Camera& cam);
		uint32 getTextureId() const { return m_textureId; }
		void   onResize(uint32 width, uint32 height);
		void   resetFrameIndex();

		private:
		TextureData          m_texData;
		std::vector<float32> m_accumlatedColorData;
		uint32               m_textureId;
		uint32               m_frameIndex= 1;
	};
} // namespace AstralRaytracer