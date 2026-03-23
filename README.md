# Server Log Analyzer

A command-line tool written in **C++17** for parsing, filtering, and analyzing Apache/Nginx server log files. Detects HTTP errors (4xx/5xx), identifies failing endpoints, and generates statistical reports.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-green.svg)
![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)

---

## Features

- Parses **Apache/Nginx Combined Log Format**
- Filters entries by **HTTP status code range** or **endpoint keyword**
- Generates reports with:
  - Total requests, error counts, error rate
  - Status code distribution
  - Top 5 failing endpoints
  - Top 5 IPs with most errors
  - Errors grouped by hour
- Saves reports to a **text file**
- **Log generator script** for testing with large datasets
- Built with **CMake**, tested with a custom unit test suite

---

## Project Structure

```
log_analyzer/
├── include/
│   ├── LogEntry.h          # Log entry data structure
│   ├── LogParser.h         # Parser & filter declarations
│   └── ReportGenerator.h   # Report generation declarations
├── src/
│   ├── LogParser.cpp       # Parsing logic (regex, STL)
│   ├── ReportGenerator.cpp # Report logic (sorting, output)
│   └── main.cpp            # CLI entry point
├── tests/
│   └── test_main.cpp       # Unit tests (no external framework)
├── logs/
│   ├── sample.log          # Sample Apache log file
│   └── generate_logs.sh    # Log generator script
├── reports/                # Generated reports go here
├── run.sh                  # Convenience run script
└── CMakeLists.txt
```

---

## Requirements

- `g++` with C++17 support
- `cmake >= 3.16`
- `ninja-build` (optional but recommended)

**Fedora/RHEL:**
```bash
sudo dnf install -y gcc-c++ cmake ninja-build
```

**Ubuntu/Debian:**
```bash
sudo apt install -y g++ cmake ninja-build
```

---

## Build

```bash
git clone https://github.com/YOUR_USERNAME/log-analyzer.git
cd log-analyzer
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## Usage

```bash
cd build

# Basic analysis
./log_analyzer ../logs/sample.log

# Show only errors (4xx/5xx)
./log_analyzer ../logs/sample.log --errors-only

# Filter by status code range
./log_analyzer ../logs/sample.log --filter-status 500 599

# Filter by endpoint keyword
./log_analyzer ../logs/sample.log --filter-endpoint /api

# Save report to file
./log_analyzer ../logs/sample.log --errors-only --output ../reports/report.txt
```

Or use the convenience script from project root:

```bash
./run.sh logs/sample.log --errors-only
```

---

## Generate Test Data

```bash
cd logs

# Generate 10 000 entries (default)
./generate_logs.sh

# Generate 100 000 entries
./generate_logs.sh big.log 100000

# Generate 1 000 000 entries
./generate_logs.sh huge.log 1000000
```

---

## Run Tests

```bash
cd build
./run_tests
```

Expected output:
```
=== Running tests ===

LogParser:
  [PASS] parseLine_valid
  [PASS] parseLine_invalid
  [PASS] isError_flags
  [PASS] filterByStatus
  [PASS] filterByEndpoint

ReportGenerator:
  [PASS] report_counts
  [PASS] report_error_rate

=== All tests passed ✓ ===
```

---

## Example Output

```
╔══════════════════════════════════════════╗
║       SERVER LOG ANALYSIS REPORT         ║
╚══════════════════════════════════════════╝

── SUMMARY ──────────────────────────────────
  Total requests  : 15
  Total errors    : 9
  Client errors   : 5 (4xx)
  Server errors   : 4 (5xx)
  Error rate      : 60.00%

── STATUS CODE DISTRIBUTION ─────────────────
  HTTP 403 : 1 requests
  HTTP 404 : 4 requests
  HTTP 500 : 3 requests
  HTTP 503 : 1 requests

── TOP 5 FAILING ENDPOINTS ──────────────────
  /api/payment/status                      → 2 errors
  /api/users                               → 2 errors
  /admin                                   → 1 errors

── TOP 5 IPs WITH MOST ERRORS ───────────────
  10.0.0.5                                 → 3 errors
  172.16.0.1                               → 2 errors
  192.168.1.1                              → 2 errors

── ERRORS BY HOUR ───────────────────────────
  14:00  →  9 errors
```

---

## Technologies

- **C++17** — `std::regex`, `std::map`, `std::vector`, file streams
- **CMake 3.16+** — build system
- **GDB** — debugging
- **Bash** — log generator script
- **Git** — version control

---

## Author

Bartosz Burszyk — [burszyk.bartosz@gmail.com](mailto:burszyk.bartosz@gmail.com)