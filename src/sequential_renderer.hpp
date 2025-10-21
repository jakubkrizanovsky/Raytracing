#pragma once

#include "cpu_renderer.hpp"

#include <glm/glm.hpp>
#include "sphere.hpp"
#include "ray.hpp"
#include "raycast_hit.hpp"

namespace rte {

class SequentialRenderer : public CpuRenderer {
public:
    SequentialRenderer(Device& device) : CpuRenderer{device} {}

    void prepareFrame() override;
private:
    glm::vec3 reflectionRay(Ray& ray, uint recursive);
    glm::vec3 shadowRay(RaycastHit hit);
    bool raySphereIntersect(Ray& ray, Sphere& sphere, RaycastHit* hit);

    const glm::vec3 inverseLightDirection = glm::normalize(glm::vec3(1, -1, 1));
    std::vector<Sphere> spheres = {
        {                   // single sphere 
            {0, 0, 0},      // in origin
            1,              // radius 1
            {0, 1, 1}       // cyan color
        }
    };
};

} // namespace rte
