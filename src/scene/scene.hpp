#pragma once

#include "camera.hpp"
#include <core/sphere.hpp>

// std
#include <vector>

namespace rte {

struct Scene {
    Camera camera;
    std::vector<Sphere> spheres;
};

} // namespace rte
