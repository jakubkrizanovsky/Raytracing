#pragma once

#include <glm/glm.hpp>

namespace rte {

struct Camera {
    glm::vec3 position;
    glm::vec3 forward;
    float fov;

    // movement
    glm::vec3 focusPoint;
    float orbitVelocity;
};

} // namespace rte
