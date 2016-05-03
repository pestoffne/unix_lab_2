#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "common.h"

void my_execute(char * command) {
    char ** args = (char**)malloc(MAX_ARGS * sizeof(char*));
    char * pch = strtok(command, " ");
    args[0] = pch;
    int i = 0;
    while (pch != NULL) {
        pch = strtok(NULL, " ");
        args[++i] = pch;
    } // TODO checks index out of array
    execvp(*args, args);
    fprintf(stderr, "%10d Can not execute \"%s\". %s.\n", getpid(), *args, strerror(errno));
    exit(2);
}

void write_buffer(int fd_num, char* buffer) {
    static char* fd_name[3] = {">0", "<1", "<2"};
    char * pch = strtok(buffer, "\n");
    while (pch != NULL) {
        fprintf(stdout, "%10d %s %s\n", getpid(), fd_name[fd_num], pch);
        pch = strtok(NULL, "\n"); // TODO next line
    }
}

void write_noio() {
    char time_str[9];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(time_str, 9, "%H:%M:%S", &tm);
    fprintf(stdout, "%s, NOIO\n", time_str);
}

void read_avaible(int read_fd, int write_fd) {
    while (1) {
        int count = read(read_fd, buffer, READ_BUFFER_SIZE);
        if (-1 == count) {
            if (EAGAIN == errno) {
                fprintf(stderr, "break EAGAIN\n"); // DEBUG
                break;
            } else if (EINTR == errno) {
                // do nothing
                fprintf(stderr, "EINTR\n"); // DEBUG
            } else {
                perror("read. ");
                exit(2);
            }
        } else if (0 == count) {
            fprintf(stderr, "break 0 == count\n"); // DEBUG
            break;
        } else { // (count > 0)
            buffer[count] = 0;
            write_buffer(write_fd, buffer);
            // TODO add file logging
        }
    }
}
