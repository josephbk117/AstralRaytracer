#pragma once
#include "Utils/Common.h"

struct Ray
{
    CoOrd3DF worldSpacePosition;
    Direction3D direction;
};

struct HitInfo
{
    float32 hitDistance = std::numeric_limits<float32>::max();
    uint32 objectIndex = 0;
    uint32 materialIndex = 0;
    Direction3D worldSpaceNormal;
    CoOrd3DF worldSpacePosition;

    bool isValid() const
    {
        return hitDistance > 0.0f && hitDistance < std::numeric_limits<float32>::max();
    }
};