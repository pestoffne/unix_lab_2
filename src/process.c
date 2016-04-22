#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// DEBUG
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    fprintf(stderr, "%10d Can not execute \"%s\". %s.\n", getpid(), *args, strerror(errno));
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
int pfd[2];
char buffer[READ_BUFFER_SIZE];

void handle_child(int signal, siginfo_t *siginfo, void *context) {
    chld_exit_code = signal;
    loop = 0;
}

void handle_write(int signal, siginfo_t *siginfo, void *context) {
    int count = read(pfd[0], buffer, READ_BUFFER_SIZE);
    if (-1 == count) {
        fprintf(stderr, "%10d Read error. %s\n", getpid(), strerror(errno));
        exit(2);
    } else {
        buffer[count] = 0;
        write_buffer(buffer);
    }
}

void process(char * logfile, char * command, char multiplex) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigset_t mask;
    sigfillset(&mask);
    sa.sa_mask = mask;
    sa.sa_sigaction = &handle_child;
    if (-1 == sigaction(SIGCHLD, &sa, NULL)) {
        fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
        exit(2);
    }

    sa.sa_sigaction = &handle_write;    
    if (-1 == sigaction(SIGUSR1, &sa, NULL)) {
        fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
        exit(2);
    }

    // handle SIFUSR2, SIGIO

    if (-1 == pipe(pfd)) {
        fprintf(stderr, "%10d Error while pipe. %s\n", getpid(), strerror(errno));
        exit(2);
    }

    loop = 1; // sets loop flag before fork, becose child can exit quickly.

    pid_t pid = fork();
    if (0 == pid) { // child
        if (-1 == dup2(pfd[1], 1)) {
            perror("Error while dup2.");
            exit(2);
        }
        my_execute(command);
    } else if (pid > 0) { // parent
        int flags = fcntl(pfd[0], F_GETFL, 0); // TODO handle error? see man
        if (fcntl(pfd[0], F_SETFL, flags | O_NONBLOCK)) {
            perror("fcntl");
            exit(2);
        }
        if (fcntl(pfd[0], F_SETSIG, SIGIO)) {
            perror("fcntl setsig");
            exit(2);
        }

        int status;
        if (-1 == wait(&status)) {
            fprintf(stderr, "%10d Failed to handle CHLD zombie. %s\n", getpid(), strerror(errno));
            exit(2);
        }
        fprintf(stderr, "%10d TERMINATED WITH EXIT CODE: %d\n", getpid(), chld_exit_code);
    } else { // (-1 == pid) error
        fprintf(stderr, "%10d Pid is negative, after fork. %s\n", getpid(), strerror(errno));
        exit(2);
    }
}
