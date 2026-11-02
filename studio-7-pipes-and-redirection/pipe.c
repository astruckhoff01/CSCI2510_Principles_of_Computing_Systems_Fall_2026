// Anna Struckhoff
// November 1, 2026
// Connect ./print to ./loop with a pipe, then wait for both children.

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("Creating pipe");
        return 1;
    }

    pid_t writer = fork();
    if (writer == -1) {
        perror("Forking writer");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    if (writer == 0) {
        // The writer's stdout now sends bytes into the pipe.
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("Redirecting stdout");
            _exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = {"print", NULL};
        execvp("./print", args);
        perror("Executing print");
        _exit(127);
    }

    pid_t reader = fork();
    if (reader == -1) {
        perror("Forking reader");
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(writer, NULL, 0);
        return 1;
    }
    if (reader == 0) {
        // The reader's stdin receives the writer's output.
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("Redirecting stdin");
            _exit(1);
        }
        close(pipefd[1]);
        close(pipefd[0]);

        char *args[] = {"loop", NULL};
        execvp("./loop", args);
        perror("Executing loop");
        _exit(127);
    }

    // The parent must release both ends before waiting for EOF.
    close(pipefd[0]);
    close(pipefd[1]);

    int writer_status;
    int reader_status;
    pid_t waited_writer = waitpid(writer, &writer_status, 0);
    pid_t waited_reader = waitpid(reader, &reader_status, 0);
    if (waited_writer == -1 || waited_reader == -1) {
        perror("Waiting for children");
        return 1;
    }

    return !(WIFEXITED(writer_status) && WEXITSTATUS(writer_status) == 0 &&
             WIFEXITED(reader_status) && WEXITSTATUS(reader_status) == 0);
}
