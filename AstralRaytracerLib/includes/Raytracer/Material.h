#pragma once
#include "Utils/Serialization.h"

namespace AstralRaytracer
{
	class Material: public Serialization::Serializable
	{
		public:
			Material()= default;

			Material(
					const ColourData& inAlbedo,
					const ColourData& inEmission,
					float32           inEmissionStr,
					float32           inRoughness
			)
					: albedo(inAlbedo),
						emission(inEmission),
						emissionStrength(inEmissionStr),
						roughness(inRoughness)
			{
			}

			Material(
					const ColourData& inAlbedo,
					const ColourData& inEmission,
					float32           inEmissionStr,
					float32           inRoughness,
					uint32            inTexture
			)
					: albedo(inAlbedo),
						emission(inEmission),
						emissionStrength(inEmissionStr),
						roughness(inRoughness),
						texture(inTexture)
			{
			}

			ColourData albedo          = Colors::Pink;
			ColourData emission        = Colors::White;
			float32    emissionStrength= 0.0f;
			float32    roughness       = 0.5;
			uint32     texture         = 0;

			glm::vec3 getEmission() const;
			void      serialize(AssetManager& assetManager, YAML::Emitter& out) const override;
			void      deserialize(AssetManager& assetManager, YAML::Node& node) override;
	};
} // namespace AstralRaytracer