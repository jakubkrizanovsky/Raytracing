#pragma once
#ifdef __ARM_NEON__


#include "rayx4.hpp"
#include "raycast_hitx4.hpp"
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

    Vec3x4 raycast(Rayx4 ray);
    Vec3x4 shadowRay(RaycastHitx4 hit);
    uint32x4_t raySphereIntersect(Rayx4 ray, Sphere& sphere, RaycastHitx4* hit);
    uint8x16_t packBGRA(Vec3x4& color);

    Vec3x4 reflect(Vec3x4 rayDirection, Vec3x4 normal);

    Vec3x4 inverseLightDirection;
    Vec3x4 ambientLight;
};

} // namespace rte

#endif // __ARM_NEON__
