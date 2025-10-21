#include "app.hpp"

// std
#include <iostream>

namespace rte {

void App::run() {
    while (!window.shouldClose()) {
        glfwPollEvents();
        swapchain.drawFrame();
    }

    vkDeviceWaitIdle(device.getDevice());
    std::cout << "Done!" << std::endl;
}

} // namespace rte
