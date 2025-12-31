#pragma once

#include <core/sphere.hpp>

#include <glm/glm.hpp>

namespace rte {

struct RaycastHit {
    glm::vec3 position;
    glm::vec3 normal;
    float distance;
    bool hit;
};

} // namespace rte
