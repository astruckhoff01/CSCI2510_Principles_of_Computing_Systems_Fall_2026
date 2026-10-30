// Anna Struckhoff
// October 27, 2026
// Optional exercise: execute the command supplied after this program's name.

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [arguments...]\n", argv[0]);
        return 1;
    }

    pid_t child = fork();
    if (child < 0) {
        perror("fork");
        return 1;
    }
    if (child == 0) {
        // Skip our executable name; the remaining array already ends in NULL.
        execvp(argv[1], argv + 1);
        perror("execvp");
        _exit(127);
    }

    int status;
    while (waitpid(child, &status, 0) < 0) {
        if (errno != EINTR) {
            perror("waitpid");
            return 1;
        }
    }
    printf("Parent PID %ld: child completed\n", (long)getpid());

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return 1;
}
