#pragma once

// std
#include <string>

namespace rte {

enum class RunType {
    SEQUENTIAL,
    SIMD,
    GPU
};

struct RunParams {
    RunType runType;
    std::string scenePath;
};


} // namespace rte
