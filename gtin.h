#ifndef GTIN_H
#define GTIN_H

/*
 * Strip every non-digit charachter from `input`, 
 * writing the result into `output`.
 * Caller musst provide `output` big enough (strlen(input) + 1).
 */

 void clean_gtin(const char *input, char *output);

/* Compute the GS1 Mod-10 check digit for a digit string. Returns the digit char. */
char compute_check_digit(const char *gtin13);

 #endif /* GTIN_H */