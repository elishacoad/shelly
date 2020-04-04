// utility.c

#include "functions.h"

void colRed() {
    printf("\033[1;31m");
}

void colGreen() {
    printf("\033[1;32m");
}

void colYellow() {
    printf("\033[1;33m");
}

void colBlue() {
    printf("\033[1;34m");
}

void colMagenta() {
    printf("\033[1;35m");
}

void colCyan() {
    printf("\033[1;36m");
}

void colReset() {
    printf("\033[0m");
}

void clearLine() {
    printf("\33[2K\r");
}

void printNewline() {
    clearLine();
    colGreen();
    printf("?: ");
    colReset();
};

void printError(char *msg) {
    colRed();
    printf("ERROR: %s\n", msg);
    colReset();
}

int getChar() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
