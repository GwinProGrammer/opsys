#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include<pthread.h>

int ***dead_end_boards;
int max_squares;
size_t dead_end_boards_size = 2;
int m = 3;
int n = 3;
int no_parallel = 1;

typedef struct {
    int **board;
    int knight_x;
    int knight_y;
    int moves_made;
} shared_data;

void print_board(int** board){
    for (int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            printf("%d",board[i][j]);
        }
        printf("\n");
    }
}

int generate_knight(int** board, int x, int y, int moves[8][2]){
    
    int possible_moves = 0;

    print_board(board);
    // printf("%d,%d\n",x,y);
    printf("\n");

    if (x - 1 >= 0 && y - 2 >= 0 && board[x-1][y-2] == 0){

        moves[possible_moves][0] = x-1;
        moves[possible_moves][1] = y-2;
        possible_moves++;
    }
    if (x - 2 >= 0 && y - 1 >= 0 && board[x-2][y-1] == 0){

        moves[possible_moves][0] = x-2;
        moves[possible_moves][1] = y-1;
        possible_moves++;
    }
    if (x - 2 >= 0 && y +1 < n && board[x-2][y+1] == 0){

        moves[possible_moves][0] = x-2;
        moves[possible_moves][1] = y+1;
        possible_moves++;
    }
    if (x - 1 >= 0 && y +2 < n && board[x-1][y+2] == 0){

        moves[possible_moves][0] = x-1;
        moves[possible_moves][1] = y+2;
        possible_moves++;
    }
    if (x + 1 < m && y + 2 < n && board[x+1][y+2] == 0){

        moves[possible_moves][0] = x+1;
        moves[possible_moves][1] = y+2;
        possible_moves++;
    }
    if (x +2 < m && y +1 < n && board[x+2][y+1] == 0){

        moves[possible_moves][0] = x+2;
        moves[possible_moves][1] = y+1;
        possible_moves++;
    }
    if (x +2 < m && y - 1 >= 0 && board[x+2][y-1] == 0){

        moves[possible_moves][0] = x+2;
        moves[possible_moves][1] = y-1;
        possible_moves++;
    }
    if (x + 1 < m && y - 2 >= 0 && board[x+1][y-2] == 0){

        moves[possible_moves][0] = x+1;
        moves[possible_moves][1] = y-2;
        possible_moves++;
    }
    
    return possible_moves;
}

int** copy_board(int** board){

    int** new_board = calloc(m,sizeof(int*));
    for (int i = 0; i < m; i++){
        int* new_row = calloc(n, sizeof(int));
        for(int j = 0; j < n; j++){
            new_row[j] = board[i][j];
        }
        new_board[i] = new_row;
    }
    return new_board;
}

void free_moves(int** moves,int num_moves){
    for (int i = 0; i < num_moves; i++){
        free(moves[i]);
    }
    free(moves);
}



void *tour(void *arg){
    shared_data *data = (shared_data *)arg;
    printf("tour %d\n", data->moves_made);
    
    while(1){
        int moves[8][2];
        int num_moves;

       num_moves = generate_knight(data->board,data->knight_x,data->knight_y,moves);

       if (num_moves == 1){
        int new_x = moves[0][0];
        int new_y = moves[0][1];
        data->board[new_x][new_y] = 1;
        data->knight_x = new_x;
        data->knight_y = new_y;
        data->moves_made += 1;
        // free_moves(moves,num_moves);
       }
       
       else if (num_moves > 1){

        printf("%d possible moves after move %d\n", num_moves,data->moves_made);
        printf("starting from (%d,%d)\n", data->knight_x,data->knight_y);
        //create new array to keep track of all the threads
        pthread_t threads[num_moves];
        for(int i = 0; i < num_moves; i++){
            int new_x = moves[i][0];
            int new_y = moves[i][1];
            printf("%d,%d\n",new_x,new_y);
            print_board(data->board);
            printf("\n");
        }

        for(int i = 0; i < num_moves; i++){

            int** new_board = copy_board(data->board);

            int new_x = moves[i][0];

            int new_y = moves[i][1];

            new_board[data->knight_x][data->knight_y] = 1;
            new_board[new_x][new_y] = 1;
            shared_data *new_data = calloc(1,sizeof(shared_data));
            new_data->board = new_board;
            new_data->knight_x = new_x;
            new_data->knight_y = new_y;
            new_data->moves_made = data->moves_made + 1;
            
            //create new thread
            pthread_create(&threads[i], NULL, tour, (void*)new_data);

            // if noparalllel, wait for it HERE
            if(no_parallel){
                pthread_join(threads[i], NULL);
                printf("Thread %d has finished\n", i + 1);
            }
        }

        if (!no_parallel){
            for (int i = 0; i < num_moves; i++) {
                pthread_join(threads[i], NULL);
                printf("Thread %d has finished\n", i + 1);
            }
        }
        // free_moves(moves,num_moves);
        unsigned int *y = malloc(sizeof(*y));
        *y = pthread_self();
        pthread_exit(y);
       }
       else{
        printf("dead end after move %d\n", data->moves_made);
        unsigned int *y = malloc(sizeof(*y));
        *y = pthread_self();
        pthread_exit(y);
       }
    }
    
    if (data->moves_made == m * n-1){
        //full board
    }
    else if (data->moves_made < m * n-1){
        //not full board
        //resize dead_end_boards
    }
    else{
        //wtf
    }

    //return num moves?
}

int main() {

    int** new_board = calloc(m,sizeof(int*));
    for (int i = 0; i < m; i++){
        int* new_row = calloc(n, sizeof(int));
        new_board[i] = new_row;
    }
    new_board[0][0] = 1;
    shared_data *new_data = calloc(1,sizeof(shared_data));
    new_data->board = new_board;
    new_data->knight_x = 0;
    new_data->knight_y = 0;
    new_data->moves_made=1;



    pthread_t thread;
    pthread_create(&thread, NULL, tour, (void*)new_data);
    


    pthread_join(thread, NULL);
    printf("Thread has finished\n");


    return(EXIT_SUCCESS);
}



/*main
 create array of every board that has ever been made    
 while (moves can still be made){

    make move
    if (more than one move can be made){
        
        create thread(s)
    }
    
    if (moves cannot be made) exit
 }
 if (moves_made == mxn){
    complete board found
 }
 else{
    check if dead end boards needs resizing
    add board to dead end boards
 }
 free memory
*/ 

/*

we need to do these things
- establish which moves are legal
- get the number of legal moves
- create a new board object for each legal move 
- send each board object into a new thread
- wait for all of these threads to finish

*/