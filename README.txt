A makefile is included to help with compiling. (Thanks to Connor Findlay I copied and
pasted his makefile from the forum and did some minor modifications)

To compile all the .c files, run the following command:
$ make

Part 1: Generalising the Game
	This part is done without problems.
	The program uses the function prototypes given on the assignment page.
	The program is checked by valgrind and there is no memory leaks.

Part 2: Experiment with UNIX Pipes
	This part is done without problems.
	I modified the FIFO into read and write mode so it allows the reader to
write "Hello" and writer to read "Hello".

Part 3: Use Unix Pipes to Create a Client-Server Solution
	This part is done without problems.
	The client inputs the size of the borad and all the human moves. All the
information(current board, winner of the game etc.) are printed out in the
client side. The client keeps making a human move until it's told by the
server that the game is over.
	The server checks if the client's move is valid, and tells the client. The
server also handles all the computer moves. The server checks if the game is
over every time a move has been made and tells the client.
