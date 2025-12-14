#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderer.hpp"
#include "run_type.hpp"
#include <scene/scene.hpp>

// std
#include <memory>

namespace rte {

class App {
public:
    App(RunType runType);
    void run();
private:
    std::shared_ptr<Window> window;
    std::shared_ptr<Device> device;
    std::shared_ptr<Renderer> renderer;
    std::unique_ptr<Swapchain> swapchain;

    std::shared_ptr<Renderer> createRenderer(RunType runType);
    std::shared_ptr<Scene> loadScene(const std::string& path);
};

} // namespace rte
