#ifndef GTIN_H
#define GTIN_H

/*
 * Strip every non-digit character from `input` into `output`.
 * output must be at least strlen(input) + 1 bytes.
 */
void clean_gtin(const char *input, char *output);

/*
 * GS1 Mod-10 check digit for a digit string (up to 13 digits).
 * Returns the check digit as a char ('0'–'9').
 */
char compute_check_digit(const char *gtin13);

/*
 * Normalize to a 14-digit lookup key.
 * Pads to 14 digits, takes the first 13, and sets the last digit to '0'.
 * This matches the JS normalizeGtin() — it is NOT a valid GTIN, just a dedup key.
 * output must be at least 15 bytes.
 */
void normalize_gtin(const char *gtin, char *output);

/*
 * Build a valid GTIN-14 with the correct GS1 check digit.
 * output must be at least 15 bytes.
 */
void gtin_with_check_digit(const char *gtin, char *output);

/*
 * Returns 1 if the GTIN has a valid GS1 check digit, 0 otherwise.
 */
int validate_gtin(const char *gtin);

#endif /* GTIN_H */