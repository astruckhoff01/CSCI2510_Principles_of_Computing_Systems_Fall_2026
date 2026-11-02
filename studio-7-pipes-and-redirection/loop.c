// Anna Struckhoff
// November 1, 2026
// Program 2 echoes input lines until standard input reaches EOF.

#include <stdio.h>

#define BUFFER_SIZE 256

int main(void) {
    char line[BUFFER_SIZE];

    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        printf("Program 2 got: %s\n", line);
    }

    if (ferror(stdin)) {
        perror("Reading input");
        return 1;
    }
    return 0;
}
