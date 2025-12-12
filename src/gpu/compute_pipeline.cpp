#include "compute_pipeline.hpp"

#include <fstream>

namespace rte {

constexpr VkDeviceSize INPUT_BUFFER_SIZE = 2048; //TODO - should probably not be fixed size
const char* SHADER_PATH = "shaders/test_shader.comp.spv"; //TODO - move somewhere else?


ComputePipeline::ComputePipeline(std::shared_ptr<Device> device) : device{device} {
    createBuffers();
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSet();
    createShaderModule();
    createPipeline();
}

ComputePipeline::~ComputePipeline() {
    vkDestroyPipeline(device->getDevice(), pipeline, nullptr);
    vkDestroyPipelineLayout(device->getDevice(), pipelineLayout, nullptr);
    vkDestroyShaderModule(device->getDevice(), shaderModule, nullptr);
    vkFreeDescriptorSets(device->getDevice(), descriptorPool, 1, &descriptorSet);
    vkDestroyDescriptorPool(device->getDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device->getDevice(), descriptorSetLayout, nullptr);
    vkDestroyBuffer(device->getDevice(), inputBuffer, nullptr);
    vkFreeMemory(device->getDevice(), inputBufferMemory, nullptr);
}

void ComputePipeline::connectDescriptorSets(VkImageView imageView) {
    VkDescriptorBufferInfo inputDescriptorBufferInfo{};
    inputDescriptorBufferInfo.buffer = inputBuffer;
    inputDescriptorBufferInfo.offset = 0;
    inputDescriptorBufferInfo.range = VK_WHOLE_SIZE;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageView = imageView;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet writeDescriptorSet[] = {
        {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, /*dstBinding=*/0, 
                /*dstArrayElement=*/0, /*descriptorCount=*/1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                /*pImageInfo=*/nullptr, /*pBufferInfo=*/&inputDescriptorBufferInfo, /*pTexelBufferView=*/nullptr},
        {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, /*dstBinding=*/1, 
                /*dstArrayElement=*/0, /*descriptorCount=*/1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
                /*pImageInfo=*/&imageInfo, /*pBufferInfo=*/nullptr, /*pTexelBufferView=*/nullptr}
    };

    vkUpdateDescriptorSets(device->getDevice(), 2, writeDescriptorSet, 0, nullptr);
}

void ComputePipeline::createBuffers() {
    device->createBuffer(INPUT_BUFFER_SIZE, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            inputBuffer, inputBufferMemory);
}

void ComputePipeline::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding bindings[] = {
        {/*binding=*/0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, /*descriptorCount=*/1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr},
        {/*binding=*/1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, /*descriptorCount=*/1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr}
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 2;
    descriptorSetLayoutCreateInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, 
            &descriptorSetLayout) != VK_SUCCESS ) 
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

void ComputePipeline::createDescriptorPool() {
    VkDescriptorPoolSize descriptorPoolSize[] = {
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = 2;
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSize;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; //TODO - ??

    if (vkCreateDescriptorPool(device->getDevice(), &descriptorPoolCreateInfo, nullptr, 
            &descriptorPool) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void ComputePipeline::createDescriptorSet() {
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(device->getDevice(), &descriptorSetAllocateInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
}

void ComputePipeline::createShaderModule() {
    // read shader SPIR-V file
    std::ifstream file{SHADER_PATH, std::ios::ate | std::ios::binary};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(SHADER_PATH));
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> shaderSPV(fileSize);

    file.seekg(0);
    file.read(shaderSPV.data(), fileSize);
    file.close();

    // create shader module
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t*>(shaderSPV.data());
    shaderModuleCreateInfo.codeSize = shaderSPV.size();

    if (vkCreateShaderModule(device->getDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }
}

void ComputePipeline::createPipeline() {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    if (vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutCreateInfo, nullptr, 
            &pipelineLayout) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;

    computePipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computePipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computePipelineCreateInfo.stage.module = shaderModule;
    computePipelineCreateInfo.stage.pName = "main";
    computePipelineCreateInfo.layout = pipelineLayout;

    if (vkCreateComputePipelines(device->getDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo,
            nullptr, &pipeline) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create compute pipeline!");
    }
}

}
