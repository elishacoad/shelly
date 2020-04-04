// functions.c

#include "functions.h"

#define NUM_COMMANDS 7
#define NUM_TOKENS 2

// List of custom commands
char *myCommands[] = {
    "alias",
    "unalias",
    "history",
    "!!",
    "cd",
    "export",
    "exit"};

// List of custom tokens
char *myTokens[] = {
    ";",
    "|"};

bool isCustCmd(char *token) {
    int i = 0;

    while (i < NUM_COMMANDS) {
        // Checks command list
        if (strcmp(token, myCommands[i]) == 0) {
            return true;
        }
        // Bang followed by a digit is also a custom command
        else if (*token == '!' && isdigit(*(token + 1))) {
            return true;
        }
        i++;
    }
    return false;
}

int executeCustCmd(char **tokens) {
    // Alias
    if (strcmp(tokens[0], myCommands[0]) == 0) {
        // Determines whether to print or add a new alias
        if (tokens[1] != NULL) {
            addAlias(tokens);
        } else {
            listAliases();
        }
    }
    // Unalias
    else if (strcmp(tokens[0], myCommands[1]) == 0) {
        removeAlias(tokens[1]);
    }
    // History
    else if (strcmp(tokens[0], myCommands[2]) == 0) {
        printHistory();
    }
    // !!
    else if (strcmp(tokens[0], myCommands[3]) == 0) {
        executeLastCommand();
    }
    // cd
    else if (strcmp(tokens[0], myCommands[4]) == 0) {
        changeDirectory(tokens);
    }
    // export
    else if (strcmp(tokens[0], myCommands[5]) == 0) {
        exportPath(tokens);
    }
    // exit
    else if (strcmp(tokens[0], myCommands[6]) == 0) {
        return exitShell();
    }
    // ! followed by a digit
    else if (*tokens[0] == '!' && isdigit(*(tokens[0] + 1))) {
        executeByNumber(atoi(tokens[0] + 1));
    }
    // Error
    else {
        printError("False custom command");
        exit(1);
    }
    return 1;
}

char **insertTokens(char **tokens, char **insertTokens, int idx) {
    // Given ["a", "b", "c"], ["k", "p"], 1
    // Returns ["a", "k", "p", "b", "c"]

    int i = 0;

    // Inserts token stream at idx
    while (insertTokens[i] != NULL) {
        insertToken(tokens, insertTokens[i], idx);
        idx++;
        i++;
    }

    return tokens;
}

char **insertToken(char **tokens, char *insertToken, int idx) {
    // Given ["a", "b", "c"], "k", 1
    // Returns ["a", "k", "b", "c"]

    char *tempPrev;
    char *tempNext;

    // Replaces index with insert token
    tempPrev = tokens[idx];
    tokens[idx] = insertToken;
    idx++;

    // Shifts everything over one
    while (tokens[idx] != NULL) {
        tempNext = tokens[idx];
        tokens[idx] = tempPrev;
        tempPrev = tempNext;
        idx++;
    }

    // Closes list
    tokens[idx] = tempPrev;
    tokens[idx + 1] = NULL;

    return tokens;
}

void printTokens(char *tag, char **tokens) {
    int i = 1;

    // Opens list and prints first token
    printf("%s: [\"%s\"", tag, tokens[0]);

    // Prints other tokens
    while (tokens[i] != NULL) {
        printf(", \"%s\"", tokens[i]);
        i++;
    }

    // Closes list
    printf("]\n");
}

char **removeToken(char **tokens, char *token) {
    int i = 0;

    while (tokens[i] != NULL) {
        // If match is found
        if (tokens[i] == token) {
            // Shifts everything to remove token
            while (tokens[i + 1] != NULL) {
                tokens[i] = tokens[i + 1];
                i++;
            }
            // Ends list one token shorter
            tokens[i] = NULL;
            break;
        }
        i++;
    }

    return tokens;
}

