#include "gtin.h"
#include <stdio.h>

int main(void) {
    char out[64];
    clean_gtin("7-310-12345678", out);
    printf("cleaned: %s\n", out);

    /* 7350053850019 is a real GTIN-13; its check digit is 9.
       We feed the 12-digit body 735005385001 and expect '9'. */
    printf("check digit: %c\n", compute_check_digit("735005385001"));
    return 0;
}