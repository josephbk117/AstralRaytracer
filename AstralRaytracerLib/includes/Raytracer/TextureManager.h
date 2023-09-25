#pragma once
#include "TextureData.h"
#include "Utils/Common.h"

#include <filesystem>
#include <glbinding/gl/gl.h>

class TextureManager
{
	public:
	TextureManager()                     = delete;
	TextureManager(const TextureManager&)= delete;
	static TextureDataRGB  loadTextureDataFromFileRGB(const std::filesystem::path& path);
	static TextureDataRGBA loadTextureDataFromFileRGBA(const std::filesystem::path& path);
	static TextureDataRGBF loadTextureDataFromFileRGBF(const std::filesystem::path& path);
	static uint32          loadTextureFromTextureData(TextureDataRGBF& textureData, bool gamma);
	static void            updateTexture(const TextureDataRGBF& textureData, uint32 textureId);
	static void            resizeTexture(const TextureDataRGBF& textureData, uint32 textureId);
	static gl::GLenum      getTextureFormatFromData(TextureDataRGBF& textureData);
	static gl::GLenum      getTextureFormatFromData(uint8 componentCount);

	private:
	static void loadTextureFromRawData(const float32* const data, uint32 width, uint32 height,
																		 uint32 textureID);
};