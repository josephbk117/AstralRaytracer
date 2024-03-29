#include "Utils/TextureManager.h"

#include <stbimage/stb_image.h>
#include <stbimage/stb_image_write.h>

#ifdef SUPPORT_STD_EXECUTION
#include <execution>
#endif

namespace AstralRaytracer
{
constexpr gl::GLenum TextureManager::getTextureFormatFromComponentCount(uint32 componentCount)
{
    ASTRAL_ASSERTM(componentCount <= 4, "Invalid component count requested");

    switch (componentCount)
    {
    case 1:
        return gl::GLenum::GL_RED;
    case 2:
        return gl::GLenum::GL_RG;
    case 3:
        return gl::GLenum::GL_RGB;
    case 4:
        return gl::GLenum::GL_RGBA;
    }

    return gl::GLenum::GL_INVALID_ENUM;
}

template <ArithmeticType T> gl::GLenum TextureManager::getTextureDataType()
{
    if (std::is_same_v<T, float32>)
    {
        return gl::GLenum::GL_FLOAT;
    }
    else if (std::is_same_v<T, uint8>)
    {
        return gl::GLenum::GL_UNSIGNED_BYTE;
    }
    else if (std::is_same_v<T, int8>)
    {
        return gl::GLenum::GL_BYTE;
    }
    else if (std::is_same_v<T, uint16>)
    {
        return gl::GLenum::GL_UNSIGNED_SHORT;
    }
    else if (std::is_same_v<T, int16>)
    {
        return gl::GLenum::GL_SHORT;
    }
    else if (std::is_same_v<T, uint32>)
    {
        return gl::GLenum::GL_UNSIGNED_INT;
    }
    else if (std::is_same_v<T, int32>)
    {
        return gl::GLenum::GL_INT;
    }

    static_assert(true, "Unsupported type");
}

template <ArithmeticType T> gl::GLenum TextureManager::getTextureInternalFormatType(uint32 componentCount)
{
    ASTRAL_ASSERTM(componentCount <= 4, "Invalid component count requested");

    if (std::is_same_v<T, float32>)
    {
        switch (componentCount)
        {
        case 1:
            return gl::GLenum::GL_R32F;
        case 2:
            return gl::GLenum::GL_RG32F;
        case 3:
            return gl::GLenum::GL_RGB32F;
        case 4:
            return gl::GLenum::GL_RGBA32F;
        }
    }
    else if (std::is_same_v<T, uint8>)
    {
        switch (componentCount)
        {
        case 1:
            return gl::GLenum::GL_R8;
        case 2:
            return gl::GLenum::GL_RG8;
        case 3:
            return gl::GLenum::GL_RGB8;
        case 4:
            return gl::GLenum::GL_RGBA8;
        }
    }
    else if (std::is_same_v<T, int8>)
    {
        switch (componentCount)
        {
        case 1:
            return gl::GLenum::GL_R8I;
        case 2:
            return gl::GLenum::GL_RG8I;
        case 3:
            return gl::GLenum::GL_RGB8I;
        case 4:
            return gl::GLenum::GL_RGBA8I;
        }
    }
    else if (std::is_same_v<T, uint16>)
    {
        switch (componentCount)
        {
        case 1:
            return gl::GLenum::GL_R16;
        case 2:
            return gl::GLenum::GL_RG16;
        case 3:
            return gl::GLenum::GL_RGB16;
        case 4:
            return gl::GLenum::GL_RGBA16;
        }
    }
    else if (std::is_same_v<T, int16>)
    {
        switch (componentCount)
        {
        case 1:
            return gl::GLenum::GL_R16I;
        case 2:
            return gl::GLenum::GL_RG16I;
        case 3:
            return gl::GLenum::GL_RGB16I;
        case 4:
            return gl::GLenum::GL_RGBA16I;
        }
    }
    else if (std::is_same_v<T, uint32>)
    {
        switch (componentCount)
        {
        case 1:
            return gl::GLenum::GL_R32UI;
        case 2:
            return gl::GLenum::GL_RG32UI;
        case 3:
            return gl::GLenum::GL_RGB32UI;
        case 4:
            return gl::GLenum::GL_RGBA32UI;
        }
    }
    else if (std::is_same_v<T, int32>)
    {
        switch (componentCount)
        {
        case 1:
            return gl::GLenum::GL_R32I;
        case 2:
            return gl::GLenum::GL_RG32I;
        case 3:
            return gl::GLenum::GL_RGB32I;
        case 4:
            return gl::GLenum::GL_RGBA32I;
        }
    }

    static_assert(true, "Unsupported type");

    return gl::GLenum::GL_INVALID_ENUM;
}

template <ArithmeticType T, uint32 componentCount>
void TextureManager::loadTextureDataFromFile(const std::filesystem::path &path,
                                             TextureData<T, componentCount> &textureData, bool gamma)
{
    constexpr size_t sizeOfT = sizeof(T);
    static_assert(sizeOfT == sizeof(uint8) || sizeOfT == sizeof(uint16) || sizeOfT == sizeof(float32),
                  "Unsupported type");

    // Use scoped enumeration for size options
    constexpr ImageSizeOption sizeOption = (sizeOfT == sizeof(uint8))    ? ImageSizeOption::Byte
                                           : (sizeOfT == sizeof(uint16)) ? ImageSizeOption::Short
                                                                         : ImageSizeOption::Float;

    int32 numChannels = 0;
    Resolution imageRes{0};

    const std::string pathString = path.string();

    T *data = loadImageFromFile<T, componentCount>(sizeOption, pathString, imageRes.x, imageRes.y, numChannels);

    if (data == nullptr)
    {
        ASTRAL_LOG_ERROR("Failed to load texture data from file: " + pathString);
        return;
    }

    textureData.resize(imageRes);

    const size_t resizeCount = static_cast<size_t>(imageRes.x) * imageRes.y * numChannels;
    std::vector<T> vecData(resizeCount);

    std::memcpy(vecData.data(), data, resizeCount * sizeOfT);

    if (gamma)
    {
        runParallel(vecData.begin(), vecData.end(), [=](T &value) { value = glm::pow(value, 1.0f / 2.2f); });
    }

    textureData.setTextureData(std::move(vecData));
    stbi_image_free(data);
}

template <ArithmeticType T, uint32 componentCount>
[[nodiscard]] uint32 TextureManager::loadTextureFromTextureData(const TextureData<T, componentCount> &textureData,
                                                                bool gamma)
{
    uint32 textureID;
    gl::glGenTextures(1, &textureID);

    const T *const data = textureData.getTextureData().data();
    const uint32 texWidth = textureData.getWidth();
    const uint32 texHeight = textureData.getHeight();

    loadTextureFromRawData(data, texWidth, texHeight, componentCount, textureID);
    return textureID;
}

template <ArithmeticType T>
void TextureManager::loadTextureFromRawData(const T *const data, uint32 width, uint32 height, uint32 componentCount,
                                            uint32 textureID)
{
    if (data == nullptr)
    {
        ASTRAL_LOG_ERROR("Texture failed to load with texture data");
        return;
    }

    gl::glPixelStorei(gl::GL_UNPACK_ALIGNMENT, 1);
    gl::glBindTexture(gl::GL_TEXTURE_2D, textureID);

    const gl::GLenum format = getTextureFormatFromComponentCount(componentCount);
    const gl::GLenum dataType = getTextureDataType<T>();
    const gl::GLenum internalFormat = getTextureFormatFromComponentCount(componentCount);

    gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
}

template <ArithmeticType T, uint32 componentCount>
T *TextureManager::loadImageFromFile(ImageSizeOption sizeOption, const std::string &pathString, int32 &width,
                                     int32 &height, int32 &numChannels)
{
    switch (sizeOption)
    {
    case ImageSizeOption::Byte:
        return reinterpret_cast<T *>(stbi_load(pathString.c_str(), &width, &height, &numChannels, componentCount));
    case ImageSizeOption::Short:
        return reinterpret_cast<T *>(stbi_load_16(pathString.c_str(), &width, &height, &numChannels, componentCount));
    case ImageSizeOption::Float:
        return reinterpret_cast<T *>(stbi_loadf(pathString.c_str(), &width, &height, &numChannels, componentCount));
    default:
        return nullptr;
    }
}

template <ArithmeticType T, uint32 componentCount>
void TextureManager::updateTexture(const TextureData<T, componentCount> &textureData, uint32 textureId)
{
    gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);

