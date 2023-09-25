#include "Raytracer/TextureManager.h"

#include "Raytracer/TextureData.h"

#include <iostream>
#include <stbimage/stb_image.h>

TextureDataRGB TextureManager::loadTextureDataFromFileRGB(const std::filesystem::path& path)
{
	int32 width;
	int32 height;
	int32 numChannels;

	stbi_uc* data= stbi_load(path.string().c_str(), &width, &height, &numChannels, 3);
	assert(data);

	TextureDataRGB      texData(width, height);
	std::vector<uint8> vecData;
	vecData.resize(width * height * numChannels);

	std::memcpy(vecData.data(), data, width * height * numChannels);

	texData.setTextureData(vecData);
	stbi_image_free(data);
	return texData;
}

TextureDataRGBA TextureManager::loadTextureDataFromFileRGBA(const std::filesystem::path& path)
{
	int32 width;
	int32 height;
	int32 numChannels;

	stbi_uc* data= stbi_load(path.string().c_str(), &width, &height, &numChannels, 4);
	assert(data);

	TextureDataRGBA     texData(width, height);
	std::vector<uint8> vecData;
	vecData.resize(width * height * numChannels);

	std::memcpy(vecData.data(), data, width * height * numChannels);

	texData.setTextureData(vecData);
	stbi_image_free(data);
	return texData;
}

TextureDataRGBF TextureManager::loadTextureDataFromFileRGBF(const std::filesystem::path& path)
{
	int32 width;
	int32 height;
	int32 numChannels;

	float32* data= stbi_loadf(path.string().c_str(), &width, &height, &numChannels, 3);
	assert(data);

	TextureDataRGBF      texData(width, height);
	std::vector<float32> vecData;
	vecData.resize(width * height * numChannels);

	std::memcpy(vecData.data(), data, width * height * numChannels);

	texData.setTextureData(vecData);
	stbi_image_free(data);
	return texData;
}

uint32 TextureManager::loadTextureFromTextureData(TextureDataRGBF& textureData, bool gamma)
{
	uint32 textureID;
	gl::glGenTextures(1, &textureID);
	const float32* const data= textureData.getTextureData().data();
	assert(data);
	loadTextureFromRawData(data, textureData.getWidth(), textureData.getHeight(), textureID);
	return textureID;
}

void TextureManager::loadTextureFromRawData(const float32* const data, uint32 width, uint32 height,
																						uint32 textureID)
{
	if(data != nullptr)
	{
		gl::glPixelStorei(gl::GL_UNPACK_ALIGNMENT, 1);
		gl::glBindTexture(gl::GL_TEXTURE_2D, textureID);
		gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB, width, height, 0, gl::GL_RGB,
										 gl::GL_FLOAT, data);

		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
	}
	else
	{
		std::cout << "\nTexture failed to load with texture data " << std::endl;
	}
}

void TextureManager::updateTexture(const TextureDataRGBF& textureData, uint32 textureId)
{
	gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
	gl::glTexSubImage2D(gl::GL_TEXTURE_2D, 0, 0, 0, textureData.getWidth(), textureData.getHeight(),
											TextureManager::getTextureFormatFromData(textureData.getComponentCount()),
											gl::GL_FLOAT, textureData.getTextureData().data());
	gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
}

void TextureManager::resizeTexture(const TextureDataRGBF& textureData, uint32 textureId)
{
	gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
	gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB, textureData.getWidth(),
									 textureData.getHeight(), 0, gl::GL_RGB, gl::GL_FLOAT,
									 textureData.getTextureData().data());
	gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
}

gl::GLenum TextureManager::getTextureFormatFromData(TextureDataRGBF& textureData)
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
