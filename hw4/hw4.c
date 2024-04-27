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

extern int total_guesses;
extern int total_wins;
extern int total_losses;
extern char ** words;





int wordle_server(int argc, char **argv) {
    
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
    server.sin_port = htons(8123);
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
    if (listen(sd, MAX_CLIENTS) < 0)
    {
        perror("listen() failed: ");
        return EXIT_FAILURE;
    }
    while(1){
        int newsd = accept(sd, (struct sockaddr *)&client, &length);
        if (newsd == -1) {
            perror("Accept failed");
            continue;
        }
        printf("Accept accepted");

        // if (pthread_create(&thread_id, NULL, handle_client, (void *)&newsd) != 0) {
        //     perror("Error creating thread");
        //     close(client_socket);
        //     continue;
        // }


    }
    
    return EXIT_SUCCESS;
}   