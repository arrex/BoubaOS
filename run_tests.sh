#!/bin/bash

# Store diretory paths
SCRIPT_DIR=$(pwd)
BUILD_DIR="$SCRIPT_DIR/build"
TEST_DIR="$SCRIPT_DIR/tests"
LOG_FILE="$SCRIPT_DIR/test_results.log"

# Create/clear log file
echo "Test Results" > "$LOG_FILE"
echo "=============" >> "$LOG_FILE"
echo "Running tests at $(date)" >> "$LOG_FILE"
echo "=============" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

# Counter for total results
TOTAL_PASSED=0
TOTAL_FAILED=0
TOTAL_TESTS=0

# Function to clean up test artifacts
cleanup_test_artifacts() {
    # Remove any directories created during tests (assuming they start with "test")
    find . -type d -name "test*" -exec rm -rf {} +
    # Remove any files created during tests (assuming they start with "test")
    find . -type f -name "test*" -exec rm -f {} +
}

# Iterate through each test directory
for test_dir in "$TEST_DIR"/*/; do
    if [ ! -d "$test_dir" ] || [ "$test_dir" = "$TEST_DIR/" ]; then
        continue
    fi

    dir_name=$(basename "$test_dir")
    echo "Testing directory: $dir_name" >> $LOG_FILE
    echo "----------------------" >> $LOG_FILE

    # Copy executable to test directory
    cp "$BUILD_DIR/mysh" "$test_dir"

    # Change to test directory
    cd "$test_dir"

    # Counter for current directory
    DIR_PASSED=0
    DIR_FAILED=0
    DIR_TOTAL=0

    # Run tests in current directory
    for test_file in *.txt; do
        # Skip if file ends with _result.txt
        if [[ $test_file == *_result.txt ]]; then
            continue
        fi

        DIR_TOTAL=$((DIR_TOTAL + 1))
        base_name=$(basename "$test_file" .txt)
        result_file="${base_name}_result.txt"

        echo "Running test: $base_name" >> $LOG_FILE

        # Check if result file exists
        if [ ! -f "$result_file" ]; then
            echo "FAILED - Missing result file for $test_file" >> $LOG_FILE
            DIR_FAILED=$((DIR_FAILED + 1))
            continue
        fi

        # Run test and capture output
        ./mysh < "$test_file" > "temp_output.txt"

        # Compare output with expected result
        if diff -w "temp_output.txt" "$result_file" > /dev/null; then
            echo "Status: PASSED" >> $LOG_FILE
            DIR_PASSED=$((DIR_PASSED + 1))
        else
            echo "Status: FAILED" >> $LOG_FILE
            echo "" >> $LOG_FILE
            echo "Expected output:" >> $LOG_FILE
            cat "$result_file" >> $LOG_FILE
            echo "" >> $LOG_FILE
            echo "Actual output:" >> $LOG_FILE
            cat "temp_output.txt" >> $LOG_FILE
            DIR_FAILED=$((DIR_FAILED + 1))
        fi
        echo "------------------------" >> $LOG_FILE

        cleanup_test_artifacts
    done

    # Clean up
    rm -f temp_output.txt
    rm -f mysh

    # Update total counters
    TOTAL_TESTS=$((TOTAL_TESTS + DIR_TOTAL))
    TOTAL_PASSED=$((TOTAL_PASSED + DIR_PASSED))
    TOTAL_FAILED=$((TOTAL_FAILED + DIR_FAILED))

    # Write directory summary
    echo "" >> $LOG_FILE
    echo "Directory Summary for $dir_name:" >> $LOG_FILE
    echo "Total tests: $DIR_TOTAL" >> $LOG_FILE
    echo "Passed: $DIR_PASSED" >> $LOG_FILE
    echo "Failed: $DIR_FAILED" >> $LOG_FILE
    echo "=============================" >> $LOG_FILE
    echo "" >> $LOG_FILE
done

# Write final summary to log
echo "Final Summary" >> $LOG_FILE
echo "=============" >> $LOG_FILE
echo "Total tests run: $TOTAL_TESTS" >> $LOG_FILE
echo "Total passed: $TOTAL_PASSED" >> $LOG_FILE
echo "Total failed: $TOTAL_FAILED" >> $LOG_FILE

# Print summary to console
echo ""
echo "TEST SUMMARY"
echo "=================="
echo "Total tests: $TOTAL_TESTS"
echo "Passed: $TOTAL_PASSED"
echo "Failed: $TOTAL_FAILED"
echo ""
echo "See test_results.log for more information"

# Exit with status 1 if any tests failed
[ $TOTAL_FAILED -eq 0 ] || exit 1
