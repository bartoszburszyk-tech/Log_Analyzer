#pragma once
#include <string>
#include <ctime>

struct LogEntry {
    std::string ip;
    std::string timestamp;
    std::string method;
    std::string endpoint;
    int statusCode;
    size_t responseSize;
    std::string userAgent;

    bool isClientError() const { return statusCode >= 400 && statusCode < 500; }
    bool isServerError() const { return statusCode >= 500 && statusCode < 600; }
    bool isError() const { return statusCode >= 400; }
};
