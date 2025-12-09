#include "sequential_camera.hpp"
#include <core/constants.hpp>

#include <iostream>

namespace rte {

void printVec(glm::vec3 vec) {
    std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
}

void SequentialCamera::prepareFrame(VkExtent2D extent) {
    right = glm::cross(UP, forward);

    float gx = tan(0.5f * glm::radians(fov));
    float gy = -gx * (extent.height - 1) / (extent.width - 1);
    qx = 2 * gx / (extent.width - 1) * right;
    qy = 2 * gy / (extent.height - 1) * UP;
    p1m = forward - gx * right - gy * UP;
}

Ray SequentialCamera::getRay(uint32_t xIndex, uint32_t yIndex) {
    glm::vec3 direction = p1m + qx * (float)xIndex + qy * (float)yIndex;
    return {position, glm::normalize(direction)};
}

}