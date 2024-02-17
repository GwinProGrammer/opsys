#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pipefd[2]; // File descriptors for the pipe
    pid_t pid_ps, pid_grep;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process for ps
    pid_ps = fork();
    if (pid_ps == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_ps == 0) { // Child process for ps
        printf("1 is running\n");
        // Close the read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // Close the write end of the pipe
        close(pipefd[1]);

        // Execute ps command
        
        char *args_ps[] = {"ps", "-ef", NULL};
        execve("/bin/ps", args_ps, NULL);
        
        // If execve fails, print an error message
        perror("execve ps");
        exit(EXIT_FAILURE);
    }

    // Fork another child process for grep
    pid_grep = fork();
    if (pid_grep == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_grep == 0) { // Child process for grep
        printf("2 is running\n");
        // Close the write end of the pipe
        close(pipefd[1]);

        // Redirect stdin to the read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);

        // Close the read end of the pipe
        close(pipefd[0]);
        

        // Execute grep command
        char *args_grep[] = {"grep", "goldsd", NULL};
        execve("/usr/bin/grep", args_grep, NULL);
        
        // If execve fails, print an error message
        perror("execve grep");
        exit(EXIT_FAILURE);
    }

    // Close the pipe in the parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both child processes to finish
    waitpid(pid_ps, NULL, 0);
    waitpid(pid_grep, NULL, 0);

    return 0;
}
