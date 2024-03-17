#pragma once
#include "Compositor/PostProcessing/NoAlphaPassThroughPostProcess.h"
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
			void initialize();
			void processImage(const Scene& scene, const Resolution& resolution, uint32 inputTextureId);

			[[nodiscard]]
			TextureId getTextureId() const;
		private:
			TextureId m_outputTextureId= 0;

			RenderTexture m_renderTexture1;
			RenderTexture m_renderTexture2;
			DrawingPanel  m_dwPanel;

			NoAlphaPassThroughPostProcessing m_noAlphaPassThrough;
	};
} // namespace AstralRaytracer