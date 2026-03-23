cmake_minimum_required(VERSION 3.16)
project(LogAnalyzer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Library (shared logic) ────────────────────────────────────────────────────
add_library(analyzer_lib
    src/LogParser.cpp
    src/ReportGenerator.cpp
)
target_include_directories(analyzer_lib PUBLIC include)

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(log_analyzer src/main.cpp)
target_link_libraries(log_analyzer PRIVATE analyzer_lib)

# ── Tests executable ──────────────────────────────────────────────────────────
add_executable(run_tests tests/test_main.cpp)
target_link_libraries(run_tests PRIVATE analyzer_lib)

# ── Convenience target ────────────────────────────────────────────────────────
enable_testing()
add_test(NAME UnitTests COMMAND run_tests)
