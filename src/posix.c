#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "posix.h"

#define MAX_SIZE 128
int ar_posix_signal_no[MAX_SIZE];
int ar_sival_int[MAX_SIZE];
pid_t ar_my_pid[MAX_SIZE];
pid_t ar_parent_pid[MAX_SIZE];
int current_index = 0;

void print_table(char *sender, int N) {
    printf("%-8s | %3s | %6s | %10s | %27s | %12s |\n", sender, "N", "MY_PID", "PARENT_PID", "RANDOM_POSIX_SIGNAL_SENT_NO", "RANDOM_VALUE");
    int i;
    for (i = 0; i < N; i++) {
        printf("%-8s | %3i | %6i | %10i | %27i | %12i |\n", sender, i + 1, ar_my_pid[i], ar_parent_pid[i], ar_posix_signal_no[i], ar_sival_int[i]);
    }
}

void handle_posix(int signal, siginfo_t *siginfo, void *context) {
    int i = current_index++;
    ar_posix_signal_no[i] = signal;
    ar_sival_int[i] = siginfo->si_value.sival_int;
    ar_my_pid[i] = getpid();
    ar_parent_pid[i] = getppid();
}

int my_rand(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// uses global: amount
void script_posix() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handle_posix;

    int j = 0;
    for (j = SIGRTMIN; j <= SIGRTMAX; j++) {
        if (-1 == sigaction(j, &sa, NULL)) {
            fprintf(stderr, "%8s Fail while creating handler. %s\n", "?:", strerror(errno));
            exit(17);
        }
    }

    pid_t pid = fork();
    if (0 == pid) {  // child
        printf("%-8s Started.\n", "Child:");
        int i;
        for (i = 0; i < amount; i++) {
            union sigval value;
            value.sival_int = rand();
            if (-1 == sigqueue(getppid(), my_rand(SIGRTMIN, SIGRTMAX), value)) {
                fprintf(stderr, "%-8s Fail while sending posix signal. %s\n", "Child:", strerror(errno));
                exit(19);
            }
        }
        printf("%-8s Finished.\n", "Child:");
    } else if (pid > 0) {  // parent
        printf("%-8s Started.\n", "Parent:");
        sleep(1);
        print_table("Parent:", amount);
        printf("%-8s Finished.\n", "Parent:");
    } else {  // (-1 == pid) error
        fprintf(stderr, "%-8s Pid is negative, after fork. %s\n", "?:", strerror(errno));
        exit(10);
    }
}
