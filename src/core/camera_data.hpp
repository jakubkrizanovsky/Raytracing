#pragma once

#include <glm/glm.hpp>

namespace rte {

struct CameraData {
    const glm::vec3 position;
    const glm::vec3 forward;
    const float fov;
};


} // namespace rte