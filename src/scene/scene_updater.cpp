#include "scene_updater.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace rte {

void SceneUpdater::updateScene(float deltaTime) {
    // update camera position based on orbit velocity
    float angle = scene->camera.orbitVelocity * deltaTime;
    glm::vec3 direction = scene->camera.position - scene->camera.focusPoint;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));

    scene->camera.position = scene->camera.focusPoint + glm::vec3(rotation * glm::vec4(direction, 0.0f));

    // update camera forward vector
    scene->camera.forward = glm::normalize(scene->camera.focusPoint - scene->camera.position);
}

} // namespace rte
