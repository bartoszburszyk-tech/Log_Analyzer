#include "LogParser.h"
#include "ReportGenerator.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

// ── helpers ──────────────────────────────────────────────────────────────────

static void pass(const std::string& name) {
    std::cout << "  [PASS] " << name << "\n";
}

static void fail(const std::string& name, const std::string& reason) {
    std::cerr << "  [FAIL] " << name << " — " << reason << "\n";
    std::exit(1);
}

// ── LogParser tests ───────────────────────────────────────────────────────────

void test_parseLine_valid() {
    const std::string line =
        R"(192.168.1.1 - - [10/Oct/2024:13:55:36 -0700] "GET /index.html HTTP/1.1" 200 2326 "http://ref.com" "Mozilla/5.0")";

    LogEntry e = LogParser::parseLine(line);

    if (e.ip         != "192.168.1.1")   fail("parseLine_valid", "wrong ip");
    if (e.method     != "GET")            fail("parseLine_valid", "wrong method");
    if (e.endpoint   != "/index.html")    fail("parseLine_valid", "wrong endpoint");
    if (e.statusCode != 200)              fail("parseLine_valid", "wrong status");
    if (e.responseSize != 2326)           fail("parseLine_valid", "wrong size");
    pass("parseLine_valid");
}

void test_parseLine_invalid() {
    try {
        LogParser::parseLine("this is not a log line");
        fail("parseLine_invalid", "should have thrown");
    } catch (const std::exception&) {
        pass("parseLine_invalid");
    }
}

void test_isError_flags() {
    LogEntry e;

    e.statusCode = 200; if (e.isError())        fail("isError_200", "200 should not be error");
    e.statusCode = 404; if (!e.isClientError())  fail("isClientError_404", "404 should be client error");
    e.statusCode = 500; if (!e.isServerError())  fail("isServerError_500", "500 should be server error");
    pass("isError_flags");
}

void test_filterByStatus() {
    std::vector<LogEntry> entries;
    for (int code : {200, 301, 404, 500, 503}) {
        LogEntry e; e.statusCode = code;
        entries.push_back(e);
    }

    auto errors = LogParser::filterByStatus(entries, 400, 599);
    if (errors.size() != 3) fail("filterByStatus", "expected 3 errors");
    pass("filterByStatus");
}

void test_filterByEndpoint() {
    std::vector<LogEntry> entries;
    for (const std::string& ep : {"/api/users", "/api/orders", "/index.html"}) {
        LogEntry e; e.endpoint = ep; e.statusCode = 200;
        entries.push_back(e);
    }

    auto api = LogParser::filterByEndpoint(entries, "/api");
    if (api.size() != 2) fail("filterByEndpoint", "expected 2 /api entries");
    pass("filterByEndpoint");
}

// ── ReportGenerator tests ─────────────────────────────────────────────────────

void test_report_counts() {
    std::vector<LogEntry> entries;

    auto makeEntry = [](int code, const std::string& ep, const std::string& ip) {
        LogEntry e;
        e.statusCode = code;
        e.endpoint   = ep;
        e.ip         = ip;
        e.timestamp  = "10/Oct/2024:13:00:00 -0700";
        return e;
    };

    entries.push_back(makeEntry(200, "/home",    "1.1.1.1"));
    entries.push_back(makeEntry(404, "/missing", "2.2.2.2"));
    entries.push_back(makeEntry(500, "/crash",   "3.3.3.3"));
    entries.push_back(makeEntry(503, "/crash",   "3.3.3.3"));

    ReportData r = ReportGenerator::generate(entries);

    if (r.totalRequests != 4) fail("report_counts", "totalRequests");
    if (r.totalErrors   != 3) fail("report_counts", "totalErrors");
    if (r.clientErrors  != 1) fail("report_counts", "clientErrors");
    if (r.serverErrors  != 2) fail("report_counts", "serverErrors");
    pass("report_counts");
}

void test_report_error_rate() {
    std::vector<LogEntry> entries;
    for (int i = 0; i < 8; ++i) { LogEntry e; e.statusCode = 200; entries.push_back(e); }
    for (int i = 0; i < 2; ++i) { LogEntry e; e.statusCode = 500; entries.push_back(e); }

    ReportData r = ReportGenerator::generate(entries);
    double expected = 20.0;
    if (r.errorRate < expected - 0.01 || r.errorRate > expected + 0.01)
        fail("report_error_rate", "expected 20.0%");
    pass("report_error_rate");
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n=== Running tests ===\n\n";

    std::cout << "LogParser:\n";
    test_parseLine_valid();
    test_parseLine_invalid();
    test_isError_flags();
    test_filterByStatus();
    test_filterByEndpoint();

    std::cout << "\nReportGenerator:\n";
    test_report_counts();
    test_report_error_rate();

    std::cout << "\n=== All tests passed ✓ ===\n\n";
    return 0;
}
