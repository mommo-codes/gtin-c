#include "gtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_N 1000000

int main(int argc, char *argv[]) {
    long n = DEFAULT_N;

    if (argc > 1) {
        n = atol(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "usage: ./gen_gtins [count]\n");
            return 1;
        }
    }

    fprintf(stderr, "Generating %ld GTINs...\n", n);
    srand((unsigned)time(NULL));

    char base[14];
    char out[16];

    for (long i = 0; i < n; i++) {
        for (int j = 0; j < 13; j++) {
            base[j] = (char)('0' + (rand() % 10));
        }
        base[13] = '\0';

        char check = compute_check_digit(base);
        for (int j = 0; j < 13; j++) out[j] = base[j];
        out[13] = check;
        out[14] = '\0';

        puts(out);
    }

    return 0;
}