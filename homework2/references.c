#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char *mypath_value = getenv("MYPATH");
    if (mypath_value != NULL) {
        printf("The value of MYPATH is: %s\n", mypath_value);
    } else {
        printf("MYPATH is not set.\n");
    }

    char *home_value = getenv("HOME");
    if (home_value != NULL) {
        printf("The value of HOME is: %s\n", home_value);
    } else {
        printf("HOME is not set.\n");
    }

    // char cwd[1024];
    long size = pathconf(".", _PC_PATH_MAX);
    char* cwd = calloc(size, sizeof(char));
    if (getcwd(cwd, size) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
        return 1;
    }

    return 0;
}
