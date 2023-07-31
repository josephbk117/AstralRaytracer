#pragma once
#include "Utils/Common.h"
#include "TextureData.h"
#include <glbinding/gl/gl.h>
class TextureManager
{
public:
	TextureManager() = delete;
	TextureManager(const TextureManager&) = delete;
	static uint32 loadTextureFromData(TextureData & textureData, bool gamma);
	static void updateTexture(const TextureData& textureData, uint32 textureId);
	static gl::GLenum getTextureFormatFromData(TextureData & textureData);
	static gl::GLenum getTextureFormatFromData(uint8 componentCount);
};