#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "async.h"
#include "common.h"

static void handle_child(int signal, siginfo_t *siginfo, void *context) {
    chld_exit_code = signal;
    loop = 0;
}

static void handle_write(int signal, siginfo_t *siginfo, void *context) {
    
    read_avaible(pfd[1][0], 1, log_fd);
}

void async(char * logfile, char * command) {
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

    // creates handler of SIGUSR1
    sa.sa_sigaction = &handle_write;    
    if (-1 == sigaction(SIGUSR1, &sa, NULL)) {
        fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
        exit(2);
    }
    // creates handler of SIGUSR2
    sa.sa_sigaction = &handle_write;
    if (-1 == sigaction(SIGUSR2, &sa, NULL)) {
        fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
        exit(2);
    }
    // TODO handle SIGIO

    init_pipes();

    log_fd = my_file_open(logfile);
    loop = 1;

    pid_t pid = fork();
    if (0 == pid) { // child
        if (-1 == dup2(pfd[1][1], 1)) {
            perror("Error while dup2.");
            exit(2);
        }
        if (-1 == fcntl(pfd[1][0], F_SETOWN, getppid())) {
            perror("fcntl F_SETOWN ");
            exit(2);
        }
        if (fcntl(pfd[1][0], F_SETFL, O_ASYNC)) {
            perror("fcntl F_SETFL ");
            exit(2);
        }
        if (-1 == fcntl(pfd[1][0], F_SETSIG, SIGUSR1)) {
            perror("fcntl F_SETSIG ");
            exit(2);
        }



        my_execute(command);
    } else if (pid > 0) { // parent
        int flags = fcntl(pfd[1][0], F_GETFL, 0); // TODO handle error? see man
        if (fcntl(pfd[1][0], F_SETFL, flags | O_NONBLOCK)) {
            perror("fcntl");
            exit(2);
        }

        while (loop) {
            sleep(1);
        }

        fprintf(stderr, "%10d TERMINATED WITH EXIT CODE: %d\n", getpid(), chld_exit_code);
    } else { // (-1 == pid) error
        fprintf(stderr, "%10d Pid is negative, after fork. %s\n", getpid(), strerror(errno));
        exit(2);
    }
}
