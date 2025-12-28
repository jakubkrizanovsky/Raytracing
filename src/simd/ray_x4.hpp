#pragma once
#ifdef __ARM_NEON__

#include "vec3_x4.hpp"

#include <arm_neon.h>

namespace rte {

struct Ray_x4 {
    Vec3_x4 origin;
    Vec3_x4 direction;
};

} // namespace rte

#endif // __ARM_NEON__
