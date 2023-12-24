#include "Utils/Common.h"

std::string AstralRaytracer::toString(AssetType assetType)
{
	switch(assetType)
	{
		case AstralRaytracer::AssetType::PROJECT: return "Project";
		case AstralRaytracer::AssetType::SCENE: return "Scene";
		case AstralRaytracer::AssetType::TEXTURE: return "Texture";
		case AstralRaytracer::AssetType::MATERIAL: return "Material";
		case AstralRaytracer::AssetType::TRACEABLE: return "Traceable";
		case AstralRaytracer::AssetType::INVALID:
		default: return "";
	}
}

std::string AstralRaytracer::toString(ResourceType resourceType)
{
	switch(resourceType)
	{
		case AstralRaytracer::ResourceType::IMAGE: return "Image";
		case AstralRaytracer::ResourceType::MODEL: return "Model";
		case AstralRaytracer::ResourceType::TEXT_FILE: return "Text";
		case AstralRaytracer::ResourceType::FONT: return "Font";
		case AstralRaytracer::ResourceType::INVALID:
		default: return "";
	}
}

const std::string& AstralRaytracer::getFileExtensionForAssetType(AssetType type)
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

const std::vector<std::string>& AstralRaytracer::getFileExtensionForResourceType(ResourceType type)
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

AstralRaytracer::AssetType
AstralRaytracer::getAssetTypeFromFilePath(const std::filesystem::path& filepath)
{
	if(!filepath.has_extension())
	{
		return AstralRaytracer::AssetType::INVALID;
	}

	std::string fileExtension= filepath.extension().string();

	if(fileExtension == AstralRaytracer::FileExtensionForProject)
	{
		return AstralRaytracer::AssetType::PROJECT;
	}
	if(fileExtension == AstralRaytracer::FileExtensionForScene)
	{
		return AstralRaytracer::AssetType::SCENE;
	}
	if(fileExtension == AstralRaytracer::FileExtensionForTexture)
	{
		return AstralRaytracer::AssetType::TEXTURE;
	}
	if(fileExtension == AstralRaytracer::FileExtensionForMaterial)
	{
		return AstralRaytracer::AssetType::MATERIAL;
	}
	if(fileExtension == AstralRaytracer::FileExtensionForTraceable)
	{
		return AstralRaytracer::AssetType::TRACEABLE;
	}

	return AstralRaytracer::AssetType::INVALID;
}

AstralRaytracer::ResourceType
AstralRaytracer::getResourceTypeFromFilePath(const std::filesystem::path& filepath)
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

AstralRaytracer::FileType
AstralRaytracer::getFileTypeFromFilePath(const std::filesystem::path& filepath)
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
