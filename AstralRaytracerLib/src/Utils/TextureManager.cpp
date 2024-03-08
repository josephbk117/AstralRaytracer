#include "Utils/TextureManager.h"

#include "Utils/TextureData.h"

#include <stbimage/stb_image.h>
#include <stbimage/stb_image_write.h>

#ifdef SUPPORT_STD_EXECUTION
	#include <execution>
#endif

namespace AstralRaytracer
{
	template<ArithMeticType T, uint32 componentCount>
	void TextureManager::loadTextureDataFromFile(
			const std::filesystem::path&    path,
			TextureData<T, componentCount>& textureData,
			bool                            gamma
	)
	{
		// Determine size of T
		constexpr size_t sizeOfT= sizeof(T);
		static_assert(
				sizeOfT == sizeof(uint8) || sizeOfT == sizeof(uint16) || sizeOfT == sizeof(float32),
				"Unsupported type"
		);

		// Use scoped enumeration for size options
		enum class SizeOption : uint8
		{
			OneByte,
			TwoByte,
			FourByte
		};
		constexpr SizeOption sizeOption= (sizeOfT == sizeof(uint8))    ? SizeOption::OneByte
																		 : (sizeOfT == sizeof(uint16)) ? SizeOption::TwoByte
																																	 : SizeOption::FourByte;

		int32 width      = 0;
		int32 height     = 0;
		int32 numChannels= 0;

		T* data= nullptr;

		const std::string pathString= path.string();
		switch(sizeOption)
		{
			case SizeOption::OneByte:
				data= reinterpret_cast<T*>(
						stbi_load(pathString.c_str(), &width, &height, &numChannels, componentCount)
				);
				break;
			case SizeOption::TwoByte:
				data= reinterpret_cast<T*>(
						stbi_load_16(pathString.c_str(), &width, &height, &numChannels, componentCount)
				);
				break;
			case SizeOption::FourByte:
				data= reinterpret_cast<T*>(
						stbi_loadf(pathString.c_str(), &width, &height, &numChannels, componentCount)
				);
				break;
		}

		assert(data);

		textureData.resize(width, height);

		const size_t   resizeCount= static_cast<size_t>(width) * height * numChannels;
		std::vector<T> vecData(resizeCount);

		std::memcpy(vecData.data(), data, resizeCount * sizeOfT);

		if(gamma)
		{
			// Apply gamma
#ifdef SUPPORT_STD_EXECUTION
			std::for_each(
					std::execution::par_unseq, vecData.begin(), vecData.end(),
#else
			std::for_each(
					vecData.begin(), vecData.end(),
#endif // SUPPORT_STD_EXECUTION
					[=](T& value) { value= glm::pow(value, 1.0f / 2.2f); }
			);
		}

		textureData.setTextureData(std::move(vecData));
		stbi_image_free(data);
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

	template void AstralRaytracer::TextureManager::loadTextureDataFromFile<uint8, 3>(
			const std::filesystem::path&,
			TextureData<uint8, 3>&,
			bool
	);

	template void AstralRaytracer::TextureManager::loadTextureDataFromFile<uint8, 4>(
			const std::filesystem::path&,
			TextureData<uint8, 4>&,
			bool
	);
	template void AstralRaytracer::TextureManager::loadTextureDataFromFile<float32, 3>(
			const std::filesystem::path&,
			TextureData<float32, 3>&,
			bool
	);
	template void AstralRaytracer::TextureManager::loadTextureDataFromFile<float32, 4>(
			const std::filesystem::path&,
			TextureData<float32, 4>&,
			bool
	);

} // namespace AstralRaytracer
