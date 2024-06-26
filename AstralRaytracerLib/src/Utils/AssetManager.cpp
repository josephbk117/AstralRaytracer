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
    auto seed_data = std::array<int, std::mt19937::state_size>{};
    std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    m_randomNumGenerator = std::mt19937(seq);

    clearAndResetCachedData();
}

bool AssetManager::loadProject(const fs::path &absolutePath)
{
    const std::string absolutePathStr = absolutePath.generic_string();

    std::ifstream stream(absolutePathStr);

    YAML::Node data = YAML::Load(stream);

    if (!data["Project"])
    {
        ASTRAL_LOG_WARN("Project failed to load: {}", absolutePathStr);
        return false;
    }

    m_currentProjectName = data["Project"].as<std::string>();
    m_defaultSceneRelativePath = data["Default Scene"].as<std::string>();
    m_currentRelativePath = absolutePath.parent_path().generic_string() + "/";

    initializeFileCache();
    clearAndResetCachedData();

    ASTRAL_LOG_INFO("Project file loaded successfully: {}", absolutePathStr);

    return true;
}

void AssetManager::clearAndResetCachedData()
{
    m_traceableNameAndPathMap.clear();
    m_materialNameAndPathMap.clear();
    m_textureNameAndPathMap.clear();

    textureCount = 0;
    matCount = 1;
    traceableCount = 0;

    // Add Default Material
    m_materialNameAndPathMap.insert(
        {uuids::uuid::from_string("00000000-0000-0000-0000-000000000000").value(), {"Default Material", "NIL"}});
}

TextureDataRGBF AssetManager::loadTextureAsset(const fs::path &path, const std::string &name)
{
    NameAndPath nameAndPath = {name, path.string()};
    m_textureNameAndPathMap.emplace(textureCount, nameAndPath);
    textureCount++;

    TextureDataRGBF outputTextureData;
    TextureManager::loadTextureDataFromFile(m_currentRelativePath + path.string(), outputTextureData, true);

    return outputTextureData;
}

bool AssetManager::loadMaterialAsset(const fs::path &path, const std::string &name, Material &outMaterial)
{
    NameAndPath nameAndPath = {name, path.string()};
    matCount++;

    std::ifstream stream(m_currentRelativePath + path.string());
    YAML::Node data = YAML::Load(stream);

    if (!data["Material"])
    {
        return false;
    }

    outMaterial.deserialize(*this, data);
    m_materialNameAndPathMap.emplace(outMaterial.getUUID(), nameAndPath);

    return true;
}

std::unique_ptr<AstralRaytracer::Traceable> AssetManager::loadTraceableAsset(const fs::path &path)
{
    std::ifstream stream(m_currentRelativePath + path.string());
    YAML::Node data = YAML::Load(stream);

    if (!data["Traceable"])
    {
        return nullptr;
    }

    NameAndPath nameAndPath = {data["Traceable"].as<std::string>(), path.string()};
    m_traceableNameAndPathMap.emplace(traceableCount, nameAndPath);

    traceableCount++;

    Serialization::TraceableType type = static_cast<Serialization::TraceableType>(data["Type"].as<uint32>());
    switch (type)
    {
    case AstralRaytracer::Serialization::TraceableType::INVALID:
        return nullptr;
    case AstralRaytracer::Serialization::TraceableType::SPEHRE: {
        auto sphere = std::make_unique<AstralRaytracer::SphereTraceable>();
        sphere->deserialize(*this, data);
        return sphere;
    };
    case AstralRaytracer::Serialization::TraceableType::TRIANGLE: {
        auto triangle = std::make_unique<AstralRaytracer::TriangleTraceable>();
        triangle->deserialize(*this, data);
        return triangle;
    }
    case AstralRaytracer::Serialization::TraceableType::STATIC_MESH: {
        auto mesh = std::make_unique<AstralRaytracer::StaticMesh>();
        mesh->deserialize(*this, data);
        return mesh;
    }
    default:
        break;
    }

    return nullptr;
}

void AssetManager::saveMaterialAsset(const fs::path &folderPath, const std::string &name, const Material &material)
{
    fs::path path = folderPath.string() + name + FileExtensionForMaterial.data();

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "UUID" << YAML::Value << generateUUIDasString();
    out << YAML::Key << "Material" << YAML::Value << name;
    material.serialize(*this, out);
    out << YAML::EndMap;

    if (!fs::exists(folderPath) && fs::is_directory(folderPath))
    {
        fs::create_directory(folderPath);
    }

    std::ofstream ofs(path);
    ofs << out.c_str();
    ofs.close();
}

