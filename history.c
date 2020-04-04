// history.c

#include "functions.h"

// Stores last 21 commands
char history[21][50];
// Increments for every new history addition
int historyNum = 0;
// Keeps track of the number of elements in history queue
int historyIdx = 0;

void addToHistory(char *line) {
    int i = 0;

    // If line is empty, do nothing
    if (*line == '\0' || *line == '\n') {
        return;
    }

    // If history is not full, add to end of history
    else if (historyIdx < 21) {
        strcpy(history[historyIdx], line);
        historyIdx++;
    }

    // If history is full, enqueue new entry onto history
    else {
        while (i < 20) {
            strcpy(history[i], history[i + 1]);
            i++;
        }
        strcpy(history[20], line);
    }

    // Increment number of history lines total
    historyNum++;
}

void printHistory() {
    int i = 0;

    // Will skip first entry if history queue is full
    if (historyIdx > 20) {
        i++;
    }

    // Print last 20 commands line-by-line in "NUM : COMMAND" format
    while (i < historyIdx) {
        printf("%d : ", historyNum - historyIdx + i);
        printf("%s", history[i]);
        i++;
    }
}

void executeLastCommand() {
    char *line;
    char **tokens;

    // Checks if able to execute last command
    if (historyIdx - 2 < 0) {
        printError("No previous command");
        return;
    } else if (strcmp(history[historyIdx - 2], "!!\n") == 0) {
        printError("!! was last command");
        return;
    }

    line = (char *)malloc(LINE_SIZE * sizeof(char));

    if (line == NULL) {
        printError("Malloc failed");
        exit(1);
    }

    // Get last command
    strcpy(line, history[historyIdx - 2]);

    // Parse command
    tokens = tokenize(line);

    // Check for aliases
    tokens = checkAliases(tokens);

    // Execute command
    run(tokens);

    // Free used memory
    free(line);
    free(tokens);
}

void executeByNumber(int num) {
    char *line;
    char **tokens;

    // Finds index of command in history based on command's number
    int idx = num + historyIdx - historyNum;

    // Checks if able to execute command at given index
    if (idx < 0 || idx >= historyIdx - 1) {
        printError("Invalid Index");
        return;
    } else if (*history[idx] == '!') {
        printError("Unable to call command");
        return;
    }

    line = (char *)malloc(LINE_SIZE * sizeof(char));

    if (line == NULL) {
        printError("Malloc failed");
        exit(1);
    }

    // Get command at given index
    strcpy(line, history[idx]);

    // Parse command
    tokens = tokenize(line);

    // Check for aliases
    tokens = checkAliases(tokens);

    // Execute command
    run(tokens);

    // Free used memory and restart
    free(line);
    free(tokens);
}
