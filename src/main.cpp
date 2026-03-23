#include "LogParser.h"
#include "ReportGenerator.h"
#include <iostream>
#include <string>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <logfile> [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --errors-only         Show only 4xx/5xx entries\n";
    std::cout << "  --filter-status <min> <max>  Filter by status code range\n";
    std::cout << "  --filter-endpoint <keyword>  Filter by endpoint keyword\n";
    std::cout << "  --output <file>       Save report to file\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << programName << " access.log --errors-only --output report.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string logFile = argv[1];
    bool errorsOnly = false;
    int filterMin = -1, filterMax = -1;
    std::string filterEndpoint;
    std::string outputFile;

    // Parse CLI arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--errors-only") {
            errorsOnly = true;
        } else if (arg == "--filter-status" && i + 2 < argc) {
            filterMin = std::stoi(argv[++i]);
            filterMax = std::stoi(argv[++i]);
        } else if (arg == "--filter-endpoint" && i + 1 < argc) {
            filterEndpoint = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else {
            std::cerr << "[ERROR] Unknown argument: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    try {
        // Parse log file
        auto entries = LogParser::parseFile(logFile);

        // Apply filters
        if (errorsOnly) {
            entries = LogParser::filterByStatus(entries, 400, 599);
            std::cout << "[INFO] Filtered to " << entries.size() << " error entries\n";
        } else if (filterMin >= 0) {
            entries = LogParser::filterByStatus(entries, filterMin, filterMax);
            std::cout << "[INFO] Filtered to " << entries.size() << " entries\n";
        }

        if (!filterEndpoint.empty()) {
            entries = LogParser::filterByEndpoint(entries, filterEndpoint);
            std::cout << "[INFO] Endpoint filter '" << filterEndpoint
                      << "' → " << entries.size() << " entries\n";
        }

        // Generate and display report
        auto report = ReportGenerator::generate(entries);
        ReportGenerator::printToConsole(report);

        if (!outputFile.empty()) {
            ReportGenerator::saveToFile(report, outputFile);
        }

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
