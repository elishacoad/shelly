// msh.c
// Elisha Coad
// 2/20/20
// CS 240

#include "functions.h"

int main(int argc, char **argv) {
    char *line;
    char **tokens;

    // Executes commands located in file mshrc
    initSequence(line, tokens);

    // Accepts input from user
    loop(line, tokens);

    return 0;
}

void initSequence(char *line, char **tokens) {
    FILE *mshrc;
    size_t bufsize = 0;

    // Open file mshrc
    mshrc = fopen("./mshrc", "r");

    if (mshrc == NULL) {
        printError("Error opening mshrc");
        exit(1);
    }

    // Reads and runs commands in file
    while ((getline(&line, &bufsize, mshrc)) != -1) {
        // Parse command
        tokens = tokenize(line);
        // Check for aliases
        tokens = checkAliases(tokens);
        // Execute command
        run(tokens);
    }

    // Free used memory
    free(line);
    free(tokens);

    // Close file mshrc
    fclose(mshrc);
}

void loop(char *line, char **tokens) {
    int status = 1;

    // Shell runs indefinitely until
    // status triggers termination of the shell
    while (status) {
        // Start each line with "?: "
        printNewline();
        // Read user input
        line = process();
        // Add input to history
        addToHistory(line);
        // Parse input
        tokens = tokenize(line);
        // Check for aliases
        tokens = checkAliases(tokens);
        // Execute command
        status = run(tokens);
        // Free used memory
        free(line);
        free(tokens);
    };
}

char *process() {
    char key;
    int count = 0;
    // currIdx tracks where user is in history stack
    int currIdx = historyIdx;
    // Line can be 50 characters long
    char *line = (char *)malloc(LINE_SIZE * sizeof(char));
    char *outputLine = (char *)malloc(LINE_SIZE * sizeof(char));

    if (!line || !outputLine) {
        printError("Malloc failed");
        exit(1);
    }

    // Check if first key is an arrow key
    // if so, allow history scrolling
    while ((key = getChar()) == '\033') {
        count = 0;

        // skip the [
        getChar();

        // Differentiate arrow
        switch (getChar()) {
            case 'A':
                // Up arrow
                // If there are previous commands,
                // go to next previous command
                if (currIdx > 0) {
                    currIdx--;

                    strcpy(line, history[currIdx]);
                    strcpy(outputLine, line);

                    // Removes \n from output
                    while (outputLine[count] != '\n') {
                        count++;
                    }
                    outputLine[count] = '\0';

                    // Displays current history command
                    printNewline();
                    printf("%s", outputLine);
                }
                break;
            case 'B':
                // Down arrow
                // If there are forward commands,
                // go to next forward command
                if (currIdx < historyIdx - 1) {
                    currIdx++;

                    strcpy(line, history[currIdx]);
                    strcpy(outputLine, line);

                    // removes \n from output line
                    while (outputLine[count] != '\n') {
                        count++;
                    }
                    outputLine[count] = '\0';

                    printNewline();
                    printf("%s", outputLine);
                }
                // If no more forward commands,
                // clear the line
                else if (currIdx == historyIdx - 1) {
                    currIdx++;

                    // Clear the line
                    printNewline();
                    *line = '\0';
                }
                break;
            case 'C':
                // Right arrow
                // Do nothing
                break;
            case 'D':
                // Left arrow
                // Do nothing
                break;
        }
    }
    while (key != '\n') {
        // Handle delete or backspace
        if (key == 127 || key == 8) {
            if (count > 0) {
                count--;
                line[count] = '\0';
                printNewline();
                printf("%s", line);
                key = getChar();
                if (key == '\n') {
                    line[count + 1] = key;
                    line[count + 2] = '\0';
                }
            }
        }
        // Handle arrow keys
        else if (key == '\033') {
            // skip the [
            getChar();

            // Differentiate arrows but
            // do nothing
            switch (getChar()) {
                case 'A':
                    break;
                case 'B':
                    break;
                case 'C':
                    break;
                case 'D':
                    break;
            }
            key = getChar();
        }
        // Handle other characters
        else {
            // Count ensures line doesn't
            // exceed its memory space
            if (count < 48) {
                printf("%c", key);
                line[count] = key;
                key = getChar();
                if (key == '\n') {
                    line[count + 1] = key;
                    line[count + 2] = '\0';
                }
                count++;
            } else {
                printf("\n");
                printError("Line has met its limit");
                printNewline();
                printf("%s", line);

                key = getChar();
                count--;
                if (key == '\n') {
                    line[count + 1] = key;
                    line[count + 2] = '\0';
                }
                count++;
            }
        }
    }
    printf("\n");
    free(outputLine);
    return line;
}

char **tokenize(char *line) {
    int idx = 0;
    char *token;
    char **tokens = (char **)malloc(TOKEN_SIZE * sizeof(char *));

    if (!tokens) {
        printError("Token malloc failed");
        exit(1);
    }

    // Converts the string into tokens using the delimiters space and newline
    while (*line != '\0') {
        token = line;
        while (*line != '\0' && *line != ' ' && *line != '\n') {
            // If there are double quotes, will group everything
            // inside the quotes as one argument
            if (*line == '\"') {
                line++;
                while (*line != '\"') {
                    line++;
                }
            }
            line++;
        }
        if (*line != '\0') {
            *line = '\0';
            line++;
        }
        tokens[idx] = token;
        idx++;
    }

    tokens[idx] = NULL;
    return tokens;
}

int run(char **tokens) {
    int status = 1;

    // No input case
    if (tokens[0] == NULL) {
        printError("No input");
    }

    // Checks for special tokens
    else if (hasSpecialToken(tokens)) {
        status = manageSpecialToken(tokens);
    }

    // Executes command
    else {
        status = executeCmd(tokens);
    }

    return status;
}

int executeCmd(char **tokens) {
    // Determines if command is a custom or system command
    if (isCustCmd(tokens[0])) {
        return executeCustCmd(tokens);
    } else {
        return executeSysCmd(tokens);
    }
    return 1;
}

int executeSysCmd(char **tokens) {
    pid_t pid, wpid;
    int status;
    pid = fork();

    // Child Process
    if (pid == 0) {
        // Execute system command using tokens
        if (execvp(tokens[0], tokens) == -1) {
            printError("Invalid command");
        }
        exit(1);
    }

    // Parent Process
    else if (pid > 0) {
        // Wait for child to finish executing
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    // Error
    else {
        printError("Error in fork");
        exit(1);
    }

    return 1;
}
