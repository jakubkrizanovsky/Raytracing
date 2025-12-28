#include "app.hpp"
#include <gpu/gpu_renderer.hpp>
#include <sequential/sequential_renderer.hpp>
#include <simd/simd_renderer.hpp>
#include <scene/parsing.hpp>

#include <nlohmann/json.hpp>

// std
#include <iostream>
#include <fstream>
#include <chrono>

namespace rte {

App::App(RunParams runParams) : runParams{runParams} {
    window = std::make_shared<Window>();
    device = std::make_shared<Device>(window);
    renderer = createRenderer(runParams.runType);
    swapchain = std::make_unique<Swapchain>(window, device, renderer);
}

void App::run() {
    std::shared_ptr<Scene> scene = loadScene(runParams.scenePath);
    renderer->setScene(scene);

    while (!window->shouldClose()) {
        auto startTime = std::chrono::steady_clock::now();

        glfwPollEvents();
        swapchain->drawFrame();

        auto endTime = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(endTime - startTime);
        std::cout << ms.count() << " ms\n";
    }

    vkDeviceWaitIdle(device->getDevice());
    std::cout << "Done!" << std::endl;
}

std::shared_ptr<Renderer> App::createRenderer(RunType runType) {
    switch(runType) {
        case RunType::SEQUENTIAL:
            return std::make_shared<SequentialRenderer>(device);
        case RunType::SIMD:
#ifdef __ARM_NEON__
            return std::make_shared<SIMDRenderer>(device);
#else
            throw std::runtime_error("SIMD run type not available on this platform - Arm Neon required!");
#endif
        case RunType::GPU:
            return std::make_shared<GPURenderer>(device);
    }
}

std::shared_ptr<Scene> App::loadScene(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open scene file");
    }

    nlohmann::json j;
    file >> j;

    return std::make_shared<Scene>(j.get<Scene>());
}

} // namespace rte
