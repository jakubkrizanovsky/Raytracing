#include "gpu_renderer.hpp"
#include "gpu_sphere.hpp"

#include <iostream>

namespace rte {

GPURenderer::GPURenderer(std::shared_ptr<Device> device) : Renderer(device) {
    computePipeline = std::make_unique<ComputePipeline>(device);
    createBuffers();
}

GPURenderer::~GPURenderer() {
    vkUnmapMemory(device->getDevice(), inputBufferMemory);
    vkDestroyBuffer(device->getDevice(), inputBuffer, nullptr);
    vkFreeMemory(device->getDevice(), inputBufferMemory, nullptr);
    cleanUpImageAndView();
}

void GPURenderer::prepareFrame() {
    size_t dataSize = sizeof(GPUSphere) * spheres.size();

    // convert spheres to padded gpu variants
    std::vector<GPUSphere> gpuSpheres;
    gpuSpheres.reserve(spheres.size());
    for (const Sphere& sphere : spheres) {
        gpuSpheres.emplace_back(sphere);
    }
    
    memcpy(inputData, gpuSpheres.data(), dataSize);
}

void GPURenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage swapchainImage, uint32_t imageIndex) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline->getPipeline());

    VkDescriptorSet descriptorSet = computePipeline->getDescriptorSet(imageIndex);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline->getPipelineLayout(),
            0, 1, &descriptorSet, 0, nullptr);

    glm::uvec2 groupCounts = calculateGroupCounts();
    vkCmdDispatch(commandBuffer, groupCounts.x, groupCounts.y, 1);

    VkImageBlit blitRegion{};
    blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitRegion.srcSubresource.mipLevel = 0;
    blitRegion.srcSubresource.baseArrayLayer = 0;
    blitRegion.srcSubresource.layerCount = 1;
    blitRegion.srcOffsets[0] = {0, 0, 0};
    blitRegion.srcOffsets[1] = {(int)extent.width, (int)extent.height, 1};

    blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitRegion.dstSubresource.mipLevel = 0;
    blitRegion.dstSubresource.baseArrayLayer = 0;
    blitRegion.dstSubresource.layerCount = 1;
    blitRegion.dstOffsets[0] = {0, 0, 0};
    blitRegion.dstOffsets[1] = {(int)extent.width, (int)extent.height, 1};

    barrier.image = swapchainImage;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_GENERAL, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            1, &blitRegion, VK_FILTER_LINEAR);

    barrier.image = swapchainImage;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}

void GPURenderer::setExtent(VkExtent2D extent) {
    Renderer::setExtent(extent);

    cleanUpImageAndView();

    createImage();
    createImageView();

    computePipeline->connectDescriptorSets(imageView, inputBuffer);
}

void GPURenderer::createBuffers() {
    VkDeviceSize dataSize = sizeof(GPUSphere) * spheres.size();

    device->createBuffer(dataSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            inputBuffer, inputBufferMemory);

    vkMapMemory(device->getDevice(), inputBufferMemory, 0, dataSize, 0, &inputData);
}

void GPURenderer::createImage() {
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = extent.width;
    createInfo.extent.height = extent.height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | // compute shader writes
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // blit from this image
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(device->getDevice(), &createInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image!");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device->getDevice(), image, &memoryRequirements);

    device->allocateMemory(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageMemory);

    if (vkBindImageMemory(device->getDevice(), image, imageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to bind image memory!");
    }
}

void GPURenderer::createImageView() {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.image = image;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device->getDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view!");
    }
}

void GPURenderer::cleanUpImageAndView() {
    if (imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device->getDevice(), imageView, nullptr);
    }
    
    if (image != VK_NULL_HANDLE) {
        vkDestroyImage(device->getDevice(), image, nullptr);
        vkFreeMemory(device->getDevice(), imageMemory, nullptr);
    }
}

glm::uvec2 GPURenderer::calculateGroupCounts() {
    return {
        (extent.width  + LOCAL_SIZE_X - 1) / LOCAL_SIZE_X,
        (extent.height + LOCAL_SIZE_Y - 1) / LOCAL_SIZE_Y
    };
}

} // namespace rte
