#include "Raytracer/Renderer.h"

#include "Raytracer/ModelManager.h"
#include "Raytracer/Scene.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/StaticMesh.h"
#include "Raytracer/Traceable/TriangleTraceable.h"
#include "Utils/Random.h"

namespace AstralRaytracer
{

	Renderer::Renderer()
	{
		m_texData= TextureData(500, 500, 3);
		m_accumlatedColorData.resize(500 * 500 * 3);
		std::memset(m_accumlatedColorData.data(), 0, 500 * 500 * 3 * sizeof(float32));
		m_textureId= TextureManager::loadTextureFromData(m_texData, false);
	}

	void Renderer::render(const Scene& scene, const Camera& cam)
	{
		const uint32 xTotalPixelCount= m_texData.getWidth() * m_texData.getComponentCount();

		for(uint32 index= 0; index < xTotalPixelCount * m_texData.getHeight();
				index+= m_texData.getComponentCount())
		{
			uint32    seedVal= index * m_frameIndex;
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

			const uint32 bounceCount= 4;

			for(uint32 bounceIndex= 0; bounceIndex < bounceCount; ++bounceIndex)
			{
				seedVal+= bounceIndex;
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
							d * scene.m_materials.at(closestHitInfo.materialIndex).albedo.getColour_32_bit();
					outColor+= colorData.getColour_32_bit();

					rayOrigin= closestHitInfo.worldSpacePosition + closestHitInfo.worldSpaceNormal * 0.001f;
					rayDir   = glm::reflect(rayDir,
																	closestHitInfo.worldSpaceNormal +
																			scene.m_materials.at(closestHitInfo.materialIndex).roughness *
																					Random::unitSphere(seedVal));
				}
				else
				{
					outColor+= glm::vec3(0.25f, 0.5f, 1.0f);
				}
			}

			m_accumlatedColorData[index]+= outColor.r / bounceCount;
			m_accumlatedColorData[index + 1]+= outColor.g / bounceCount;
			m_accumlatedColorData[index + 2]+= outColor.b / bounceCount;

			const glm::vec3& finalColorVec=
					glm::vec3(m_accumlatedColorData[index], m_accumlatedColorData[index + 1],
										m_accumlatedColorData[index + 2]) /
					(float32)m_frameIndex;
			const glm::u8vec3& finalColorData= ColourData(finalColorVec).getColour_8_BitClamped();
			m_texData.setTexelColorAtPixelIndex(index, finalColorData.r, finalColorData.g,
																					finalColorData.b);
		}

		TextureManager::updateTexture(m_texData, m_textureId);
		++m_frameIndex;
	}

	void Renderer::onResize(uint32 width, uint32 height)
	{
		if(m_texData.getWidth() == width && m_texData.getHeight() == height)
		{
			return;
		}

		m_accumlatedColorData.resize(width * height * 3);
		m_texData.resize(width, height);
		TextureManager::updateTexture(m_texData, m_textureId);
	}

	void Renderer::resetFrameIndex()
	{
		m_frameIndex= 1;
		std::memset(m_accumlatedColorData.data(), 0, 500 * 500 * 3 * sizeof(float32));
	}

} // namespace AstralRaytracer