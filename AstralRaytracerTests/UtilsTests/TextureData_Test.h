#pragma once
#include <Utils/TextureData.h>
#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

class TextureDataTest : public testing::Test
{
  protected:
    static inline const int32 RGBComponentCount = 3;
    static inline const int32 RGBAComponentCount = 4;
    static inline const Resolution resolution{3, 3};

    AstralRaytracer::TextureDataRGB TextureRGB;
    AstralRaytracer::TextureDataRGBA TextureRGBA;
    AstralRaytracer::TextureDataRGBF TextureRGBF;
    AstralRaytracer::TextureDataRGBAF TextureRGBAF;

    static inline const std::vector<uint8> RGBColorData1 = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
                                                            0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1};
    static inline const std::vector<uint8> RGBColorData2 = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1};

    static inline const std::vector<uint8> RGBAColorData1 = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1,
                                                             0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1};
    static inline const std::vector<uint8> RGBAColorData2 = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1,
                                                             1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1};

    static inline const std::vector<float32> RGBFColorData1 = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
                                                               0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1};
    static inline const std::vector<float32> RGBFColorData2 = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1};

    static inline const std::vector<float32> RGBAFColorData1 = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1,
                                                                0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1};
    static inline const std::vector<float32> RGBAFColorData2 = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1,
                                                                1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1};

    void SetUp() override
    {
        TextureRGB = AstralRaytracer::TextureDataRGB(resolution, RGBColorData1);
        TextureRGBA = AstralRaytracer::TextureDataRGBA(resolution, RGBAColorData1);
        TextureRGBF = AstralRaytracer::TextureDataRGBF(resolution, RGBFColorData1);
        TextureRGBAF = AstralRaytracer::TextureDataRGBAF(resolution, RGBAFColorData1);
    }

    // Inherited via Test
    void TestBody() override
    {
    }
};

using TextureDataDeathTest = TextureDataTest;

TEST_F(TextureDataDeathTest, ValidateTextureDataHandleMismatchedDataAssignment)
{
    ASSERT_DEATH(TextureRGB.setTextureData(RGBColorData2), "Mismatched data size");
    ASSERT_DEATH(TextureRGBA.setTextureData(RGBAColorData2), "Mismatched data size");
    ASSERT_DEATH(TextureRGBF.setTextureData(RGBFColorData2), "Mismatched data size");
    ASSERT_DEATH(TextureRGBAF.setTextureData(RGBAFColorData2), "Mismatched data size");
}

TEST_F(TextureDataDeathTest, SetTexelColorAtPixelIndexEdgeCases)
{
    TextureRGB.setTextureData(RGBColorData1);
    // Attempt to set a color at an out-of-bounds index
    EXPECT_DEATH(TextureRGB.setTexelColorAtPixelIndex(99, glm::vec3(0, 255, 0)), "Invalid index");
}

TEST_F(TextureDataDeathTest, SetTexelColorEdgeCases)
{
    TextureRGB.setTextureData(RGBColorData1);
    // Attempt to set a color at an out-of-bounds position
    EXPECT_DEATH(TextureRGB.setTexelColor(99, 99, glm::u8vec3(0, 255, 0)), "Invalid index");
}

TEST_F(TextureDataTest, ResizeTextureDataEdgeCases)
{
    // Test resizing to a larger size
    TextureRGB.setTextureData(RGBColorData1);

    Resolution res1{6, 8};
    TextureRGB.resize(res1);
    EXPECT_EQ(TextureRGB.getWidth(), res1.x);
    EXPECT_EQ(TextureRGB.getHeight(), res1.y);
    // Verify that the original data is preserved and new data is initialized as expected

    // Test resizing to a smaller size
    Resolution res2{2, 3};
    TextureRGB.resize(res2);
    EXPECT_EQ(TextureRGB.getWidth(), res2.x);
    EXPECT_EQ(TextureRGB.getHeight(), res2.y);
    // Verify that the data is correctly trimmed
}

TEST_F(TextureDataTest, ValidateTextureDataGetTexelCompare)
{
    TextureRGB.setTextureData(RGBColorData1);
    TextureRGBA.setTextureData(RGBAColorData1);
    TextureRGBF.setTextureData(RGBFColorData1);
    TextureRGBAF.setTextureData(RGBAFColorData1);

    // Parameterized test for getting texel color
    struct TexelTestParams
    {
        Resolution res;
        float32 normalizedX;
        float32 normalizedY;
    };

    CoOrd2D startPoint{0, 0};
    CoOrd2D midPoint = resolution / 2;
    CoOrd2D endPoint = resolution - 1;

    std::vector<TexelTestParams> testParams = {
        {startPoint, 0.0f, 0.0f}, {midPoint, 0.5f, 0.5f}, {endPoint, 1.0f, 1.0f}};

    constexpr uint32 minSampleTypeIndex = static_cast<uint32>(AstralRaytracer::SamplingType::MIN);
    constexpr uint32 maxSampleTypeIndex = static_cast<uint32>(AstralRaytracer::SamplingType::MAX);
    for (const auto &params : testParams)
    {
        for (uint32 enumIndex = minSampleTypeIndex; enumIndex <= maxSampleTypeIndex; ++enumIndex)
        {
            const AstralRaytracer::SamplingType samplingType = static_cast<AstralRaytracer::SamplingType>(enumIndex);

            CoOrd2D pixCoord = params.res;

            EXPECT_EQ(TextureRGB.getTexelColor(pixCoord.x, pixCoord.y),
                      TextureRGB.getTexelColor(params.normalizedX, params.normalizedY, samplingType)

            );
            EXPECT_EQ(TextureRGBA.getTexelColor(pixCoord.x, pixCoord.y),
                      TextureRGBA.getTexelColor(params.normalizedX, params.normalizedY, samplingType)

            );
            EXPECT_EQ(TextureRGBF.getTexelColor(pixCoord.x, pixCoord.y),
                      TextureRGBF.getTexelColor(params.normalizedX, params.normalizedY, samplingType)

            );
            EXPECT_EQ(TextureRGBAF.getTexelColor(pixCoord.x, pixCoord.y),
                      TextureRGBAF.getTexelColor(params.normalizedX, params.normalizedY, samplingType)

            );
        }
    }
}

TEST_F(TextureDataTest, ValidateTextureDataComponentCounts)
{
    EXPECT_EQ(TextureRGB.getComponentCount(), RGBComponentCount);
    EXPECT_EQ(TextureRGBF.getComponentCount(), RGBComponentCount);
    EXPECT_EQ(TextureRGBA.getComponentCount(), RGBAComponentCount);
    EXPECT_EQ(TextureRGBAF.getComponentCount(), RGBAComponentCount);
}