    constexpr gl::GLenum format = getTextureFormatFromComponentCount(componentCount);
    const gl::GLenum dataType = getTextureDataType<T>();

    const Resolution &texRes = textureData.getResolution();

    gl::glTexSubImage2D(gl::GL_TEXTURE_2D, 0, 0, 0, texRes.x, texRes.y, format, dataType,
                        textureData.getTextureData().data());
    gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
}

template <ArithmeticType T, uint32 componentCount>
void TextureManager::resizeTexture(const TextureData<T, componentCount> &textureData, uint32 textureId)
{
    gl::glBindTexture(gl::GL_TEXTURE_2D, textureId);

    const Resolution &texRes = textureData.getResolution();

    constexpr gl::GLenum format = getTextureFormatFromComponentCount(componentCount);
    const gl::GLenum dataType = getTextureDataType<T>();
    const gl::GLenum internalFormat = getTextureInternalFormatType<T>(componentCount);

    gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, internalFormat, texRes.x, texRes.y, 0, format, dataType,
                     textureData.getTextureData().data());
    gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
}

template <ArithmeticType T, uint32 componentCount>
void TextureManager::saveTextureToFile(const TextureData<T, componentCount> &textureData,
                                       const std::filesystem::path &path)
{
    const Resolution &texRes = textureData.getResolution();

    const std::string pathStr = path.string();

    if (std::is_same_v<T, uint8>)
    {
        const void *data = reinterpret_cast<const void *>(textureData.getTextureData().data());

        const int32 strideInBytes = texRes.x * componentCount;
        stbi_write_png(pathStr.c_str(), texRes.x, texRes.y, componentCount, data, strideInBytes);
    }
    else if (std::is_same_v<T, float32>)
    {
        stbi_flip_vertically_on_write(static_cast<int32>(true));

        const float *data = reinterpret_cast<const float *>(textureData.getTextureData().data());
        stbi_write_hdr(pathStr.c_str(), texRes.x, texRes.y, componentCount, data);
    }
}

