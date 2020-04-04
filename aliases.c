// aliases.c

#include "functions.h"

int numOfAliases = 0;
aliasList *head = NULL;

void addAlias(char **tokens) {
    aliasList *node;
    aliasList *last = head;

    // Allocate space
    char *name = (char *)malloc(LINE_SIZE * sizeof(char));
    char *val = (char *)malloc(LINE_SIZE * sizeof(char));

    if (!name || !val) {
        printError("Malloc failure");
        exit(1);
    }

    // Read values from input
    strcpy(name, strtok(tokens[1], "="));
    strcpy(val, strtok(NULL, "\""));

    if (isAlias(name)) {
        printError("Alias already exists");
        return;
    }

    // Check validity of this
    node = (aliasList *)malloc(sizeof(aliasList));

    if (!node) {
        printError("Malloc failure");
    }

    // Initialize node with values
    node->aliasEntry.name = name;
    node->aliasEntry.tokens = tokenize(val);
    node->next = NULL;

    // Add node to linked list
    if (head == NULL) {
        head = node;
    } else {
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = node;
    }

    numOfAliases++;
}

bool isAlias(char *token) {
    aliasList *node = head;

    while (node != NULL) {
        // If token matches an alias name
        if (strcmp(token, node->aliasEntry.name) == 0) {
            return true;
        }
        node = node->next;
    }

    return false;
}

char **getAliasTokens(char *name) {
    char *val;
    aliasList *node = head;

    while (node != NULL) {
        // If name matches an alias, return alias tokens
        if (strcmp(name, node->aliasEntry.name) == 0) {
            return node->aliasEntry.tokens;
        } else {
            node = node->next;
        }
    }

    printError("Could not find Alias Tokens");
    exit(1);
    return NULL;
}

void listAliases() {
    int i = 0;
    aliasList *node = head;

    while (node != NULL) {
        printf("alias %s=\"", node->aliasEntry.name);
        printf("%s", node->aliasEntry.tokens[i]);
        i++;
        while (node->aliasEntry.tokens[i] != NULL) {
            printf(" %s", node->aliasEntry.tokens[i]);
            i++;
        }
        printf("\"\n");
        i = 0;
        node = node->next;
    }
}

char **checkAliases(char **tokens) {
    int i = 0;
    char **aliasTokens;

    // Ignore if NULL
    if (tokens[0] == NULL) {
        return tokens;
    }

    // Ignore if trying to unalias
    if (strcmp(tokens[0], "unalias") == 0) {
        return tokens;
    }

    while (tokens[i] != NULL) {
        // If token is an alias, remove it and
        // insert its tokens into the token stream
        if (isAlias(tokens[i])) {
            // Get alias tokens
            aliasTokens = getAliasTokens(tokens[i]);
            // Remove alias from tokenstream
            tokens = removeToken(tokens, tokens[i]);
            // Insert alias tokens in its place
            tokens = insertTokens(tokens, aliasTokens, i);
        }
        i++;
    }

    return tokens;
}

void removeAlias(char *alias) {
    aliasList *node = head;
    aliasList *prev = head;

    while (node != NULL) {
        if (strcmp(node->aliasEntry.name, alias) == 0) {
            // If node is head
            if (prev == node) {
                head = NULL;
            } else {
                prev->next = node->next;
            }

            // Free allocated memory
            free(node->aliasEntry.name);
            free(node->aliasEntry.tokens);
            free(node);

            numOfAliases--;
            break;
        }
        prev = node;
        node = node->next;
    }
}