#pragma once
#include "TextureData.h"
#include <glbinding/gl/gl.h>
class TextureManager
{
public:
	TextureManager() = delete;
	TextureManager(const TextureManager&) = delete;
	static unsigned int loadTextureFromData(TextureData & textureData, bool gamma);
	static gl::GLenum getTextureFormatFromData(TextureData & textureData);
	static gl::GLenum getTextureFormatFromData(int componentCount);
};