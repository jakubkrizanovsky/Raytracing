#pragma once

#include <core/cpu_renderer.hpp>
#include <core/sphere.hpp>
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
    SIMDRenderer(Device& device) : CpuRenderer{device} {}

    void prepareFrame() override;
private:
    Vec3Field raycast(RayField ray);
    glm::vec3 shadowRay(RaycastHit hit);
    uint32x4_t raySphereIntersect(Rayx4 ray, Sphere& sphere, RaycastHitx4* hit);

    glm::vec3 reflect(glm::vec3 rayDirection, glm::vec3 normal);

    const glm::vec3 inverseLightDirection = glm::normalize(glm::vec3(1, 1, -1));
    const glm::vec3 ambientLight = glm::vec3(0.1f, 0.1f, 0.1f);
    std::vector<Sphere> spheres = {
        {                   // first sphere 
            {0, 0, 0},      // in origin
            1,              // radius 1
            {0, 1, 1}       // cyan color
        }//,
        // {                   // second sphere 
        //     {1, 0.5f, 1},   // slightly moved
        //     0.5f,           // half the size
        //     {1, 0.1f, 0.1f} // red color
        // },
        // {                   // third sphere 
        //     {-1, 0.5f, -1}, // slightly moved
        //     0.5f,           // half the size
        //     {0.1f, 1, 0.1f} // green color
        // }
    };
};

} // namespace rte
