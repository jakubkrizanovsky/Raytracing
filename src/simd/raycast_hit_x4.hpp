#pragma once
#ifdef __ARM_NEON__

#include "vec3_x4.hpp"

#include <arm_neon.h>

namespace rte {

struct RaycastHit_x4 {
    Vec3_x4 position;
    Vec3_x4 normal;
    uint32x4_t mask;
    float32x4_t distance;
};

} // namespace rte

#endif // __ARM_NEON__
