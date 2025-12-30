#pragma once

#include "ray_x8.hpp"
#include <scene/camera.hpp>

#include <vulkan/vulkan.h>

namespace rte {

class SIMDCamera {
public:
    SIMDCamera(Camera cameraData) : position{cameraData.position}, 
            forward{cameraData.forward}, fov{cameraData.fov} {}

    void updateCameraData(Camera cameraData);
    void prepareFrame(VkExtent2D extent);
    Ray_x8 getRay(uint32_t xIndex, uint32_t yIndex);
private:
    glm::vec3 position;
    glm::vec3 forward;
    const float fov;

    //frame specific data
    Vec3_x8 right;
    Vec3_x8 qx;
    Vec3_x8 qy;
    Vec3_x8 p1m;
};

} // namespace rte
