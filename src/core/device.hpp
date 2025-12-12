#pragma once

#include "window.hpp"

#include <vulkan/vulkan.h>

// std
#include <vector>
#include <string>

namespace rte {

class Device {
public:
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    Device(std::shared_ptr<Window> window);
    ~Device();

    // Not copyable or movable
    Device(const Device &) = delete;
    void operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

    bool getSwapchainSupport(VkSurfaceCapabilitiesKHR &capabilities, std::vector<VkSurfaceFormatKHR>& formats, 
            std::vector<VkPresentModeKHR>& presentModes);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
            VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void allocateMemory(VkMemoryRequirements requrements, VkMemoryPropertyFlags properties, 
            VkDeviceMemory& memory);
    
    VkDevice getDevice() {return device;}
    VkQueue getComputeQueue() {return computeQueue;}
    VkQueue getPresentQueue() {return presentQueue;}
    VkCommandPool getCommandPool() {return commandPool;}
    
private:
    std::shared_ptr<Window> window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue computeQueue;
    VkQueue presentQueue;
    VkCommandPool commandPool;

    void createInstance();
    void setupDebugMessenger();
    void createWindowSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    bool isDeviceSuitable(VkPhysicalDevice physDevice);
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    bool findQueueFamilyIndices(VkPhysicalDevice physicalDevice, uint32_t& computeIndex, uint32_t& presentIndex);
    bool querySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR &capabilities, 
        std::vector<VkSurfaceFormatKHR>& formats, std::vector<VkPresentModeKHR>& presentModes);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    const std::string validationLayerName = "VK_LAYER_KHRONOS_validation";
    const std::vector<const char *> deviceExtensions = {"VK_KHR_portability_subset", VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

} // namespace rte
