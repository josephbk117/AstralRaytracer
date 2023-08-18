#include "Raytracer/TextureManager.h"

#include "Raytracer/TextureData.h"

#include <iostream>

uint32 TextureManager::loadTextureFromData(TextureData& textureData, bool gamma)
{
	uint32 textureID;
	gl::glGenTextures(1, &textureID);
	const uint8* const data= textureData.getTextureData().data();
	if(data)
	{
		gl::glPixelStorei(gl::GL_UNPACK_ALIGNMENT, 1);
		gl::glBindTexture(gl::GL_TEXTURE_2D, textureID);
		gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB, textureData.getWidth(),
										 textureData.getHeight(), 0, gl::GL_RGB, gl::GL_UNSIGNED_BYTE, data);

		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
	}
	else
	{
		std::cout << "\nTexture failed to load with texture data " << std::endl;
	}
	return textureID;
}

void TextureManager::updateTexture(const TextureData& textureData, uint32 textureId)
{
	gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
	gl::glTexSubImage2D(gl::GL_TEXTURE_2D, 0, 0, 0, textureData.getWidth(), textureData.getHeight(),
											TextureManager::getTextureFormatFromData(textureData.getComponentCount()),
											gl::GL_UNSIGNED_BYTE, textureData.getTextureData().data());
	gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
}

void TextureManager::resizeTexture(const TextureData& textureData, uint32 textureId)
{
	gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
	gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB, textureData.getWidth(),
									 textureData.getHeight(), 0, gl::GL_RGB, gl::GL_UNSIGNED_BYTE,
									 textureData.getTextureData().data());
	gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
}

gl::GLenum TextureManager::getTextureFormatFromData(TextureData& textureData)
{
	using namespace gl;
	GLenum format= GL_RED;
	if(textureData.getComponentCount() == 1)
		format= GL_RED;
	else if(textureData.getComponentCount() == 3)
		format= GL_RGB;
	else if(textureData.getComponentCount() == 4)
		format= GL_RGBA;
	return format;
}

gl::GLenum TextureManager::getTextureFormatFromData(uint8 componentCount)
{
	using namespace gl;
	GLenum format= GL_RED;
	if(componentCount == 1)
		format= GL_RED;
	else if(componentCount == 3)
		format= GL_RGB;
	else if(componentCount == 4)
		format= GL_RGBA;
	return format;
}