#pragma once

#include <core/device.hpp>

#include <vulkan/vulkan.h>

namespace rte {

class ComputePipeline {
public:
    static constexpr std::string_view SHADER_PATH = "shaders/raytracer.comp.spv";
    static constexpr VkDeviceSize INPUT_BUFFER_SIZE = 2048; //TODO - should probably not be fixed size

    ComputePipeline(std::shared_ptr<Device> device);
    ~ComputePipeline();

    // Not copyable or movable
    ComputePipeline(const ComputePipeline &) = delete;
    void operator=(const ComputePipeline &) = delete;
    ComputePipeline(ComputePipeline &&) = delete;
    ComputePipeline &operator=(ComputePipeline &&) = delete;

    void connectDescriptorSets(VkImageView imageView);
    
    VkDescriptorSet getDescriptorSet(uint32_t index) {return descriptorSet;}
    VkPipelineLayout getPipelineLayout() {return pipelineLayout;}
    VkPipeline getPipeline() {return pipeline;}
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
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSet();
    void createShaderModule();
    void createPipeline();
};

}
