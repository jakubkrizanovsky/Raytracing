#pragma once

#include "compute_pipeline.hpp"
#include <core/renderer.hpp>

#include <glm/glm.hpp>

namespace rte {

class GPURenderer : public Renderer {
public:
    static constexpr uint32_t LOCAL_SIZE_X = 16;
    static constexpr uint32_t LOCAL_SIZE_Y = 16;

    GPURenderer(std::shared_ptr<Device> device);
    ~GPURenderer();

    void prepareFrame() override {} //do nothing
    void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image, uint32_t imageIndex) override;
    void setExtent(VkExtent2D extent) override;
private:
    std::unique_ptr<ComputePipeline> computePipeline;
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;

    void createImage();
    void createImageView();
    void cleanUpImageAndView();

    glm::uvec2 calculateGroupCounts();
};

} // namespace rte
