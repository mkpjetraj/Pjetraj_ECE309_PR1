#include <stdio.h>    // for printf, fgets, snprintf, fprintf
#include <stdlib.h>   // for malloc, free
#include <string.h>   // for strcspn, strcmp, strstr, strncpy, strdup

#define MAX_INPUT 256      // max characters allowed per line of user input
#define MAX_RESPONSE 512   // max characters allowed in a generated response
#define MAX_HISTORY 5      // how many conversation turns to remember at once
#define ROLE_LEN 16        // max characters for a role label ("user"/"assistant")

// ---------- Context Management ----------
// A single turn in the conversation (either something the user said,
// or something the mock model said back).
typedef struct {
    char role[ROLE_LEN];  // "user" or "assistant"
    char *text;           // heap-allocated copy of the message text (NULL if slot unused)
} Turn;

// A fixed-size ring buffer holding the last MAX_HISTORY turns.
// Using a ring buffer means we never need to shift array elements around;
// we just overwrite the oldest slot once the buffer is full.
typedef struct {
    Turn turns[MAX_HISTORY]; // the fixed-size backing array of turns
    int count;                // how many valid turns are currently stored (0..MAX_HISTORY)
    int next;                 // index of the next slot to write into (wraps around)
} History;

// Initializes a History struct so every text pointer starts NULL.
// Doing this explicitly avoids ever calling free() on garbage/uninitialized memory.
void history_init(History *h) {
    h->count = 0;                          // no turns stored yet
    h->next = 0;                           // start writing at slot 0
    for (int i = 0; i < MAX_HISTORY; i++) {
        h->turns[i].text = NULL;           // mark every slot's text as "not allocated"
    }
}

// Adds one turn (role + text) into the ring buffer, allocating a heap copy of text.
// If the target slot already held an older turn, its memory is freed first,
// which is what prevents a memory leak as the buffer wraps around and reuses slots.
void history_add(History *h, const char *role, const char *text) {
    int idx = h->next;                                   // the slot we are about to write into
    if (h->turns[idx].text != NULL) {                    // slot held a previous turn (buffer has wrapped)
        free(h->turns[idx].text);                        // release that old heap memory first
        h->turns[idx].text = NULL;                        // avoid a dangling pointer between free and reassignment
    }
    h->turns[idx].text = strdup(text);                    // allocate new heap memory and copy the text into it
    strncpy(h->turns[idx].role, role, ROLE_LEN - 1);      // copy the role label safely (leave room for null terminator)
    h->turns[idx].role[ROLE_LEN - 1] = '\0';              // guarantee null termination even if role was truncated
    h->next = (idx + 1) % MAX_HISTORY;                    // advance to the next slot, wrapping back to 0 at the end
    if (h->count < MAX_HISTORY) {                         // only grow count until the buffer is actually full
        h->count++;                                       // one more turn is now stored
    }
}

// Frees every heap-allocated text field still held in the history.
// Must be called once, exactly once, before the History struct itself is freed/discarded.
void history_free(History *h) {
    for (int i = 0; i < MAX_HISTORY; i++) {
        if (h->turns[i].text != NULL) {   // only free slots that were actually allocated
            free(h->turns[i].text);       // release the heap memory
            h->turns[i].text = NULL;      // defensive: avoid a dangling pointer if freed twice by mistake
        }
    }
}

// ---------- Tool Execution ----------
// A very small "calculator tool" the mock model can call for arithmetic it
// isn't actually designed to do itself. Understands the pattern "<num> <op> <num>",
// e.g. "12 + 7". Returns 1 and sets *result on success, 0 on failure.
int tool_calculate(const char *input, double *result) {
    double a, b;               // the two numeric operands
    char op;                   // the operator character (+, -, *, /)

    // sscanf returns how many of the requested fields it successfully parsed.
    // We require all 3 (number, operator, number) or we treat it as "not a calculation".
    if (sscanf(input, "%lf %c %lf", &a, &op, &b) != 3) {
        return 0;               // input doesn't look like "number operator number"
    }

    switch (op) {               // dispatch on which operator was found
        case '+': *result = a + b; return 1;   // addition
        case '-': *result = a - b; return 1;   // subtraction
        case '*': *result = a * b; return 1;   // multiplication
        case '/':                              // division needs a zero-check
            if (b == 0) return 0;              // refuse to divide by zero
            *result = a / b;
            return 1;
        default:
            return 0;            // unrecognized operator, not a valid calculation
    }
}

// ---------- Mock Model ----------
// Stands in for a real LLM call. Given the latest user input (and, optionally,
// the conversation history for context), decides what to respond with.
// Writes the response into the caller-supplied buffer instead of allocating,
// so the caller controls the memory's lifetime.
void mock_model(const char *input, const History *history, char *response, size_t response_size) {
    double calc_result;                                    // holds the tool's numeric answer, if any

    if (tool_calculate(input, &calc_result)) {              // try routing to the calculator tool first
        snprintf(response, response_size, "Tool result: %.2f", calc_result); // format the tool's answer
    } else if (strstr(input, "hello") != NULL) {             // check for the greeting keyword
        snprintf(response, response_size, "Hello there, how may I assist you?");
    } else if (strcmp(input, "debug") == 0) {                 // special command to inspect state, useful for testing
        snprintf(response, response_size, "History currently holds %d turn(s).", history->count);
    } else {                                                  // fallback: no rule matched
        snprintf(response, response_size, "%s", input);       // echo the input back unchanged
    }
}

// ---------- Core Loop ----------
int main(void) {
    History *history = malloc(sizeof(History));   // allocate the conversation history on the heap
    if (history == NULL) {                          // always check malloc's return value
        fprintf(stderr, "Fatal: could not allocate memory for history\n");
        return 1;                                   // abort if we can't even get started
    }
    history_init(history);                          // zero out / initialize the history's contents

    char input[MAX_INPUT];        // buffer for one line of raw user input
    char response[MAX_RESPONSE];  // buffer for the mock model's generated response

    while (1) {                                      // main read-generate-respond loop
        printf("> ");                                // prompt so the user knows to type
        fflush(stdout);                              // force the prompt to appear before we block on input

        if (fgets(input, sizeof(input), stdin) == NULL) { // read one line; NULL means EOF or read error
            break;                                    // nothing more to read, end the program
        }
        input[strcspn(input, "\n")] = '\0';           // strip the trailing newline fgets leaves in place

        if (strcmp(input, "exit") == 0) {             // exact match on "exit" ends the session
            break;
        }

        history_add(history, "user", input);          // record the user's turn before generating a response

        mock_model(input, history, response, sizeof(response)); // "call the model" to produce a response

        history_add(history, "assistant", response);  // record the assistant's turn for future context

        printf("%s\n", response);                     // show the response to the user
    }

    history_free(history);   // release every heap-allocated turn text still stored
    free(history);           // release the History struct itself
    return 0;                // clean exit
}