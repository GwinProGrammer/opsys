#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {

    long size = pathconf(".", _PC_PATH_MAX);
    char* cwd = calloc(size, sizeof(char));

    int cache_size = 1024;
    int token_size = 64;

    if (getcwd(cwd, size) == NULL) {
        perror("getcwd");
        return 1;
    } 


    char* env_value = getenv("MYPATH");

    

    int num_directories = 0;
    int e1 = 0;
    while(env_value[e1] != '.'){
        if (env_value[e1] == ':') {
            num_directories++;
        }
        e1++;
    }

    char **directories = calloc(num_directories,sizeof(char*));
    for (int i = 0; i < num_directories; i++){
        directories[i] = calloc(token_size,sizeof(char));
    }
    int e = 0;
    int direct = 0;
    int direct_count = 0;
    while (env_value[e] != '.'){
        
        if (env_value[e] == ':'){
            directories[direct][direct_count] = '\0';
            direct++;
            direct_count = 0;
        }
        else{
            directories[direct][direct_count] = env_value[e];
            direct_count++;
        }
        e++;
    }

    for (int i = 0; i < num_directories; i++){
        // printf("%s\n",directories[i]);
        DIR * dir = opendir( directories[i]);   
        if ( dir == NULL )
        {
            perror( "opendir() failed" );
            return EXIT_FAILURE;
        }
    }


    

 

    int done = 0;

    while (!done){

        printf("%s$ ", cwd);

        char* input = calloc(cache_size, sizeof(char));
        fgets(input, cache_size*sizeof(char), stdin);
        // printf("%d",strcmp(input, "exit\n"));
        if (strcmp(input, "exit\n") == 0) {
            done = 1;
            break;
        }

        int num_arguments = 1;
        for (int i = 0; i < cache_size; i++) {
            if (input[i] == ' ') {
                num_arguments++;
            }
        }
        printf("%d\n", num_arguments);

        char **arguments = calloc(num_arguments,sizeof(char*));

        for (int i = 0; i < num_arguments; i++){
            arguments[i] = calloc(token_size,sizeof(char));
        }

        // char *token = calloc(token_size,sizeof(char));
        int j = 0;

        int c = 0;
        int arg = 0;
        int arg_count = 0;



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




        struct dirent * file;
        int end = 0;
        printf("COMMAND: %s\n", arguments[0]);

        for(int d = 0; d < num_directories; d++){
            DIR* dir = opendir( directories[d]);
            printf("%s\n",directories[d]);
            while ( ( file = readdir( dir ) ) != NULL )
            {   
                struct stat buf;
                int rc = lstat(file->d_name, &buf);
                char* path = calloc(cache_size,sizeof(char)); 


                snprintf(path, cache_size*sizeof(char), "%s/%s", directories[d],file->d_name);

                // printf("file: %s\n",file->d_name);
                // printf("%d\n",rc);
                if (strcmp(file->d_name, arguments[0]) == 0){
                    printf("file: %s\n",file->d_name);
                    struct stat buf;
                    int rc = lstat(path, &buf);
                    printf("%d\n",rc);
                    if (rc != -1) {
                        if (buf.st_mode & S_IXUSR) {
                            printf("/bin/%s exists and is executable\n",file->d_name);
                        } else {
                            printf("/bin/%s exists but is not executable\n",file->d_name);
                        }
                        
                    } else {
                    perror("lstat() failed");
                    return EXIT_FAILURE;
                    }
                    end = 1;
                    break;
                } 
            }
            closedir(dir);
            if (end != 0) break;
        }
         

       
        for (int k = 0; k < num_arguments; k++){
            // printf("%s-", *(arguments+k));
            free(*(arguments+k));
        }
        free(arguments);

        
       

        
    }
    // free(input);
    printf("bye\n");
    
    
    
    return 0;
}

