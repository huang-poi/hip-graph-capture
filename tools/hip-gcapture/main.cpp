#include <iostream>
#include <string>
#include <cstring>

void print_usage() {
    std::cout << "Usage: hip-gcapture <command> [options]\n\n"
              << "Commands:\n"
              << "  capture   Capture HIP graph from application\n"
              << "  replay    Replay a captured graph\n"
              << "  analyze   Analyze graph structure\n"
              << "  visualize Generate graph visualization\n\n"
              << "Options:\n"
              << "  --app <path>        Application to capture\n"
              << "  --trace <path>      Path to trace file\n"
              << "  --output <path>     Output path\n"
              << "  --format <fmt>      Output format (json, dot, binary)\n"
              << "  --iterations <n>    Replay iterations\n"
              << "  --critical-path     Show critical path analysis\n"
              << "  --help              Show this help\n";
}

int main(int argc, char** argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        print_usage();
        return 0;
    }

    std::string cmd = argv[1];

    if (cmd == "capture") {
        std::cout << "[hip-gcapture] Starting capture...\n";
        // Would use LD_PRELOAD injection or roctracer API
        std::cout << "[hip-gcapture] Capture mode not available without ROCm runtime\n";
        return 0;
    }

    if (cmd == "replay") {
        std::cout << "[hip-gcapture] Replay mode\n";
        return 0;
    }

    if (cmd == "analyze") {
        std::cout << "[hip-gcapture] Analysis mode\n";
        return 0;
    }

    if (cmd == "visualize") {
        std::cout << "[hip-gcapture] Visualization mode\n";
        return 0;
    }

    std::cerr << "Unknown command: " << cmd << "\n";
    print_usage();
    return 1;
}
