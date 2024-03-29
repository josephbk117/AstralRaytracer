#pragma once
#include "Common.h"
#include "TextureData.h"

#include <filesystem>
#include <glbinding/gl/gl.h>

namespace AstralRaytracer
{
class TextureManager
{
  public:
    TextureManager() = delete;
    TextureManager(const TextureManager &) = delete;

    [[nodiscard]] static constexpr gl::GLenum getTextureFormatFromComponentCount(uint32 componentCount);

    template <ArithmeticType T> [[nodiscard]] static gl::GLenum getTextureDataType();

    template <ArithmeticType T> [[nodiscard]] static gl::GLenum getTextureInternalFormatType(uint32 componentCount);

    template <ArithmeticType T, uint32 componentCount>
    static void loadTextureDataFromFile(const std::filesystem::path &path, TextureData<T, componentCount> &textureData,
                                        bool gamma);

    template <ArithmeticType T, uint32 componentCount>
    [[nodiscard]] static uint32 loadTextureFromTextureData(const TextureData<T, componentCount> &textureData,
                                                           bool gamma);

    template <ArithmeticType T, uint32 componentCount>
    static void updateTexture(const TextureData<T, componentCount> &textureData, uint32 textureId);
    template <ArithmeticType T, uint32 componentCount>
    static void resizeTexture(const TextureData<T, componentCount> &textureData, uint32 textureId);
    template <ArithmeticType T, uint32 componentCount>
    static void saveTextureToFile(const TextureData<T, componentCount> &textureData, const std::filesystem::path &path);

  private:
    enum class ImageSizeOption
    {
        Byte,
        Short,
        Float
    };

    template <ArithmeticType T>
    static void loadTextureFromRawData(const T *const data, uint32 width, uint32 height, uint32 componentCount,
                                       uint32 textureID);

    template <ArithmeticType T, uint32 componentCount>
    [[nodiscard]] static T *loadImageFromFile(ImageSizeOption sizeOption, const std::string &pathString, int32 &width,
                                              int32 &height, int32 &numChannels);
};

} // namespace AstralRaytracer