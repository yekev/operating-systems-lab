#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t sigchld_received = 0;

static void handle_sigchld(int signal_number)
{
    (void)signal_number;
    sigchld_received = 1;
}

static const char *signal_name(int signal_number)
{
    switch (signal_number) {
    case SIGABRT: return "SIGABRT";
    case SIGALRM: return "SIGALRM";
    case SIGBUS:  return "SIGBUS";
    case SIGFPE:  return "SIGFPE";
    case SIGHUP:  return "SIGHUP";
    case SIGILL:  return "SIGILL";
    case SIGINT:  return "SIGINT";
    case SIGKILL: return "SIGKILL";
    case SIGPIPE: return "SIGPIPE";
    case SIGQUIT: return "SIGQUIT";
    case SIGSEGV: return "SIGSEGV";
    case SIGSTOP: return "SIGSTOP";
    case SIGTERM: return "SIGTERM";
    case SIGTRAP: return "SIGTRAP";
    default:      return "UNKNOWN";
    }
}

static int install_sigchld_handler(void)
{
    struct sigaction action;

    action.sa_handler = handle_sigchld;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    return sigaction(SIGCHLD, &action, NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <test-program> [arguments...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (install_sigchld_handler() == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("Parent: starting child process\n");
    fflush(stdout);

    pid_t child = fork();
    if (child == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child == 0) {
        printf("Child: pid=%ld, executing %s\n", (long)getpid(), argv[1]);
        fflush(stdout);
        execv(argv[1], &argv[1]);
        perror("execv");
        _exit(127);
    }

    for (;;) {
        int status = 0;
        pid_t result;

        do {
            result = waitpid(child, &status, WUNTRACED | WCONTINUED);
        } while (result == -1 && errno == EINTR);

        if (result == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        }

        if (sigchld_received) {
            printf("Parent: received SIGCHLD\n");
            sigchld_received = 0;
        }

        if (WIFEXITED(status)) {
            printf("Child: exited normally with status %d\n", WEXITSTATUS(status));
            break;
        }

        if (WIFSIGNALED(status)) {
            int terminating_signal = WTERMSIG(status);
            printf("Child: terminated by %s (%d)\n",
                   signal_name(terminating_signal), terminating_signal);
            break;
        }

        if (WIFSTOPPED(status)) {
            int stopping_signal = WSTOPSIG(status);
            printf("Child: stopped by %s (%d); sending SIGCONT\n",
                   signal_name(stopping_signal), stopping_signal);
            if (kill(child, SIGCONT) == -1) {
                perror("kill(SIGCONT)");
                return EXIT_FAILURE;
            }
            continue;
        }

        if (WIFCONTINUED(status)) {
            printf("Child: continued\n");
        }
    }

    return EXIT_SUCCESS;
}
