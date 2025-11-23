#pragma once

#include "vec3x4.hpp"

#include <arm_neon.h>

namespace rte {

struct RaycastHitx4 {
    Vec3x4 position;
    Vec3x4 normal;
    uint32x4_t mask;
    float32x4_t distance;
};

} // namespace rte
