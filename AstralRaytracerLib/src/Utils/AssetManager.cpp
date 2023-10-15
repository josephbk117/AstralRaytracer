#include "Utils/AssetManager.h"

#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/TriangleTraceable.h"
#include "Utils/ModelManager.h"
#include "Utils/TextureManager.h"

#include <fstream>

namespace AstralRaytracer
{

	AssetManager::AssetManager()
	{
		std::random_device rd;
		auto               seed_data= std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
		std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
		m_randomNumGenerator= std::mt19937(seq);

		// Add Default Material
		m_materialNameAndPathMap.insert({
				0, {"Default Material", "NIL"}
    });
	}

	TextureDataRGBF AssetManager::LoadTextureAsset(const fs::path& path, const std::string& name)
	{
		static uint32 textureCount= 0;
		NameAndPath   nameAndPath = { name, path.string() };
		m_textureNameAndPathMap.emplace(textureCount, nameAndPath);
		textureCount++;
		return TextureManager::loadTextureDataFromFileRGBF(path);
	}

	bool AssetManager::LoadMaterialAsset(
			const fs::path&    path,
			const std::string& name,
			Material&          outMaterial
	)
	{
		static uint32 matCount   = 1;
		NameAndPath   nameAndPath= { name, path.string() };
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

	std::unique_ptr<AstralRaytracer::Traceable> AssetManager::LoadTraceableAsset(const fs::path& path)
	{
		static uint32 traceableCount= 0;

		std::ifstream stream(path);
		YAML::Node    data= YAML::Load(stream);

		if(!data["Traceable"])
		{
			return nullptr;
		}

		NameAndPath nameAndPath= { data["Traceable"].as<std::string>(), path.string() };
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

	void AssetManager::SaveMaterialAsset(
			const fs::path&    folderPath,
			const std::string& name,
			const Material&    material
	)
	{
		fs::path path= folderPath.string() + name + FileExtensionForMaterial;

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "UUID" << YAML::Value << generateUUIDasString();
		out << YAML::Key << "Material" << YAML::Value << name;
		material.serialize(out);
		out << YAML::EndMap;

		if(!fs::exists(folderPath) && fs::is_directory(folderPath))
		{
			fs::create_directory(folderPath);
		}

		std::ofstream ofs(path);
		ofs << out.c_str();
		ofs.close();
	}

	void AssetManager::SaveMaterialAsset(const std::string& name, const Material& material)
	{
		fs::path path= "/resources/materials/";
		SaveMaterialAsset(path, name, material);
	}

	void AssetManager::SaveTraceableAsset(
			const std::string&                name,
			const std::unique_ptr<Traceable>& traceable
	)
	{
		fs::path path= "/resources/traceables/" + name + FileExtensionForTraceable;

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "UUID" << YAML::Value << generateUUIDasString();
		out << YAML::Key << "Traceable" << YAML::Value << name;
		traceable->serialize(out);
		out << YAML::EndMap;

		const fs::path outputPath= fs::current_path().string() + path.string();

		fs::create_directory(outputPath.parent_path());

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

	uuids::uuid AssetManager::generateUUID()
	{
		return uuids::uuid_random_generator(m_randomNumGenerator)();
	}

	std::string AssetManager::generateUUIDasString() { return uuids::to_string(generateUUID()); }

} // namespace AstralRaytracer