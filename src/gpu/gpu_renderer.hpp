#pragma once

#include <core/renderer.hpp>

namespace rte {

class GPURenderer : public Renderer {
public:
    GPURenderer(std::shared_ptr<Device> device) : Renderer(device) {}

    void prepareFrame() override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, VkImage image) override;
};

} // namespace rte
