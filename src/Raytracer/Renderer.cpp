#include "Raytracer/Renderer.h"

#include "../../includes/Utils/Random.h"
#include "Raytracer/ModelManager.h"
#include "Raytracer/Scene.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/StaticMesh.h"
#include "Raytracer/Traceable/TriangleTraceable.h"

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
			uint32    seedVal= index;
			glm::vec2 coord{(index % xTotalPixelCount) / (float32)xTotalPixelCount,
											(index / xTotalPixelCount) / (float32)m_texData.getHeight()};
			coord= (coord * 2.0f) - 1.0f;

			const glm::vec4 target= cam.getInverseProjection() * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
			const glm::vec3 targetNormalized= glm::normalize(glm::vec3(target) / target.w);
			const glm::vec3 initialRayDir=
					glm::vec3(cam.getInverseView() * glm::vec4(targetNormalized, 0));
			const glm::vec3 initialRayOrigin= cam.getPosition();
			const glm::vec3 lightDir        = glm::normalize(glm::vec3(-0.5f, 1, 1));

			glm::vec3 outColor(0.0f);
			glm::vec3 rayOrigin= initialRayOrigin;
			glm::vec3 rayDir   = initialRayDir;

			const uint32 bounceCount= 2;

			for(uint32 bounceIndex= 0; bounceIndex < bounceCount; ++bounceIndex)
			{
				seedVal += bounceIndex;
				HitInfo closestHitInfo;
				for(uint32 objectIndex= 0; objectIndex < scene.m_sceneTraceables.size(); ++objectIndex)
				{
					HitInfo hitInfo;
					if(scene.m_sceneTraceables[objectIndex]->trace({rayOrigin, rayDir}, hitInfo))
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
					float32          d= (glm::dot(closestHitInfo.worldSpaceNormal, lightDir) + 1.0f) * 0.5f;
					const ColourData colorData=
							d * scene.m_materials.at(closestHitInfo.materialIndex).albedo.getColourIn_0_1_Range();
					outColor+= colorData.getColourIn_0_1_Range();

					rayOrigin= closestHitInfo.worldSpacePosition + closestHitInfo.worldSpaceNormal * 0.001f;
					rayDir   = glm::reflect(
              rayDir, closestHitInfo.worldSpaceNormal +
                          scene.m_materials.at(closestHitInfo.materialIndex).roughness *
                          Random::unitSphere(seedVal));
				}
				else
				{
					break;
				}
			}

			const glm::vec3 finalColorData= ColourData(outColor).getColourIn_0_1_Range();
			m_texData.setTexelColorAtPixelIndex(index, finalColorData.r * 255.0f / bounceCount,
																					finalColorData.g * 255.0f / bounceCount,
																					finalColorData.b * 255.0f / bounceCount);
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