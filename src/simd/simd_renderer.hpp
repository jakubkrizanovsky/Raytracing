#pragma once
#ifdef __ARM_NEON__

#include <core/cpu_renderer.hpp>
#include "vec3_field.hpp"
#include "ray_field.hpp"
#include "rayx4.hpp"
#include "raycast_hitx4.hpp"
#include <sequential/raycast_hit.hpp>
#include <sequential/ray.hpp>

#include <glm/glm.hpp>


namespace rte {

class SIMDRenderer : public CpuRenderer {
public:
    SIMDRenderer(std::shared_ptr<Device> device) : CpuRenderer{device} {}

    void prepareFrame() override;
private:
    Vec3Field raycast(RayField ray);
    Vec3x4 shadowRay(RaycastHitx4 hit);
    uint32x4_t raySphereIntersect(Rayx4 ray, Sphere& sphere, RaycastHitx4* hit);

    Vec3x4 reflect(Vec3x4 rayDirection, Vec3x4 normal);

    const Vec3x4 inverseLightDirection = glm::normalize(glm::vec3(1, 1, -1));
    const Vec3x4 ambientLight = {0.1f};
};

} // namespace rte

#endif // __ARM_NEON__