template gl::GLenum TextureManager::getTextureDataType<uint8>();
template gl::GLenum TextureManager::getTextureDataType<float32>();

template void TextureManager::saveTextureToFile(const TextureData<uint8, 3> &, const std::filesystem::path &);
template void TextureManager::saveTextureToFile(const TextureData<uint8, 4> &, const std::filesystem::path &);
template void TextureManager::saveTextureToFile(const TextureData<float32, 1> &, const std::filesystem::path &);
template void TextureManager::saveTextureToFile(const TextureData<float32, 3> &, const std::filesystem::path &);
template void TextureManager::saveTextureToFile(const TextureData<float32, 4> &, const std::filesystem::path &);

template uint8 *TextureManager::loadImageFromFile<uint8, 3>(ImageSizeOption, const std::string &, int32 &, int32 &,
                                                            int32 &);
template uint8 *TextureManager::loadImageFromFile<uint8, 4>(ImageSizeOption, const std::string &, int32 &, int32 &,
                                                            int32 &);
template uint16 *TextureManager::loadImageFromFile<uint16, 3>(ImageSizeOption, const std::string &, int32 &, int32 &,
                                                              int32 &);
template uint16 *TextureManager::loadImageFromFile<uint16, 4>(ImageSizeOption, const std::string &, int32 &, int32 &,
                                                              int32 &);
template float32 *TextureManager::loadImageFromFile<float32, 1>(ImageSizeOption, const std::string &, int32 &, int32 &,
                                                                int32 &);
template float32 *TextureManager::loadImageFromFile<float32, 3>(ImageSizeOption, const std::string &, int32 &, int32 &,
                                                                int32 &);
template float32 *TextureManager::loadImageFromFile<float32, 4>(ImageSizeOption, const std::string &, int32 &, int32 &,
                                                                int32 &);

template void TextureManager::loadTextureFromRawData<uint8>(const uint8 *const, uint32, uint32, uint32, uint32);
template void TextureManager::loadTextureFromRawData<float32>(const float32 *const, uint32, uint32, uint32, uint32);

template uint32 TextureManager::loadTextureFromTextureData<uint8, 3>(const TextureData<uint8, 3> &, bool);
template uint32 TextureManager::loadTextureFromTextureData<uint8, 4>(const TextureData<uint8, 4> &, bool);
template uint32 TextureManager::loadTextureFromTextureData<float32, 1>(const TextureData<float32, 1> &, bool);
template uint32 TextureManager::loadTextureFromTextureData<float32, 3>(const TextureData<float32, 3> &, bool);
template uint32 TextureManager::loadTextureFromTextureData<float32, 4>(const TextureData<float32, 4> &, bool);

template void TextureManager::loadTextureDataFromFile<uint8, 3>(const std::filesystem::path &, TextureData<uint8, 3> &,
                                                                bool);
template void TextureManager::loadTextureDataFromFile<uint8, 4>(const std::filesystem::path &, TextureData<uint8, 4> &,
                                                                bool);
template void TextureManager::loadTextureDataFromFile<float32, 1>(const std::filesystem::path &,
                                                                  TextureData<float32, 1> &, bool);
template void TextureManager::loadTextureDataFromFile<float32, 3>(const std::filesystem::path &,
                                                                  TextureData<float32, 3> &, bool);
template void TextureManager::loadTextureDataFromFile<float32, 4>(const std::filesystem::path &,
                                                                  TextureData<float32, 4> &, bool);

template void TextureManager::updateTexture(const TextureData<uint8, 3> &, uint32);
template void TextureManager::updateTexture(const TextureData<uint8, 4> &, uint32);
template void TextureManager::updateTexture(const TextureData<float32, 1> &, uint32);
template void TextureManager::updateTexture(const TextureData<float32, 3> &, uint32);
template void TextureManager::updateTexture(const TextureData<float32, 4> &, uint32);

template void TextureManager::resizeTexture(const TextureData<uint8, 3> &, uint32);
template void TextureManager::resizeTexture(const TextureData<uint8, 4> &, uint32);
template void TextureManager::resizeTexture(const TextureData<float32, 1> &, uint32);
template void TextureManager::resizeTexture(const TextureData<float32, 3> &, uint32);
template void TextureManager::resizeTexture(const TextureData<float32, 4> &, uint32);
} // namespace AstralRaytracer