#pragma once

#include <vulkan/vulkan.h>

namespace rte {

class Renderer
{
public:
    Renderer();
    ~Renderer();

    // Not copyable or movable
    Renderer(const Renderer &) = delete;
    void operator=(const Renderer &) = delete;
    Renderer(Renderer &&) = delete;
    Renderer &operator=(Renderer &&) = delete;

    void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image);
};

} // namespace rte
