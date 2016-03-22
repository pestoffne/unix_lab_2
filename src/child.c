#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "child.h"

void handle_child(int signal, siginfo_t *siginfo, void *context) {
    if (SIGCHLD == signal) {
        printf("Handler: SIGCHLD catched.\n");
        printf("Handler: si_pid=%i\n", siginfo->si_pid);
        printf("Handler: si_uid=%i\n", siginfo->si_uid);
        printf("Handler: si_status=%i\n", siginfo->si_status);
        printf("Handler: si_code=%i\n", siginfo->si_code);
    } else {
        perror("Handler: Unexpected signal.\n"); // TODO
    }
}

void script_child() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handle_child;
    int sleep_time = 5;
    if (-1 == sigaction(SIGCHLD, &sa, NULL)) {
        // TODO
        exit(17);
    }

    pid_t pid = fork();
    if (0 == pid) {  // child
        printf("Child:   Started.\n");
        printf("Child:   Sleep %d seconds.\n", sleep_time);
        sleep(sleep_time);
        printf("Child:   I woke up.\n");
        printf("Child:   Finished.\n");
    } else if (pid > 0) {  // parent
        printf("Parent:  Started.\n");
        int status;
        if (-1 == wait(&status)) {
            // TODO
            exit(16);
        }
        printf("Parent:  Finished.\n");
    } else {  // (-1 == pid) error
        perror("?:       Pid is negative, after fork.\n");
        exit(10);
    }
}
