#pragma once
#include "TextureData.h"
#include "Utils/Common.h"

#include <filesystem>
#include <glbinding/gl/gl.h>
#include <stb_image.h>

class TextureManager
{
	public:
	TextureManager()                     = delete;
	TextureManager(const TextureManager&)= delete;
	static TextureData loadTextureDataFromFile(const std::filesystem::path& path);
	static uint32      loadTextureFromTextureData(TextureData& textureData, bool gamma);
	static void        updateTexture(const TextureData& textureData, uint32 textureId);
	static void        resizeTexture(const TextureData& textureData, uint32 textureId);
	static gl::GLenum  getTextureFormatFromData(TextureData& textureData);
	static gl::GLenum  getTextureFormatFromData(uint8 componentCount);

	private:
	static void loadTextureFromRawData(const stbi_uc* const data, uint32 width, uint32 height,
																		 uint32 textureID);
};