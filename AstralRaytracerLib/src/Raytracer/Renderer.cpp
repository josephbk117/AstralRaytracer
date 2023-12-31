#include "Raytracer/Renderer.h"

#include "Compositor/PostProcessing/PostProcessing.h"
#include "Raytracer/Scene.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/StaticMesh.h"
#include "Raytracer/Traceable/TriangleTraceable.h"
#include "Utils/Random.h"
#include "Utils/TextureManager.h"

#ifdef SUPPORT_STD_EXECUTION
	#include <execution>
#endif

namespace AstralRaytracer
{

	Renderer::Renderer() { }

	Renderer::~Renderer() { }

	void Renderer::initialize()
	{
		constexpr uint32 initialWidth = 16;
		constexpr uint32 initialHeight= 16;

		// Made initial resolution small so that OnResize can run
		m_texData  = TextureDataRGBF(initialWidth, initialHeight);
		m_textureId= TextureManager::loadTextureFromTextureData(m_texData, false);

		onResize(32, 32);
	}

	void Renderer::render(const Scene& scene, const Camera& cam)
	{
		onResize(cam.getResolution().x, cam.getResolution().y);

		const uint32     xAxisPixelCount  = m_texData.getWidth() * m_texData.getComponentCount();
		const float32    imageHeight      = m_texData.getHeight();
		const glm::mat4& inverseView      = cam.getInverseView();
		const glm::mat4& inverseProjection= cam.getInverseProjection();

		const float32 oneOverBounceCount= 1.0f / m_BounceCount;
		const float32 oneOverFrameIndex = 1.0f / m_frameIndex;

#ifdef SUPPORT_STD_EXECUTION
		std::for_each(
				std::execution::par_unseq, m_rayIterator.begin(), m_rayIterator.end(),
#else
		std::for_each(
				m_rayIterator.begin(), m_rayIterator.end(),
#endif // SUPPORT_STD_EXECUTION,
				[this, xAxisPixelCount, imageHeight, oneOverBounceCount, oneOverFrameIndex, &inverseView,
				 &inverseProjection, &scene, &cam](uint32 index)
				{
					uint32       seedVal         = index * m_frameIndex;
					const uint32 pixelAccessIndex= index * 3;

					const float32 randFloat1= (Random::randomFloat(seedVal) * 2.0f) - 1.0f;
					const float32 randFloat2= (Random::randomFloat(seedVal) * 2.0f) - 1.0f;

					const float32 xIndex= (pixelAccessIndex % xAxisPixelCount) + randFloat1;
					const float32 yIndex=
							static_cast<float32>(pixelAccessIndex / xAxisPixelCount) + randFloat2;

					const glm::vec2 coOrd{ xIndex / xAxisPixelCount, yIndex / imageHeight };

					glm::vec3 rayOrigin= cam.getPosition();

					// Depth of Field: Adjust the ray origin based on aperture size
					if(cam.getApertureDiameter() > 0.0f)
					{
						glm::vec3 apertureSample= Random::unitDisk(seedVal) * cam.getApertureDiameter();
						glm::vec3 apertureOffset=
								glm::normalize(glm::cross(cam.getUp(), cam.getDirection())) * apertureSample.x +
								cam.getUp() * apertureSample.y;

						rayOrigin+= apertureOffset;
					}

					glm::vec3 rayDir=
							getRayDirectionFromNormalizedCoord(coOrd, inverseProjection, inverseView);

					// Adjust ray direction
					glm::vec3 focalPoint= cam.getPosition() + cam.getFocusDistance() * rayDir;
					rayDir              = glm::normalize(focalPoint - rayOrigin);

					const glm::vec3& outColor=
							perPixel(seedVal, scene, rayOrigin, rayDir) * oneOverBounceCount;

					float32& redChannel  = m_accumulatedColorData[pixelAccessIndex];
					float32& greenChannel= m_accumulatedColorData[pixelAccessIndex + 1u];
					float32& blueChannel = m_accumulatedColorData[pixelAccessIndex + 2u];

					redChannel+= outColor.r;
					greenChannel+= outColor.g;
					blueChannel+= outColor.b;

					const glm::vec3  finalColorVec(redChannel, greenChannel, blueChannel);
					const glm::vec3& finalColorData= finalColorVec * oneOverFrameIndex;
					m_texData.setTexelColorAtPixelIndex(pixelAccessIndex, finalColorData);
				}
		);

		TextureManager::updateTexture(m_texData, m_textureId);
		++m_frameIndex;
	}

	glm::vec3 Renderer::getRayDirectionFromNormalizedCoord(
			glm::vec2        coord,
			const glm::mat4& inverseProjection,
			const glm::mat4& inverseView
	) const
	{
		coord                            = (coord * 2.0f) - 1.0f;
		const glm::vec4& target          = inverseProjection * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
		const glm::vec3& targetNormalized= glm::normalize(glm::vec3(target) / target.w);

		return glm::vec3(inverseView * glm::vec4(targetNormalized, 0.0f));
	}

	bool Renderer::onResize(uint32 width, uint32 height)
	{
		if(m_texData.getWidth() == width && m_texData.getHeight() == height)
		{
			return false;
		}
		m_texData.resize(width, height);

		const size_t newSizePixelCount= width * height;
		m_accumulatedColorData.resize(newSizePixelCount * 3);
		resetFrameIndex();

		m_rayIterator.resize(newSizePixelCount);
		std::iota(m_rayIterator.begin(), m_rayIterator.end(), 0);

		TextureManager::resizeTexture(m_texData, m_textureId);

		return true;
	}

	void Renderer::resetFrameIndex()
	{
		m_frameIndex= 1;
		std::memset(
				m_accumulatedColorData.data(), 0,
				m_texData.getWidth() * m_texData.getHeight() * 3 * sizeof(float32)
		);
	}

	glm::vec3
	Renderer::perPixel(uint32& seedVal, const Scene& scene, glm::vec3& rayOrigin, glm::vec3& rayDir)
			const
	{
		glm::vec3 light(0.0f);
		glm::vec3 contribution(1.0f);

		constexpr float32 kEpsilon= std::numeric_limits<float32>::epsilon();

		const TextureDataRGBF& skyTexData= scene.m_textures[scene.m_textures.size() - 1];

		for(uint32 bounceIndex= 0; bounceIndex < m_BounceCount; ++bounceIndex)
		{
			seedVal+= bounceIndex;
			HitInfo closestHitInfo;

			findClosestHit(closestHitInfo, scene, rayOrigin, rayDir);

			if(closestHitInfo.isValid())
			{
				const Material& mat= scene.m_materials[closestHitInfo.materialIndex];

				const TextureDataRGBF& texData= scene.m_textures[mat.texture];

				glm::vec3 colorData= texData.getTexelColor(
						closestHitInfo.worldSpacePosition.x * 0.1f, closestHitInfo.worldSpacePosition.z * 0.1f
				);

				contribution*= mat.albedo.getColour_32_bit() * colorData;
				light+= mat.getEmission() * colorData;

				// Avoid self-intersection by moving the ray origin slightly
				rayOrigin= closestHitInfo.worldSpacePosition + closestHitInfo.worldSpaceNormal * kEpsilon;

				// Compute the new ray direction (reflection)
				const glm::vec3 roughnessOffset=
						mat.roughness * Random::unitHemiSphere(seedVal, closestHitInfo.worldSpaceNormal);
				rayDir= glm::reflect(rayDir, closestHitInfo.worldSpaceNormal + roughnessOffset);
			}
			else // No hit, Render sky
			{
				constexpr float32 PI   = std::numbers::pi;
				const float32     theta= glm::acos(-rayDir.y);          // Polar angle (latitude)
				const float32     phi  = glm::atan(rayDir.z, rayDir.x); // Azimuthal angle (longitude)

				const float32 u= (phi + PI) / (2.0f * PI); // Map longitude to U
				const float32 v= 1.0f - (theta / PI);      // Map latitude to V

				// No intersection; add background color

				const glm::vec3& skyColor= skyTexData.getTexelColor(u, v);
				light+= skyColor * contribution;
			}
		}

		return light;
	}

	void Renderer::findClosestHit(
			HitInfo&         closestHitInfo,
			const Scene&     scene,
			const glm::vec3& rayOrigin,
			const glm::vec3& rayDir
	) const
	{
		// Initialize closest hit distance to a large value
		closestHitInfo.hitDistance= std::numeric_limits<float32>::max();

		for(uint32 traceableIndex= 0; traceableIndex < scene.m_sceneTraceables.size(); ++traceableIndex)
		{
			HitInfo hitInfo;
			if(scene.m_sceneTraceables[traceableIndex]->trace({ rayOrigin, rayDir }, hitInfo) &&
				 hitInfo.hitDistance < closestHitInfo.hitDistance)
			{
				hitInfo.objectIndex= traceableIndex;
				closestHitInfo     = hitInfo;
			}
		}
	}

} // namespace AstralRaytracer