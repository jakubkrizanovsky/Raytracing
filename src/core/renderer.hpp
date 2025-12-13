#pragma once

#include "device.hpp"
#include "sphere.hpp"

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
protected:
    std::shared_ptr<Device> device;
    VkExtent2D extent;

    std::vector<Sphere> spheres = {
        {                   // first sphere 
            {0, 0, 0},      // in origin
            1,              // radius 1
            {0, 1, 1}       // cyan color
        },
        {                   // second sphere 
            {1, 0.5f, 1},   // slightly moved
            0.5f,           // half the size
            {1, 0.1f, 0.1f} // red color
        },
        {                   // third sphere 
            {-1, 0.5f, -1}, // slightly moved
            0.5f,           // half the size
            {0.1f, 1, 0.1f} // green color
        }
    };
};

} // namespace rte
