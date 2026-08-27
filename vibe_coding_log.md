Kole Pjetraj
NCSU ECE306 - Fall 2026
Project 1 -  An LLM Mini-Harness in C via Vibe Coding 
LLM in use: Claude by Anthropic

SPECS FROM ASSIGNMENT: *********************************
In this foundational project, you will build a minimal LLM agent harness in C. An agent
harness acts as the bridge between an LLM and the operating system, managing inputs,
context boundaries, tool execution, etc. 

Implementation Requirements
1. Core Loop: Implement a terminal-based loop that captures user input, passes it to a mock model function (which mimics an LLM), and outputs the simulated response. Note that the purpose of the mock model is that you don’t need call the LLM APIs.
2. Context Management: Allocate and manage memory safely to store a minimal conversation history (e.g., the last 5 turns)
3. Tool execution: calls a tool to execute functions such as mathematical calculation that an LLM is not designed for.
4. Vibe Coding Log: Document the architectural rules and prompts you used to generate the C code, demonstrating your application of SDD principles.
5. AI-Generated Testing: Instruct your AI assistant to write a separate testing script that validates your harness's state management and checks for basic memory leaks

VERSION 1 *********************************

    PROMPT 1 (adapted from assignment):*********************************

        "I need help writing a simple command-line program in C using simple programming concepts. 

        Instructions:
        Only utilize the standard C library functions. Ex: <stdio.h> and <string.h>. 
        
        1. The program must run an infinite while loop that asks for user input using fgets.
        2. If the user types 'exit', the loop should break and the program should end.
        3. If the user types a sentence containing the word 'hello', the program should print a
        hardcoded greeting as highlighted in section 3a.
            3a. greeting message: "Hello there, how may I assist you?"
        4. If the user inputs any other message, the program will echo their input back to them.
        5. The program must include clear comments on every line explaining what the respective line of code is doing." 


    OUTPUT 1 (saved as tag v1.0):*********************************

    Code: 
        #include <stdio.h>   // for printf, fgets, stdout functions
        #include <string.h>  // for strstr, strcspn, strncmp functions

        int main() {                                   // program entry point
            char input[256];                           // buffer to hold user input, up to 255 chars + null terminator

            while (1) {                                // start infinite loop to keep asking for input
                printf("> ");                          // print a simple prompt so the user knows to type something
                fflush(stdout);                        // flush stdout to make sure the prompt shows before input is read

                if (fgets(input, sizeof(input), stdin) == NULL) {  // read a line of input into 'input'; if fgets fails (e.g., EOF)
                    break;                             // exit the loop if input reading failed
                }

                input[strcspn(input, "\n")] = '\0';    // find the newline character fgets keeps, and replace it with null terminator

                if (strncmp(input, "exit", 4) == 0 && input[4] == '\0') { // check if the trimmed input is exactly "exit"
                    break;                             // if user typed exactly "exit", break out of the loop and end program
                }

                if (strstr(input, "hello") != NULL) {  // check if the word "hello" appears anywhere in the input string
                    printf("Hello there, how may I assist you?\n"); // print the hardcoded greeting message
                } else {                               // otherwise, for any other input
                    printf("%s\n", input);             // echo the user's input back to them exactly as typed
                }
            }

            return 0;                                  // return 0 to indicate the program ended successfully
        }


    OUTPUT 1 TESTING:*********************************
            The program only included standard header files <stdio.h> and <string.h> as specified in specs
            
            The program implemented an infinite while() loop requesting user input via fgets(), line-by-line comments explaining functionality, and cases for special input as defined in specs

            The program compiled and yielded no errors. It performed as specified when ran, testing for special cases of text including "hello" and text limited to "quit", echoing all othe rinput to the console.

        Script for deterministic testing:

            Prompt (from assignment): 
                "I have a compiled C program named harness. Write a very simple Bash script (for
                Linux/Mac) that automatically sends the word 'hello', followed by the word 'exit', into the
                program to test if it works." 

            Output (saved to test.sh):
                #!/bin/bash
                # simple bash script to automate testing of the 'harness' program

                # use printf to generate the two lines of input, separated by newlines,
                # and pipe them into the harness program's stdin
                printf "hello\nexit\n" | ./harness

            Testing Bash Script: 
                command 'bash test.sh' was ran in terminal. Output "Hello there, how may I assist you?" as expected.


