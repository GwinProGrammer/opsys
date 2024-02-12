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
    char* input = calloc(cache_size, sizeof(char));
    fgets(input, cache_size*sizeof(char), stdin);
    
    int num_arguments = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == ' ') {
            num_arguments++;
        }

        //s
    }

    print("hi");

    char **arguments = calloc(num_arguments,sizeof(char*));

    char *token = strtok(input, " ");
    int j = 0;
    while (token != NULL) {
        *(arguments + j)  = calloc(sizeof(token)+1,sizeof(char));
        // printf("%s\n", token);
        for (int i = 0; i < sizeof(token)+1; i++){
        *(*(arguments + j) +i)= *(token + i);
            if (i == sizeof(token)) {
                *(*(arguments + j) +i) = '\0';
            }
        }  
        token = strtok(NULL, " ");
        j++;
    }



    

    

    printf("You entered: %s", input);
    for (int k = 0; k < num_arguments; k++){
        printf("%s\n", *(arguments+1));
    }
    return 0;
}

