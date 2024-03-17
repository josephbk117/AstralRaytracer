#pragma once

#include "Traceable.h"

namespace AstralRaytracer
{
class TriangleTraceable : public Traceable
{
  public:
    TriangleTraceable();
    TriangleTraceable(const glm::vec3 &vertexA, const glm::vec3 &vertexB, const glm::vec3 &vertexC);

    virtual void setScale(const glm::vec3 &scale) override;

    [[nodiscard]] float32 getTriangleDistance() const;

    [[nodiscard]] bool trace(const Ray &rayIn, HitInfo &hitInfo) const override;

    void serialize(AssetManager &assetManager, YAML::Emitter &out) const override;
    void deserialize(AssetManager &assetManager, YAML::Node &node) override;

  private:
    glm::vec3 m_vertexA, m_vertexB, m_vertexC;
    glm::vec3 m_InitialVertexA, m_InitialVertexB, m_InitialVertexC;
    glm::vec3 m_normal;
    glm::vec3 m_vAvB;
    glm::vec3 m_vAvC;
};

} // namespace AstralRaytracer