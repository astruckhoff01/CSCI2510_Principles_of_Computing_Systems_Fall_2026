// Anna Struckhoff
// September 28, 2026
// Prints a message to the standard output using write.

#include <unistd.h>

int main(int argc, char *argv[]) {
    const char message[] = "Hello, world!\n";
    write(STDOUT_FILENO, message, sizeof(message) - 1);
    return 0;
}

