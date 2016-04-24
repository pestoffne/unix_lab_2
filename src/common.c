#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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
