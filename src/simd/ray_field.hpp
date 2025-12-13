#pragma once
#ifdef __ARM_NEON__

#include "vec3_field.hpp"

namespace rte {

struct RayField {
    Vec3Field origins;
    Vec3Field directions;
};

} // namespace rte

#endif // __ARM_NEON__
