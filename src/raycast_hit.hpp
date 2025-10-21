#pragma once

#include "sphere.hpp"

#include <glm/glm.hpp>

namespace rte {

struct RaycastHit {
    glm::vec3 position;
    float distance;
};

} // namespace rte
