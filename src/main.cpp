#include <core/app.hpp>
#include <core/run_params.hpp>

using namespace rte;

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <map>


RunParams parseArgs(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        throw std::invalid_argument("Invalid number of argumets - should be 2 or 3!");
    }

    static const std::map<std::string, RunType> flagMap = {
        {"--sequential", RunType::SEQUENTIAL},
        {"--simd", RunType::SIMD},
        {"--gpu", RunType::GPU}
    };

    RunParams runParams;
    try {
        runParams.runType = flagMap.at(argv[1]);
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid run type: " + std::string(argv[1]));
    }

    if (argc == 3) {
        runParams.scenePath = argv[2];
    } else {
        runParams.scenePath = "scenes/default.json";
    }

    return runParams;
}

int main(int argc, char* argv[]) {
    try {
        RunParams runParams = parseArgs(argc, argv);
        App app(runParams);

        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}