#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "child.h"

void handle_child(int signal, siginfo_t *siginfo, void *context) {
    if (SIGCHLD == signal) {
        printf("SIGCHLD handling\n");
        printf("Child PID: %i\n", siginfo->si_pid);
        printf("User of process: %i\n", siginfo->si_uid);
        printf("Status: %i\n", siginfo->si_status);
        printf("Code: %i\n", siginfo->si_code);
        exit(0);
    } else {
        perror("unexpected signal\n");
        exit(11);
    }
}

void script_child() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handle_child;

    int sleep_time = 5;

    pid_t pid = fork();
    if (0 == pid) {  // child
        printf("sleep %d seconds.\n", sleep_time);
        sleep(sleep_time);
        printf("I woke up.\n");
    } else if (pid > 0) {  // parent
        sigaction(SIGCHLD, &sa, NULL);
        int status;
        if (wait(&status) > 0) {
            printf("S\n");
            exit(0);
        } else {
            perror("Failed to handle CHLD zombie\n");
            exit(12);
        }
    } else {  // (pid < 0) error
        perror("pid is negative, after fork.\n");
        exit(10);
    }
}
