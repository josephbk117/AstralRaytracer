#include <Utils/TextureData.h>
#include <gtest/gtest.h>

class TextureDataTest: public testing::Test
{
	protected:
		const int32 RGBComponentCount = 3;
		const int32 RGBAComponentCount= 4;

		uint32 width = 3;
		uint32 height= 3;

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

	EXPECT_EQ(TextureRGB.getTextureData(), RGBColorData1);
	EXPECT_EQ(TextureRGBA.getTextureData(), RGBAColorData1);
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