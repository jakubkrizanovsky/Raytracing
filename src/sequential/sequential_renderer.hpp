#pragma once

#include <core/cpu_renderer.hpp>
#include "ray.hpp"
#include "raycast_hit.hpp"
#include "sequential_camera.hpp"

#include <glm/glm.hpp>


namespace rte {

class SequentialRenderer : public CpuRenderer {
public:
    SequentialRenderer(std::shared_ptr<Device> device) : CpuRenderer{device} {}

    void prepareFrame() override;
private:
    glm::vec3 raycast(Ray& ray);
    glm::vec3 shadowRay(RaycastHit hit);
    bool raySphereIntersect(Ray& ray, Sphere& sphere, RaycastHit& hit);

    SequentialCamera camera = SequentialCamera({{0, 0, -5}, {0, 0, 1}, 90.0f});

    const glm::vec3 inverseLightDirection = glm::normalize(glm::vec3(1, 1, -1));
    const glm::vec3 ambientLight = glm::vec3(0.1f, 0.1f, 0.1f);
};

} // namespace rte
