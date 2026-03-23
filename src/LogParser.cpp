#include "LogParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

LogEntry LogParser::parseLine(const std::string& line) {
    LogEntry entry;

    // Apache/Nginx Combined Log Format:
    // 127.0.0.1 - - [10/Oct/2024:13:55:36 -0700] "GET /index.html HTTP/1.1" 200 2326 "ref" "UA"
    static const std::regex logRegex(
        R"REGEX((\S+) \S+ \S+ \[([^\]]+)\] "(\S+) (\S+) \S+" (\d+) (\d+|-)(?: "([^"]*)" "([^"]*)")?)REGEX"
    );

    std::smatch match;
    if (!std::regex_match(line, match, logRegex)) {
        throw std::runtime_error("Failed to parse line: " + line.substr(0, 60));
    }

    entry.ip           = match[1].str();
    entry.timestamp    = match[2].str();
    entry.method       = match[3].str();
    entry.endpoint     = match[4].str();
    entry.statusCode   = std::stoi(match[5].str());
    entry.responseSize = (match[6].str() == "-") ? 0 : std::stoul(match[6].str());
    entry.userAgent    = match[8].str();

    return entry;
}

std::vector<LogEntry> LogParser::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::vector<LogEntry> entries;
    std::string line;
    int lineNumber = 0;
    int skipped = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (line.empty()) continue;

        try {
            entries.push_back(parseLine(line));
        } catch (const std::exception& e) {
            ++skipped;
            std::cerr << "[WARN] Line " << lineNumber << " skipped: " << e.what() << "\n";
        }
    }

    std::cout << "[INFO] Parsed " << entries.size() << " entries"
              << " (" << skipped << " skipped)\n";
    return entries;
}

std::vector<LogEntry> LogParser::filterByStatus(
    const std::vector<LogEntry>& entries, int minStatus, int maxStatus)
{
    std::vector<LogEntry> result;
    for (const auto& e : entries) {
        if (e.statusCode >= minStatus && e.statusCode <= maxStatus) {
            result.push_back(e);
        }
    }
    return result;
}

std::vector<LogEntry> LogParser::filterByEndpoint(
    const std::vector<LogEntry>& entries, const std::string& keyword)
{
    std::vector<LogEntry> result;
    for (const auto& e : entries) {
        if (e.endpoint.find(keyword) != std::string::npos) {
            result.push_back(e);
        }
    }
    return result;
}
