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

		private:
		TextureData m_texData;
		uint32      m_textureId;
	};
} // namespace AstralRaytracer