void AssetManager::saveMaterialAsset(const std::string &name, const Material &material)
{
    fs::path path = "/resources/materials/";
    saveMaterialAsset(path, name, material);
}

void AssetManager::SaveTraceableAsset(const std::string &name, const std::unique_ptr<Traceable> &traceable)
{
    fs::path path = "/resources/traceables/" + name + FileExtensionForTraceable.data();

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "UUID" << YAML::Value << generateUUIDasString();
    out << YAML::Key << "Traceable" << YAML::Value << name;
    traceable->serialize(*this, out);
    out << YAML::EndMap;

    const fs::path outputPath = fs::current_path().string() + path.string();

    fs::create_directory(outputPath.parent_path());

    std::ofstream ofs(outputPath);
    ofs << out.c_str();
    ofs.close();
}

std::optional<AssetManager::NameAndPath> AssetManager::getNameAndPathOfTexture(uint32 id) const
{
    auto it = m_textureNameAndPathMap.find(id);
    if (it != m_textureNameAndPathMap.end())
    {
        return it->second;
    }

    return std::nullopt;
}

std::optional<AssetManager::NameAndPath> AssetManager::getNameAndPathOfMaterial(uint32 index) const
{
    auto it = m_materialNameAndPathMap.begin();

    while (it != m_materialNameAndPathMap.end() && index > 0)
    {
        index--;
        it++;
    }

    if (it != m_materialNameAndPathMap.end())
    {
        return it->second;
    }

    return std::nullopt;
}

std::optional<AssetManager::NameAndPath> AssetManager::getNameAndPathOfMaterial(uuids::uuid id) const
{
    auto it = m_materialNameAndPathMap.find(id);
    if (it != m_materialNameAndPathMap.end())
    {
        return it->second;
    }

    return std::nullopt;
}

uint32 AssetManager::getMaterialIndex(uuids::uuid id) const
{
    auto it = m_materialNameAndPathMap.begin();
    uint32 iterationCount = 0;
    while (it != m_materialNameAndPathMap.end())
    {
        if (it->first == id)
        {
            return iterationCount;
        }
        iterationCount++;
        it++;
    }

    return iterationCount;
}

std::optional<AstralRaytracer::AssetManager::NameAndPath> AssetManager::getNameAndPathOfTraceable(uint32 id) const
{
    auto it = m_traceableNameAndPathMap.find(id);
    if (it != m_traceableNameAndPathMap.end())
    {
        return it->second;
    }

    return std::nullopt;
}

const std::string &AssetManager::getCurrentRelativePath() const
{
    return m_currentRelativePath;
}

const std::string &AssetManager::getDefaultSceneRelativePath() const
{
    return m_defaultSceneRelativePath;
}

std::string AssetManager::getDefaultSceneAbsolutePath() const
{
    return m_currentRelativePath + m_defaultSceneRelativePath;
}

const std::string &AssetManager::getCurrentProjectName() const
{
    return m_currentProjectName;
}

bool AssetManager::isProjectOpen() const
{
    return !m_currentProjectName.empty();
}

uuids::uuid AssetManager::generateUUID()
{
    return uuids::uuid_random_generator(m_randomNumGenerator)();
}

std::string AssetManager::generateUUIDasString()
{
    return uuids::to_string(generateUUID());
}

namespace
{
void visitPathNode(std::unique_ptr<FileSystem::PathNode> &node)
{
    ASTRAL_LOG_INFO("Visiting: {}", node->pathStr.generic_string());

    for (std::unique_ptr<FileSystem::PathNode> &child : node->nodes)
    {
        visitPathNode(child);
    }
}
} // namespace

void AssetManager::initializeFileCache()
{
    std::ofstream ofs(m_currentRelativePath + "FILECACHE");

    auto rootNode = std::make_unique<FileSystem::PathNode>();
    rootNode->pathStr = m_currentRelativePath;

    FileSystem::traverseDirectoryFromRoot(rootNode);
    visitPathNode(rootNode);
    ofs.close();
}

} // namespace AstralRaytracer