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