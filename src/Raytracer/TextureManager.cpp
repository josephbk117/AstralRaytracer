#include "Raytracer/TextureManager.h"

#include "Raytracer/TextureData.h"

#include <iostream>
uint32 TextureManager::loadTextureFromData(TextureData& textureData, bool gamma)
{
	using namespace gl;
	uint32 textureID;
	glGenTextures(1, &textureID);
	const uint8* const data= textureData.getTextureData().data();
	if(data)
	{
		GLenum format= GL_RED;
		if(textureData.getComponentCount() == 1)
			format= GL_RED;
		else if(textureData.getComponentCount() == 3)
			format= GL_RGB;
		else if(textureData.getComponentCount() == 4)
			format= GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, textureData.getWidth(), textureData.getHeight(), 0,
								 format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
#if _DEBUG
	else
		std::cout << "\nTexture failed to load with texture data " << std::endl;
#endif
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