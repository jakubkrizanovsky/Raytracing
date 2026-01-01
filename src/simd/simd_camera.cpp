#include "simd_camera.hpp"
#include <core/constants.hpp>

namespace rte {

void SIMDCamera::updateCameraData(Camera cameraData) {
    position = cameraData.position;
    forward = cameraData.forward;
}

void SIMDCamera::prepareFrame(VkExtent2D extent) {
    right = Vec3_x4(glm::cross(UP, forward));
    Vec3_x4 up = Vec3_x4(UP);

    float gx = tan(0.5f * glm::radians(fov));
    float gy = -gx * (extent.height - 1) / (extent.width - 1);
    qx = 2 * gx / (extent.width - 1) * right;
    qy = 2 * gy / (extent.height - 1) * up;
    p1m = Vec3_x4(forward) - gx * right - gy * up;
}

Ray_x4 SIMDCamera::getRay(uint32_t xIndex, uint32_t yIndex) {
    static const uint32x4_t ramp0123 = {0, 1, 2, 3};
    float32x4_t xIndices = vcvtq_f32_s32(vreinterpretq_s32_u32(vaddq_u32(vdupq_n_u32(xIndex), ramp0123)));

    Vec3_x4 direction = p1m + qx * xIndices + qy * yIndex;
    return {Vec3_x4(position), direction.normalized()};
}

} // namespace rte
