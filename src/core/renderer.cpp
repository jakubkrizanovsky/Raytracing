#include "renderer.hpp"

namespace rte {

void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image, uint32_t imageIndex) {
    cmdTransitionImageLayout(commandBuffer, image, VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
}

void Renderer::cmdTransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, 
        VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage) 
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

} // namespace rte
