#pragma once
#include "ColourData.h"
#include "Common.h"
#include "Raytracer/Transform.h"

#include <uuid.h>
#include <yaml-cpp/yaml.h>

namespace AstralRaytracer
{
class AssetManager;
}

namespace YAML
{
template <> struct convert<glm::vec3>
{
    static Node encode(const glm::vec3 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node &node, glm::vec3 &rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
        {
            return false;
        }
        rhs.x = node[0].as<float32>();
        rhs.y = node[1].as<float32>();
        rhs.z = node[2].as<float32>();

        return true;
    }
};

template <> struct convert<AstralRaytracer::ColourData>
{
    static Node encode(const AstralRaytracer::ColourData &rhs)
    {
        Node node;
        glm::vec3 colour = rhs.getColour_32_bit();
        node.push_back(colour.x);
        node.push_back(colour.y);
        node.push_back(colour.z);
        return node;
    }

    static bool decode(const Node &node, AstralRaytracer::ColourData &rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
        {
            return false;
        }
        const float32 x = node[0].as<float32>();
        const float32 y = node[1].as<float32>();
        const float32 z = node[2].as<float32>();

        rhs = AstralRaytracer::ColourData(x, y, z, 1.0f);
        return true;
    }
};

template <> struct convert<Transform>
{
    static Node encode(const Transform &rhs)
    {
        Node node;

        const glm::vec3 &pos = rhs.getPosition();
        const float32 rot = rhs.getRotation();
        const glm::vec3 &scale = rhs.getScale();

        node.push_back(pos.x);
        node.push_back(pos.y);
        node.push_back(pos.z);

        node.push_back(rot);

        node.push_back(scale.x);
        node.push_back(scale.y);
        node.push_back(scale.z);

        return node;
    }

    static bool decode(const Node &node, Transform &rhs)
    {
        if (!node.IsSequence() || node.size() != 7)
        {
            return false;
        }
        const float32 posX = node[0].as<float32>();
        const float32 posY = node[1].as<float32>();
        const float32 posZ = node[2].as<float32>();

        const float32 rot = node[3].as<float32>();

        const float32 scaleX = node[4].as<float32>();
        const float32 scaleY = node[5].as<float32>();
        const float32 scaleZ = node[6].as<float32>();

        rhs = Transform({posX, posY, posZ}, rot, {scaleX, scaleY, scaleZ});
        return true;
    }
};

} // namespace YAML

namespace AstralRaytracer
{
namespace Serialization
{
YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &vec3);
YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &vec4);
YAML::Emitter &operator<<(YAML::Emitter &out, const ColourData &colourData);
YAML::Emitter &operator<<(YAML::Emitter &out, const Transform &transform);

enum class TraceableType : uint32
{
    INVALID,
    SPEHRE,
    TRIANGLE,
    STATIC_MESH,
    MAX = STATIC_MESH
};

class Serializable
{
  public:
    Serializable() = default;
    virtual void serialize(AssetManager &assetManager, YAML::Emitter &out) const = 0;
    virtual void deserialize(AssetManager &assetManager, YAML::Node &node) = 0;

    void setUUID(uuids::uuid id)
    {
        m_id = id;
    };

    [[nodiscard]] const uuids::uuid &getUUID() const;

  protected:
    uuids::uuid m_id;
};
} // namespace Serialization
} // namespace AstralRaytracer