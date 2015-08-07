#include <stdio.h>
#include <stdlib.h>
#include "tictactoe.h"

typedef struct TicTacToe
{
    int size; // this is the size of the game board
    int **board; // this is the game board
    int winner; // who won
} TicTacToe;

int check(TicTacToe *game);
void init_game(TicTacToe *game, int size);
void free_game(TicTacToe *game);
int player_move(TicTacToe *game);
void computer_move(TicTacToe *game);
void print_game(TicTacToe game);
char tokenstr(int token);
void print_result(TicTacToe game);

int main(void)
{
    TicTacToe game;
    int size;

    printf("This is the game of Tic Tac Toe.\n");
    printf("You will be playing against the computer.\n");

    printf("Enter the size of the board: ");
    scanf("%d", &size);
    printf("The game board is %d by %d.\n", size, size);

    init_game(&game, size); // initialise the board

    int done;
    do {
        print_game(game);
        do {
            done = player_move(&game);
        } while (!done); // loop until valid move
        if(check(&game) != FALSE) break; // was a winner or a draw
        computer_move(&game);
        if(check(&game) != FALSE) break; // was a winner or a draw
    } while (TRUE);

    print_result(game);
    print_game(game); // show final positions

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

    // use malloc to dynamically allocated 2D array
    game->board = (int **) malloc(sizeof(int *) * game->size);

    for (i = 0; i < game->size; i++)
        game->board[i] = (int *) malloc(sizeof(int) * game->size);

    // now initialise it
    for (i = 0; i < game->size; i++)
        for (j = 0; j < game->size; j++)
            // set to empty of tokens
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
int player_move(TicTacToe *game)
{
    int row, column;
    int valid;

    printf("Enter coordinates (row,column) for your move: ");
    scanf("%d%*c%d", &row, &column);

    if (row > game->size || column > game->size)
    {
        printf("Your move is outside the board, try again.\n");
        return FALSE;
    }

    row--; column--;

    if (game->board[row][column] != NONE)
    {
        printf("Invalid move, try again.\n");
        valid = FALSE;
    }
    else
    {
        game->board[row][column] = HUMAN;
        valid = TRUE;
    }
    return valid;
}

/* Get a move from the computer. */
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

/* Map the board token ID to a character. */
char tokenstr(int t)
{
    if (t == HUMAN)
        return 'X';
    else if (t == COMPUTER)
        return 'O';
    return '.';
}

/* Display the game board. */
void print_game(TicTacToe game)
{
    int i, j;

    // read and print the board one character at a time
    for (i = 0; i < game.size; i++)
    {
        for (j = 0; j < game.size; j++)
            printf("%c", tokenstr(game.board[i][j]));
        printf("\n");
    }
    printf("\n");
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
        count = 0;
        int row_leader = game->board[i][0];
        if (row_leader != NONE)
            for (j = 0; j < game->size; j++)
                if(game->board[i][j] == row_leader)
                    count++;

        if (count == game->size)
        {
            game->winner = row_leader;
            return TRUE;
        }
    }

    // check columns
    for (i = 0; i < game->size; i++)
    {
        count = 0;
        int column_leader = game->board[0][i];
        if (column_leader != NONE)
            for (j = 0; j < game->size; j++)
                if (game->board[j][i] == column_leader)
                    count++;

        if (count == game->size)
        {
            game->winner = column_leader;
            return TRUE;
        }
    }

    // test diagonals
    count = 0;
    for (i = 0; i < game->size; i++)
    {
        int left_diagonal_leader = game->board[0][0];
        if (left_diagonal_leader != NONE)
            for (j = 0; j < game->size; j++)
                if (i == j && game->board[i][j] == left_diagonal_leader)
                    count++;

        if (count == game->size)
        {
            game->winner = left_diagonal_leader;
            return TRUE;
        }
    }

    count = 0;
    for (i = 0; i < game->size; i++)
    {
        int right_diagonal_leader = game->board[0][game->size - 1];
        if (right_diagonal_leader != NONE)
            for (j = 0; j < game->size; j++)
                if (i + j == game->size - 1 &&
                    game->board[i][j] == right_diagonal_leader)
                    count++;

        if (count == game->size)
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

/* Print the result */
void print_result(TicTacToe game)
{
    if (game.winner == HUMAN) printf("You won!\n");
    else if (game.winner == COMPUTER) printf("I won!!!!\n");
    else printf("Draw :(\n");
}
