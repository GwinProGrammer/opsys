/* hw4.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

#define MAX_CLIENTS 5
#define MAXBUFFER 8192

#define MAX_LINES 6000
#define MAX_LINE_LENGTH 100

extern int total_guesses;
extern int total_wins;
extern int total_losses;
extern char ** words;

char lines[MAX_LINES][MAX_LINE_LENGTH];
int num_lines = 0;

int sd;

volatile sig_atomic_t end = 0;

void sigusr1_handler(int signum) {
    printf("damn signal recieved bro\n");
    close(sd);
    end = 1;
}

void toLowercase(char *str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

int is_valid(char* word){

    for(int i = 0; i < num_lines; i++){
        int r = strcmp(lines[i],word);
     
        if (r == 0){
            return 1;
        }
    }
    return 0;
}

char* calculate_string(char* input, char* correct){
    char* in_ptr = input;
    char* cor_ptr = correct;
    char* result = (char *)calloc(7, sizeof(char));
    char* str = result;
    int perfect = 0;
    int stop = 0;
    while(*in_ptr != '\0'){
        if (tolower(*in_ptr) == tolower(*cor_ptr)){
            *str = toupper(*in_ptr);
            perfect++;
        }
        else {
            int elsewhere = 0;
            for(char* ptr = correct; *ptr != '\0'; ptr++){
                if (tolower(*in_ptr) == tolower(*ptr)){
                    *str = tolower(*in_ptr);
                    elsewhere = 1;
                    break;
                }
            }
            if (!elsewhere){
                *str = '-';
            }
        }
        in_ptr++;
        cor_ptr++;
        str++;
        stop++;
        if (stop > 10){
            break;
        }
    }
    *str = '\0';
    if (perfect == 5){
        *str = '1';
    }
    str++;
    *str = '\0';
    return result;
}

void* wordle(void *arg){

    int num_guesses = 6;
    int newsd = *(int*)arg;
    printf("thread successfully created\n");
    char *buffer = (char *)calloc(6,sizeof(char));

    while(num_guesses > 0){
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
            buffer[n] = '\0';   

            printf( "Rcvd message: %s\n",buffer );


            toLowercase(buffer);

            int valid = is_valid(buffer);

            char *send_buffer = (char *)calloc(8,sizeof(char));

            if (!valid){
                printf("%s is not valid", buffer);
                *send_buffer = 'N';
                *(short*)(send_buffer + 1) = htons(num_guesses);
                strncpy(send_buffer + 4, "?????\0", 5);
                n = send( newsd, send_buffer, 8, 0 );
                
            }
            else{
                printf( "Word is valid\n" );
                num_guesses--;
                char* result = calculate_string(buffer,"rapid");
                if (result[5] == '1'){
                    result[5] = '\0';
                    num_guesses = 0;
                    printf("You got it!\n");
                }
                *send_buffer = 'Y';
                printf("guesses: %i\n", num_guesses);
                *(short*)(send_buffer + 1) = htons((short)num_guesses);
                strncpy(send_buffer + 4, result, 5);
                unsigned short combined_short = (send_buffer[1] << 8) | send_buffer[2];
                printf("sending %c%hd%c%c%c%c%c%c\n", send_buffer[0],combined_short,send_buffer[3],send_buffer[4],send_buffer[5],send_buffer[6],send_buffer[7],send_buffer[8]);
                n = send( newsd, send_buffer, 8, 0 );
                
            }



            printf("%i bytes sent\n", n);
            // if ( n != 5 )
            // {
            //     perror( "send() failed" );
            //     pthread_exit(NULL);
            //     // return EXIT_FAILURE;
            // }
        }
    }
    close(newsd);
    // free(arg);
    pthread_exit(NULL);
}


int wordle_server(int argc, char **argv) {

    signal(SIGUSR1, sigusr1_handler);

    // do argument checking

    FILE *file = fopen("/Users/gwin/Documents/OpSys/hw4/wordle_words.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    

    while (fgets(lines[num_lines], MAX_LINE_LENGTH, file) != NULL) {
        // Remove newline character if present
        lines[num_lines][strcspn(lines[num_lines], "\n")] = '\0';
        num_lines++;

        // Check if maximum number of lines has been reached
        if (num_lines >= MAX_LINES) {
            printf("Maximum number of lines reached. Exiting.\n");
            break;
        }
    }

    fclose(file);

    printf("Contents of the file:\n");
    for (int i = 0; i < num_lines; i++) {
        printf("%s\n", lines[i]);
    }
    
    //  struct hostent * hp = gethostbyname( "linux02.cs.rpi.edu" );

    sd = socket(AF_INET, SOCK_STREAM, 0);
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

    while(!end){
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
        printf("go\n");

    }
    
    return EXIT_SUCCESS;
}   