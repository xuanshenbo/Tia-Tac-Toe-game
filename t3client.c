#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tictactoe.h"

int init_game(int serverfd);
int check(int clientfd);
int player_move(int clientfd, int serverfd);
void print_game(int clientfd, int size);
void print_winner(int clientfd);

// implement a very simple client that sends and receives
// all other logic, just send and receive strings
// extension is to add extra clients
int main(void)
{
    // define our variables related to pipes
    int clientfd, serverfd;
    char *clientpipe = "clientpipe";
    char *serverpipe = "serverpipe";
    char buf[MAX_BUF];
    int size;

    // create the FIFO (named pipe) and open for reading
    mkfifo(clientpipe, 0666);
    mkfifo(serverpipe, 0666);
    serverfd = open(serverpipe, O_WRONLY);
    clientfd = open(clientpipe, O_RDONLY);

    printf("This is the game of Tic Tac Toe.\n");
    printf("You will be playing against the computer\n");

    read(clientfd, buf, MAX_BUF);
    printf("%s", buf);

    size = init_game(serverfd);

    int done;  // used to check validity of move
    int game_over; // used to check if game has completed

    // client must send commands and wait for responses
    // client exits loop if game is over
    // client should continue if receives continue message
    do {
        print_game(clientfd, size);
        do {
            done = player_move(clientfd, serverfd);
        } while (!done); // loop until valid move
        if (check(clientfd) != FALSE) break;
    } while (TRUE);

    print_game(clientfd, size);
    print_winner(clientfd);

    // tidy up
    close(clientfd);
    unlink(clientpipe);

    return 0;
}

/* Start the game by telling server size of the board */
int init_game(int serverfd)
{
    int size;
    scanf("%d", &size);
    // tell the server
    write(serverfd, &size, sizeof(size));
    return size;
}

/* Send player's move to the server and check if was valid. */
int player_move(int clientfd, int serverfd)
{
    int row, column;
    int valid;

    // ask the player
    printf("Enter coordinates (row,column) for your move: ");
    scanf("%d%*c%d", &row, &column);

    // send the details
    write(serverfd, &row, sizeof(row));
    write(serverfd, &column, sizeof(column));

    // check the result
    read(clientfd, &valid, sizeof(valid));
    if (!valid)
        printf("Invalid move, try again.\n");

    return valid;
}

/* Display the game on the screen. */
void print_game(int clientfd, int size)
{
    int i, j;
    char c;

    // read and print the board one character at a time
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            read(clientfd, &c, sizeof(c));
            printf("%c", c);
        }
        printf("\n");
    }
    printf("\n");
}

/* Check whether game has ended or whether computer is waiting
   for the next move */
int check(int clientfd)
{
    int game_over;
    // what does the server tell us? do we continue?
    read(clientfd, &game_over, sizeof(game_over));
    return game_over;
}

/* Receive the final results from the server */
void print_winner(int clientfd)
{
    int winner;
    read(clientfd, &winner, sizeof(winner));
    if (winner == HUMAN) printf("You won!\n");
    else if (winner == COMPUTER) printf("I won!!!!\n");
    else printf("Draw :(\n");
}

