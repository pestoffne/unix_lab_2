#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "process.h"

#define READ_BUFFER_SIZE 256

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

void write_buffer(char* buffer) {
    char * pch = strtok(buffer, "\n");
    while (pch != NULL) {
        fprintf(stdout, "%10d >1 %s\n", getpid(), pch);
        pch = strtok(NULL, "\n");
    }
}

int chld_exit_code;
char loop;

void handle_child(int signal, siginfo_t *siginfo, void *context){
    chld_exit_code = signal;
    loop = 0;
}

void process(char * logfile, char * command, char multiplex) {
    fprintf(stderr, "process(\"%s\", \"%s\", %i);\n", logfile, command, multiplex); // DEBUG

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handle_child;
    if (-1 == sigaction(SIGCHLD, &sa, NULL)) { // TODO double signal handle
        fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
        exit(2);
    }

    int pfd[2];
    if (-1 == pipe(pfd)) {
        fprintf(stderr, "%10d Error while pipe. %s\n", getpid(), strerror(errno));
        exit(2);
    }

    pid_t pid = fork();
    if (0 == pid) { // child
        //fprintf(stderr, "%10d Child started.\n", getpid()); // DEBUG
        if (-1 == dup2(pfd[1], 1)) {
            perror("Error while dup2.");
            exit(2);
        }
        my_execute(command);
    } else if (pid > 0) { // parent
        //fprintf(stderr, "%10d Parent started.\n", getpid()); // DEBUG
        // prepare read loop
        int count = 0;
        char buffer[READ_BUFFER_SIZE];
        loop = 1;
        int flags = fcntl(pfd[0], F_GETFL, 0); // TODO handle error? see man
        if (fcntl(pfd[0], F_SETFL, flags | O_NONBLOCK)) {
            perror("fcntl");
            exit(2);
        }
        while (loop) {
            count = read(pfd[0], buffer, READ_BUFFER_SIZE);
            //fprintf(stderr, "count = %d\n", count); // DEBUG
            if (-1 == count) {
                if (EAGAIN != errno) {
                    perror("Read error. ");
                    exit(2);
                } // else there are no data in pipe
            } else {
                buffer[count] = 0;
                write_buffer(buffer);
                //fprintf(stderr, "%d >1 %s", getpid(), buffer);
            }
        }
        fprintf(stderr, "%10d TERMINATED WITH EXIT CODE: %d\n", getpid(), chld_exit_code);
        //fprintf(stderr, "%10d Parent finished.\n", getpid()); // DEBUG
    } else { // (-1 == pid) error
        fprintf(stderr, "%10d Pid is negative, after fork. %s\n", getpid(), strerror(errno));
        exit(2);
    }
}
