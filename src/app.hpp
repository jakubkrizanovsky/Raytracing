#pragma once

#include "device.hpp"

namespace rte {

class App {
public:
    void run();
private:
    Device device{};
};

} // namespace rte