int changeDirectory(char **tokens) {
    if (tokens[1] == NULL) {
        printError("Missing address");
    } else if (chdir(tokens[1]) != 0) {
        printError("Invalid address");
    }
    return 1;
}

int exportPath(char **tokens) {
    char *localPath;
    char *path;

    // Takes input of format "export PATH=$PATH:/usr/path/example"
    if (tokens[1] == NULL) {
        printf("Missing arguments\n");
        return 1;
    } else if ((strncmp("PATH=$PATH:", tokens[1], 11) != 0) ||
               (tokens[1][11] == '\0')) {
        printf("Invalid Format\n");
        return 1;
    }

    // Point local path to beginning of local path variable
    localPath = tokens[1];
    localPath = &localPath[10];

    // Get environment PATH and add local path to it
    path = getenv("PATH");
    printf("Old PATH\n %s\n", path);
    strcat(path, localPath);
    setenv("PATH", path, 1);
    printf("New PATH\n %s\n", path);

    return 1;
}

int exitShell() {
    return 0;
}

bool hasSpecialToken(char **tokens) {
    int idx = 0, i = 0;

    // Checks each token
    while (tokens[idx] != NULL) {
        // Compares token to each special token
        while (i < NUM_TOKENS) {
            if (strcmp(tokens[idx], myTokens[i]) == 0) {
                return true;
            }
            i++;
        }
        i = 0;
        idx++;
    }

    return false;
}

int manageSpecialToken(char **tokens) {
    int tokensIdx = 0;
    int tokenCacheIdx = 0;
    char **tokenCache = (char **)malloc(TOKEN_SIZE * sizeof(char *));

    while (tokens[tokensIdx] != NULL) {
        // check for |
        if (strcmp(tokens[tokensIdx], "|") == 0) {
            // If tokenCache has a command in it
            if (tokenCacheIdx != 0) {
                // End the list
                tokenCache[tokenCacheIdx] = NULL;
                // Reset tokenCacheIdx
                tokenCacheIdx = 0;
                // skip | token
                tokensIdx++;

                // Execute command "firstHalf | secondHalf"
                return pipeCommand(tokenCache, &tokens[tokensIdx]);
            }
        }

        // check for ;
        else if (strcmp(tokens[tokensIdx], ";") == 0) {
            // If tokenCache has a command in it
            if (tokenCacheIdx != 0) {
                // End the list
                tokenCache[tokenCacheIdx] = NULL;
                // Reset tokenCacheIdx
                tokenCacheIdx = 0;
                // skip ; token
                tokensIdx++;

                // Execute command, checks for exit command
                if (executeCmd(tokenCache) == 0) {
                    return 0;
                }
                // Else execute second half
                else {
                    return executeCmd(&tokens[tokensIdx]);
                }
            }
        } else {
            // Add token to tokenCache
            tokenCache[tokenCacheIdx] = tokens[tokensIdx];
            tokenCacheIdx++;
        }
        tokensIdx++;
    }

    // Rewrite to enable
    // free(tokenCache);

    return 1;
}

int pipeCommand(char **tokens1, char **tokens2) {
    int des_p[2];
    if (pipe(des_p) == -1) {
        printError("Pipe failed");
        exit(1);
    }

    // First child
    if (fork() == 0) {
        // Closing stdout
        close(STDOUT_FILENO);
        // Replacing stdout with pipe write
        dup(des_p[1]);
        // Closing pipe read
        close(des_p[0]);
        close(des_p[1]);

        execvp(tokens1[0], tokens1);
        printError("Execvp of first command failed");
        exit(1);
    }

    // Second child
    if (fork() == 0) {
        // Closing stdin
        close(STDIN_FILENO);
        // Replacing stdin with pipe read
        dup(des_p[0]);
        // Closing pipe write
        close(des_p[1]);
        close(des_p[0]);

        execvp(tokens2[0], tokens2);
        printError("Execvp of second command failed");
        exit(1);
    }

    close(des_p[0]);
    close(des_p[1]);
    wait(0);
    wait(0);
    return 1;
}