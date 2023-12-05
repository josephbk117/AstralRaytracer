#include "Compositor/Compositor.h"

#include "Compositor/PostProcessing/PostProcessing.h"
#include "Raytracer/Scene.h"

namespace AstralRaytracer
{
	Compositor::Compositor() 
	{
		m_renderTexture1.init({ 32, 32 });
		m_renderTexture2.init({ 32, 32 });
		m_dwPanel.init(1.0f, 1.0f);
		m_dwPanel.setTextureID(m_renderTexture1.getTexture());
	}

	void Compositor::processImage(
			const Scene&        scene,
			const glm::u32vec2& resolution,
			uint32              inputTextureId
	)
	{
		m_renderTexture1.resize(resolution);
		m_renderTexture2.resize(resolution);

		m_outputTextureId= inputTextureId;

		if(scene.m_postProcessingStack.size() > 0)
		{
			scene.m_postProcessingStack[0]->processImage(m_dwPanel, m_renderTexture1, inputTextureId);
			scene.m_postProcessingStack[0]->setMainColorTexture(inputTextureId);
			m_outputTextureId= m_renderTexture1.getTexture();
			for(uint32 index= 1; index < scene.m_postProcessingStack.size(); ++index)
			{
				const bool isOddIndex= (index - 1) % 2 != 0;

				const RenderTexture& selectedRenderTex= (isOddIndex) ? m_renderTexture1 : m_renderTexture2;

				const uint32 selectedInputTex=
						(isOddIndex) ? m_renderTexture2.getTexture() : m_renderTexture1.getTexture();

				scene.m_postProcessingStack[index]
						->setMainColorTexture(inputTextureId);
				scene.m_postProcessingStack[index]->processImage(
						m_dwPanel, selectedRenderTex, selectedInputTex
				);
				m_outputTextureId= selectedRenderTex.getTexture();
			}
		}
	}

} // namespace AstralRaytracer