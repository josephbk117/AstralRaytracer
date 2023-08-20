#include "Raytracer/Renderer.h"

#include "Raytracer/ModelManager.h"
#include "Raytracer/Scene.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/StaticMesh.h"
#include "Raytracer/Traceable/TriangleTraceable.h"
#include "Utils/Random.h"

#include <execution>

namespace AstralRaytracer
{

	Renderer::Renderer()
	{
		constexpr uint32 initialWidth = 16;
		constexpr uint32 initialHeight= 16;

		// Made initial resolution small so that OnResize can run
		m_texData  = TextureData(initialWidth, initialHeight, 3);
		m_textureId= TextureManager::loadTextureFromTextureData(m_texData, false);

		onResize(32, 32);
	}

	void Renderer::render(const Scene& scene, const Camera& cam)
	{
		const uint32     xAxisPixelCount  = m_texData.getWidth() * m_texData.getComponentCount();
		const float32    imageHeight      = m_texData.getHeight();
		const glm::mat4& inverseView      = cam.getInverseView();
		const glm::mat4& inverseProjection= cam.getInverseProjection();

		// Cache Ray directions
		if(m_frameIndex == 1)
		{
			std::for_each(
					std::execution::par, m_rayIterator.begin(), m_rayIterator.end(),
					[this, xAxisPixelCount, imageHeight, &inverseView, &inverseProjection](uint32 index)
					{
						const uint32 pixelIndex= index * 3;
						glm::vec2    coord{(pixelIndex % xAxisPixelCount) / (float32)xAxisPixelCount,
                            (pixelIndex / xAxisPixelCount) / imageHeight};
						coord                  = (coord * 2.0f) - 1.0f;
						const glm::vec4& target= inverseProjection * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
						const glm::vec3& targetNormalized= glm::normalize(glm::vec3(target) / target.w);

						m_cachedRayDirections[index]=
								glm::vec3(inverseView * glm::vec4(targetNormalized, 0.0f));
					});
		}

		const float32 oneOverBounceCount= 1.0f / m_BounceCount;
		const float32 oneOverFrameIndex = 1.0f / m_frameIndex;

		std::for_each(std::execution::par, m_rayIterator.begin(), m_rayIterator.end(),
									[this, oneOverBounceCount, oneOverFrameIndex, &scene, &cam](uint32 index)
									{
										uint32    seedVal  = index * m_frameIndex;
										glm::vec3 rayOrigin= cam.getPosition();
										glm::vec3 rayDir   = m_cachedRayDirections[index];

										const glm::vec3& outColor= perPixel(seedVal, scene, rayOrigin, rayDir);

										const uint32 pixelAcessIndex= index * 3;

										float32& redChannel  = m_accumlatedColorData[pixelAcessIndex];
										float32& greenChannel= m_accumlatedColorData[pixelAcessIndex + 1u];
										float32& blueChannel = m_accumlatedColorData[pixelAcessIndex + 2u];

										redChannel+= (outColor.r * oneOverBounceCount);
										greenChannel+= (outColor.g * oneOverBounceCount);
										blueChannel+= (outColor.b * oneOverBounceCount);

										const glm::vec3    finalColorVec(redChannel, greenChannel, blueChannel);
										const glm::u8vec3& finalColorData=
												ColourData(finalColorVec * oneOverFrameIndex).getColour_8_BitClamped();
										m_texData.setTexelColorAtPixelIndex(pixelAcessIndex, finalColorData);
									});

		TextureManager::updateTexture(m_texData, m_textureId);
		++m_frameIndex;
	}

	bool Renderer::onResize(uint32 width, uint32 height)
	{
		if(m_texData.getWidth() == width && m_texData.getHeight() == height)
		{
			return false;
		}
		m_texData.resize(width, height);

		m_accumlatedColorData.resize(width * height * 3);
		resetFrameIndex();
		m_cachedRayDirections.resize(width * height);
		m_rayIterator.resize(width * height);

		for(uint32 index= 0; index < width * height; ++index)
		{
			m_rayIterator[index]= index;
		}

		TextureManager::resizeTexture(m_texData, m_textureId);

		return true;
	}

	void Renderer::resetFrameIndex()
	{
		m_frameIndex= 1;
		std::memset(m_accumlatedColorData.data(), 0,
								m_texData.getWidth() * m_texData.getHeight() * 3 * sizeof(float32));
	}

	glm::vec3 Renderer::perPixel(uint32& seedVal, const Scene& scene, glm::vec3& rayOrigin,
															 glm::vec3& rayDir)
	{
		const glm::vec3& lightDir= glm::normalize(glm::vec3(-0.5f, 1, 1));
		glm::vec3        outColor(0.0f);

		for(uint32 bounceIndex= 0; bounceIndex < m_BounceCount; ++bounceIndex)
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
				const float32      d  = (glm::dot(closestHitInfo.worldSpaceNormal, lightDir) + 1.0f) * 0.5f;
				const Material&    mat= scene.m_materials.at(closestHitInfo.materialIndex);
				const TextureData& texData= scene.m_textures.at(mat.texture);

				const ColourData& colorData=
						texData.getTexelColor(closestHitInfo.worldSpacePosition.x,
																	closestHitInfo.worldSpacePosition.z);
				outColor+= d * mat.albedo.getColour_32_bit() * colorData.getColour_32_bit();

				rayOrigin= closestHitInfo.worldSpacePosition + closestHitInfo.worldSpaceNormal * 0.0001f;

				rayDir= glm::reflect(
						rayDir,
						closestHitInfo.worldSpaceNormal +
								mat.roughness * Random::unitHemiSphere(seedVal, closestHitInfo.worldSpaceNormal));
			}
			else
			{
				outColor+= glm::vec3(0.25f, 0.5f, 1.0f);
			}
		}

		return outColor;
	}

} // namespace AstralRaytracer