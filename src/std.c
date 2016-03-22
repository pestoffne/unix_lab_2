#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "std.h"

void handle_signal(int signal, siginfo_t *siginfo, void *context) {
    switch (signal) {
        case SIGUSR1:
            printf("Handler: SIGUSR1 catched.\n");
            break;
        case SIGUSR2:
            printf("Handler: SIGUSR2 catched.\n");
            break;
        case SIGHUP:
            printf("Handler: SIGHUP catched.\n");
            break;
        default:
            perror("Handler: Unexpected signal.\n");
            break;
    }
    printf("Handler: Number of signal = %d.\n", signal);
    printf("Handler: Sender PID = %d.\n", siginfo->si_pid);
    printf("Handler: Sender GID = %d.\n", getpgid(siginfo->si_pid));
}

void script_std() {
    printf("Process: My PID = %i .\n", getpid());
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handle_signal;
    sigfillset(&sa.sa_mask);  // initializes sa.sa_mask set to full, including all signals.
    if (-1 == sigaction(SIGUSR1, &sa, NULL)) {
        perror("Process: Fail while changing SIGUSR1 handler. ");
        exit(11);
    }
    if (-1 == sigaction(SIGUSR2, &sa, NULL)) {
        perror("Process: Fail while changing SIGUSR2 handler. ");
        exit(11);
    }
    if (-1 == sigaction(SIGHUP, &sa, NULL)) {
        perror("Process: Fail while changing SIGHUP handler. ");
        exit(11);
    }
    printf("Process: I wait for a SIGUSR1(%d), SIGUSR2(%d) or SIGHUP(%d).\n", SIGUSR1, SIGUSR2, SIGHUP);
    while (1) {
        sleep(1);
    }
    printf("Process: Finished.\n");
}
