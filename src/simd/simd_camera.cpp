#include "simd_camera.hpp"
#include <core/constants.hpp>

namespace rte {

void SIMDCamera::updateCameraData(Camera cameraData) {
    position = cameraData.position;
    forward = cameraData.forward;
}

void SIMDCamera::prepareFrame(VkExtent2D extent) {
    right = glm::cross(UP, forward);

    float gx = tan(0.5f * glm::radians(fov));
    float gy = -gx * (extent.height - 1) / (extent.width - 1);
    qx = 2 * gx / (extent.width - 1) * right;
    qy = 2 * gy / (extent.height - 1) * UP;
    p1m = Vec3_x8(forward) - gx * right - gy * UP;
}

Ray_x8 SIMDCamera::getRay(uint32_t xIndex, uint32_t yIndex) {
    static const uint16x8_t ramp0123 = {0, 1, 2, 3};
    float16x8_t xIndices = vcvtq_f16_s16(vreinterpretq_s32_u16(vaddq_u16(vdupq_n_u16(xIndex), ramp0123)));

    Vec3_x8 direction = p1m + qx * xIndices + qy * yIndex;
    return {Vec3_x8(position), direction.normalized()};
}

} // namespace rte
