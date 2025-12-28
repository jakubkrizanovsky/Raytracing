#pragma once

#include "ray_x4.hpp"
#include <scene/camera.hpp>

#include <vulkan/vulkan.h>

namespace rte {

class SIMDCamera {
public:
    SIMDCamera(Camera cameraData) : position{cameraData.position}, 
            forward{cameraData.forward}, fov{cameraData.fov} {}

    void updateCameraData(Camera cameraData);
    void prepareFrame(VkExtent2D extent);
    Ray_x4 getRay(uint32_t xIndex, uint32_t yIndex);
private:
    glm::vec3 position;
    glm::vec3 forward;
    const float fov;

    //frame specific data
    Vec3_x4 right;
    Vec3_x4 qx;
    Vec3_x4 qy;
    Vec3_x4 p1m;
};

} // namespace rte
