#include <Utils/TextureData.h>
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
			TextureRGB  = AstralRaytracer::TextureDataRGB(width, height);
			TextureRGBA = AstralRaytracer::TextureDataRGBA(width, height);
			TextureRGBF = AstralRaytracer::TextureDataRGBF(width, height);
			TextureRGBAF= AstralRaytracer::TextureDataRGBAF(width, height);

			// Fill textures with data
			TextureRGB.setTextureData(RGBColorData1);
		}

		// Inherited via Test
		void TestBody() override { }
};

TEST_F(TextureDataTest, ValidateTextureDataGetAndSetData)
{
	TextureRGB.setTextureData(RGBColorData1);
	TextureRGBA.setTextureData(RGBAColorData1);
	TextureRGBF.setTextureData(RGBFColorData1);
	TextureRGBAF.setTextureData(RGBAFColorData1);

	// First Pixel
	EXPECT_EQ(TextureRGB.getTexelColor(0u, 0u), TextureRGB.getTexelColor(0.0f, 0.0f));
	EXPECT_EQ(TextureRGBA.getTexelColor(0u, 0u), TextureRGBA.getTexelColor(0.0f, 0.0f));

	EXPECT_EQ(TextureRGBF.getTexelColor(0u, 0u), TextureRGBF.getTexelColor(0.0f, 0.0f));
	EXPECT_EQ(TextureRGBAF.getTexelColor(0u, 0u), TextureRGBAF.getTexelColor(0.0f, 0.0f));

	const uint32 midPointXIndex= width * 0.5f;
	const uint32 midPointYIndex= height * 0.5f;

	// Mid Pixel
	EXPECT_EQ(
			TextureRGB.getTexelColor(midPointXIndex, midPointYIndex), TextureRGB.getTexelColor(0.5f, 0.5f)
	);
	EXPECT_EQ(
			TextureRGBA.getTexelColor(midPointXIndex, midPointYIndex),
			TextureRGBA.getTexelColor(0.5f, 0.5f)
	);

	EXPECT_EQ(
			TextureRGBF.getTexelColor(midPointXIndex, midPointYIndex),
			TextureRGBF.getTexelColor(0.5f, 0.5f)
	);
	EXPECT_EQ(
			TextureRGBAF.getTexelColor(midPointXIndex, midPointYIndex),
			TextureRGBAF.getTexelColor(0.5f, 0.5f)
	);

	// Last Pixel
	EXPECT_EQ(TextureRGB.getTexelColor(width - 1, height - 1), TextureRGB.getTexelColor(1.0f, 1.0f));
	EXPECT_EQ(
			TextureRGBA.getTexelColor(width - 1, height - 1), TextureRGBA.getTexelColor(1.0f, 1.0f)
	);

	EXPECT_EQ(
			TextureRGBF.getTexelColor(width - 1, height - 1), TextureRGBF.getTexelColor(1.0f, 1.0f)
	);
	EXPECT_EQ(
			TextureRGBAF.getTexelColor(width - 1, height - 1), TextureRGBAF.getTexelColor(1.0f, 1.0f)
	);
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