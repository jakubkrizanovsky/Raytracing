#pragma once

#include <core/cpu_renderer.hpp>

namespace rte {

class TestRenderer : public CpuRenderer {
public:
    TestRenderer(Device& device) : CpuRenderer{device} {}

    void prepareFrame() override;
private:
    uint8_t frameCounter = 0;
};

} // namespace rte
