#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include<pthread.h>

#define NO_PARALLEL 0

int ***dead_end_boards = NULL;
int max_squares;
size_t dead_end_boards_size = 0;
int num_dead_boards = 0;
int m;
int n;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int **board;
    int knight_x;
    int knight_y;
    int moves_made;
} shared_data;

void print_board(int** board){
    for (int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            printf("%d",board[i][j]);
        }
        printf("\n");
    }
}



int generate_knight(int** board, int x, int y, int moves[8][2]){
    
    int possible_moves = 0;

    // print_board(board);

    // printf("%d,%d\n",x,y);
    // printf("\n");

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

    pthread_t tidd = pthread_self();
    unsigned long tid = (unsigned long)(void*)tidd;
    if (data->moves_made == 1){
        printf("THREAD %lu: Solving Sonny's knight's tour problem for a %dx%d board\n",tid,m,n);
    }
    
    
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
       // MULTIPLE MOVES
       else if (num_moves > 1){

        printf("THREAD %lu: %d moves possible after move #%d; creating threads...\n", tid, num_moves,data->moves_made);

        pthread_t threads[num_moves];
   
        int return_values[num_moves];

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
            #if NO_PARALLEL
                void* result;
                pthread_join(threads[i], &result);
                return_values[i] = (int)result;
            #endif
        }

        //if parallel, wait for it HERE
        #if !NO_PARALLEL
            for (int i = 0; i < num_moves; i++) {
                void* result;
                pthread_join(threads[i], &result);
                return_values[i] = (int)result;
            }
        #endif
        int total = 0;
        for (int i = 0; i < num_moves; i++) {
            total += return_values[i];
            printf("THREAD %lu: Thread [%lu] joined (returned %d)\n", tid, (unsigned long)(void*)threads[i], return_values[i]);
        }

        if(data->moves_made == 1){
            break;
        }
        int* total_ptr = total;
        pthread_exit((void*)(total_ptr));
       }
        // NO MORE MOVES
       else{ 
        if (data->moves_made == m * n){
            printf("THREAD %lu: Sonny found a full knight's tour!\n",tid);
            pthread_mutex_lock(&mutex);
            max_squares = m*n;
            pthread_mutex_unlock(&mutex);
        }
        else if (data->moves_made < m * n){
            printf("THREAD %lu: Dead end after move #%d\n",tid,data->moves_made);
            pthread_mutex_lock(&mutex);
            num_dead_boards++;
            if (num_dead_boards > dead_end_boards_size){
                if (dead_end_boards_size == 0) {dead_end_boards_size = 1;}
                else{dead_end_boards_size *= 2;}
                
                int*** new_dead = realloc(dead_end_boards,dead_end_boards_size*sizeof(int**));
                dead_end_boards = new_dead;
                
                
            }
            dead_end_boards[num_dead_boards-1] = data->board;
            if (data->moves_made > max_squares){
                max_squares = data->moves_made;
            }
            pthread_mutex_unlock(&mutex);
        }
        else{
            perror("wtf\n");
        }
        if(data->moves_made == 1){
            break;
        }
        pthread_exit((void*)(data->moves_made));
       }
    }
    printf("THREAD %lu: Best solution(s) found visit %d squares (out of %d)\n", tid,max_squares, m*n);
    for(int k = 0; k < num_dead_boards; k++){

        for (int i = 0; i < m; i++){
            if (i == 0){
                printf("THREAD %lu: > ", tid);
            }
            else{
                printf("THREAD %lu:   ", tid);
            }
            
            for(int j = 0; j < n; j++){
                if (dead_end_boards[k][i][j] == 0){
                    printf(".");
                }
                else{
                    printf("S");
                }
            }
            printf("\n");
        }
    }
    pthread_exit((void*)(data->moves_made));

}

int main(int argc, char *argv[]) {

    m = atoi(argv[1]);
    n = atoi(argv[2]);


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
    // pthread_t tid = pthread_self();
    // printf("%lu\n", tid);

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