#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#define GLM_FORCE_INTRINSICS
#include <cstddef>
#include <filesystem>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <limits>
#include <memory>
#include <numbers>
#include <spdlog/spdlog.h>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <uuid.h>

#ifndef __APPLE__
	#define SUPPORT_STD_EXECUTION
#endif

#define ASTRAL_LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define ASTRAL_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define ASTRAL_LOG_INFO(...)  spdlog::info(__VA_ARGS__)
#define ASTRAL_LOG_WARN(...)  spdlog::warn(__VA_ARGS__)
#define ASTRAL_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define ASTRAL_ASSERTM(exp, msg) \
	if(!(exp))                     \
		spdlog::critical(msg);       \
	assert(((void)msg, exp))

using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using int8  = int8_t;
using uint16= uint16_t;
using uint32= uint32_t;
using uint64= uint64_t;
using uint8 = uint8_t;

using float32= float;
using float64= double;

using uintptr= std::uintptr_t;

struct UiBounds
{
		glm::vec2 min{ 0, 0 };
		glm::vec2 max{ 1, 1 };

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
		PROJECT= MIN,
		SCENE,
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
		FONT,
		MAX= FONT
	};

	static const std::string              invalidString{};
	static const std::vector<std::string> invalidVectorOfString{};
	static const std::vector<std::string> fileExtensionForImages{ ".png", ".jpg", ".jpeg", ".bmp",
																																".hdr" };
	static const std::vector<std::string> fileExtensionForModels{ ".gltf", ".glb" };
	static const std::vector<std::string> fileExtensionForText{ ".txt", ".md", ".csv" };
	static const std::vector<std::string> fileExtensionForFont{ ".ttf" };

	static const std::string FileExtensionForProject{ ".asproj" };
	static const std::string FileExtensionForScene{ ".ascene" };
	static const std::string FileExtensionForTexture{ ".astex" };
	static const std::string FileExtensionForMaterial{ ".asmat" };
	static const std::string FileExtensionForTraceable{ ".astra" };

	std::string toString(AssetType assetType);
	std::string toString(ResourceType resourceType);

	const std::string&              getFileExtensionForAssetType(AssetType type);
	const std::vector<std::string>& getFileExtensionForResourceType(ResourceType type);

	AssetType    getAssetTypeFromFilePath(const std::filesystem::path& filepath);
	ResourceType getResourceTypeFromFilePath(const std::filesystem::path& filepath);

	FileType getFileTypeFromFilePath(const std::filesystem::path& filepath);

} // namespace AstralRaytracer