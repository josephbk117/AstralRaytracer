#include "Utils/Common.h"
#include "TextureData.h"
#include "Camera.h"

namespace AstralRaytracer
{
	class Renderer
	{
	public:
	Renderer();

	void render(const Camera& cam);
	uint32 getTextureId() const { return m_textureId; }
	void onResize(uint32 width, uint32 height);

	private:
		TextureData m_texData;
		uint32 m_textureId;
	};
}