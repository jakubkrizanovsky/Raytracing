#pragma once

#include "device.hpp"
#include "renderer.hpp"

#include <vulkan/vulkan.h>

// std
#include <vector>

namespace rte {

class Swapchain
{
public:
    Swapchain(Window& window, Device& device, Renderer& renderer);
    ~Swapchain();

    // Not copyable or movable
    Swapchain(const Swapchain &) = delete;
    void operator=(const Swapchain &) = delete;
    Swapchain(Swapchain &&) = delete;
    Swapchain &operator=(Swapchain &&) = delete;

    void drawFrame();
private:
    Window& window;
    Device& device;
    Renderer& renderer;
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    void createSwapchain();
    void createImageViews();
    void createCommandBuffer();
    void createSyncObjects();

    VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);
};

} // namespace rte
