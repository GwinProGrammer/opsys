#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <wait.h>

int main()
{

    /*
     * fd Table:
     *
     *  0: stdin
     *  1: stdout
     *  2: stderr
     */
    int pipefd[2];
    int rc = pipe(pipefd);

    if (rc == -1)
    {
	    perror("pipe error");
	    return EXIT_FAILURE;
    }

    /*
     * fd Table:
     *
     *  0: stdin
     *  1: stdout
     *  2: stderr                +--------+
     *  3: pipefd[0] <========== | buffer |  Buffer (transient) in memory that we can write to pipefd[1]
     *  4: pipefd[1] ==========> | buffer |  and read from. (pipefd[0])
     *                           +--------+
     */

    pid_t pid = fork();
    if (pid == -1)
    {
	    perror("fork error");
	    return EXIT_FAILURE;
    }

    /*
     * fd Table:
     *
     *  Parent:                                          Child
     *  0: stdin                                         0: stdin
     *  1: stdout                                        1: stdout
     *  2: stderr                +--------+              2: stderr
     *  3: pipefd[0] <========== | buffer | ==========>  3: pipefd[0]
     *  4: pipefd[1] ==========> | buffer | <==========  4: pipefd[1]
     *                           +--------+
     */

    if (pid == 0)
    {
    /* CHILD */
    /*
     * fd Table:
     *
     *  Parent:                                          Child
     *  0: stdin                                         0: stdin
     *  1: stdout                                        1: stdout
     *  2: stderr                +--------+              2: stderr
     *  3: pipefd[0] <========== | buffer |              3: pipefd[0]
     *  4: pipefd[1]             | buffer | <==========  4: pipefd[1]
     *                           +--------+
     */

        close(pipefd[0]);
	printf("CHILD: About to write to pipe.\n");
        int bw = write(pipefd[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26);
        if (bw == -1)
        {
	        perror("write error");
	        return EXIT_FAILURE;
        }
	printf("CHILD: Wrote %d bytes.\n", bw);

#if 1
	close(pipefd[1]); // What happens 
        while(1);
#endif
    }
    else
    {
        /* PARENT */
        close(pipefd[1]);
        char buffer[80];

        int br = read(pipefd[0], buffer, 10);
        if (br == -1)
        {
	        perror("read error");
	        return EXIT_FAILURE;
        }
        buffer[br] = '\0';
        printf("PARENT: Read \"%s\" from pipe\n", buffer);

        br = read(pipefd[0], buffer, 10);
        if (br == -1)
        {
	        perror("read error");
	        return EXIT_FAILURE;
        }
        buffer[br] = '\0';
        printf("PARENT: Read \"%s\" from pipe\n", buffer);

        br = read(pipefd[0], buffer, 10);
        if (br == -1)
        {
	        perror("read error");
	        return EXIT_FAILURE;
        }
        buffer[br] = '\0';
        printf("PARENT: Read \"%s\" from pipe\n", buffer);

        br = read(pipefd[0], buffer, 10);
        if (br == -1)
        {
	        perror("read error");
	        return EXIT_FAILURE;
        }
        buffer[br] = '\0';
        printf("PARENT: Read \"%s\" from pipe\n", buffer);

#if 0
	int status;
	waitpid(pid, &status, 0); // Explore behavior with wait here and with close of pipefd[1] in child
#endif 

    }

    return EXIT_SUCCESS;
}

