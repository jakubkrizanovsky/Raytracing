#include "test_renderer.hpp"

namespace rte {

void TestRenderer::prepareFrame() {
    
    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    for (uint32_t y = 0; y < extent.height; y++) {
        for (uint32_t x = 0; x < extent.width ; x++) {
            uint32_t i = (x + y * extent.width) * 4;
            data[i + 0] = frameCounter;                                  // B
            data[i + 1] = (uint8_t) ((float) y / extent.height * 256);   // G
            data[i + 2] = (uint8_t) ((float) x / extent.width * 256);    // R
            data[i + 3] = 255;                                           // A
        }
    }

    frameCounter = (frameCounter + 1) % 256;
}

} // namespace rte
