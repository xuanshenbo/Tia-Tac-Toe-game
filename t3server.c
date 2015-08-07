#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tictactoe.h"

typedef struct TicTacToe
{
    int size;      // this is the size of the game board
    int **board;   // this is the game board
    int winner;    // who won the game
} TicTacToe;

int check(TicTacToe *game);
void init_game(TicTacToe *game, int size);
void free_game(TicTacToe *game);
int get_player_move(int serverfd, int clientfd, TicTacToe *game);
void computer_move(TicTacToe *game);
void print_game(int clientfd, TicTacToe game);
char tokenstr(int token);
void print_result(int clientfd, TicTacToe game);
void client_continue(int clientfd);

int main(void)
{
    // define our variables related to pipes
    int clientfd, serverfd;
    char *clientpipe = "clientpipe";
    char *serverpipe = "serverpipe";
    char buf[MAX_BUF];
    int size;

    // create the FIFO (named pipe) and open for reading
    mkfifo(serverpipe, 0666);
    mkfifo(clientpipe, 0666);
    serverfd = open(serverpipe, O_RDONLY); // client talks to server
    clientfd = open(clientpipe, O_WRONLY); // server talks to client

    memset(buf, 0, MAX_BUF);
    memcpy(buf, "Enter the size of the board: ",
            sizeof("Enter the size of the board: "));

    write(clientfd, buf, sizeof(buf));

    printf("This is the game of Tic Tac Toe.\n");
    printf("You will be playing against the computer.\n");

    // wait for a client to tell us how large is the game board
    read(serverfd, &size, sizeof(size));
    printf("The game board is %d by %d.\n", size, size);

    // initialise the board
    TicTacToe game;
    init_game(&game, size);

    int done;
    do {
        print_game(clientfd, game);
        do {
            done = get_player_move(serverfd, clientfd, &game);
        } while (!done); // loop until valid move
        if (check(&game) != FALSE)
        {
            //write(clientfd, &MSG_GAME_OVER, sizeof(MSG_GAME_OVER));
            break; // there is a winner or draw
        }
        computer_move(&game);
        if (check(&game) != FALSE)
        {
            //write(clientfd, &MSG_GAME_OVER, sizeof(MSG_GAME_OVER));
            break; // there is a winner or draw
        }
        client_continue(clientfd);
    } while (TRUE);

    write(clientfd, &MSG_GAME_OVER, sizeof(MSG_GAME_OVER));
    print_game(clientfd, game); // show final positions
    print_result(clientfd, game);

    // clean up
    close(serverfd);
    unlink(serverpipe);
    free_game(&game);
    return 0;
}

/* Initialize the matrix. */
void init_game(TicTacToe *game, int size)
{
    int i, j;

    // set the size of the board
    game->size = size;

    // set winner to undecided
    game->winner = NONE;

    // use malloc to dynamically allocate 2D array
    game->board = (int **) malloc(sizeof(int *) * game->size);

    for (i = 0; i < game->size; i++)
        game->board[i] = (int *) malloc(sizeof(int) * game->size);

    // now initialise it
    for (i = 0; i < game->size; i++)
        for (j = 0; j < game->size; j++)
            // set to empty tokens
            game->board[i][j] = NONE;
}

/* Deallocate the dynamically allocated memory */
void free_game(TicTacToe *game)
{
    int i;
    for (i = 0; i < game->size; i++)
        free(game->board[i]);

    free(game->board);
}

/* Get a player's move. */
int get_player_move(int serverfd, int clientfd, TicTacToe *game)
{
    int row, column;
    int valid;

    // read move from client one int at a time
    read(serverfd, &row, sizeof(row));
    read(serverfd, &column, sizeof(column));

    // check if move if out of the board
    if (row > game->size || column > game->size)
    {
        valid = FALSE;
        write(clientfd, &valid, sizeof(valid));
        return valid;
    }

    row--; column--;

    // check invalid move
    if (game->board[row][column] != NONE)
        valid = FALSE;
    else
    {
        game->board[row][column] = HUMAN;
        valid = TRUE;
    }

    // return result to client
    write(clientfd, &valid, sizeof(valid));
    return valid;
}

