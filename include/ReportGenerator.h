#pragma once
#include "LogEntry.h"
#include <vector>
#include <string>
#include <map>

struct ReportData {
    int totalRequests;
    int totalErrors;
    int clientErrors;   // 4xx
    int serverErrors;   // 5xx
    double errorRate;

    std::map<int, int> statusCodeCount;
    std::map<std::string, int> topFailingEndpoints;
    std::map<std::string, int> topClientIPs;
    std::map<std::string, int> errorsByHour;
};

class ReportGenerator {
public:
    static ReportData generate(const std::vector<LogEntry>& entries);
    static void printToConsole(const ReportData& report);
    static void saveToFile(const ReportData& report, const std::string& outputPath);

private:
    static std::string extractHour(const std::string& timestamp);
    static void printTopN(const std::map<std::string, int>& data, int n);
};
