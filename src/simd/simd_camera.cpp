#include "simd_camera.hpp"
#include <core/constants.hpp>

namespace rte {

void SIMDCamera::prepareFrame(VkExtent2D extent) {
    right = glm::cross(UP, forward);

    float gx = tan(0.5f * glm::radians(fov));
    float gy = -gx * (extent.height - 1) / (extent.width - 1);
    qx = 2 * gx / (extent.width - 1) * right;
    qy = 2 * gy / (extent.height - 1) * UP;
    p1m = Vec3x4(forward) - gx * right - gy * UP;
}

Rayx4 SIMDCamera::getRay(uint32_t xIndex, uint32_t yIndex) {
    static const uint32x4_t ramp0123 = {0, 1, 2, 3};
    float32x4_t xIndices = vcvtq_f32_s32(vreinterpretq_s32_u32(vaddq_u32(vdupq_n_u32(xIndex), ramp0123)));

    Vec3x4 direction = p1m + qx * xIndices + qy * yIndex;
    return {Vec3x4(position), direction.normalized()};
}

} // namespace rte
