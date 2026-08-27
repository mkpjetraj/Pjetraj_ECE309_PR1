#!/bin/bash
# test_harness.sh
# Runs the compiled 'harness' program through a fixed sequence of inputs,
# checks that the output matches what each feature should produce, and
# (if valgrind is installed) checks for memory leaks / invalid memory use.

BINARY="./harness"   # path to the compiled program under test

# The scripted conversation we feed into harness, one line per input:
#   hello      -> should trigger the hardcoded greeting
#   12 + 7     -> should trigger the calculator tool -> 19
#   debug      -> should report the current history size (state check)
#   3 more turns to push the ring buffer past its 5-turn capacity
#   exit       -> should end the program cleanly
INPUT_SEQUENCE="hello
12 + 7
debug
foo
bar
baz
exit
"

# Basic sanity check: make sure the binary actually exists before we try to run it.
if [ ! -x "$BINARY" ]; then
    echo "FAIL: $BINARY not found or not executable. Build it first with: gcc -o harness harness.c"
    exit 1
fi

echo "=== Functional test ==="
OUTPUT=$(printf "%s" "$INPUT_SEQUENCE" | $BINARY)   # feed the whole scripted conversation in at once
echo "$OUTPUT"                                      # show the raw transcript for the human reader
echo

FAIL_COUNT=0   # tracks how many checks fail, so we can give a clear pass/fail summary at the end

# Check 1: the greeting appeared for "hello"
if echo "$OUTPUT" | grep -q "Hello there, how may I assist you?"; then
    echo "PASS: greeting response"
else
    echo "FAIL: greeting response"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi

# Check 2: the calculator tool correctly computed 12 + 7 = 19
if echo "$OUTPUT" | grep -q "Tool result: 19.00"; then
    echo "PASS: tool execution (calculator)"
else
    echo "FAIL: tool execution (calculator)"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi

# Check 3: state management — after 2 turns (user+assistant for hello, user+assistant for calc,
# then the user's "debug" turn itself gets recorded before the model responds) history should
# report having stored turns, and specifically should never report more than 5 (MAX_HISTORY).
DEBUG_LINE=$(echo "$OUTPUT" | grep "History currently holds")
if echo "$DEBUG_LINE" | grep -qE "History currently holds [1-5] turn\(s\)\."; then
    echo "PASS: state management (history count in valid 1-5 range) -> $DEBUG_LINE"
else
    echo "FAIL: state management (unexpected or missing history count) -> $DEBUG_LINE"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi

echo
echo "=== Memory leak / safety check (valgrind) ==="
if command -v valgrind >/dev/null 2>&1; then
    VALGRIND_LOG="/tmp/harness_valgrind.log"   # where we stash valgrind's full report

    # --leak-check=full gives a detailed leak report instead of just a summary count.
    # --error-exitcode=1 makes valgrind itself return a non-zero exit code if it finds
    # any memory errors (not just leaks), which we check below as a second signal.
    printf "%s" "$INPUT_SEQUENCE" | valgrind --leak-check=full --error-exitcode=1 \
        "$BINARY" > "$VALGRIND_LOG" 2>&1
    VALGRIND_EXIT=$?

    # Valgrind reports a clean run in one of two ways:
    #   1. A full "LEAK SUMMARY" with explicit "definitely/indirectly lost: 0 bytes" lines, or
    #   2. A short "All heap blocks were freed -- no leaks are possible" line when there's
    #      nothing left allocated at all. Accept either form as a pass.
    if grep -q "All heap blocks were freed -- no leaks are possible" "$VALGRIND_LOG"; then
        LEAKS_OK=1
    elif grep -q "definitely lost: 0 bytes" "$VALGRIND_LOG" && \
         grep -q "indirectly lost: 0 bytes" "$VALGRIND_LOG"; then
        LEAKS_OK=1
    else
        LEAKS_OK=0
    fi

    if [ "$LEAKS_OK" -eq 1 ] && [ "$VALGRIND_EXIT" -eq 0 ]; then
        echo "PASS: no memory leaks or memory errors detected"
    else
        echo "FAIL: potential memory leak or error detected — see $VALGRIND_LOG"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi
else
    echo "SKIP: valgrind is not installed, so leak checking was skipped."
    echo "      Install it with: sudo apt install valgrind"
fi

echo
if [ "$FAIL_COUNT" -eq 0 ]; then
    echo "=== ALL CHECKS PASSED ==="
    exit 0
else
    echo "=== $FAIL_COUNT CHECK(S) FAILED ==="
    exit 1
fi