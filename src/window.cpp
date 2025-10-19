#include "window.hpp"

// std
#include <stdexcept>

namespace rte {

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, "Hello World", nullptr, nullptr);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::createSurface(VkInstance instance) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

void Window::destroySurface(VkInstance instance) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

}
