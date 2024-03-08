#pragma once
#include "Common.h"
#include "TextureData.h"

#include <filesystem>
#include <glbinding/gl/gl.h>

namespace AstralRaytracer
{
	class TextureManager
	{
		public:
			TextureManager()                     = delete;
			TextureManager(const TextureManager&)= delete;

			template<ArithMeticType T, uint32 componentCount>
			static void loadTextureDataFromFile(
					const std::filesystem::path&    path,
					TextureData<T, componentCount>& textureData,
					bool                            gamma
			);

			[[nodiscard]]
			static uint32 loadTextureFromTextureData(TextureDataRGBF& textureData, bool gamma);
			static void   updateTexture(const TextureDataRGBF& textureData, uint32 textureId);
			static void   resizeTexture(const TextureDataRGBF& textureData, uint32 textureId);
			static void saveTextureToFile(const TextureDataRGBF& data, const std::filesystem::path& path);
			[[nodiscard]]
			static gl::GLenum getTextureFormatFromData(TextureDataRGBF& textureData);
			[[nodiscard]]
			static gl::GLenum getTextureFormatFromData(uint32 componentCount);
		private:
			static void loadTextureFromRawData(
					const float32* const data,
					uint32               width,
					uint32               height,
					uint32               textureID
			);
	};

} // namespace AstralRaytracer