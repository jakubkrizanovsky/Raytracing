#include "app.hpp"
#include <gpu/gpu_renderer.hpp>
#include <sequential/sequential_renderer.hpp>
#include <simd/simd_renderer.hpp>


// std
#include <iostream>

namespace rte {

App::App(RunType runType) {
    window = std::make_shared<Window>();
    device = std::make_shared<Device>(window);
    renderer = createRenderer(runType);
    swapchain = std::make_unique<Swapchain>(window, device, renderer);
}

void App::run() {
    while (!window->shouldClose()) {
        glfwPollEvents();
        swapchain->drawFrame();
    }

    vkDeviceWaitIdle(device->getDevice());
    std::cout << "Done!" << std::endl;
}

std::shared_ptr<Renderer> App::createRenderer(RunType runType) {
    switch(runType) {
        case RunType::SEQUENTIAL:
            return std::make_shared<SequentialRenderer>(device);
        case RunType::SIMD:
            return std::make_shared<SIMDRenderer>(device);
        case RunType::GPU:
            return std::make_shared<GPURenderer>(device);
    }
}

} // namespace rte
