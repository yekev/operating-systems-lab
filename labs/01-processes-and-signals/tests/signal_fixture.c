#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct signal_case {
    const char *name;
    int number;
};

static const struct signal_case cases[] = {
    {"abort", SIGABRT},
    {"alarm", SIGALRM},
    {"bus", SIGBUS},
    {"floating-point", SIGFPE},
    {"hangup", SIGHUP},
    {"illegal-instruction", SIGILL},
    {"interrupt", SIGINT},
    {"kill", SIGKILL},
    {"pipe", SIGPIPE},
    {"quit", SIGQUIT},
    {"segmentation-fault", SIGSEGV},
    {"stop", SIGSTOP},
    {"terminate", SIGTERM},
    {"trap", SIGTRAP},
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <normal|signal-case>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "normal") == 0) {
        puts("fixture: normal exit");
        return EXIT_SUCCESS;
    }

    for (size_t index = 0; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        if (strcmp(argv[1], cases[index].name) == 0) {
            printf("fixture: raising %s (%d)\n", cases[index].name, cases[index].number);
            fflush(stdout);
            if (raise(cases[index].number) != 0) {
                perror("raise");
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
    }

    fprintf(stderr, "Unknown signal case: %s\n", argv[1]);
    return EXIT_FAILURE;
}
