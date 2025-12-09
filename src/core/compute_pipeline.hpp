#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>

namespace rte {

class ComputePipeline {
public:
    ComputePipeline(std::shared_ptr<Device> device);
    ~ComputePipeline();

    // Not copyable or movable
    ComputePipeline(const ComputePipeline &) = delete;
    void operator=(const ComputePipeline &) = delete;
    ComputePipeline(ComputePipeline &&) = delete;
    ComputePipeline &operator=(ComputePipeline &&) = delete;
private:
    std::shared_ptr<Device> device;
    VkBuffer inputBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputBufferMemory = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkShaderModule shaderModule = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    void createBuffers();
    void createDescriptorSets();
    void connectDescriptorSets();
    void createShaderModule();
    void createPipeline();
};

}
