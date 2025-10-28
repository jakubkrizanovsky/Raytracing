#pragma once

#include <core/cpu_renderer.hpp>
#include <core/sphere.hpp>
#include "ray.hpp"
#include "raycast_hit.hpp"

#include <glm/glm.hpp>


namespace rte {

class SequentialRenderer : public CpuRenderer {
public:
    SequentialRenderer(Device& device) : CpuRenderer{device} {}

    void prepareFrame() override;
private:
    glm::vec3 reflectionRay(Ray& ray, uint recursive);
    glm::vec3 shadowRay(RaycastHit hit);
    bool raySphereIntersect(Ray& ray, Sphere& sphere, RaycastHit* hit);

    glm::vec3 reflect(Ray& ray, glm::vec3 normal);

    const glm::vec3 inverseLightDirection = glm::normalize(glm::vec3(1, 1, -1));
    const glm::vec3 ambientLight = glm::vec3(0.1f, 0.1f, 0.1f);
    std::vector<Sphere> spheres = {
        {                   // first sphere 
            {0, 0, 0},      // in origin
            1,              // radius 1
            {0, 1, 1}       // cyan color
        },
        {                   // second sphere 
            {1, 0.5f, 1},   // slightly moved
            0.5f,           // half the size
            {1, 0.1f, 0.1f} // red color
        },
        {                   // third sphere 
            {-1, 0.5f, -1}, // slightly moved
            0.5f,           // half the size
            {0.1f, 1, 0.1f} // green color
        }
    };
};

} // namespace rte
