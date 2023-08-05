#include "Raytracer/Renderer.h"

#include "Raytracer/Scene.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/TriangleTraceable.h"
#include "Raytracer/Traceable/StaticMesh.h"
#include "Raytracer/ModelManager.h"

namespace AstralRaytracer
{

	Renderer::Renderer()
	{
		m_texData  = TextureData(500, 500, 3);
		m_textureId= TextureManager::loadTextureFromData(m_texData, false);
	}

	void Renderer::render(const Scene& scene, const Camera& cam)
	{
		const uint32 xTotalPixelCount= m_texData.getWidth() * m_texData.getComponentCount();

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
			HitInfo closestHitInfo;
			for(uint32 objectIndex= 0; objectIndex < scene.m_sceneTraceables.size(); ++objectIndex)
			{
				if(scene.m_sceneTraceables[objectIndex]->trace({cam.getPosition(), rayDir}, hitInfo))
				{
					if(hitInfo.hitDistance < closestHitInfo.hitDistance)
					{
						closestHitInfo= hitInfo;
					}
				}
			}
			if(closestHitInfo.hitDistance > 0.0f &&
				 closestHitInfo.hitDistance < std::numeric_limits<float32>::max())
			{
				const glm::vec3 lightDir= glm::normalize(glm::vec3(-0.5f, -1, 1));
				float32         d       = (glm::dot(closestHitInfo.normal, -lightDir) + 1.0f) * 0.5f;
				const ColourData colorData= d * scene.m_materials.at(closestHitInfo.materialIndex).albedo.getColourIn_0_1_Range();
				const glm::u8vec3 color = colorData.getColour_8_Bit();
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