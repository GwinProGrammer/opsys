#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    FILE *file;
    int character;

    char* filename = *(argv + 2);
    // Open the file for reading
    file = fopen(filename, "r");

    // Check if the file was opened successfully
    if (file != NULL) {
        // Allocate memory for the string
        char *s = malloc(100);
        if (s == NULL) {
            // Handle memory allocation failure
            fprintf(stderr, "Memory allocation failed.\n");
            return 1; // Return an error code
        }

        char *start = s;

        // Read characters from the file and null-terminate the string
        while ((character = fgetc(file)) != EOF && s - start < 99) {
            *s = (char)character;
            s++;
        }
        *s = '\0';  // Null-terminate the string

        // Close the file when done
        fclose(file);

        // Print the result
        printf("result: %s\n", start);

        // Free allocated memory
        free(start);
    } else {
        // Handle error opening the file
        fprintf(stderr, "Error opening the file.\n");
    }

    return 0;
}
