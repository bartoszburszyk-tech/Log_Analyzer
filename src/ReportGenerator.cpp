#include "ReportGenerator.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>

ReportData ReportGenerator::generate(const std::vector<LogEntry>& entries) {
    ReportData report{};
    report.totalRequests = static_cast<int>(entries.size());

    for (const auto& e : entries) {
        // Count status codes
        report.statusCodeCount[e.statusCode]++;

        // Count errors
        if (e.isError()) {
            report.totalErrors++;
            report.topFailingEndpoints[e.endpoint]++;
            report.topClientIPs[e.ip]++;

            if (e.isClientError()) report.clientErrors++;
            if (e.isServerError()) report.serverErrors++;

            // Group errors by hour
            std::string hour = extractHour(e.timestamp);
            if (!hour.empty()) report.errorsByHour[hour]++;
        }
    }

    report.errorRate = report.totalRequests > 0
        ? (100.0 * report.totalErrors / report.totalRequests)
        : 0.0;

    return report;
}

void ReportGenerator::printToConsole(const ReportData& report) {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║       SERVER LOG ANALYSIS REPORT         ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    std::cout << "── SUMMARY ──────────────────────────────────\n";
    std::cout << "  Total requests  : " << report.totalRequests << "\n";
    std::cout << "  Total errors    : " << report.totalErrors << "\n";
    std::cout << "  Client errors   : " << report.clientErrors << " (4xx)\n";
    std::cout << "  Server errors   : " << report.serverErrors << " (5xx)\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Error rate      : " << report.errorRate << "%\n\n";

    std::cout << "── STATUS CODE DISTRIBUTION ─────────────────\n";
    for (const auto& [code, count] : report.statusCodeCount) {
        std::cout << "  HTTP " << code << " : " << count << " requests\n";
    }

    std::cout << "\n── TOP 5 FAILING ENDPOINTS ──────────────────\n";
    printTopN(report.topFailingEndpoints, 5);

    std::cout << "\n── TOP 5 IPs WITH MOST ERRORS ───────────────\n";
    printTopN(report.topClientIPs, 5);

    std::cout << "\n── ERRORS BY HOUR ───────────────────────────\n";
    for (const auto& [hour, count] : report.errorsByHour) {
        std::cout << "  " << hour << ":00  →  " << count << " errors\n";
    }
    std::cout << "\n";
}

void ReportGenerator::saveToFile(const ReportData& report, const std::string& outputPath) {
    std::ofstream out(outputPath);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot write report to: " + outputPath);
    }

    out << "SERVER LOG ANALYSIS REPORT\n";
    out << "==========================\n\n";
    out << "Total requests : " << report.totalRequests << "\n";
    out << "Total errors   : " << report.totalErrors << "\n";
    out << "Client errors  : " << report.clientErrors << "\n";
    out << "Server errors  : " << report.serverErrors << "\n";
    out << std::fixed << std::setprecision(2);
    out << "Error rate     : " << report.errorRate << "%\n\n";

    out << "STATUS CODES:\n";
    for (const auto& [code, count] : report.statusCodeCount) {
        out << "  " << code << " -> " << count << "\n";
    }

    out << "\nTOP FAILING ENDPOINTS:\n";
    // Convert map to vector for sorting
    std::vector<std::pair<std::string,int>> endpoints(
        report.topFailingEndpoints.begin(), report.topFailingEndpoints.end());
    std::sort(endpoints.begin(), endpoints.end(),
        [](const auto& a, const auto& b){ return a.second > b.second; });
    int n = 0;
    for (const auto& [ep, cnt] : endpoints) {
        out << "  " << ep << " -> " << cnt << " errors\n";
        if (++n >= 5) break;
    }

    std::cout << "[INFO] Report saved to: " << outputPath << "\n";
}

std::string ReportGenerator::extractHour(const std::string& timestamp) {
    // Format: 10/Oct/2024:13:55:36 -0700
    auto pos = timestamp.find(':');
    if (pos == std::string::npos || pos + 2 >= timestamp.size()) return "";
    return timestamp.substr(pos + 1, 2);
}

void ReportGenerator::printTopN(const std::map<std::string, int>& data, int n) {
    std::vector<std::pair<std::string,int>> vec(data.begin(), data.end());
    std::sort(vec.begin(), vec.end(),
        [](const auto& a, const auto& b){ return a.second > b.second; });

    int count = 0;
    for (const auto& [key, val] : vec) {
        std::cout << "  " << std::setw(40) << std::left << key
                  << " → " << val << " errors\n";
        if (++count >= n) break;
    }
}
