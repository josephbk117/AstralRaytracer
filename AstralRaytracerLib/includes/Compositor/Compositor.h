#pragma once
#include "DrawingPanel.h"
#include "RenderTexture.h"

namespace AstralRaytracer
{
	class Scene;
}

namespace AstralRaytracer
{
	class Compositor
	{
		public:
			Compositor();

			void processImage(const Scene& scene, const glm::u32vec2& resolution, uint32 inputTextureId);

			[[nodiscard]]
			uint32 getTextureId() const;
		private:
			uint32 m_outputTextureId= 0;

			RenderTexture m_renderTexture1;
			RenderTexture m_renderTexture2;
			DrawingPanel  m_dwPanel;
	};
} // namespace AstralRaytracer