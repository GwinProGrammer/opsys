#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    int cache_size;
    char * filename;

    if (argc == 3){
        cache_size = atoi(*(argv + 1));

        if (cache_size < 1) {
            perror("ERROR: <Cache size is 0>\n");
            exit(EXIT_FAILURE);
        }     

        filename = *(argv + 2);
      

        char ** cache = calloc(cache_size, sizeof(char*));

        FILE *file;
        
        file = fopen(filename,"r");
        

        if (file != NULL){

            int character;
            char * word = calloc(128, sizeof(char));
            char * reader = word;
            int word_length = 0;
            int ascii_total = 0;
            while ((character = fgetc(file)) != EOF){

                if (((character >= '0' && character <= '9') || 
                      (character >= 'A' && character <= 'Z') ||
                      (character >= 'a' && character <= 'z'))){
                        
                    *reader = (char)character;
                    reader++;
                    word_length++;
                    ascii_total += character;
                }
                else if (word_length >= 3){
                    
                    *reader = '\0';

                    int hashvalue = ascii_total % cache_size;
                    if (*(cache + hashvalue) == NULL){
                        *(cache + hashvalue) = calloc(word_length + 1, sizeof(char));
                        printf("Word \"%s\" ==> %i (calloc)\n", word,hashvalue);
                        
                    }  
                    else{
                        *(cache + hashvalue) = realloc(*(cache + hashvalue), word_length + 1);
                        printf("Word \"%s\" ==> %i (realloc)\n", word,hashvalue);
                    }
                    for (int i = 0; i < word_length+1; i++){
                        *(*(cache + hashvalue) +i)= *(word + i);
                        if (i == word_length) {
                            *(*(cache + hashvalue) +i) = '\0';
                        }
                    }   
                    
                    word_length = 0;
                    reader = word;
                    ascii_total = 0;
                }   
                else{
                    word_length = 0;
                    reader = word;
                    ascii_total = 0;
                }

            }

            
            for(int i = 0; i < cache_size; i++){
                if (*(cache + i)!= NULL){
                    printf("Cache index %i ==> \"%s\"\n",i,*(cache + i));
                    free(*(cache + i));
                }
            }
            free(cache);
            free(word);
            
        }   
        else{
            perror("ERROR: <Can't open file>\n");
            exit(EXIT_FAILURE);
        }
        fclose(file);
        
    }
    else {
        perror("ERROR: <Invalid number of arguments>\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}