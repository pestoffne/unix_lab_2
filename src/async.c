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

int chld_exit_code;
int pfd[2];

static void handle_child(int signal, siginfo_t *siginfo, void *context) {
    fprintf(stderr, "SIGCHLD\n"); // DEBUG
    chld_exit_code = signal;
}

static void handle_write(int signal, siginfo_t *siginfo, void *context) {
    fprintf(stderr, "write handler. %d\n", signal); // DEBUG
    read_avaible(pfd[0], 1);
}

void async(char * logfile, char * command) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigset_t mask;
    sigfillset(&mask);
    sa.sa_mask = mask;
    //sa.sa_sigaction = &handle_child;
    //if (-1 == sigaction(SIGCHLD, &sa, NULL)) {
    //    fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
    //    exit(2);
    //}

    // creates handler of SIGUSR1
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

    pid_t pid = fork();
    if (0 == pid) { // child
        if (-1 == dup2(pfd[1], 1)) {
            perror("Error while dup2.");
            exit(2);
        }
        //sleep(1); // DEBUG
        //if (fcntl(1, F_SETSIG, SIGUSR1)) {
        //    perror("fcntl setsig");
        //    exit(2);
        //}
        fcntl(pfd[0], F_SETOWN, getppid());
        fcntl(pfd[0], F_SETFL, O_ASYNC);
        fprintf(stderr, "flag =  %d", fcntl(1, F_GETFD));
        if (fcntl(pfd[0], F_SETSIG, SIGUSR1)) {
            perror("fcntl setsig");
            exit(2);
        }
        

        //write(1, "aaa", 3); // DEBUG
        //fprintf(stdout, "bbb\n");
        my_execute(command);
    } else if (pid > 0) { // parent
        //int flags = fcntl(pfd[0], F_GETFL, 0); // TODO handle error? see man
        //if (fcntl(pfd[0], F_SETFL, flags | O_NONBLOCK)) {
        //    perror("fcntl");
        //    exit(2);
        //}
        //if (fcntl(pfd[0], F_SETSIG, SIGUSR1)) {
        //    perror("fcntl setsig");
        //    exit(2);
        //}

        //fprintf(stderr, "wait ...\n"); // DEBUG
        int status;
        if (-1 == wait(&status)) {
            fprintf(stderr, "%10d Failed to handle CHLD zombie. %s\n", getpid(), strerror(errno));
            exit(2);
        }
        //fprintf(stderr, "pid = %d\n", getpid()); // DEBUG
        //sleep(2000); // DEBUG
        
        fprintf(stderr, "%10d TERMINATED WITH EXIT CODE: %d\n", getpid(), chld_exit_code);
    } else { // (-1 == pid) error
        fprintf(stderr, "%10d Pid is negative, after fork. %s\n", getpid(), strerror(errno));
        exit(2);
    }
}
