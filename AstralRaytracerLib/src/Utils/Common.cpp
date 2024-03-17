#include "Utils/Common.h"

#include <fstream>
#include <mimalloc-override.h> // redefines malloc etc.
#include <mimalloc.h>
#ifdef _WIN32
	#include <mimalloc-new-delete.h>
#endif

namespace AstralRaytracer
{
	const std::string_view getFileExtensionForAssetType(AssetType type)
	{
		switch(type)
		{
			case AssetType::PROJECT: return FileExtensionForProject;
			case AssetType::SCENE: return FileExtensionForScene;
			case AssetType::TEXTURE: return FileExtensionForTexture;
			case AssetType::MATERIAL: return FileExtensionForMaterial;
			case AssetType::TRACEABLE: return FileExtensionForTraceable;
			case AssetType::INVALID:
			default: return {};
		}
	}

	const std::vector<std::string_view>& getFileExtensionForResourceType(ResourceType type)
	{
		switch(type)
		{
			case ResourceType::IMAGE: return FileExtensionForImages;
			case ResourceType::MODEL: return FileExtensionForModels;
			case ResourceType::TEXT_FILE: return FileExtensionForText;
			case ResourceType::FONT: return FileExtensionForFont;
			case ResourceType::INVALID:
			default: return InvalidVectorOfStringViews;
		}
	}

	AssetType getAssetTypeFromFilePath(const std::filesystem::path& filepath)
	{
		if(!filepath.has_extension())
		{
			return AssetType::INVALID;
		}

		std::string fileExtension= filepath.extension().string();

		if(fileExtension == FileExtensionForProject)
		{
			return AssetType::PROJECT;
		}
		if(fileExtension == FileExtensionForScene)
		{
			return AssetType::SCENE;
		}
		if(fileExtension == FileExtensionForTexture)
		{
			return AssetType::TEXTURE;
		}
		if(fileExtension == FileExtensionForMaterial)
		{
			return AssetType::MATERIAL;
		}
		if(fileExtension == FileExtensionForTraceable)
		{
			return AssetType::TRACEABLE;
		}

		return AssetType::INVALID;
	}

	ResourceType getResourceTypeFromFilePath(const std::filesystem::path& filepath)
	{
		const std::string& extension= filepath.extension().string();

		auto it= std::find(FileExtensionForImages.begin(), FileExtensionForImages.end(), extension);
		if(it != FileExtensionForImages.end())
		{
			return ResourceType::IMAGE;
		}

		it= std::find(FileExtensionForModels.begin(), FileExtensionForModels.end(), extension);
		if(it != FileExtensionForModels.end())
		{
			return ResourceType::MODEL;
		}

		it= std::find(FileExtensionForText.begin(), FileExtensionForText.end(), extension);
		if(it != FileExtensionForText.end())
		{
			return ResourceType::TEXT_FILE;
		}

		it= std::find(FileExtensionForFont.begin(), FileExtensionForFont.end(), extension);
		if(it != FileExtensionForFont.end())
		{
			return ResourceType::FONT;
		}

		return ResourceType::INVALID;
	}

	FileType getFileTypeFromFilePath(const std::filesystem::path& filepath)
	{
		const std::string& extension= filepath.extension().string();

		if(getResourceTypeFromFilePath(filepath) != ResourceType::INVALID)
		{
			return FileType::RESOURCE;
		}

		if(getAssetTypeFromFilePath(filepath) != AssetType::INVALID)
		{
			return FileType::ASSET;
		}

		return FileType::INVALID;
	}

	Errors::GenericError
	getFileContent(const std::filesystem::path& filepath, std::string& outFileContent)
	{
		const std::string filePathString= filepath.string();

		std::ifstream fileStream(filePathString.c_str());
		if(fileStream.fail())
		{
			fileStream.close();
			return Errors::GenericError::FAILURE;
		}
		std::string fileContents= "";
		std::string line;
		while(std::getline(fileStream, line))
		{
			fileContents+= line + "\n";
		}
		fileStream.close();
		return Errors::GenericError::SUCCESS;
	}
} // namespace AstralRaytracer