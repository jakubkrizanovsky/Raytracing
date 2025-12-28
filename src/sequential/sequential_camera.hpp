#pragma once

#include "ray.hpp"
#include <scene/camera.hpp>

#include <vulkan/vulkan.h>

namespace rte {

class SequentialCamera {
public:
    SequentialCamera(Camera cameraData) : position{cameraData.position}, 
            forward{cameraData.forward}, fov{cameraData.fov} {}

    void updateCameraData(Camera cameraData);
    void prepareFrame(VkExtent2D extent);
    Ray getRay(uint32_t xIndex, uint32_t yIndex);

private:
    glm::vec3 position;
    glm::vec3 forward;
    const float fov;

    //frame specific data
    glm::vec3 right;
    glm::vec3 qx;
    glm::vec3 qy;
    glm::vec3 p1m;
};

} // namespace rte