/* Get a move from the computer. */
/* Return true (not 0) if can move */
/* Return false () if cannot move */
void computer_move(TicTacToe *game)
{
    int done = FALSE;
    int i, j, cx, cy;
    cx = cy = -1;
    for (i = 0; i < game->size; i++)
    {
        for (j = 0; j < game->size; j++)
            if (game->board[i][j] == NONE)
            {
                cx = i; cy = j;
                break;
            }
        if (cx != -1)
        {
            game->board[cx][cy] = COMPUTER;
            break;
        }
    }
}

/* Map the board token ID into a character */
char tokenstr(int t)
{
    if(t==HUMAN)
        return 'X';
    else if (t==COMPUTER)
        return 'O';
    return '.';
}

/* Send the game result to the client */
/* Do it a character at a time -- easy! */
void print_game(int clientfd, TicTacToe game)
{
    int i, j;
    char c;

    // write out the board one character at a time
    for (i = 0; i < game.size; i++)
    {
        for (j = 0; j < game.size; j++)
        {
            c = tokenstr(game.board[i][j]);
            write(clientfd, &c, sizeof(c));
        }
    }
}

/* See if there is a winner. */
/* return true (0) if so otherwise false */
int check(TicTacToe *game)
{
    int i, j;
    int count;

    // check rows
    for (i = 0; i < game->size; i++)
    {
        int row_count = 0;
        int row_leader = game->board[i][0];
        if (row_leader != NONE)
            for (j = 0; j < game->size; j++)
                if(game->board[i][j] == row_leader)
                    row_count++;

        if (row_count == game->size)
        {
            game->winner = row_leader;
            return TRUE;
        }
    }

    // check columns
    for (i = 0; i < game->size; i++)
    {
        int column_count = 0;
        int column_leader = game->board[0][i];
        if (column_leader != NONE)
            for (j = 0; j < game->size; j++)
                if (game->board[j][i] == column_leader)
                    column_count++;

        if (column_count == game->size)
        {
            game->winner = column_leader;
            return TRUE;
        }
    }

    // test diagonals
    int left_diagonal_count = 0;
    for (i = 0; i < game->size; i++)
    {
        int left_diagonal_leader = game->board[0][0];
        if (left_diagonal_leader != NONE)
            for (j = 0; j < game->size; j++)
                if (i == j && game->board[i][j] == left_diagonal_leader)
                    left_diagonal_count++;

        if (left_diagonal_count == game->size)
        {
            game->winner = left_diagonal_leader;
            return TRUE;
        }
    }

    int right_diagonal_count = 0;
    for (i = 0; i < game->size; i++)
    {
        int right_diagonal_leader = game->board[0][game->size - 1];
        if (right_diagonal_leader != NONE)
            for (j = 0; j < game->size; j++)
                if (i + j == game->size - 1 &&
                    game->board[i][j] == right_diagonal_leader)
                    right_diagonal_count++;

        if (right_diagonal_count == game->size)
        {
            game->winner = right_diagonal_leader;
            return TRUE;
        }
    }

    // test if out of space on the board
    count = 0;
    for (i = 0; i < game->size; i++)
        for (j = 0; j < game->size; j++)
            if (game->board[i][j] == NONE) count++;

    if (count == 0)
    {
        game->winner = DRAW;
        return TRUE;
    }

    // no-one and nor was there a draw
    return FALSE;
}

/* Tell the client that game has ended and the result of the game */
void print_result(int clientfd, TicTacToe game)
{
    // send game over
    //write(clientfd, &MSG_GAME_OVER, sizeof(MSG_GAME_OVER));
    // tell the client the winner
    write(clientfd, &game.winner, sizeof(game.winner));
}

/* Tell the client to continue */
void client_continue(int clientfd)
{
    // tell the client that it should continue playing
    write(clientfd, &MSG_CONTINUE, sizeof(MSG_CONTINUE));
}
