#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#define GLM_FORCE_INTRINSICS
#include <filesystem>
#include <glm.hpp>
#include <limits>
#include <string>
#include <uuid.h>

#define assertm(exp, msg) assert(((void)msg, exp))

typedef int8_t   int8;
typedef uint8_t  uint8;
typedef int16_t  int16;
typedef uint16_t uint16;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef int64_t  int64;
typedef uint64_t uint64;

typedef float  float32;
typedef double float64;

typedef std::uintptr_t uintptr;

struct UiBounds
{
	glm::vec2 min{0, 0};
	glm::vec2 max{1, 1};

	bool isPointInBounds(const glm::vec2 point) const
	{
		return point.x > min.x && point.x < max.x && point.y > min.y && point.y < max.y;
	}
};

namespace AstralRaytracer
{
	enum class FileType
	{
		INVALID,
		MIN,
		ASSET= MIN,
		RESOURCE,
		MAX= RESOURCE
	};

	enum class AssetType
	{
		INVALID,
		MIN,
		SCENE= MIN,
		TEXTURE,
		MATERIAL,
		TRACEABLE,
		MAX= TRACEABLE
	};

	enum class ResourceType
	{
		INVALID,
		MIN,
		IMAGE= MIN,
		MODEL,
		TEXT_FILE,
		MAX= TEXT_FILE
	};

	static const std::string              invalidString{};
	static const std::vector<std::string> invalidVectorOfString{};
	static const std::vector<std::string> fileExtensionForImages{".png", ".jpg", ".jpeg", ".bmp"};
	static const std::vector<std::string> fileExtensionForModels{".gltf", ".glb"};
	static const std::vector<std::string> fileExtensionForText{".txt", ".md", ".csv"};

	static const std::string FileExtensionForScene{".ascene"};
	static const std::string FileExtensionForTexture{".astex"};
	static const std::string FileExtensionForMaterial{".asmat"};
	static const std::string FileExtensionForTraceable{".astra"};

	std::string toString(AssetType assetType);
	std::string toString(ResourceType resourceType);

	const std::string&              getFileExtensionForAssetType(AssetType type);
	const std::vector<std::string>& getFileExtensionForResourceType(ResourceType type);

	AssetType    getAssetTypeFromFilePath(const std::filesystem::path& filepath);
	ResourceType getResourceTypeFromFilePath(const std::filesystem::path& filepath);

	FileType getFileTypeFromFilePath(const std::filesystem::path& filepath);

} // namespace AstralRaytracer