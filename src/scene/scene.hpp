#pragma once

#include "camera.hpp"
#include "light_data.hpp"
#include <core/sphere.hpp>

// std
#include <vector>

namespace rte {

struct Scene {
    Camera camera;
    LightData lightData;
    std::vector<Sphere> spheres;
};

} // namespace rte