REVISION 1*********************************

    While the program works for expected inputs and outputs, it is missing features required in the assignment
    Specifically implementation requirements 2, 3, and memory leak testing in 5 (see "Implementation Requirements" above)
 

    PROMPT 2*********************************

        (adapted from assignment)
        "While the harness program produces the expected outputs for the given inputs, I need it to implement more features: 
            1. Core Loop: Implement a terminal-based loop that captures user input, passes it to
            a mock model function (which mimics an LLM), and outputs the simulated
            response. Note that the purpose of the mock model is that you don’t need call the
            LLM APIs.
            2. Context Management: Allocate and manage memory safely to store a minimal
            conversation history (e.g., the last 5 turns)
            3. Tool execution: calls a tool to execute functions such as mathematical calculation
            that an LLM is not designed for.
        
        In addition to this, I need the testing program to not only run the program but to validate the harness's state management and check for basic memory leaks."


    OUTPUT 2 (saved as tag v2.0)*********************************
        Output is saved in the main branch of repository under harness.c
        Test script saved under main branch of repository as test.sh
        
    OUTPUT 2 TESTING*********************************
        The program only included standard header files <stdio.h>, <stdlib.h>, and <string.h> as specified in specs
            
        The program implemented an infinite while() loop requesting user input via fgets(), line-by-line comments explaining functionality, and cases for special input as defined in specs

        The program utilizes a function called MockModel() that stands in for a real LLM call

        The program utilizes functions for context management and tool execution.

        The program compiled and yielded no errors. It performed as specified when ran, testing for special cases of text including "hello" and text limited to "quit", echoing all othe rinput to the console.

        The LLM generated a robust test program (saved as test.sh) that utilizes terminal commands to test multiple input cases for expected output, as well as checking that each test is passed and generating an output message for tests passed and failed. 

        Test.sh was ran in the terminal - but did not immediately pass all tests (see more below)

    Explaination from Claude:*********************************

        What each piece does:

        harness.c

        Core loop: same read/respond loop as before, but now routes through a mock_model() function that stands in for a real LLM call.
        
        Context management: a History struct implemented as a fixed-size ring buffer (MAX_HISTORY = 5). Each turn's text is heap-allocated with strdup; when the buffer wraps around and overwrites an old slot, that slot's old memory is freed first — so you get the "last 5 turns" behavior without ever leaking as the conversation grows past 5 turns.
        
        Tool execution: tool_calculate() parses a "<number> <op> <number>" pattern (e.g. 12 + 7) and does the math — the sort of thing a real LLM would hand off to a tool rather than compute internally.
       
        A debug command was added so the test script has a way to inspect internal state (how many turns are stored) without needing to peek at memory directly.

        test_harness.sh

        Feeds a scripted conversation through the program (greeting, calculation, debug, then three throwaway turns to push the ring buffer past capacity, then exit).
        Checks each feature's output with grep and reports PASS/FAIL per check, including a state-management check that the reported history count never exceeds 5.
       
        If valgrind is installed, runs the same input through it with --leak-check=full and fails the test if any bytes are "definitely lost" or "indirectly lost." If valgrind isn't installed (as in this sandbox), it skips that check explicitly rather than pretending it passed.

        To get the leak-check working on your end, in WSL:

        bash
        sudo apt install valgrind"

    TEST SCRIPT: *********************************
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

