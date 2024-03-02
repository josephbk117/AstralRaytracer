#include "Utils/TextureManager.h"

#include "Utils/TextureData.h"

#include <stbimage/stb_image.h>
#include <stbimage/stb_image_write.h>

#ifdef SUPPORT_STD_EXECUTION
	#include <execution>
#endif

namespace AstralRaytracer
{
	TextureDataRGB TextureManager::loadTextureDataFromFileRGB(const std::filesystem::path& path)
	{
		int32 width;
		int32 height;
		int32 numChannels;

		stbi_uc* data= stbi_load(path.string().c_str(), &width, &height, &numChannels, 3);
		assert(data);

		TextureDataRGB     texData(width, height);
		std::vector<uint8> vecData;

		const size_t resizeCount= static_cast<size_t>(width) * height * numChannels;
		vecData.resize(resizeCount);

		std::memcpy(vecData.data(), data, resizeCount);

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

		TextureDataRGBA    texData(width, height);
		std::vector<uint8> vecData;

		const size_t resizeCount= static_cast<size_t>(width) * height * numChannels;
		vecData.resize(resizeCount);

		std::memcpy(vecData.data(), data, resizeCount);

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

		const size_t resizeCount= static_cast<size_t>(width) * height * numChannels;
		vecData.resize(resizeCount);

		std::memcpy(vecData.data(), data, resizeCount * sizeof(float32));

		// Apply gamma
#ifdef SUPPORT_STD_EXECUTION
		std::for_each(
				std::execution::par_unseq, vecData.begin(), vecData.end(),
#else
		std::for_each(
				vecData.begin(), vecData.end(),
#endif // SUPPORT_STD_EXECUTION,
				[=](float32& value) { value= glm::pow(value, 1.0f / 2.2f); }
		);

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

	void TextureManager::loadTextureFromRawData(
			const float32* const data,
			uint32               width,
			uint32               height,
			uint32               textureID
	)
	{
		if(data == nullptr)
		{
			ASTRAL_LOG_ERROR("Texture failed to load with texture data");
			return;
		}

		gl::glPixelStorei(gl::GL_UNPACK_ALIGNMENT, 1);
		gl::glBindTexture(gl::GL_TEXTURE_2D, textureID);
		gl::glTexImage2D(
				gl::GL_TEXTURE_2D, 0, gl::GL_RGB32F, width, height, 0, gl::GL_RGB, gl::GL_FLOAT, data
		);

		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
		gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
	}

	void TextureManager::updateTexture(const TextureDataRGBF& textureData, uint32 textureId)
	{
		gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
		gl::glTexSubImage2D(
				gl::GL_TEXTURE_2D, 0, 0, 0, textureData.getWidth(), textureData.getHeight(),
				TextureManager::getTextureFormatFromData(textureData.getComponentCount()), gl::GL_FLOAT,
				textureData.getTextureData().data()
		);
		gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
	}

	void TextureManager::resizeTexture(const TextureDataRGBF& textureData, uint32 textureId)
	{
		gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);
		gl::glTexImage2D(
				gl::GL_TEXTURE_2D, 0, gl::GL_RGB32F, textureData.getWidth(), textureData.getHeight(), 0,
				gl::GL_RGB, gl::GL_FLOAT, textureData.getTextureData().data()
		);
		gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
	}

	void
	TextureManager::saveTextureToFile(const TextureDataRGBF& data, const std::filesystem::path& path)
	{
		stbi_flip_vertically_on_write(static_cast<int32>(true));
		stbi_write_hdr(
				path.string().c_str(), data.getWidth(), data.getHeight(), data.getComponentCount(),
				data.getTextureData().data()
		);
	}

	gl::GLenum TextureManager::getTextureFormatFromData(TextureDataRGBF& textureData)
	{
		return getTextureFormatFromData(textureData.getComponentCount());
	}

	gl::GLenum TextureManager::getTextureFormatFromData(uint32 componentCount)
	{
		ASTRAL_ASSERTM(componentCount <= 4, "Invalid component count requested");

		switch(componentCount)
		{
			case 1: return gl::GLenum::GL_RED;
			case 2: return gl::GLenum::GL_RG;
			case 3: return gl::GLenum::GL_RGB;
			case 4: return gl::GLenum::GL_RGBA;
		}

		return gl::GLenum::GL_INVALID_ENUM;
	}

} // namespace AstralRaytracer
