#pragma once

#include <glm/glm.hpp>

namespace rte {

struct Camera {
    glm::vec3 position;
    glm::vec3 forward;
    float fov;
};

} // namespace rte
