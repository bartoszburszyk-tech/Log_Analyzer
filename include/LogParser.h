#pragma once
#include "LogEntry.h"
#include <vector>
#include <string>
#include <stdexcept>

class LogParser {
public:
    // Parse a single log line (Apache/Nginx Combined Log Format)
    static LogEntry parseLine(const std::string& line);

    // Parse entire file, returns all valid entries
    static std::vector<LogEntry> parseFile(const std::string& filepath);

    // Filter entries by status code range
    static std::vector<LogEntry> filterByStatus(
        const std::vector<LogEntry>& entries,
        int minStatus, int maxStatus
    );

    // Filter entries by endpoint substring
    static std::vector<LogEntry> filterByEndpoint(
        const std::vector<LogEntry>& entries,
        const std::string& keyword
    );

private:
    static std::string extractField(const std::string& line, size_t& pos, char delimiter);
};