TEST SCRIPT REVISION: *********************************
   
    'sudo apt install valgrind' was ran per reccomendation by Claude to run the memory leak debugging segment of test.sh

    When 'bash test.sh' was ran in terminal, it yielded the results: 

        test.sh: line 6: $'\r': command not found
        test.sh: line 8: $'\r': command not found
        test.sh: line 23: $'\r': command not found
        test.sh: line 96: syntax error near unexpected token fi'
        test.sh: line 96: fi'

    The error codes were prompted back to Claude. 

    Claude Output: 

        our file has \r\n (Windows-style) line endings instead of \n (Unix-style), which happens when a file is created or edited in a Windows text editor (like Notepad) before being used in WSL/bash. Bash reads that trailing \r character as part of the command itself, which is why you're getting $'\r': command not found and eventually a syntax error once it throws things off enough to break control structures like if/fi.

        Fix it with this one command:

        bash
        sed -i 's/\r$//' test.sh

        This strips the trailing \r from every line in place. Then run it again:

        bash
        bash test.sh

        It should execute cleanly now, with no more $'\r' errors.

        Why this happened: you likely pasted the script content into a Windows-based editor (Notepad, or even VS Code if it wasn't explicitly set to use Unix line endings) rather than creating/pasting it directly within a WSL-connected terminal or a WSL-aware editor.

        To prevent this going forward: if you're using VS Code, check the bottom-right status bar when a file is open — it shows either CRLF (Windows) or LF (Unix). You can click on it and switch to LF for any script files you intend to run in WSL/bash.

    The reccomendations were followed and 'bash test.sh' was ran again in the terminal, yielding the following output: 

        === Functional test ===

        Hello there, how may I assist you?
        Tool result: 19.00
        History currently holds 5 turn(s).
        foo
        bar
        baz

        PASS: greeting response
        PASS: tool execution (calculator)
        PASS: state management (history count in valid 1-5 range) -> > History currently holds 5 turn(s).
        === Memory leak / safety check (valgrind) ===
        FAIL: potential memory leak or error detected — see /tmp/harness_valgrind.log
        === 1 CHECK(S) FAILED ===

    This error code was then prompted back to Claude.

    Claude Output: 

        Good progress — functional tests all pass, and now valgrind is actually running and catching something. Let's see what it found:

        bash
        cat /tmp/harness_valgrind.log

        Paste the output here (or at least the "LEAK SUMMARY" section and anything above it mentioning harness.c line numbers) and I'll help pin down the exact cause. A common one to watch for with this specific program: strdup allocates memory that must be freed — if there's any code path where a turn gets overwritten or the program exits without every stored turn being freed, valgrind will flag it as "definitely lost."

    The command 'cat /tmp/harness_valgrind.log' was ran in terminal per reccomendation, yielding the following output: 

        ==1243== Memcheck, a memory error detector
        ==1243== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
        ==1243== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
        ==1243== Command: ./harness
        ==1243== 
        > Hello there, how may I assist you?
        > Tool result: 19.00
        > History currently holds 5 turn(s).
        > foo
        > bar
        > baz
        > ==1243== 
        ==1243== HEAP SUMMARY:
        ==1243==     in use at exit: 0 bytes in 0 blocks
        ==1243==   total heap usage: 15 allocs, 15 frees, 8,452 bytes allocated
        ==1243== 
        ==1243== All heap blocks were freed -- no leaks are possible
        ==1243== 
        ==1243== For lists of detected and suppressed errors, rerun with: -s
        ==1243== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

    This output was then prompted back to Claude.

    Claude Output: 

        Good news: your program is actually fine. Look closely at valgrind's own output:

        in use at exit: 0 bytes in 0 blocks
        All heap blocks were freed -- no leaks are possible
        ERROR SUMMARY: 0 errors from 0 contexts

        That's valgrind's way of saying "everything was freed, nothing to report" — a clean bill of health. The bug is in my test script, not your program. When there are zero leaks, valgrind sometimes prints that short summary line instead of the usual definitely lost: 0 bytes / indirectly lost: 0 bytes lines my grep checks were looking for. Since those exact strings weren't present, my script's check failed even though the real result was a pass — a false negative on my end.

        Here's the fix — update the check to also accept that alternate valgrind output:

        Edited a file, ran a command

        Script runs clean here (valgrind isn't installed in this sandbox, so it correctly skips that step). The logic fix itself is straightforward and doesn't need valgrind present to verify — it's just a text-matching change.

        Ran a command, read a file

        Grab the updated test.sh, apply the same line-ending fix if needed (sed -i 's/\r$//' test.sh), and rerun. It should now show PASS: no memory leaks or memory errors detected and ALL CHECKS PASSED.

    The new file 'test.sh' was downloaded, edited using the command 'sed -i 's/\r$//' test.sh', and ran in terminal, producing the following output: 

        === Functional test ===
        > Hello there, how may I assist you?
        > Tool result: 19.00
        > History currently holds 5 turn(s).
        > foo
        > bar
        > baz
        > 

        PASS: greeting response
        PASS: tool execution (calculator)
        PASS: state management (history count in valid 1-5 range) -> > History currently holds 5 turn(s).

        === Memory leak / safety check (valgrind) ===
        PASS: no memory leaks or memory errors detected

        === ALL CHECKS PASSED ===

The final program (harness.c) and test script (test.sh) were saved under tag v2.1

                
            

                



            