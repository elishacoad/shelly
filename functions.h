// functions.h

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

// Used for memory allocation
#define LINE_SIZE 50
#define TOKEN_SIZE 20

extern char history[21][50];
extern int historyIdx;

typedef struct alias alias;
typedef struct aliasList aliasList;

// Stores an alias with its value
struct alias {
    char *name;
    char **tokens;
};

// Linked list nodes for aliases
struct aliasList {
    alias aliasEntry;
    aliasList *next;
};

// ----------------------------------------
// CORE COMMANDS
// ----------------------------------------

// Reads and runs commands in file mshrc
void initSequence(char *line, char **tokens);

// Reads and executes user input in a loop
void loop(char *line, char **tokens);

// Reads user input
char *process();

// Converts a string into tokens
char **tokenize(char *line);

// Runs command if able
int run(char **tokens);

// ----------------------------------------
// EXECUTION OF COMMANDS
// ----------------------------------------

// Finds then calls correct execution command
int executeCmd(char **tokens);

// Executes a system command
int executeSysCmd(char **tokens);

// Determines if command is a custom command
bool isCustCmd(char *token);

// Executes a custom command
int executeCustCmd(char **tokens);

// Implements bash standard for piping
int pipeCommand(char **tokens1, char **tokens2);

// Implements bash standard for cd
// Currently does not support spaces
int changeDirectory(char **tokens);

// Implements bash standard for export PATH=$PATH:/path/here
int exportPath(char **tokens);

// Exits shell
int exitShell();

// ----------------------------------------
// ALIAS MANAGEMENT
// ----------------------------------------

// Creates a new alias
void addAlias(char **tokens);

// Lists all aliases
void listAliases();

// Determines if a token is an alias
bool isAlias(char *token);

// Gets the tokens of an alias
char **getAliasTokens(char *name);

// Scans tokens for aliases
// Returns tokens with aliases replaced with their children tokens
char **checkAliases(char **tokens);

// Removes alias from current aliases
void removeAlias(char *alias);

// ----------------------------------------
// TOKEN MANAGEMENT
// ----------------------------------------

// Determines if tokens have a special token
bool hasSpecialToken(char **tokens);

// Processes a command with special tokens
int manageSpecialToken(char **tokens);

// Inserts tokens into another token stream at a given index
char **insertTokens(char **tokens, char **insertTokens, int idx);

// Inserts a single token into another token stream at a given index
char **insertToken(char **tokens, char *insertToken, int idx);

// Prints tokens
void printTokens(char *tag, char **tokens);

// Removes a single token from another token stream at a given index
char **removeToken(char **tokens, char *token);

// ----------------------------------------
// HISTORY
// ----------------------------------------

// Enqueue a line to the history queue
void addToHistory(char *line);

// Prints last 20 commands line-by-line in "NUM : COMMAND" format
void printHistory();

// Executes last command in history
void executeLastCommand();

// Executes a command in history by its index number
void executeByNumber(int num);

// ----------------------------------------
// UTILITY
// ----------------------------------------

// Colors terminal output red
void colRed();

// Colors terminal output green
void colGreen();

// Colors terminal output yellow
void colYellow();

// Colors terminal output blue
void colBlue();

// Colors terminal output magenta
void colMagenta();

// Colors terminal output cyan
void colCyan();

// Resets terminal colors
void colReset();

// Clears current line and resets cursor
void clearLine();

// Clears line then prints "?: " in green
void printNewline();

// Prints an error message in red
void printError(char *msg);

// Mac and Linux implementation of getch()
// Gets input character on key press
int getChar();