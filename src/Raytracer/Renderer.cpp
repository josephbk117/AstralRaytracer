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
			glm::vec3 targetNormalized = glm::normalize(glm::vec3(target) / target.w);
			glm::vec3 rayDir = glm::vec3(cam.getInverseView() * glm::vec4(targetNormalized, 0));

			float32 radius = 0.5f;

			glm::vec3 rayDirection(coord.x, coord.y, -1.0f);

			float32 a = glm::dot(rayDir, rayDir);
			float32 b = 2.0f * glm::dot(cam.getPosition(), rayDir);
			float32 c = glm::dot(cam.getPosition(), cam.getPosition()) - radius * radius;

			float32 discriminant = b * b - 4.0f * a * c;

			if (discriminant >= 0.0f)
			{
				float32 t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
				float32 closesrT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

				glm::vec3 hitPoint = cam.getPosition() + rayDir * closesrT;
				glm::vec3 normal = glm::normalize(hitPoint);

				glm::vec3 lightDir = glm::normalize(glm::vec3(-1,-1,-1));

				float32 d = (glm::dot(normal, -lightDir) + 1.0f) * 0.5f;

				m_texData.setTexelColorAtPixelIndex(index, d * 255.0f, d * 255.0f, d * 255.0f);
			}
			else
			{
				m_texData.setTexelColorAtPixelIndex(index, std::max(targetNormalized.x * 255, 0.0f), std::max(targetNormalized.y * 255, 0.0f), targetNormalized.z * 255);
			}
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