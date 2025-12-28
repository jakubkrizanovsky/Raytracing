#include "scene_updater.hpp"

#include <iostream>

namespace rte {

void SceneUpdater::updateScene(float deltaTime) {
    // Update camera position based on orbit velocity
    float angle = scene->camera.orbitVelocity * deltaTime;
    glm::vec3 direction = scene->camera.position - scene->camera.focusPoint;
    float radius = glm::length(direction);
    float currentAngle = atan2(direction.z, direction.x);
    float newAngle = currentAngle + angle;

    scene->camera.position.x = scene->camera.focusPoint.x + radius * cos(newAngle);
    scene->camera.position.z = scene->camera.focusPoint.z + radius * sin(newAngle);

    // Update camera forward vector
    scene->camera.forward = glm::normalize(scene->camera.focusPoint - scene->camera.position);
}

} // namespace rte
