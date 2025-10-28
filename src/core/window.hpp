#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace rte {

class Window {
public:
    static constexpr uint32_t width = 800;
    static constexpr uint32_t height = 600;

    Window();
    ~Window();

    // Not copyable or movable
    Window(const Window &) = delete;
    void operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    void createSurface(VkInstance instance);
    void destroySurface(VkInstance instance);
    VkSurfaceKHR getSurface() {return surface;}
    bool shouldClose();
    GLFWwindow* getWindow() {return window;}

private:
    GLFWwindow* window;
    VkSurfaceKHR surface;
};

} // namespace rte
