#pragma once
#ifdef __ARM_NEON__

#include "vec3x4.hpp"

#include <arm_neon.h>

namespace rte {

struct Rayx4 {
    Vec3x4 origin;
    Vec3x4 direction;
};

} // namespace rte

#endif // __ARM_NEON__
