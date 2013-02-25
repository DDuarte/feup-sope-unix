#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <sys/times.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("usage: p8a n1 n2\n");
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    int n1 = atoi(argv[1]);
    int n2 = atoi(argv[2]);

    int i = 0;
    while (true)
    {
        int r = rand() % n1;
        printf("%i - %i\n", i++, r);
        if (r == n2)
            break;
    }

    struct tms t;
    clock_t rt = times(&t);

    long ticksSeg = sysconf(_SC_CLK_TCK);

    printf("Real-time: %f s\n", (double)rt/(double)ticksSeg);
    printf("CPU-time (user): %f s\n", (double)t.tms_utime/(double)ticksSeg);
    printf("CPU-time (proc): %f s\n", (double)t.tms_stime/(double)ticksSeg);

    return EXIT_SUCCESS;
}
