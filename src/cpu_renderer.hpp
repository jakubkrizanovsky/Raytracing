#pragma once

#include "renderer.hpp"

namespace rte {

class CpuRenderer : public Renderer {
public:
    CpuRenderer(Device& device) : Renderer{device} {}
    ~CpuRenderer() override;

    void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image) override;
    void setExtent(VkExtent2D extent) override;
protected:
    void* stagingData = nullptr;
private:
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

    void createStagingBuffer();
    void cleanupStagingBuffer();
};

} // namespace rte
