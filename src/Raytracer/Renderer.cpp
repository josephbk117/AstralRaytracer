#include "Raytracer/Renderer.h"

#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/TriangleTraceable.h"

namespace AstralRaytracer
{

	Renderer::Renderer()
	{
		m_texData  = TextureData(500, 500, 3);
		m_textureId= TextureManager::loadTextureFromData(m_texData, false);
	}

	void Renderer::render(const Camera& cam)
	{
		const uint32 xTotalPixelCount= m_texData.getWidth() * m_texData.getComponentCount();

		SphereTraceable   sphere;
		TriangleTraceable triangle;

		for(uint32 index= 0; index < xTotalPixelCount * m_texData.getHeight();
				index+= m_texData.getComponentCount())
		{
			glm::vec2 coord{(index % xTotalPixelCount) / (float32)xTotalPixelCount,
											(index / xTotalPixelCount) / (float32)m_texData.getHeight()};
			coord= (coord * 2.0f) - 1.0f;

			const glm::vec4 target= cam.getInverseProjection() * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
			const glm::vec3 targetNormalized= glm::normalize(glm::vec3(target) / target.w);
			const glm::vec3 rayDir= glm::vec3(cam.getInverseView() * glm::vec4(targetNormalized, 0));

			HitInfo hitInfo;
			if(triangle.trace({cam.getPosition(), rayDir}, hitInfo))
			{
				const glm::u8vec3 color= hitInfo.colorData.getColour_8_Bit();
				m_texData.setTexelColorAtPixelIndex(index, color.r, color.g, color.b);
			}
			else
			{
				m_texData.setTexelColorAtPixelIndex(index, 0, 0, 0);
			}
		}

		TextureManager::updateTexture(m_texData, m_textureId);
	}

	void Renderer::onResize(uint32 width, uint32 height)
	{
		if(m_texData.getWidth() == width && m_texData.getHeight() == height)
		{
			return;
		}

		m_texData.resize(width, height);
		TextureManager::updateTexture(m_texData, m_textureId);
	}

} // namespace AstralRaytracer