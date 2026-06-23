#ifndef GTIN_H
#define GTIN_H

/*
 * Strip every non-digit charachter from `input`, 
 * writing the result into `output`.
 * Caller musst provide `output` big enough (strlen(input) + 1).
 */

 void clean_gtin(const char *input, char *output);

 #endif /* GTIN_H */