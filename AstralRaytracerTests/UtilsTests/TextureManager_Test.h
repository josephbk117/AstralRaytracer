#include <Utils/TextureManager.h>
#include <gtest/gtest.h>

class TextureManagerTest : public ::testing::Test {
protected:
  // Set up any resources needed for the tests
  void SetUp() override {
    // Add any setup code here
  }

  // Tear down any resources allocated in SetUp
  void TearDown() override {
    // Add any teardown code here
  }
};

TEST_F(TextureManagerTest, LoadTextureDataFromFileRGB) {
  // Test loading texture data from an RGB file
  //ASSERT_EQ(expectedTextureData, TextureManager::loadTextureDataFromFileRGB("path_to_rgb_file"));
}

TEST_F(TextureManagerTest, LoadTextureDataFromFileRGBA) {
  // Test loading texture data from an RGBA file
  // ASSERT_EQ(expectedTextureData, TextureManager::loadTextureDataFromFileRGBA("path_to_rgba_file"));
}

TEST_F(TextureManagerTest, LoadTextureDataFromFileRGBF) {
  // Test loading texture data from an RGBF file
  // ASSERT_EQ(expectedTextureData, TextureManager::loadTextureDataFromFileRGBF("path_to_rgbf_file"));
}

TEST_F(TextureManagerTest, LoadTextureFromTextureData) {
  // Test loading a texture from texture data
  // ASSERT_EQ(expectedTextureId, TextureManager::loadTextureFromTextureData(textureData, true));
}
