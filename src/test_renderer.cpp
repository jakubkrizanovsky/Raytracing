#include "test_renderer.hpp"

namespace rte {

TestRenderer::~TestRenderer() {
    cleanupStagingBuffer();
}

void TestRenderer::prepareFrame() {
    
    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    for (uint32_t y = 0; y < extent.height; y++) {
        for (uint32_t x = 0; x < extent.width ; x++) {
            uint32_t i = (x + y * extent.width) * 4;
            data[i + 0] = frameCounter;                                  // B
            data[i + 1] = (uint8_t) ((float) y / extent.height * 256);   // G
            data[i + 2] = (uint8_t) ((float) x / extent.width * 256);    // R
            data[i + 3] = 255;                                           // A
        }
    }

    frameCounter = (frameCounter + 1) % 256;
}

void TestRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = { extent.width, extent.height, 1 };

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

void TestRenderer::setExtent(VkExtent2D extent) {
    Renderer::setExtent(extent);
    cleanupStagingBuffer();
    createStagingBuffer();
}

void TestRenderer::createStagingBuffer() {
    VkDeviceSize bufferSize = extent.width * extent.height * 4;
    device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

    vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &stagingData);
}

void TestRenderer::cleanupStagingBuffer(){
    if (stagingBuffer != VK_NULL_HANDLE) {
        vkUnmapMemory(device.getDevice(), stagingBufferMemory);
        vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
    }
}

} // namespace rte
