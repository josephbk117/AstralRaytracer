#include "Raytracer/Scene.h"

#include "Compositor/PostProcessing/BilateralFilterPostProcess.h"
#include "Compositor/PostProcessing/DesaturatePostProcess.h"
#include "Compositor/PostProcessing/GammaCorrectionPostProcess.h"
#include "Compositor/PostProcessing/LuminanceThresholdPostProcess.h"
#include "Compositor/PostProcessing/PostProcessing.h"
#include "Raytracer/Traceable/Traceable.h"

#include <fstream>

namespace AstralRaytracer
{
Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::initialize()
{
    unload();
}

const std::string_view Scene::getName() const
{
    return m_name;
}

bool Scene::hasSceneLoaded() const
{
    return m_sceneTraceables.size() > 0;
}

void Scene::addTraceable(std::unique_ptr<Traceable> &&traceable)
{
    m_sceneTraceables.push_back(std::move(traceable));
}

void Scene::addMaterial(const Material &material)
{
    m_materials.push_back(material);
}

void Scene::addTexture(TextureDataRGBF &&texture)
{
    m_textures.push_back(std::move(texture));
}

void Scene::addPostProcessing(std::unique_ptr<PostProcessing> &&postProcessing)
{
    postProcessing->init();
    m_postProcessingStack.push_back(std::move(postProcessing));
}

void Scene::serialize(const AssetManager &assetManager, const std::filesystem::path &path)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "New";

    // Textures
    out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;

    for (uint32 textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
    {
        std::optional<AssetManager::NameAndPath> nameAndPath = assetManager.getNameAndPathOfTexture(textureIndex);
        if (nameAndPath.has_value())
        {
            out << YAML::BeginMap;

            out << YAML::Key << nameAndPath.value().assetName << YAML::Value << nameAndPath.value().relativePath;
            out << YAML::EndMap;
        }
    }
    out << YAML::EndSeq;

    // Materials
    out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;

    for (uint32 matIndex = 0; matIndex < m_materials.size(); ++matIndex)
    {
        std::optional<AssetManager::NameAndPath> nameAndPath = assetManager.getNameAndPathOfMaterial(matIndex);
        if (nameAndPath.has_value())
        {
            out << YAML::BeginMap;

            out << YAML::Key << nameAndPath.value().assetName << YAML::Value << nameAndPath.value().relativePath;
            out << YAML::EndMap;
        }
    }

    out << YAML::EndSeq;

    // Trace-ables
    out << YAML::Key << "Traceables" << YAML::Value << YAML::BeginSeq;

    for (uint32 traceableIndex = 0; traceableIndex < m_sceneTraceables.size(); ++traceableIndex)
    {
        std::optional<AssetManager::NameAndPath> nameAndPath = assetManager.getNameAndPathOfTraceable(traceableIndex);
        if (nameAndPath.has_value())
        {
            out << YAML::BeginMap;

            out << YAML::Key << nameAndPath.value().assetName << YAML::Value << nameAndPath.value().relativePath;

            out << YAML::EndMap;
        }
    }
    out << YAML::EndSeq;

    const std::filesystem::path outputPath = std::filesystem::current_path().string() + path.string();

    std::filesystem::create_directory(outputPath.parent_path());

    std::ofstream ofs(outputPath);
    ofs << out.c_str();
    ofs.close();
}

void Scene::deserialize(AssetManager &assetManager, const std::filesystem::path &absolutePath)
{
    const std::string absolutePathStr = absolutePath.string();

    if (!std::filesystem::exists(absolutePath))
    {
        ASTRAL_LOG_ERROR("Scene file at path :{}, does not exist", absolutePathStr);
    }
    else
    {
        ASTRAL_LOG_INFO("Successfully deserialized scene file : {}", absolutePathStr);
    }

    std::ifstream stream(absolutePathStr);
    YAML::Node data = YAML::Load(stream);

    if (!data["Scene"])
    {
        ASTRAL_LOG_WARN("Tried to deserialize non-existing scene");
        return;
    }

    m_name = data["Scene"].as<std::string>();
    ASTRAL_LOG_INFO("Scene : {}", m_name);

    const auto &textures = data["Textures"];
    ASTRAL_LOG_TRACE("Number of textures : {}", textures.size());

    for (uint32 texIndex = 0; texIndex < textures.size(); ++texIndex)
    {
        const auto &tex = textures[texIndex];
        for (auto it : tex)
        {
            addTexture(assetManager.loadTextureAsset(it.second.as<std::string>(), it.first.as<std::string>()));
            ASTRAL_LOG_TRACE("Texture at index : {}, {}", texIndex, it.first.as<std::string>());
        }
    }

    const auto &materials = data["Materials"];
    ASTRAL_LOG_TRACE("Number of materials : {}", materials.size());

    for (uint32 matIndex = 0; matIndex < materials.size(); ++matIndex)
    {
        const auto &mat = materials[matIndex];
        for (auto it : mat)
        {
            Material materialNew;
            if (assetManager.loadMaterialAsset(it.second.as<std::string>(), it.first.as<std::string>(), materialNew))
            {
                addMaterial(materialNew);
                ASTRAL_LOG_TRACE("Material at index {} : {}", matIndex, it.first.as<std::string>());
            }
            else
            {
                ASTRAL_LOG_ERROR("material at index {}, could not be loaded", matIndex);
            }
        }
    }

    const auto &traceables = data["Traceables"];
    ASTRAL_LOG_TRACE("Number of traceables {}", traceables.size());

    for (uint32 traceIndex = 0; traceIndex < traceables.size(); ++traceIndex)
    {
        const auto &traceable = traceables[traceIndex];
        for (auto it : traceable)
        {
            addTraceable(assetManager.loadTraceableAsset(it.second.as<std::string>()));
            ASTRAL_LOG_TRACE("Traceable at index {} : {}", traceIndex, it.first.as<std::string>());
        }
    }
}

void Scene::unload()
{
    m_sceneTraceables.clear();
    m_materials.clear();
    m_textures.clear();
    m_postProcessingStack.clear();

    // Need all tracked assets to be empty before setting defaults
    setupDefaults();
}

void Scene::setupDefaults()
{
    ASTRAL_ASSERTM(m_sceneTraceables.size() == 0 && m_materials.size() == 0 && m_textures.size() == 0 &&
                       m_postProcessingStack.size() == 0,
                   "Scene tried to set defaults while data already exists");

    addMaterial(Material());
    TextureDataRGBF defaultTexData({1, 1});
    defaultTexData.setTexelColorAtPixelIndex(0, glm::vec3(1, 1, 1));
    addTexture(std::move(defaultTexData));
    addPostProcessing(std::make_unique<GammaCorrectionPostProcessing>());
}

} // namespace AstralRaytracer