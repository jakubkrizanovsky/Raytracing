#pragma once

#include "device.hpp"

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
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image);
    virtual void setExtent(VkExtent2D newExtent) {extent = newExtent;}
protected:
    std::shared_ptr<Device> device;
    VkExtent2D extent;
};

} // namespace rte
