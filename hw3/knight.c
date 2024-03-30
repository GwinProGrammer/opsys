#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include<pthread.h>
int main()
{
    int x = -2;
    int x_dir = 1;
    while(1){
        if (x >= 2 || x_dir == 0){
            x_dir = 0;
            x--;
        }
        if (x < -2 || x_dir == 1){
            x_dir = 1;
            x++;
        }
        printf("%i\n", x);

        // while(1){

        // }
    }
    return 0;
}
