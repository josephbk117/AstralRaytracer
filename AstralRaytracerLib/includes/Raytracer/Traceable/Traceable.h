#pragma once
#include "Raytracer/Ray.h"
#include "Raytracer/Transform.h"
#include "Utils/Common.h"
#include "Utils/Serialization.h"

namespace AstralRaytracer
{
class Traceable : public Serialization::Serializable
{
  public:
    Traceable() = default;
    Traceable(const AABB &boundingBox);
    virtual ~Traceable(){};

    virtual void setPosition(const glm::vec3 &position)
    {
        m_transform.setPosition(position);
    }

    [[nodiscard]] virtual const glm::vec3 &getPosition() const;

    virtual void setRotation(float32 rotation)
    {
        m_transform.setRotation(rotation);
    }

    [[nodiscard]] virtual float32 getRotation() const;

    virtual void setScale(const glm::vec3 &scale)
    {
        m_transform.setScale(scale);
    }

    [[nodiscard]] virtual const glm::vec3 &getScale() const;

    [[nodiscard]] virtual const glm::mat4 &getTransformMatrix() const;

    [[nodiscard]] virtual Transform &getTransform();

    virtual void setMaterialIndex(uint32 materialIndex)
    {
        m_materialIndex = materialIndex;
    }

    [[nodiscard]] virtual uint32 getMaterialIndex() const;

    [[nodiscard]] virtual bool trace(const Ray &rayIn, HitInfo &hitInfo) const = 0;

    virtual void serialize(AssetManager &assetManager, YAML::Emitter &out) const override;
    virtual void deserialize(AssetManager &assetManager, YAML::Node &node) override;

  protected:
    Transform m_transform;
    uint32 m_materialIndex = 0; // points to default material

    AABB m_boundingBox;
    AABB m_initialBoundingBox;
};
} // namespace AstralRaytracer