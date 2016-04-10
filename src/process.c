#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "process.h"

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
    fprintf(stderr, "%-8s Error while execvp. %s\n", "Child:", strerror(errno));
    exit(2);
}

void process(char * logfile, char * command, char multiplex) {
    fprintf(stderr, "process(\"%s\", \"%s\", %i);\n", logfile, command, multiplex); // DEBUG

    int pfd[2];
    if (-1 == pipe(pfd)) {
        fprintf(stderr, "%-8s Error while pipe. %s\n", "Process:", strerror(errno));
        exit(2);
    }

    pid_t pid = fork();
    if (0 == pid) { // child
        fprintf(stderr, "%-8s Started.\n", "Child:");
        //if (-1 == close(1)) {
        //    perror("Error while close stdout.");
        //    exit(2);
        //}
        //if (-1 == dup(pfd[1])) {
        //    perror("Error while dup.");
        //    exit(2);
        //}
        if (-1 == dup2(pfd[1], 1)) {
            perror("Error while dup2.");
            exit(2);
        }
        my_execute(command);
    } else if (pid > 0) { // parent
        fprintf(stderr, "%-8s Started.\n", "Parent:");
        int status;
        if (-1 == wait(&status)) {
            fprintf(stderr, "%-8s Failed to handle CHLD zombie. %s\n", "Parent:", strerror(errno));
            exit(2);
        }
        fprintf(stderr, "%-8s Finished.\n", "Parent:");
    } else { // (-1 == pid) error
        fprintf(stderr, "%-8s Pid is negative, after fork. %s\n", "Process:", strerror(errno));
        exit(2);
    }
}
