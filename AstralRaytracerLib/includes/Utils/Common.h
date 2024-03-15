#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#define GLM_FORCE_INTRINSICS
#include <array>
#include <cstddef>
#include <execution>
#include <filesystem>
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

		[[nodiscard]]
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

	namespace MathConstants
	{
		static const glm::vec3 UpDirection{ 0, 1, 0 };
		static const glm::vec3 DownDirection{ 0, -1, 0 };
		static const glm::vec3 RightDirection{ 1, 0, 0 };
		static const glm::vec3 LeftDirection{ -1, 0, 0 };
		static const glm::vec3 ForwardDirection{ 0, 0, 1 };
		static const glm::vec3 BackwardDirection{ 0, 0, -1 };
	} // namespace MathConstants

	namespace Errors
	{
		enum class GenericError : uint8
		{
			INVALID,
			MIN,
			SUCCESS= MIN,
			WARNING,
			FAILURE,
			MAX= FAILURE
		};
	}

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

	[[nodiscard]]
	inline constexpr std::string_view toString(AssetType assetType)
	{
		switch(assetType)
		{
			case AssetType::PROJECT: return "Project";
			case AssetType::SCENE: return "Scene";
			case AssetType::TEXTURE: return "Texture";
			case AssetType::MATERIAL: return "Material";
			case AssetType::TRACEABLE: return "Traceable";
			case AssetType::INVALID:
			default: return "UNKNOWN";
		}
	}

	[[nodiscard]]
	inline constexpr std::string_view toString(ResourceType resourceType)
	{
		switch(resourceType)
		{
			case ResourceType::IMAGE: return "Image";
			case ResourceType::MODEL: return "Model";
			case ResourceType::TEXT_FILE: return "Text";
			case ResourceType::FONT: return "Font";
			case ResourceType::INVALID:
			default: return "UNKNOWN";
		}
	}

	[[nodiscard]]
	const std::string& getFileExtensionForAssetType(AssetType type);
	[[nodiscard]]
	const std::vector<std::string>& getFileExtensionForResourceType(ResourceType type);

	[[nodiscard]]
	AssetType getAssetTypeFromFilePath(const std::filesystem::path& filepath);
	[[nodiscard]]
	ResourceType getResourceTypeFromFilePath(const std::filesystem::path& filepath);
	[[nodiscard]]
	FileType getFileTypeFromFilePath(const std::filesystem::path& filepath);
	[[nodiscard]]
	Errors::GenericError
	getFileContent(const std::filesystem::path& filepath, std::string& outFileContent);

	template<typename Iterator, typename Function>
	void runParallel(Iterator first, Iterator last, Function func)
	{
		// Apply gamma
#ifdef SUPPORT_STD_EXECUTION
		std::for_each(
				std::execution::par_unseq, first, last,
#else
		std::for_each(
				first, last,
#endif // SUPPORT_STD_EXECUTION
				func
		);
	}

} // namespace AstralRaytracer