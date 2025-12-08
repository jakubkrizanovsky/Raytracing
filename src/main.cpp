#include <core/app.hpp>
#include <core/run_type.hpp>

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <map>


rte::RunType parseArgs(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::invalid_argument("Invalid number of argumets - should be 2!");
    }

    static const std::map<std::string, rte::RunType> flagMap = {
        {"--sequential", rte::RunType::SEQUENTIAL},
        {"--simd", rte::RunType::SIMD},
        {"--gpu", rte::RunType::GPU}
    };

    try {
        return flagMap.at(argv[1]);
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid run type: " + std::string(argv[1]));
    }
}

int main(int argc, char* argv[]) {
    try {
        rte::RunType runType = parseArgs(argc, argv);
        rte::App app(runType);

        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}