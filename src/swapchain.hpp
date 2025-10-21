#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>

// std
#include <vector>

namespace rte {

class Swapchain
{
public:
    Swapchain(Window& window, Device& device);
    ~Swapchain();

    // Not copyable or movable
    Swapchain(const Swapchain &) = delete;
    void operator=(const Swapchain &) = delete;
    Swapchain(Swapchain &&) = delete;
    Swapchain &operator=(Swapchain &&) = delete;
private:
    Window& window;
    Device& device;
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

    void createSwapchain();
    void createImageViews();

    VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);
};

} // namespace rte
