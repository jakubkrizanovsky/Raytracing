#pragma once

#include <arm_neon.h>

// std
#include <vector>

namespace rte {

struct Vec3Field {
    std::vector<float32_t> x;
    std::vector<float32_t> y;
    std::vector<float32_t> z;
};

} // namespace rte
