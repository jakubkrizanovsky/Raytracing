#pragma once

#include "device.hpp"
#include "sphere.hpp"
#include "camera_data.hpp"
#include <scene/scene.hpp>

#include <vulkan/vulkan.h>

namespace rte {

class Renderer {
public:
    Renderer(std::shared_ptr<Device> device) : device{device} {}
    virtual ~Renderer() = default;

    // Not copyable or movable
    Renderer(const Renderer &) = delete;
    void operator=(const Renderer &) = delete;
    Renderer(Renderer &&) = delete;
    Renderer &operator=(Renderer &&) = delete;

    virtual void prepareFrame() = 0;
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image, uint32_t imageIndex);
    virtual void setExtent(VkExtent2D newExtent) {extent = newExtent;}
    virtual void setScene(std::shared_ptr<Scene> newScene) {scene = newScene;}
protected:
    std::shared_ptr<Device> device;
    VkExtent2D extent;

    std::shared_ptr<Scene> scene = nullptr;

    void cmdTransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, 
            VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, 
            VkPipelineStageFlags dstStage);
};

} // namespace rte
