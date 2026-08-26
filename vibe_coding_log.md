Kole Pjetraj
NCSU ECE306 - Fall 2026
Project 1 -  An LLM Mini-Harness in C via Vibe Coding 
LLM in use: Claude by Anthropic

SPECS FROM ASSIGNMENT: ************************************************************************************
In this foundational project, you will build a minimal LLM agent harness in C. An agent
harness acts as the bridge between an LLM and the operating system, managing inputs,
context boundaries, tool execution, etc. 

1. Core Loop: Implement a terminal-based loop that captures user input, passes it to
a mock model function (which mimics an LLM), and outputs the simulated
response. Note that the purpose of the mock model is that you don’t need call the
LLM APIs.
2. Context Management: Allocate and manage memory safely to store a minimal
conversation history (e.g., the last 5 turns)
3. Tool execution: calls a tool to execute functions such as mathematical calculation
that an LLM is not designed for.
4. Vibe Coding Log: Document the architectural rules and prompts you used to
generate the C code, demonstrating your application of SDD principles.
5. AI-Generated Testing: Instruct your AI assistant to write a separate testing script
that validates your harness's state management and checks for basic memory leaks


PROMPT 1 (adapted from assignment):***************************************************************************

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


OUTPUT 1:******************************************************************************************************

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


OUTPUT 1 TESTING:**********************************************************************************************
    Basic Examination: 
        The code only included standard header files <stdio.h> and <string.h> as specified in specs

        The code implemented an infinite while() loop requesting user input via fgets()

        The code included line-by-line comments explaining functionality

        The code implemented cases for special input:
            1. input including "hello"
            2. input of "exit"

    
    Compilation Test:
        The code was ran in WSL using 'gcc harness.c -o harness'. The code compiled and yielded no errors. 

    Output Test:
        The program was run in WSL terminal using './harness'

        The program responded as specified to input from user. 
        
        Input including the word "hello" produced expected output of "Hello there, how may I assist you?" as highlighted in specs. 
        
        Input of the word "exit" resulted in the program exiting.
        
        All other input was echoed back to the user on the console. Output including but not limited to the word "exit" was echoed back to console.

    Bash Script for deterministic testing:

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