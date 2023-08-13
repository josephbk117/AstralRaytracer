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
		m_cachedRayDirections.resize(500 * 500);
		m_textureId= TextureManager::loadTextureFromData(m_texData, false);
	}

	void Renderer::render(const Scene& scene, const Camera& cam)
	{
		const uint32 xAxisPixelCount   = m_texData.getWidth() * m_texData.getComponentCount();
		const uint32 totalPixelCount   = xAxisPixelCount * m_texData.getHeight();
		const uint32 pixelComponentSize= m_texData.getComponentCount();
		// Cache Ray directions
		if(m_frameIndex == 1)
		{
			for(uint32 index= 0; index < totalPixelCount; index+= pixelComponentSize)
			{
				glm::vec2 coord{(index % xAxisPixelCount) / (float32)xAxisPixelCount,
												(index / xAxisPixelCount) / (float32)m_texData.getHeight()};
				coord= (coord * 2.0f) - 1.0f;

				const glm::vec4& target=
						cam.getInverseProjection() * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
				const glm::vec3& targetNormalized= glm::normalize(glm::vec3(target) / target.w);
				m_cachedRayDirections[index / 3]=
						glm::vec3(cam.getInverseView() * glm::vec4(targetNormalized, 0.0f));
			}
		}

		constexpr uint32  bounceCount       = 4;
		constexpr float32 oneOverBounceCount= 1.0f / bounceCount;

		for(uint32 index= 0; index < totalPixelCount; index+= pixelComponentSize)
		{
			uint32    seedVal  = index * m_frameIndex;
			glm::vec3 rayOrigin= cam.getPosition();
			glm::vec3 rayDir   = m_cachedRayDirections[index / 3];

			const glm::vec3& outColor= perPixel(bounceCount, seedVal, scene, rayOrigin, rayDir);

			m_accumlatedColorData[index]+= outColor.r * oneOverBounceCount;
			m_accumlatedColorData[index + 1]+= outColor.g * oneOverBounceCount;
			m_accumlatedColorData[index + 2]+= outColor.b * oneOverBounceCount;

			const glm::vec3& finalColorVec=
					glm::vec3(m_accumlatedColorData[index], m_accumlatedColorData[index + 1],
										m_accumlatedColorData[index + 2]) /
					(float32)m_frameIndex;
			const glm::u8vec3& finalColorData= ColourData(finalColorVec).getColour_8_BitClamped();
			m_texData.setTexelColorAtPixelIndex(index, finalColorData);
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
		m_cachedRayDirections.resize(width * height);
		m_texData.resize(width, height);
		TextureManager::updateTexture(m_texData, m_textureId);
	}

	void Renderer::resetFrameIndex()
	{
		m_frameIndex= 1;
		std::memset(m_accumlatedColorData.data(), 0, 500 * 500 * 3 * sizeof(float32));
	}

	glm::vec3 Renderer::perPixel(const uint32 bounceCount, uint32& seedVal, const Scene& scene,
															 glm::vec3& rayOrigin, glm::vec3& rayDir)
	{
		const glm::vec3& lightDir= glm::normalize(glm::vec3(-0.5f, 1, 1));
		glm::vec3        outColor(0.0f);

		for(uint32 bounceIndex= 0; bounceIndex < bounceCount; ++bounceIndex)
		{
			seedVal+= bounceIndex;
			HitInfo closestHitInfo;
			for(uint32 objectIndex= 0; objectIndex < scene.m_sceneTraceables.size(); ++objectIndex)
			{
				HitInfo hitInfo;
				if(scene.m_sceneTraceables[objectIndex]->trace({rayOrigin, rayDir}, hitInfo) &&
					 hitInfo.hitDistance < closestHitInfo.hitDistance)
				{
					closestHitInfo= hitInfo;
				}
			}
			if(closestHitInfo.hitDistance > 0.0f &&
				 closestHitInfo.hitDistance < std::numeric_limits<float32>::max())
			{
				const float32   d  = (glm::dot(closestHitInfo.worldSpaceNormal, lightDir) + 1.0f) * 0.5f;
				const Material& mat= scene.m_materials.at(closestHitInfo.materialIndex);
				outColor+= d * mat.albedo.getColour_32_bit();

				rayOrigin= closestHitInfo.worldSpacePosition + closestHitInfo.worldSpaceNormal * 0.001f;
				rayDir   = glm::reflect(rayDir, closestHitInfo.worldSpaceNormal +
																						mat.roughness * Random::unitSphere(seedVal));
			}
			else
			{
				outColor+= glm::vec3(0.25f, 0.5f, 1.0f);
			}
		}

		return outColor;
	}

} // namespace AstralRaytracer