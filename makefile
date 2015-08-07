#Simple Makefile to make the NWEN241 projects I am making.

#Compiler. This may need to change to g++ sometimes, but for 241 should probs stay
# as gcc
CC=gcc

#Compiler Flags.
# The ANSI C standard, aka C90, is just C89 with different name.
# This does pose a problem; the example files do NOT conform to the ANSI C standard;
# comments starting with '//' are not part of this standard; therefore you should run
# the script in uni/241/util/commentTr to fix this.
# The '$@' variable contatains the name of the rule calling the commands; it can be used in
# CFLAGS so that the output file is named after the rule calling it.
CFLAGS?= -o $@

#Make all the executables in the assignment
all: tictactoe reader writer server

#The tictactoe program itself. The variables $(CFLAGS) and $(CC) are defined above;
# they are what should change if they need to.
tictactoe: generic-tictactoe.c tictactoe.h
	$(CC) $(CFLAGS) generic-tictactoe.c

#The reader file
reader: reader.c
	$(CC) $(CFLAGS) reader.c

#The writer file
writer: writer.c
	$(CC) $(CFLAGS) writer.c

#The server version
server: t3client t3server

#The client code
t3client: t3client.c
	$(CC) $(CFLAGS) t3client.c

#The server code
t3server: t3server.c
	$(CC) $(CFLAGS) t3server.c

#Clean up the working directory
clean:
	rm t3client t3server reader writer tictactoe

# vim: set noexpandtab tabstop=4
