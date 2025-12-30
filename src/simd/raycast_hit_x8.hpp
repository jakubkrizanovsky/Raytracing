#pragma once
#ifdef __ARM_NEON__

#include "vec3_x8.hpp"

#include <arm_neon.h>

namespace rte {

struct RaycastHit_x8 {
    Vec3_x8 position;
    Vec3_x8 normal;
    uint16x8_t mask;
    float16x8_t distance;
};

} // namespace rte

#endif // __ARM_NEON__
