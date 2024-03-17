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
			default: return invalidString;
		}
	}

	const std::vector<std::string>& getFileExtensionForResourceType(ResourceType type)
	{
		switch(type)
		{
			case ResourceType::IMAGE: return fileExtensionForImages;
			case ResourceType::MODEL: return fileExtensionForModels;
			case ResourceType::TEXT_FILE: return fileExtensionForText;
			case ResourceType::FONT: return fileExtensionForFont;
			case ResourceType::INVALID:
			default: return invalidVectorOfString;
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

		auto it= std::find(fileExtensionForImages.begin(), fileExtensionForImages.end(), extension);
		if(it != fileExtensionForImages.end())
		{
			return ResourceType::IMAGE;
		}

		it= std::find(fileExtensionForModels.begin(), fileExtensionForModels.end(), extension);
		if(it != fileExtensionForModels.end())
		{
			return ResourceType::MODEL;
		}

		it= std::find(fileExtensionForText.begin(), fileExtensionForText.end(), extension);
		if(it != fileExtensionForText.end())
		{
			return ResourceType::TEXT_FILE;
		}

		it= std::find(fileExtensionForFont.begin(), fileExtensionForFont.end(), extension);
		if(it != fileExtensionForFont.end())
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