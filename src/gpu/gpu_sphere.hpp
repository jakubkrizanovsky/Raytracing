#pragma once

#include <core/sphere.hpp>

#include <glm/glm.hpp>

namespace rte {

struct GPUSphere {
    glm::vec3 position;
    float radius;
    glm::vec3 color;
    float _padding;

    GPUSphere(const Sphere& cpuSphere)
        : position(cpuSphere.position),
          radius(cpuSphere.radius),
          color(cpuSphere.color),
          _padding(0.0f) {}
};


} // namespace rte
