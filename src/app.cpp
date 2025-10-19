#include "app.hpp"

// std
#include <iostream>

namespace rte {

void App::run() {
    while (!window.shouldClose()) {
        glfwPollEvents();
    }

    std::cout << "Done!" << std::endl;
}

} // namespace rte
