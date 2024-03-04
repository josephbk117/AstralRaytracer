#include <Utils/TextureData.h>
#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

class TextureDataTest: public testing::Test
{
	protected:
		const int32 RGBComponentCount = 3;
		const int32 RGBAComponentCount= 4;

		const uint32 width = 3;
		const uint32 height= 3;

		AstralRaytracer::TextureDataRGB   TextureRGB;
		AstralRaytracer::TextureDataRGBA  TextureRGBA;
		AstralRaytracer::TextureDataRGBF  TextureRGBF;
		AstralRaytracer::TextureDataRGBAF TextureRGBAF;

		std::vector<uint8> RGBColorData1= { 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
																				0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
		std::vector<uint8> RGBColorData2= { 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1 };

		std::vector<uint8> RGBAColorData1= { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1,
																				 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1 };
		std::vector<uint8> RGBAColorData2= { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1,
																				 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1 };

		std::vector<float32> RGBFColorData1= { 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
																					 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
		std::vector<float32> RGBFColorData2= { 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1 };

		std::vector<float32> RGBAFColorData1= { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1,
																						0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1 };
		std::vector<float32> RGBAFColorData2= { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1,
																						1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1 };

		void SetUp() override
		{
			TextureRGB  = AstralRaytracer::TextureDataRGB(width, height, RGBColorData1);
			TextureRGBA = AstralRaytracer::TextureDataRGBA(width, height, RGBAColorData1);
			TextureRGBF = AstralRaytracer::TextureDataRGBF(width, height, RGBFColorData1);
			TextureRGBAF= AstralRaytracer::TextureDataRGBAF(width, height, RGBAFColorData1);
		}

		// Inherited via Test
		void TestBody() override { }
};

using TextureDataDeathTest= TextureDataTest;

TEST_F(TextureDataDeathTest, ValidateTextureDataHandleMismatchedDataAssignment)
{
	EXPECT_DEATH(TextureRGB.setTextureData(RGBColorData2), "Mismatched data size");
	EXPECT_DEATH(TextureRGBA.setTextureData(RGBAColorData2), "Mismatched data size");
	EXPECT_DEATH(TextureRGBF.setTextureData(RGBFColorData2), "Mismatched data size");
	EXPECT_DEATH(TextureRGBAF.setTextureData(RGBAFColorData2), "Mismatched data size");
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
			uint32  x;
			uint32  y;
			float32 normalizedX;
			float32 normalizedY;
	};

	std::vector<TexelTestParams> testParams= {
		{0,					0,          0.0f, 0.0f},
		{ width / 2, height / 2, 0.5f, 0.5f},
		{ width - 1, height - 1, 1.0f, 1.0f}
	};

	for(const auto& params : testParams)
	{
		EXPECT_EQ(
				TextureRGB.getTexelColor(params.x, params.y),
				TextureRGB.getTexelColor(params.normalizedX, params.normalizedY)
		);
		EXPECT_EQ(
				TextureRGBA.getTexelColor(params.x, params.y),
				TextureRGBA.getTexelColor(params.normalizedX, params.normalizedY)
		);
		EXPECT_EQ(
				TextureRGBF.getTexelColor(params.x, params.y),
				TextureRGBF.getTexelColor(params.normalizedX, params.normalizedY)
		);
		EXPECT_EQ(
				TextureRGBAF.getTexelColor(params.x, params.y),
				TextureRGBAF.getTexelColor(params.normalizedX, params.normalizedY)
		);
	}
}

TEST_F(TextureDataTest, ValidateTextureDataComponentCounts)
{
	EXPECT_EQ(TextureRGB.getComponentCount(), RGBComponentCount);
	EXPECT_EQ(TextureRGBF.getComponentCount(), RGBComponentCount);
	EXPECT_EQ(TextureRGBA.getComponentCount(), RGBAComponentCount);
	EXPECT_EQ(TextureRGBAF.getComponentCount(), RGBAComponentCount);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}