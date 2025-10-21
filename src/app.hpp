#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderer.hpp"

namespace rte {

class App {
public:
    void run();
private:
    Window window {};
    Device device {window};
    Renderer renderer {};
    Swapchain swapchain {window, device, renderer};
};

} // namespace rte
