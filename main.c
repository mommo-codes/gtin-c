#include "gtin.h"
#include <stdio.h>

int main(void) {
    char out[64];
    clean_gtin("7-310-12345678", out);
    printf("cleaned: %s\n", out);
    return 0;
}