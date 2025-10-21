#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"

namespace rte {

class App {
public:
    void run();
private:
    Window window {};
    Device device {window};
    Swapchain swapchain {window, device};
};

} // namespace rte
