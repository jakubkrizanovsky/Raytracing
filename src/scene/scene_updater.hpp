#pragma once

#include <scene/scene.hpp>

namespace rte {

class SceneUpdater {
public:
    SceneUpdater() = default;
    ~SceneUpdater() = default;

    // Not copyable or movable
    SceneUpdater(const SceneUpdater &) = delete;
    void operator=(const SceneUpdater &) = delete;
    SceneUpdater(SceneUpdater &&) = delete;
    SceneUpdater &operator=(SceneUpdater &&) = delete;

    void setScene(std::shared_ptr<Scene> newScene) {scene = newScene;}
    void updateScene(float deltaTime);
private:
    std::shared_ptr<Scene> scene = nullptr;
};

} // namespace rte
