#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
LOG_DIR="$SCRIPT_DIR/logs"

# Buduj jeśli binarki nie ma
if [ ! -f "$BUILD_DIR/log_analyzer" ]; then
    echo "[INFO] Brak binarki – buduję projekt..."
    cmake --build "$BUILD_DIR" --parallel
fi

# Domyślny log jeśli nie podano argumentu
LOG_FILE="${1:-$LOG_DIR/sample.log}"

# Pozostałe argumenty przekaż do programu
shift 2>/dev/null
"$BUILD_DIR/log_analyzer" "$LOG_FILE" "$@"
