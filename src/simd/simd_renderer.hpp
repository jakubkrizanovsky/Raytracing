#pragma once
#ifdef __ARM_NEON__


#include "ray_x4.hpp"
#include "raycast_hit_x4.hpp"
#include "simd_camera.hpp"
#include <core/cpu_renderer.hpp>
#include <sequential/raycast_hit.hpp>
#include <sequential/ray.hpp>

#include <glm/glm.hpp>


namespace rte {

class SIMDRenderer : public CpuRenderer {
public:
    SIMDRenderer(std::shared_ptr<Device> device) : CpuRenderer{device} {}

    void prepareFrame() override;
    void setScene(std::shared_ptr<Scene> newScene) override;
private:
    std::unique_ptr<SIMDCamera> camera = nullptr;

    Vec3_x4 raycast(Ray_x4 ray);
    Vec3_x4 shadowRay(RaycastHit_x4 hit);
    uint32x4_t raySphereIntersect(Ray_x4 ray, Sphere& sphere, RaycastHit_x4* hit);
    uint8x16_t packBGRA(Vec3_x4& color);

    Vec3_x4 reflect(Vec3_x4 rayDirection, Vec3_x4 normal);

    Vec3_x4 inverseLightDirection;
    Vec3_x4 ambientLight;
};

} // namespace rte

#endif // __ARM_NEON__
