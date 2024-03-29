#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


int main() {

    setvbuf( stdout, NULL, _IONBF, 0 );

    int cache_size = 1024;
    int token_size = 64;




    char* env_value = getenv("MYPATH");

    

    int num_directories = 0;
    
    if (env_value != NULL){
        int e1 = 0;
        while(env_value[e1] != '.'){
            if (env_value[e1] == ':') {
                num_directories++;
            }
            e1++;
        }
    }
    else{
        num_directories = 1;
    }
    

    
    char **directories = calloc(num_directories+1,sizeof(char*));
    for (int i = 0; i < num_directories+1; i++){
        directories[i] = calloc(token_size,sizeof(char));
    }
    
    if (env_value != NULL){
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
    }
    else{
        strcpy(directories[0],"/bin");
    }
    

    

    int done = 0;

    while (!done){

        
        long size = pathconf(".", _PC_PATH_MAX);
        char* cwd = calloc(size, sizeof(char));

        if (getcwd(cwd, size) == NULL) {
            perror("getcwd");
            return EXIT_FAILURE;
        } 
        free(directories[num_directories]);
        directories[num_directories] = cwd;

        printf("%s$ ", cwd);

        char* input = calloc(cache_size, sizeof(char));
        fgets(input, cache_size*sizeof(char), stdin);
        // printf("%d",strcmp(input, "exit\n"));
        if (strcmp(input, "exit\n") == 0) {
            done = 1;
            free(input);
            break;
        }

        int num_arguments = 1;
        int p1num_arguments;
        int p2num_arguments  = 0;
        int ispipe = 0;
        int run_in_back = 0;

        for (int i = 0; i < cache_size; i++) {
            if (input[i] == ' ') {
                if (ispipe){
                    p2num_arguments++;
                }
                else{
                    num_arguments++;
                }
                
            }
            if (input[i] == '&'){
                input[i-1] = '\n';
                run_in_back = 1;
                if (ispipe){
                    p2num_arguments--;
                }
                else{
                    num_arguments--;
                }
                
            }
            if (input[i] == '|'){
                ispipe = 1; 
                p1num_arguments = num_arguments -1;
            }
        }
        
        // printf("%d\n", p1num_arguments);
        // printf("%d\n", p2num_arguments);

        char **arguments;
        char **arguments2;

        if (ispipe){ //pipe
            arguments = calloc(p1num_arguments+1,sizeof(char*));
            arguments2 = calloc(p2num_arguments+1,sizeof(char*));

            for (int i = 0; i < p1num_arguments; i++){
                arguments[i] = calloc(token_size,sizeof(char));
            }
            arguments[p1num_arguments] = NULL;
            for (int i = 0; i < p2num_arguments; i++){
                arguments2[i] = calloc(token_size,sizeof(char));
            }
            arguments2[p2num_arguments] = NULL;

            int c = 0;
            int arg = 0;
            int arg_count = 0;



            while (input[c] != '|'){
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

            c+=2;
            arg = 0;
            arg_count = 0;

            while (input[c] != '\n'){
                if (input[c] == ' '){
                    arguments2[arg][arg_count] = '\0';
                    arg++;
                    arg_count = 0;
                }
                else{
                    arguments2[arg][arg_count] = input[c];
                    arg_count++;
                }
                c++;
            }

            arguments2[arg][arg_count] = '\0';

            // struct dirent * file;
            // int end = 0;
            int p1is_executable = 0;
            int p1found = 0;
            // int is_cd = 0;

            char* path1 = calloc(cache_size,sizeof(char));
            char* path2 = calloc(cache_size,sizeof(char));
            

            //ARG1
            for(int d = 0; d < num_directories+1; d++){
                // DIR* dir = opendir( directories[d]);
                // printf("%s\n",directories[d]);


                path1 = realloc(path1, cache_size*sizeof(char)); 
                snprintf(path1, cache_size*sizeof(char), "%s/%s", directories[d],arguments[0]);
                struct stat buf;

                if (lstat(path1,&buf) != -1){
                    p1found = 1;
                    p1is_executable = ((buf.st_mode & S_IXUSR) != 0);
                    if (p1is_executable){
                        break;
                    }
                }

            }

            
            int p2is_executable = 0;
            int p2found = 0;
            //ARG2
            for(int d = 0; d < num_directories+1; d++){
                // DIR* dir = opendir( directories[d]);
                // printf("%s\n",directories[d]);


                path2 = realloc(path2, cache_size*sizeof(char)); 
                snprintf(path2, cache_size*sizeof(char), "%s/%s", directories[d],arguments2[0]);
                struct stat buf;

                if (lstat(path2,&buf) != -1){
                    p2found = 1;
                    p2is_executable = ((buf.st_mode & S_IXUSR) != 0);
                    if (p2is_executable){
                        break;
                    }
                }

            }
            
            if (!p1found || !p2found){
                printf("ERROR: command \"%s\" not found", arguments[0]);
                perror("Error:");
                return EXIT_FAILURE;
            }
            if (p1is_executable && p2is_executable && !run_in_back){

                int pipefd[2];
                pid_t pid1;
                pid_t pid2;
                int rc;

                

                rc = pipe(pipefd);
                if (rc == -1){
                    perror("failed to pipe");
                    return EXIT_FAILURE;
                }
                
                pid1 = fork();

                if (pid1 == -1){
                    perror("Failed to fork ... ");
                    return EXIT_FAILURE;
                }
                if (pid1 == 0){

                    // for(int i = 0; i < p1num_arguments; i++){
                    //     printf("%s ",arguments[i]);
                    // }
                    // printf("\n");
                    // printf("%s\n",path1);
                    
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                    
                    int result = execv(path1,arguments);
                    
                    printf("result 1: %d\n", result);
                    if (result == -1){
                        perror("Program 1 cannot run\n");
                        return EXIT_FAILURE;
                    }
                }
                
                pid2 = fork();

                if (pid2 == -1){
                    perror("Failed to fork ... ");
                    return EXIT_FAILURE;
                }
                if (pid2 == 0){
                    // printf("2 is running\n");

                    // for(int i = 0; i < p2num_arguments; i++){
                    //     printf("%s ",arguments2[i]);
                    // }
                    // printf("\n");
                    // printf("%s\n",path2);

                    close(pipefd[1]);
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);

                    int result = execv(path2,arguments2);
                    
                    printf("result 2: %d\n", result);
                    if (result == -1){
                        perror("Program 2 cannot run\n");
                        return EXIT_FAILURE;
                    }
                }
                
                close(pipefd[0]);
                close(pipefd[1]);
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
                
                // int status;
                // waitpid(rc, &status,0);   
                // // printf("%d: Child %d terminated. status 0x%x\n", getpid(), child_pid, status);   
                // if ( WIFSIGNALED( status ) )  /* child process was terminated   */
                // {                             /*  by a signal (e.g., seg fault) */
                //     int exit_status = WTERMSIG( status );
                //     // printf("[process %d terminated abnormally]\n",  child_pid);
                //     // printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
                // }
                // else if ( WIFEXITED( status ) )
                // {
                //     int exit_status = WEXITSTATUS( status );
                //     // printf("[process %d terminated with exit status %d]\n",  child_pid, exit_status);
                // }
                
                
                // if (pid1 !=0 && pid2 != 0){
                    
                // }
                // if (pid1 != 0 && pid2){
                    
                // }


                // printf("%d\n",rc);
            }
            

            if (p1is_executable && p2is_executable && run_in_back){
                int pipefd[2];
                pid_t pid1;
                pid_t pid2;
                int rc;

                printf("[running background process \"%s\"]\n", arguments[0]);
                printf("[running background process \"%s\"]\n", arguments2[0]);

                rc = pipe(pipefd);
                if (rc == -1){
                    perror("failed to pipe");
                    return EXIT_FAILURE;
                }

                pid1 = fork();

                if (pid1 == -1){
                    perror("Failed to fork ... ");
                    return EXIT_FAILURE;
                }
                if (pid1 == 0){

                    // for(int i = 0; i < p1num_arguments; i++){
                    //     printf("%s ",arguments[i]);
                    //     printf("%d",i);
                    // }
                    // printf("\n");
                    // printf("%s\n",path1);
                    
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);

                    int result = execv(path1,arguments);

                    printf("result 1: %d\n", result);
                    if (result == -1){
                        perror("Program 1 cannot run\n");
                        return EXIT_FAILURE;
                    }
                }

                pid2 = fork();

                if (pid2 == -1){
                    perror("Failed to fork ... ");
                    return EXIT_FAILURE;
                }
                if (pid2 == 0){
                    // printf("2 is running\n");

                    // for(int i = 0; i < p2num_arguments; i++){
                    //     printf("%s ",arguments2[i]);
                    //     printf("%d",i);
                    // }
                    // printf("\n");
                    // printf("%s\n",path2);

                    close(pipefd[1]);
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);

                    int result = execv(path2,arguments2);
                    
                    printf("result 2: %d\n", result);
                    if (result == -1){
                        perror("Program 2 cannot run\n");
                        return EXIT_FAILURE;
                    }
                }
                close(pipefd[0]);
                close(pipefd[1]);
                waitpid(pid1, NULL, WNOHANG);
                waitpid(pid2, NULL, WNOHANG);
                
                int status1;
                int status2;
                waitpid(pid1, &status1,0);   
                waitpid(pid1, &status2,0);   
                // printf("%d: Child %d terminated. status 0x%x\n", getpid(), child_pid, status);   
                if ( WIFSIGNALED( status1 ) )  /* child process was terminated   */
                {                             /*  by a signal (e.g., seg fault) */
                    // int exit_status = WTERMSIG( status1 );
                    printf("[process %d terminated abnormally]\n",  pid1);
                    // printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
                }
                else if ( WIFEXITED( status1 ) )
                {
                    int exit_status = WEXITSTATUS( status1 );
                    printf("[process %d terminated with exit status %d]\n",  pid1, exit_status);
                }
                if ( WIFSIGNALED( status2 ) )  /* child process was terminated   */
                {                             /*  by a signal (e.g., seg fault) */
                    // int exit_status = WTERMSIG( status2 );
                    printf("[process %d terminated abnormally]\n",  pid2);
                    // printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
                }
                else if ( WIFEXITED( status2 ) )
                {
                    int exit_status = WEXITSTATUS( status2 );
                    printf("[process %d terminated with exit status %d]\n",  pid2, exit_status);
                }
                
                // if (pid1 !=0 && pid2 != 0){
                    
                // }
                // if (pid1 != 0 && pid2){
                    
                // }


                // printf("%d\n",rc);
            }

            // if (is_cd){
            //     if (num_arguments == 1){
            //         char *home_dir = getenv("HOME");
            //         if (home_dir == NULL) {
            //             fprintf(stderr, "No Home\n");
            //             return EXIT_FAILURE;
            //         }
            //         if (chdir(home_dir) != 0) {
            //             perror("chdir");
            //             return EXIT_FAILURE;
            //         }
            //     }
            //         else{
            //         char* new_path = calloc(cache_size,sizeof(char)); 
            //         snprintf(new_path, cache_size*sizeof(char), "%s/%s", cwd,arguments[1]);
            //         int result = chdir(new_path);
            //     }
            // }
                
                
            
            free(path1);
            free(path2);

            
        }
        else{ //not pipe

            arguments = calloc(num_arguments+1,sizeof(char*));
            for (int i = 0; i < num_arguments; i++){
                arguments[i] = calloc(token_size,sizeof(char));
            }
            arguments[num_arguments] = NULL;

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

            // struct dirent * file;
            // int end = 0;
            int is_executable = 0;
            int found = 0;

            int is_cd = 0;
            if (strcmp("cd",arguments[0]) == 0){
                is_cd = 1;
                found = 1;
            } 
            
            // char* argpath;
            
            // char* looking_for_this_command = arguments[0];
            // printf("COMMAND: %s\n", arguments[0]);

            char *path = calloc(cache_size,sizeof(char));
            for(int d = 0; d < num_directories+1; d++){
                // DIR* dir = opendir( directories[d]);
                path = realloc(path, cache_size*sizeof(char)); 

                
                        
                snprintf(path, cache_size*sizeof(char), "%s/%s", directories[d],arguments[0]);

                struct stat buf;

                if (lstat(path,&buf) != -1){
                    found = 1;
                    is_executable = ((buf.st_mode & S_IXUSR) != 0);
                    if (is_executable){
                        break;
                    }
                }

            }
            if (!found){
                fprintf(stderr,"ERROR: command \"%s\" not found\n", arguments[0]);
            }

            if (is_executable && !run_in_back && !is_cd){
                pid_t rc;
                rc = fork();

                if (rc == -1){
                    perror("Failed to fork ... ");
                    return EXIT_FAILURE;
                }
                else if (rc != 0){
                    int status;
                    waitpid(rc, &status,0);   
                    // // printf("%d: Child %d terminated. status 0x%x\n", getpid(), child_pid, status);   
                    // if ( WIFSIGNALED( status ) )  /* child process was terminated   */
                    // {                             /*  by a signal (e.g., seg fault) */
                    //     int exit_status = WTERMSIG( status );
                    //     // printf("[process %d terminated abnormally]\n",  child_pid);
                    //     // printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
                    // }
                    // else if ( WIFEXITED( status ) )
                    // {
                    //     int exit_status = WEXITSTATUS( status );
                    //     // printf("[process %d terminated with exit status %d]\n",  child_pid, exit_status);
                    // }
                }
                else {
                    int result = execv(path,arguments);
                    if (result == -1){
                        perror("This program cannot run\n");
                        return EXIT_FAILURE;
                    }
                }
                
                // printf("%d\n",rc);
            }


            if (is_executable && run_in_back && !is_cd){

                pid_t rc;
                printf("[running background process \"%s\"]\n", arguments[0]);  
                rc = fork();
                
                if (rc == -1){
                    perror("Failed to fork ... ");
                    return EXIT_FAILURE;
                }
                else if (rc != 0){
                    int status;
                    waitpid(rc, &status,WNOHANG);
                    // printf("%d: Child %d terminated. status 0x%x\n", getpid(), child_pid, status);   
                    if ( WIFSIGNALED( status ) )  /* child process was terminated   */
                    {                             /*  by a signal (e.g., seg fault) */
                        // int exit_status = WTERMSIG( status );
                        printf("[process %d terminated abnormally]\n",  rc);
                        // printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
                    }
                    else if ( WIFEXITED( status ) )
                    {
                        int exit_status = WEXITSTATUS( status );
                        printf("[process %d terminated with exit status %d]\n",  rc, exit_status);
                    }
                }
                else {
                    
                    int result = execv(path,arguments);
                    if (result == -1){
                        perror("This program cannot run\n");
                        return EXIT_FAILURE;
                    }
                }
                
                // printf("%d\n",rc);
            }

            if (is_cd){

                
                if (num_arguments == 1){
                    char *home_dir = getenv("HOME");
                    if (home_dir == NULL) {
                        fprintf(stderr, "No Home\n");
                        return EXIT_FAILURE;
                    }
                    if (chdir(home_dir) != 0) {
                        perror("chdir");
                        return EXIT_FAILURE;
                    }
                }
                else if (strcmp(arguments[1],"/") == 0){
                    char* new_path = calloc(cache_size,sizeof(char)); 
                    snprintf(new_path, cache_size*sizeof(char), "%s",arguments[1]);
                    chdir(new_path);
                    free(new_path);
                }
                else{
                    int is_path = 0;
                    const char* arg = arguments[1];
                    while(*arg){
                        if (*arg == '/') is_path = 1;
                        arg++;
                    }
                    if (is_path){
                        chdir(arguments[1]);
                    }
                    else{
                        char* new_path = calloc(cache_size,sizeof(char)); 
                        snprintf(new_path, cache_size*sizeof(char), "%s/%s", cwd,arguments[1]);
                        chdir(new_path);
                        free(new_path);
                    }
                    
                }
            }
                
                
            free(path);
            

        
            // free(argpath);
        }
        
        if (ispipe){
            for (int i = 0; i < p1num_arguments; i++){
                free(arguments[i]);
            }
            arguments[p1num_arguments] = NULL;
            for (int i = 0; i < p2num_arguments; i++){
                free(arguments2[i]);
            }
            free(arguments);
            free(arguments2);

        }
        else{
            for (int i = 0; i < num_arguments+1; i++){
                free(arguments[i]);
            }
            free(arguments);
        }

        

        
       

        free(input);
    }
    

    for (int i = 0; i < num_directories+1; i++){
        free(directories[i]);
    }
    free(directories);
    printf("bye\n");
    
    
    
    return 0;
}

