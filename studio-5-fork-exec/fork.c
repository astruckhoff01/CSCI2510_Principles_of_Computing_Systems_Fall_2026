// Anna Struckhoff
// October 27, 2026
// Run ls -l in a child; print the parent's PID after the child finishes.

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    char *child_argv[] = {"ls", "-l", NULL};
    char *child_prog = child_argv[0];
    pid_t child_PID = fork();

    switch (child_PID) {
    case -1:
        perror("fork");
        return 1;
    case 0:
        sleep(3);
        execvp(child_prog, child_argv);
        perror("execvp");
        _exit(127);
    default:
        // A signal may interrupt the wait without ending the child.
        while (waitpid(child_PID, NULL, 0) == -1) {
            if (errno != EINTR) {
                perror("waitpid");
                return 1;
            }
        }
        printf("Parent finished waiting; PID = %ld\n", (long)getpid());
        return 0;
    }
}
