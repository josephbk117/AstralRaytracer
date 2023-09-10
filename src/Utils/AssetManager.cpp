#include "Utils/AssetManager.h"

#include "Raytracer/ModelManager.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/TriangleTraceable.h"

#include <fstream>
namespace AstralRaytracer
{

	TextureData AssetManager::LoadTextureAsset(const std::filesystem::path& path,
																						 const std::string&           name)
	{
		static uint32 textureCount= 0;
		NameAndPath   nameAndPath = {name, path.string()};
		m_textureNameAndPathMap.emplace(textureCount, nameAndPath);
		textureCount++;
		return TextureManager::loadTextureDataFromFile(path);
	}

	bool AssetManager::LoadMaterialAsset(const std::filesystem::path& path, const std::string& name,
																			 Material& outMaterial)
	{
		static uint32 matCount   = 0;
		NameAndPath   nameAndPath= {name, path.string()};
		m_materialNameAndPathMap.emplace(matCount, nameAndPath);
		matCount++;

		std::ifstream stream(path);
		YAML::Node    data= YAML::Load(stream);

		if(!data["Material"])
		{
			return false;
		}

		outMaterial.deserialize(data);
		return true;
	}

	std::unique_ptr<AstralRaytracer::Traceable>
	AssetManager::LoadTraceableAsset(const std::filesystem::path& path)
	{
		static uint32 traceableCount= 0;

		std::ifstream stream(path);
		YAML::Node    data= YAML::Load(stream);

		if(!data["Traceable"])
		{
			return nullptr;
		}

		NameAndPath nameAndPath= {data["Traceable"].as<std::string>(), path.string()};
		m_traceableNameAndPathMap.emplace(traceableCount, nameAndPath);

		traceableCount++;

		Serialization::TraceableType type=
				static_cast<Serialization::TraceableType>(data["Type"].as<uint32>());
		switch(type)
		{
			case AstralRaytracer::Serialization::TraceableType::INVALID: return nullptr;
			case AstralRaytracer::Serialization::TraceableType::SPEHRE:
			{
				auto sphere= std::make_unique<AstralRaytracer::SphereTraceable>();
				sphere->deserialize(data);
				return sphere;
			};
			case AstralRaytracer::Serialization::TraceableType::TRIANGLE:
			{
				auto triangle= std::make_unique<AstralRaytracer::TriangleTraceable>();
				triangle->deserialize(data);
				return triangle;
			}
			case AstralRaytracer::Serialization::TraceableType::STATIC_MESH:
			{
				auto mesh= std::make_unique<AstralRaytracer::StaticMesh>();
				mesh->deserialize(data);
				return mesh;
			}
			default: break;
		}

		return nullptr;
	}

	void AssetManager::SaveMaterialAsset(const std::string& name, const Material& material)
	{
		std::filesystem::path path= "/resources/materials/" + name + ".mat";
		YAML::Emitter         out;
		out << YAML::BeginMap;
		out << YAML::Key << "Material" << YAML::Value << name;
		material.serialize(out);
		out << YAML::EndMap;

		const std::filesystem::path outputPath=
				std::filesystem::current_path().string() + path.string();

		std::filesystem::create_directory(outputPath.parent_path());

		std::ofstream ofs(outputPath);
		ofs << out.c_str();
		ofs.close();
	}

	void AssetManager::SaveTraceableAsset(const std::string&                name,
																				const std::unique_ptr<Traceable>& traceable)
	{
		std::filesystem::path path= "/resources/traceables/" + name + ".tble";
		YAML::Emitter         out;
		out << YAML::BeginMap;
		out << YAML::Key << "Traceable" << YAML::Value << name;
		traceable->serialize(out);
		out << YAML::EndMap;

		const std::filesystem::path outputPath=
				std::filesystem::current_path().string() + path.string();

		std::filesystem::create_directory(outputPath.parent_path());

		std::ofstream ofs(outputPath);
		ofs << out.c_str();
		ofs.close();
	}

	std::optional<AssetManager::NameAndPath> AssetManager::getNameAndPathOfTexture(uint32 id) const
	{
		auto it= m_textureNameAndPathMap.find(id);
		if(it != m_textureNameAndPathMap.end())
		{
			return it->second;
		}

		return std::nullopt;
	}

	std::optional<AssetManager::NameAndPath> AssetManager::getNameAndPathOfMaterial(uint32 id) const
	{
		auto it= m_materialNameAndPathMap.find(id);
		if(it != m_materialNameAndPathMap.end())
		{
			return it->second;
		}

		return std::nullopt;
	}

	std::optional<AstralRaytracer::AssetManager::NameAndPath>
	AssetManager::getNameAndPathOfTraceable(uint32 id) const
	{
		auto it= m_traceableNameAndPathMap.find(id);
		if(it != m_traceableNameAndPathMap.end())
		{
			return it->second;
		}

		return std::nullopt;
	}

} // namespace AstralRaytracer