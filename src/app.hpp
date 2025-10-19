#pragma once

#include "window.hpp"
#include "device.hpp"

namespace rte {

class App {
public:
    void run();
private:
    Window window{};
    Device device {window};
};

} // namespace rte
