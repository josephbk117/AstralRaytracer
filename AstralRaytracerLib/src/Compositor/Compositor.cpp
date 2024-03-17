#include "Compositor/Compositor.h"

#include "Compositor/PostProcessing/PostProcessing.h"
#include "Raytracer/Scene.h"

namespace AstralRaytracer
{
	Compositor::Compositor() { }

	void Compositor::initialize()
	{
		m_outputTextureId= 0;

		const Resolution initialResolution{ 32, 32 };

		m_renderTexture1.init(initialResolution);
		m_renderTexture2.init(initialResolution);
		m_noAlphaPassThrough.init();
		m_dwPanel.init(1.0f, 1.0f);
		m_dwPanel.setTextureID(m_renderTexture1.getTexture());
	}

	void
	Compositor::processImage(const Scene& scene, const Resolution& resolution, TextureId inputTextureId)
	{
		m_renderTexture1.resize(resolution);
		m_renderTexture2.resize(resolution);

		m_outputTextureId= inputTextureId;

		// Using noAlphaPassThrough since depth is stored in alpha channel, prevents transparency

		m_noAlphaPassThrough.processImage(m_dwPanel, m_renderTexture1, inputTextureId);
		m_outputTextureId= m_renderTexture1.getTexture();

		if(scene.m_postProcessingStack.size() <= 0)
		{
			return;
		}

		scene.m_postProcessingStack[0]->processImage(m_dwPanel, m_renderTexture1, inputTextureId);
		scene.m_postProcessingStack[0]->setMainColorTexture(inputTextureId);
		m_outputTextureId= m_renderTexture1.getTexture();
		for(uint32 index= 1; index < scene.m_postProcessingStack.size(); ++index)
		{
			const bool isOddIndex= (index - 1) % 2 != 0;

			const RenderTexture& selectedRenderTex= (isOddIndex) ? m_renderTexture1 : m_renderTexture2;

			const uint32 selectedInputTex=
					(isOddIndex) ? m_renderTexture2.getTexture() : m_renderTexture1.getTexture();

			scene.m_postProcessingStack[index]->setMainColorTexture(inputTextureId);
			scene.m_postProcessingStack[index]->processImage(
					m_dwPanel, selectedRenderTex, selectedInputTex
			);
			m_outputTextureId= selectedRenderTex.getTexture();
		}
	}

	TextureId Compositor::getTextureId() const { return m_outputTextureId; }

} // namespace AstralRaytracer