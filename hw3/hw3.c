#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include<pthread.h>

int ***dead_end_boards;
int max_squares;
size_t dead_end_boards_size = 2;
int m = 3;
int n = 3;

typedef struct {
    int **board;
    int knight_x;
    int knight_y;
    int moves_made;
} shared_data;

int generate_knight(int** board, int x, int y, int** moves){
    
    int possible_moves = 0;
    int** moves = calloc(8,sizeof(int*));

    if (x - 1 > 0 && y - 2 > 0 && board[x-1][y-2] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x-1;
        moves[possible_moves][1] = y-2;
        possible_moves++;
    }
    if (x - 2 > 0 && y - 1 > 0 && board[x-2][y-1] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x-2;
        moves[possible_moves][1] = y-1;
        possible_moves++;
    }
    if (x - 2 > 0 && y +1 < n && board[x-2][y+1] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x-2;
        moves[possible_moves][1] = y+1;
        possible_moves++;
    }
    if (x - 1 > 0 && y +1 < n && board[x-1][y+2] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x-1;
        moves[possible_moves][1] = y+2;
        possible_moves++;
    }
    if (x + 1 < m && y + 2 < n && board[x+1][y+2] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x+1;
        moves[possible_moves][1] = y+2;
        possible_moves++;
    }
    if (x +2 < m && y +1 < n && board[x+2][y+1] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x+2;
        moves[possible_moves][1] = y+1;
        possible_moves++;
    }
    if (x +2 < m && y - 1 > 0 && board[x+2][y-1] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x+2;
        moves[possible_moves][1] = y-1;
        possible_moves++;
    }
    if (x + 1 < m && y - 2 > 0 && board[x+1][y-2] == 0){
        moves[possible_moves] = calloc(2,sizeof(int));
        moves[possible_moves][0] = x+1;
        moves[possible_moves][1] = y-2;
        possible_moves++;
    }
    return possible_moves;
}

int** copy_board(int** board){
    int 

}

void *tour(void *arg){
    shared_data *data = (shared_data *)arg;
    

    while(1){
        int moves_made_now = 0;

        //move across by 1, up/down by 2
        for (int i = -1; i < 2; i++){
            for (int j = -2; j < 3; j += 2){
                int x_ = data->knight_x + i;
                int y_ = data->knight_y + j;
                if (x_ < 0 || y_ < 0 || x_ >= m || y_ <= n){
                    //invalid move
                    continue;
                }
                else{
                    if (data->board[x_][y_] != 1){
                        if (moves_made_now > 0){
                        //change the board state of the CURRENT thread
                        data->moves_made++;
                        }
                        else{
                            //create a new array with the board state changed
                            //create a new thread and pass the new array to the thread
                        }
                        moves_made_now++;
                    }
                    
                }
            }
        }
        //move across by 2, up/down by 1
        for (int i = -2; i < 3; i += 2){
            for (int j = -1; j < 2; j ++){
                int x_ = data->knight_x + i;
                int y_ = data->knight_y + j;
                if (x_ < 0 || y_ < 0 || x_ >= m || y_ <= n){
                    //invalid move
                    continue;
                }
                else{
                    if (data->board[x_][y_] != 1){
                        if (moves_made_now > 0){
                            //change the board state of the CURRENT thread
                            data->moves_made++;
                        }
                        else{
                            //create a new array with the board state changed
                            //create a new thread and pass the new array to the thread
                        }
                        moves_made_now++;
                    }
                }
            }
        }
        if (moves_made_now == 0){
            break;
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