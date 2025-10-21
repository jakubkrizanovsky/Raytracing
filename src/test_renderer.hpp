#pragma once

#include "renderer.hpp"

namespace rte {

class TestRenderer : public Renderer {
public:
    TestRenderer(Device& device) : Renderer{device} {}
    ~TestRenderer() override;

    void prepareFrame() override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image) override;
    void setExtent(VkExtent2D extent) override;
private:
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    void* stagingData = nullptr;

    uint8_t frameCounter = 0;

    void createStagingBuffer();
    void cleanupStagingBuffer();
};

} // namespace rte
