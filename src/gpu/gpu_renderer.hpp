#pragma once

#include "compute_pipeline.hpp"
#include <core/renderer.hpp>

#include <glm/glm.hpp>

namespace rte {

class GPURenderer : public Renderer {
public:
    const uint32_t LOCAL_SIZE_X = 16;
    const uint32_t LOCAL_SIZE_Y = 16;

    GPURenderer(std::shared_ptr<Device> device);
    ~GPURenderer();

    void prepareFrame() override {} //do nothing
    void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image, uint32_t imageIndex) override;
private:
    ComputePipeline computePipeline;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    void createImage();
    void createImageView();

    glm::uvec2 calculateGroupCounts();
};

} // namespace rte
