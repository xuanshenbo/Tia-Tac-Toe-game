#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 1024

int main()
{
    int fd;  // file descriptor
    char *myfifo = "myfifo"; // declare the name of the pipe
    char buf[MAX_BUF]; // our buffer for reading up to MAX_BUF chars

    // open, read, and display the message from the FIFO
    do {
        sleep(5); // insert a wait so that the other process has time to create the pipe
        fd = open(myfifo, O_RDWR); // open read and write mode
    } while(fd == -1); // loop until we successfully open the pipe

    if (fd < 0)
        printf("open fifo error: %d\n", fd);

    read(fd, buf, MAX_BUF); // read from the pipe into buf
    printf("Received: %s\n", buf); // let us know what has been read

    write(fd, "Hello", sizeof("Hello")); // write "Hello" to the FIFO
    close(fd); // close the pipe

    unlink(myfifo); // remove the FIFO

    return 0;
}
