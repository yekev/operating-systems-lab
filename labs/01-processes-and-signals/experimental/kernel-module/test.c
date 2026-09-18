#include <signal.h>
#include <stdio.h>

int main(void)
{
    puts("user test: raising SIGBUS");
    raise(SIGBUS);
    return 0;
}
