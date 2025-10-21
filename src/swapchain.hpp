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
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

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
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t imageCount;
    uint32_t currentFrame = 0;

    void createSwapchain();
    void createImageViews();
    void createCommandBuffers();
    void createSyncObjects();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void submitCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void presentImage(uint32_t imageIndex);

    VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);
};

} // namespace rte
