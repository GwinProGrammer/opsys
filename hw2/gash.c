#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


int main() {

    

    int cache_size = 1024;
    int token_size = 64;

    


    char* env_value = getenv("MYPATH");

    

    int num_directories = 0;
    int e1 = 0;
    while(env_value[e1] != '.'){
        if (env_value[e1] == ':') {
            num_directories++;
        }
        e1++;
    }

    char **directories = calloc(num_directories+1,sizeof(char*));
    for (int i = 0; i < num_directories+1; i++){
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

 

    int done = 0;

    while (!done){

        
        long size = pathconf(".", _PC_PATH_MAX);
        char* cwd = calloc(size, sizeof(char));

        if (getcwd(cwd, size) == NULL) {
            perror("getcwd");
            return 1;
        } 
        free(directories[num_directories]);
        directories[num_directories] = cwd;

        printf("%s$ ", cwd);

        char* input = calloc(cache_size, sizeof(char));
        fgets(input, cache_size*sizeof(char), stdin);
        // printf("%d",strcmp(input, "exit\n"));
        if (strcmp(input, "exit\n") == 0) {
            done = 1;
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
                run_in_back = 1;
            }
            if (input[i] == '|'){
                ispipe = 1;
                p1num_arguments = num_arguments -1;
            }
        }

        printf("%d\n", p1num_arguments);
        printf("%d\n", p2num_arguments);

        char **arguments;
        char **arguments2;

        if (ispipe){ //pipe
            arguments = calloc(p1num_arguments,sizeof(char*));
            arguments2 = calloc(p2num_arguments,sizeof(char*));

            for (int i = 0; i < p1num_arguments; i++){
                arguments[i] = calloc(token_size,sizeof(char));
            }
            for (int i = 0; i < p2num_arguments; i++){
                arguments2[i] = calloc(token_size,sizeof(char));
            }

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
            
        }
        else{ //not pipe
            arguments = calloc(num_arguments,sizeof(char*));
            for (int i = 0; i < num_arguments; i++){
                arguments[i] = calloc(token_size,sizeof(char));
            }

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
        }
        

        

        struct dirent * file;
        int end = 0;
        int is_executable = 0;
        int found = 0;
        int is_cd = 0;
        
        char* looking_for_this_command = arguments[0];
        printf("COMMAND: %s\n", arguments[0]);


        for(int d = 0; d < num_directories+1; d++){
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
                if (strcmp(file->d_name, looking_for_this_command) == 0){
                    printf("file: %s\n",file->d_name);
                    struct stat buf;
                    int rc = lstat(path, &buf);
                    if (rc != -1) {
                        if (buf.st_mode & S_IXUSR) {
                            
                            found = 1;
                            if (strcmp("cd", looking_for_this_command) == 0){
                                is_cd = 1;
                            }
                            else{
                                is_executable = 1;
                            }
                            free(arguments[0]);
                            
                            arguments[0] = path;
                            
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
        if (!found){
            printf("ERROR: command \"%s\" not found", arguments[0]);
            perror("Error:");
        }

        if (is_executable){
            pid_t rc;
            rc = fork();

            if (rc == -1){
                perror("Failed to fork ... ");
		        return EXIT_FAILURE;
            }
            else if (rc != 0){
                int status;
                pid_t child_pid = waitpid(rc, &status,0);   
                // printf("%d: Child %d terminated. status 0x%x\n", getpid(), child_pid, status);   
                if ( WIFSIGNALED( status ) )  /* child process was terminated   */
                {                             /*  by a signal (e.g., seg fault) */
                    int exit_status = WTERMSIG( status );
                    // printf("[process %d terminated abnormally]\n",  child_pid);
                    // printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
                }
                else if ( WIFEXITED( status ) )
                {
                    int exit_status = WEXITSTATUS( status );
                    // printf("[process %d terminated with exit status %d]\n",  child_pid, exit_status);
                }
            }
            else {
                int result = execve(arguments[0],arguments,NULL);
                if (result == -1){
                    perror("This program cannot run\n");
                }
            }
            
            // printf("%d\n",rc);
        }

        if (run_in_back){
            pid_t rc;
            rc = fork();

            if (rc == -1){
                perror("Failed to fork ... ");
		        return EXIT_FAILURE;
            }
            else if (rc != 0){
                int status;
                pid_t child_pid = waitpid(rc, &status,WNOHANG);

                // // printf("%d: Child %d terminated. status 0x%x\n", getpid(), child_pid, status);   
                // if ( WIFSIGNALED( status ) )  /* child process was terminated   */
                // {                             /*  by a signal (e.g., seg fault) */
                //     int exit_status = WTERMSIG( status );
                //     printf("[process %d terminated abnormally]\n",  child_pid);
                //     // printf( "PARENT: ...abnormally (killed by a signal) %d %s\n", exit_status, strsignal(exit_status) );
                // }
                // else if ( WIFEXITED( status ) )
                // {
                //     int exit_status = WEXITSTATUS( status );
                //     printf("[process %d terminated with exit status %d]\n",  child_pid, exit_status);
                // }
            }
            else {
                int result = execve(arguments[0],arguments,NULL);
                if (result == -1){
                    perror("This program cannot run\n");
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
                else{
                char* new_path = calloc(cache_size,sizeof(char)); 
                snprintf(new_path, cache_size*sizeof(char), "%s/%s", cwd,arguments[1]);
                int result = chdir(new_path);
            }
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

