#pragma once

#include "vec3_field.hpp"

namespace rte {

struct RayField {
    Vec3Field origins;
    Vec3Field directions;
};

} // namespace rte
