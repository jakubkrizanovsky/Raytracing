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
    void setScene(std::shared_ptr<Scene> newScene) override;
private:
    std::unique_ptr<SequentialCamera> camera = nullptr;

    glm::vec3 raycast(Ray& ray);
    glm::vec3 shadowRay(RaycastHit hit);
    glm::vec3 specular(Ray& ray, RaycastHit& hit);
    bool raySphereIntersect(Ray& ray, Sphere& sphere, RaycastHit& hit);
};

} // namespace rte
