#pragma once
#include "Common.h"

namespace AstralRaytracer
{
namespace Random
{
inline uint32 pcgHash(uint32 input)
{
    uint32 state = input * 747796405u + 2891336453u;
    uint32 word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

inline float32 randomFloat(uint32 &seed)
{
    constexpr float32 oneOverFloatLimit = 1.0f / static_cast<float32>(std::numeric_limits<uint32>::max());

    seed = pcgHash(seed);
    return static_cast<float32>(seed) * oneOverFloatLimit;
}

inline float32 randomFloatSymmetric(uint32 &seed)
{
    return randomFloat(seed) * 2.0f - 1.0f;
}

inline glm::vec3 unitSphere(uint32 &seed)
{
    return glm::normalize(
        glm::vec3(randomFloatSymmetric(seed), randomFloatSymmetric(seed), randomFloatSymmetric(seed)));
}

inline glm::vec3 unitHemiSphere(uint32 &seed, const glm::vec3 &normal)
{
    glm::vec3 on_unit_sphere = unitSphere(seed);
    return on_unit_sphere * glm::sign(glm::dot(on_unit_sphere, normal));
}

inline glm::vec3 unitDisk(uint32 &seed)
{
    constexpr float32 twoPI = 2.0f * std::numbers::pi;

    const float32 rand1 = randomFloat(seed);

    const float32 angle = twoPI * rand1;
    const float32 radius = glm::max(randomFloat(seed), randomFloat(seed));

    const float32 x = radius * glm::cos(angle);
    const float32 y = radius * glm::sin(angle);

    return glm::vec3(x, y, 0.0f);
}
} // namespace Random
} // namespace AstralRaytracer