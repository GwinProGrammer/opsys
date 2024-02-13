#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    long size = pathconf(".", _PC_PATH_MAX);
    char* cwd = calloc(size, sizeof(char));

    if (getcwd(cwd, size) != NULL) {
        printf("%s$ ", cwd);
    } else {
        perror("getcwd");
        return 1;
    }


    int cache_size = 1024;
    int token_size = 64;

    char* input = calloc(cache_size, sizeof(char));
    fgets(input, cache_size*sizeof(char), stdin);

    int done = 0;

    while (!done){

        int num_arguments = 1;
        for (int i = 0; i < cache_size; i++) {
            if (input[i] == ' ') {
                num_arguments++;
            }
        }
        printf("s");
        printf("%d\n", num_arguments);
        printf("s1\n");
        char **arguments = calloc(num_arguments,sizeof(char*));
        printf("s2\n");
        for (int i = 0; i < num_arguments; i++){
            arguments[i] = calloc(token_size,sizeof(char));
        }

        // char *token = calloc(token_size,sizeof(char));
        int j = 0;

        int c = 0;
        int arg = 0;
        int arg_count = 0;

        printf("s3\n");

        while (input[c] != '\n'){
            if (input[c] == ' '){
                arguments[arg][arg_count] = '\0';
                arg++;
                arg_count = 0;
            }
            else{
                arguments[arg][arg_count] = input[c];
                arg_count++;
            }
            c++;
        }
        arguments[arg][arg_count] = '\0';

        printf("s4\n");


        // while (token != NULL) {
        //     *(arguments + j)  = calloc(sizeof(token)+1,sizeof(char));
        //     // printf("%s\n", token);
        //     for (int i = 0; i < sizeof(token)+1; i++){
        //     *(*(arguments + j) +i)= *(token + i);
        //         if (i == sizeof(token)) {
        //             *(*(arguments + j) +i) = '\0';
        //         }
        //     }  
        //     token = strtok(NULL, " ");
        //     j++;
        // }

        // printf("You entered: %s", input);
        for (int k = 0; k < num_arguments; k++){
            printf("%s-", *(arguments+k));
            // free(*(arguments+k));
        }
        // free(arguments);

        printf("\n");

        printf("%s$ ", cwd);
        // char* input = calloc(cache_size, sizeof(char));
        // free(input);
        char* input = calloc(cache_size, sizeof(char));
        fgets(input, cache_size*sizeof(char), stdin);
        // printf("%d",strcmp(input, "exit\n"));
        if (strcmp(input, "exit\n") == 0) {
            done = 1;
        }
    }
    free(input);
    
    
    
    return 0;
}

