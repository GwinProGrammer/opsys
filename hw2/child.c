/* fork.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char * argv[])
{
	pid_t rc;

  	setvbuf( stdout, NULL, _IONBF, 0 ); // Override all buffer (no buffering) for stdout.
	printf("Before the fork.\n");

	/* Create a new process (child). */
	rc = fork();
		
	if (rc == -1)
	{
		perror("Failed to fork ... ");
		return EXIT_FAILURE;
	}

	/*
	 * A copy of all the memory space is made and a child process 
	 * is launched as a duplicate of the existing process.
	 */

	if (rc != 0)
	{ // This is the parent VVVVVVVVVV the sleep helps randomize the output
		usleep(50);
		int status;
		printf("%d: I have an ungrateful child named %d.\n", getpid(), rc);
#if 0
			sleep(10);
#endif
#if 0
		pid_t child_pid = waitpid(rc, &status, 0);
#endif
		pid_t child_pid;
	        do
		{
			child_pid = waitpid(rc, &status, WNOHANG);
			if (child_pid == 0)
			{
				printf("Still waiting for the child %d\n", rc);
				sleep(1);
			}
		}  while (child_pid == 0);

		printf("%d: Child %d terminated. status 0x%x\n", getpid(), child_pid, status);

		//  WE WERE HERE ^^^^ 

    		if ( WIFSIGNALED( status ) )  /* child process was terminated   */
    		{                             /*  by a signal (e.g., seg fault) */
      			int exit_status = WTERMSIG( status );
      			printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
    		}
    		else if ( WIFEXITED( status ) )
    		{
      			int exit_status = WEXITSTATUS( status );
      			printf( "PARENT: ...normally with exit status %d\n", exit_status );
    		}
	}
	else
	{ // This is the child VVVVVVVVV; it takes longer to spin up
		printf("%d: I love my parent process %d.\n", getpid(), getppid());
#if 1
		sleep(10); // put in to see the defunct process
#endif
#if 0
		int *q = NULL; // Put in to cause segfault
		*q = 100;
#endif
		return 0xabcd; // Return values are limited to the lower
		               //  order 8 bits. When we return larger values
			       //    only the last 8 bits show. In this case, 
			       //      that is 0xcd.
	}

	/*
	 * Now that the child is exiting above, only the parent hits this line.
	 */
	printf("%d: After the fork ... rc: %d parent: %d\n", getpid(), rc, getppid());

	return EXIT_SUCCESS;
}

