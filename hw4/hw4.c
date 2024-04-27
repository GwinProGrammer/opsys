/* hw4.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 5
#define MAXBUFFER 8192

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 100

extern int total_guesses;
extern int total_wins;
extern int total_losses;
extern char ** words;


void* wordle(void *arg){
    int newsd = *(int*)arg;
    printf("thread successfully created\n");
    char buffer[MAXBUFFER + 1];
    
    while(1){
        int n = recv( newsd, buffer, MAXBUFFER, 0 );
        if ( n == -1 )
        {
            perror( "recv() failed" );
            pthread_exit(NULL);
        }
        else if ( n == 0 )
        {
            printf( "[%i]: Rcvd 0 from recv(); closing socket...\n", newsd );
        }
        else /* n > 0 */
        {
            buffer[n] = '\0';   /* assume this is text data */
            // inet_ntoa() takes an IP address and returns it as a string
            // for printing.
            printf( "Rcvd message: %s\n",buffer );

            /* send ACK message back to client */
            n = send( newsd, "ACK\n", 4, 0 );

            if ( n != 4 )
            {
                perror( "send() failed" );
                pthread_exit(NULL);
                // return EXIT_FAILURE;
            }
        }
    }
    close(newsd);
    // free(arg);
    pthread_exit(NULL);
}


int wordle_server(int argc, char **argv) {

    // do argument checking

    FILE *file = fopen("/Users/gwin/Documents/OpSys/hw4/wordle_words.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int num_lines = 0;
    
    //  struct hostent * hp = gethostbyname( "linux02.cs.rpi.edu" );

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("socket() failed: ");
        return EXIT_FAILURE;
    }
    printf("Socket: %d\n", sd);

    struct sockaddr_in server;
    socklen_t length = sizeof(server);

    server.sin_family = AF_INET;
    server.sin_port = htons(8192);
    server.sin_addr.s_addr = htonl( INADDR_ANY );

    if (bind(sd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        perror("bind() failed: ");
        return EXIT_FAILURE;
    }
    printf("Bound to port %d\n", ntohs(server.sin_port));

    printf("Before getsockname() port %d\n", ntohs(server.sin_port));
    if (getsockname(sd, (struct sockaddr *) &server, &length) < 0)
    {
        perror("getsockname() failed: ");
        return EXIT_FAILURE;
    }

    // int n;
    // char buffer[MAXBUFFER + 1];
    struct sockaddr_in client;
    printf("listening...\n");
    if (listen(sd, MAX_CLIENTS) < 0)
    {
        perror("listen() failed: ");
        return EXIT_FAILURE;
    }

    while(1){
        printf("hey\n");
        int newsd = accept(sd, (struct sockaddr *)&client, &length);
        if (newsd == -1) {
            perror("Accept failed");
            continue;
        }
        printf("Accept accepted\n");
        pthread_t thread_id;
        printf("%i\n", newsd);
        if (pthread_create(&thread_id, NULL, wordle, (void *)&newsd) != 0) {
            perror("Error creating thread");
            close(newsd);
            continue;
        }


    }
    
    return EXIT_SUCCESS;
}   