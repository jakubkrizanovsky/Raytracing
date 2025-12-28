#pragma once

#include <glm/glm.hpp>

namespace rte {

struct LightData {
    glm::vec3 directionalLightDirection;
    glm::vec3 directionalLightColor;
    glm::vec3 ambientLightColor;
};

} // namespace rte
