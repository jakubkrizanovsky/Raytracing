#pragma once
#ifdef __ARM_NEON__


#include "ray_x8.hpp"
#include "raycast_hit_x8.hpp"
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

    Vec3_x8 raycast(Ray_x8 ray);
    Vec3_x8 shadowRay(RaycastHit_x8 hit);
    uint16x8_t raySphereIntersect(Ray_x8 ray, Sphere& sphere, RaycastHit_x8* hit);
    uint8x8x4_t packBGRA(Vec3_x8& color);

    Vec3_x8 reflect(Vec3_x8 rayDirection, Vec3_x8 normal);

    Vec3_x8 inverseLightDirection;
    Vec3_x8 ambientLight;
};

} // namespace rte

#endif // __ARM_NEON__
