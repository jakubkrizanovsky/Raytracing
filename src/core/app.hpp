#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderer.hpp"
#include <simd/simd_renderer.hpp>
#include <sequential/sequential_renderer.hpp>

namespace rte {

class App {
public:
    void run();
private:
    Window window {};
    Device device {window};
    SIMDRenderer renderer {device};
    Swapchain swapchain {window, device, renderer};
};

} // namespace rte
