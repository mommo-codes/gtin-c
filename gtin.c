#include "gtin.h"
#include <ctype.h>   /* for isdigit() */

void clean_gtin(const char *input, char *output) {
    int j = 0; /* write position in output */

    for (int i = 0; input[i] != '\0'; i++) { /* walk to the null terminator */
        
        if (isdigit((unsigned char)input[i])) {
            output[j] = input[i];
            j++;
        }
        
    }
    output[j] = '\0';  /* terminate the result ourselves */
}

char compute_check_digit(const char *gtin13) {
    int len = strlen(gtin13);
    int total = 0;

    for (int i = 0; i < len; i++) {
        int digit = gtin13[len - 1 - i] - '0';   /* rightmost digit first */

        // TODO
    }

    return (char)('0' + (10 - (total % 10)) % 10);
}