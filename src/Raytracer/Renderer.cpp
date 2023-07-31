#include "Raytracer/Renderer.h"
#include "Raytracer/TextureManager.h"

namespace AstralRaytracer
{

	Renderer::Renderer()
	{
		m_texData = TextureData(500, 500, 3);
		m_textureId = TextureManager::loadTextureFromData(m_texData, false);
	}

	void Renderer::render(const Camera& cam)
	{
		const uint32 xTotalPixelCount = m_texData.getWidth() * m_texData.getComponentCount();

		for (uint32 index = 0; index < xTotalPixelCount * m_texData.getHeight(); index += m_texData.getComponentCount())
		{
			glm::vec2 coord { (index % xTotalPixelCount) / (float32)xTotalPixelCount, (index / xTotalPixelCount) / (float32)m_texData.getHeight() };
			coord = (coord * 2.0f) - 1.0f;

			glm::vec4 target = cam.getInverseProjection() * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
			glm::vec3 rayDir = glm::vec3(cam.getInverseView() * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));

			m_texData.setTexelColorAtPixelIndex(index, rayDir.x * 255, rayDir.y * 255, rayDir.z * 255);
		}

		TextureManager::updateTexture(m_texData, m_textureId);
	}

	void Renderer::onResize(uint32 width, uint32 height)
	{
		if (m_texData.getWidth() == width && m_texData.getHeight() == height)
		{
			return;
		}

		m_texData.resize(width, height);
		TextureManager::updateTexture(m_texData, m_textureId);
	}

}