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
