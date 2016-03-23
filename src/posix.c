#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "posix.h"

int resieved_signals_count = 0;

void handle_posix(int signal, siginfo_t *siginfo, void *context) {
        const char *signal_name;

        fprintf(stderr, "Handler: N=%i | MYPID=%i | PPID=%i | POSIXSIGNALNO=%i | VALUE=%i\n",
                resieved_signals_count, siginfo->si_pid, getpid(), signal, siginfo->si_value.sival_int);
        resieved_signals_count++;
}

// uses global: amount
void script_posix() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handle_posix;

    // sigset_t set;
    if(sigaction(SIGRTMIN, &sa, NULL) == -1){
        perror("Ошибка: не удается обработать сигнал SIGRTMIN");
    }

    int j = 0;
    for (j = 0; j < SIGRTMAX; j++) {
        sigaction(SIGRTMIN + j, &sa, NULL);
    }

    pid_t pid = fork();
    if (0 == pid) {  // child
        printf("Child:   Started.\n");
        int range = 1 + SIGRTMAX - SIGRTMIN;
        int buckets = RAND_MAX / range;
        int limit = buckets * range;
        int i;
        for (i = 0; i < amount; ++i) {
            union sigval value;

            int r_signal;
            do {
                r_signal = rand();
            } while (r_signal >= limit);

            r_signal = SIGRTMIN + (r_signal / buckets);

            value.sival_int = rand();
            sigqueue(getppid(), r_signal, value);
            fprintf(stderr, "Child:   N=%i | MYPID=%i | PPID=%i | POSIXSIGNALNO=%i | VALUE=%i\n",
                i, getpid(), getppid(), r_signal, value.sival_int);
        }
        printf("Child:   Finished.\n");
    } else if (pid > 0) {  // parent
        printf("Parent:  Started.\n");
        int status;
        sleep(10);
        if (-1 == wait(&status)) {
            perror("Parent:  Failed to handle CHLD zombie");
            exit(16);
        }
        printf("Parent:  Finished.\n");
    } else {  // (-1 == pid) error
        perror("?:       Pid is negative, after fork.\n");
        exit(10);
    }
}
