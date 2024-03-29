#include "Raytracer/Traceable/SphereTraceable.h"

namespace AstralRaytracer
{
float32 SphereTraceable::getRadius() const
{
    return m_radius;
}

void SphereTraceable::setScale(const glm::vec3 &scale)
{
    glm::vec3 newScale = scale;
    m_radius = ((scale.x == scale.y) ? scale.z : ((scale.y == scale.z) ? scale.x : scale.y));
    Traceable::setScale(glm::vec3(m_radius));
}

bool SphereTraceable::trace(const Ray &rayIn, HitInfo &hitInfo) const
{
    const glm::vec3 adjustedOrigin(rayIn.worldSpacePosition - m_transform.getPosition());

    const float32 a = glm::dot(rayIn.direction, rayIn.direction);
    const float32 b = 2.0f * glm::dot(adjustedOrigin, rayIn.direction);
    const float32 c = glm::dot(adjustedOrigin, adjustedOrigin) - m_radius * m_radius;

    const float32 discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f)
    {
        return false;
    }

    float32 closestHit = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    // float32 t0      = (-b + glm::sqrt(discriminant)) / (2.0f * a); // second hit

    if (closestHit < 0.0f)
    {
        return false;
    }

    hitInfo.worldSpacePosition = adjustedOrigin + rayIn.direction * closestHit;
    hitInfo.worldSpaceNormal = glm::normalize(hitInfo.worldSpacePosition);
    hitInfo.worldSpacePosition += m_transform.getPosition();

    hitInfo.materialIndex = m_materialIndex;
    hitInfo.hitDistance = closestHit;
    return true;
}

void SphereTraceable::serialize(AssetManager &assetManager, YAML::Emitter &out) const
{
    using namespace Serialization;
    Traceable::serialize(assetManager, out);
    out << YAML::Key << "Type" << YAML::Value << static_cast<uint32>(TraceableType::SPEHRE);
    out << YAML::Key << "Radius" << YAML::Value << m_radius;
}

void SphereTraceable::deserialize(AssetManager &assetManager, YAML::Node &node)
{
    Traceable::deserialize(assetManager, node);
    m_id = uuids::uuid::from_string(node["UUID"].as<std::string>()).value();
    m_radius = node["Radius"].as<float32>();
    Traceable::setScale(glm::vec3(m_radius));
}

} // namespace AstralRaytracer