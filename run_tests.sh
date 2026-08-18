#!/bin/bash

shopt -s nullglob

SCRIPT_DIR="$(pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
TEST_DIR="$SCRIPT_DIR/tests"
LOG_FILE="$SCRIPT_DIR/test_results.log"

# ---------------------------------------------------------------------------
# Test result counters
# ---------------------------------------------------------------------------

TOTAL_PASSED=0
TOTAL_FAILED=0
TOTAL_TESTS=0

# ---------------------------------------------------------------------------
# Initialize log
# ---------------------------------------------------------------------------

{
    echo "Test Results"
    echo "============="
    echo "Running tests at $(date)"
    echo "============="
    echo ""
} > "$LOG_FILE"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

build_project() {
    local frame_size="$1"
    local frame_store_size="$2"
    local var_store_size="$3"

    cd "$SCRIPT_DIR" || exit 1

    rm -rf "$BUILD_DIR"

    cmake -B "$BUILD_DIR" \
        -DFRAME_SIZE="$frame_size" \
        -DFRAME_STORE_SIZE="$frame_store_size" \
        -DVAR_STORE_SIZE="$var_store_size" \
        -S "$SCRIPT_DIR" \
        -DCMAKE_BUILD_TYPE=Debug || return 1

    cmake --build "$BUILD_DIR" || return 1
}

cleanup_test_artifacts() {
    find . -type d -name "test*" -exec rm -rf {} +
    find . -type f -name "test*" -exec rm -f {} +
}

run_test() {
    local test_file="$1"
    local result_file="$2"

    local base_name
    base_name="$(basename "$test_file" .txt)"

    echo "Running test: $base_name" >> "$LOG_FILE"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    if [ ! -f "$result_file" ]; then
        echo "FAILED - Missing result file for $test_file" >> "$LOG_FILE"
        TOTAL_FAILED=$((TOTAL_FAILED + 1))
        return
    fi

    ./mysh < "$test_file" > temp_output.txt
    local exit_code=$?

    if [ "$exit_code" -ne 0 ]; then
        echo "Status: FAILED" >> "$LOG_FILE"
        echo "" >> "$LOG_FILE"
        echo "Expected output:" >> "$LOG_FILE"
        cat "$result_file" >> "$LOG_FILE"
        echo "" >> "$LOG_FILE"
        echo "Actual output:" >> "$LOG_FILE"
        cat temp_output.txt >> "$LOG_FILE"
        echo "" >> "$LOG_FILE"
        echo "Process exited with code: $exit_code" >> "$LOG_FILE"
        TOTAL_FAILED=$((TOTAL_FAILED + 1))
    elif diff -w temp_output.txt "$result_file" > /dev/null; then
        echo "Status: PASSED" >> "$LOG_FILE"
        TOTAL_PASSED=$((TOTAL_PASSED + 1))
    else
        echo "Status: FAILED" >> "$LOG_FILE"
        echo "" >> "$LOG_FILE"
        echo "Expected output:" >> "$LOG_FILE"
        cat "$result_file" >> "$LOG_FILE"
        echo "" >> "$LOG_FILE"
        echo "Actual output:" >> "$LOG_FILE"
        cat temp_output.txt >> "$LOG_FILE"
        TOTAL_FAILED=$((TOTAL_FAILED + 1))
    fi

    echo "------------------------" >> "$LOG_FILE"

    cleanup_test_artifacts
    rm -f temp_output.txt
}

# ===========================================================================
# 1. Interpreter command tests
# ===========================================================================

echo "Testing directory: interpreter-cmd-tests" >> "$LOG_FILE"
echo "----------------------" >> "$LOG_FILE"

build_project 100 1000 10 || exit 1

cd "$TEST_DIR/interpreter-cmd-tests" || exit 1
cp "$BUILD_DIR/mysh" .

for test_file in *.txt; do
    [[ "$test_file" == *_result.txt ]] && continue

    base_name="${test_file%.txt}"
    run_test "$test_file" "${base_name}_result.txt"
done

rm -f mysh temp_output.txt

echo "" >> "$LOG_FILE"
echo "=============================" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

# ===========================================================================
# 2. Scheduler tests
# ===========================================================================

echo "Testing directory: scheduler-tests" >> "$LOG_FILE"
echo "----------------------" >> "$LOG_FILE"

build_project 100 1000 10 || exit 1

cd "$TEST_DIR/scheduler-tests" || exit 1
cp "$BUILD_DIR/mysh" .

for test_file in *.txt; do
    [[ "$test_file" == *_result.txt ]] && continue

    base_name="${test_file%.txt}"
    run_test "$test_file" "${base_name}_result.txt"
done

rm -f mysh temp_output.txt

echo "" >> "$LOG_FILE"
echo "=============================" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

# ===========================================================================
# 3. Demand paging tests
# ===========================================================================

DEMAND_PAGING_DIR="$TEST_DIR/demand-paging-tests"

echo "Testing directory: demand-paging-tests" >> "$LOG_FILE"
echo "----------------------" >> "$LOG_FILE"

cd "$DEMAND_PAGING_DIR" || exit 1

for test_file in *.txt; do
    [[ "$test_file" == *_result.txt ]] && continue

    base_name="${test_file%.txt}"
    result_file="${base_name}_result.txt"
    config_file="${base_name}.config"

    DIR_TOTAL=$((DIR_TOTAL + 1))

    echo "Running test: $base_name" >> "$LOG_FILE"

    if [ ! -f "$config_file" ]; then
        echo "FAILED - Missing config file: $config_file" >> "$LOG_FILE"
        DIR_FAILED=$((DIR_FAILED + 1))
        continue
    fi

    source "$config_file"

    build_project "$FRAME_SIZE" "$FRAME_STORE_SIZE" "$VAR_STORE_SIZE" || exit 1

    # build_project changes cwd to SCRIPT_DIR
    cd "$DEMAND_PAGING_DIR" || exit 1

    cp "$BUILD_DIR/mysh" .

    run_test "$test_file" "$result_file"

    rm -f mysh
done

# ===========================================================================
# Final summary
# ===========================================================================

{
    echo "Final Summary"
    echo "============="
    echo "Total tests run: $TOTAL_TESTS"
    echo "Total passed: $TOTAL_PASSED"
    echo "Total failed: $TOTAL_FAILED"
} >> "$LOG_FILE"

echo ""
echo "TEST SUMMARY"
echo "=================="
echo "Total tests: $TOTAL_TESTS"
echo "Passed: $TOTAL_PASSED"
echo "Failed: $TOTAL_FAILED"
echo ""
echo "See test_results.log for more information"

if [ "$TOTAL_FAILED" -ne 0 ]; then
    exit 1
fi
