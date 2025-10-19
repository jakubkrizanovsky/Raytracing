#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace rte {

class Device {
public:
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    Device();
    ~Device();

    // Not copyable or movable
    Device(const Device &) = delete;
    void operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;
				
private:
    VkInstance instance;
    VkDevice device;
    VkDebugUtilsMessengerEXT debugMessenger;

    void createInstance();
    void setupDebugMessenger();
    // void pickPhysicalDevice();
    // void createLogicalDevice();
    // void createCommandPool();

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();

    const std::string validationLayerName = "VK_LAYER_KHRONOS_validation";
};

} // namespace rte
