#pragma once

#include <glm/glm.hpp>

namespace rte {

struct PushConstants {
    glm::vec3 cameraPosition;
    float _pad0; // padding
    glm::vec3 cameraForward;
    float cameraFOV;

    glm::vec3 inverseLightDirection;
    float _pad1; // padding
    glm::vec3 directionalLightColor;
    float _pad2; // padding
    glm::vec3 ambientLightColor;

    uint32_t maxReflections;
    float diffuseReflectionConstant;
    float specularReflectionConstant;
    float specularExponent;
    
    uint32_t sphereCount;
    uint32_t width;
    uint32_t height;
};


} // namespace rte
