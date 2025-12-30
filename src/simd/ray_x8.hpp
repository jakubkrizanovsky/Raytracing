#pragma once
#ifdef __ARM_NEON__

#include "vec3_x8.hpp"

#include <arm_neon.h>

namespace rte {

struct Ray_x8 {
    Vec3_x8 origin;
    Vec3_x8 direction;
};

} // namespace rte

#endif // __ARM_NEON__
