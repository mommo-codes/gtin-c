#include "gtin.h"
#include <ctype.h>    /* isdigit()  */
#include <string.h>   /* strlen()   */

/* ─── internal helper ────────────────────────────────────────────────────────
 * Right-align `src` into `buf` with leading '0' padding to exactly
 * `target_len` characters. If src is *longer* than target_len, we take
 * only the first target_len chars (mirrors JS .padStart(14).substring(0,13)).
 * buf must be at least target_len + 1 bytes.
 */
static void pad_left(const char *src, char *buf, int target_len) {
    int len = (int)strlen(src);
    int pad = target_len - len;

    if (pad < 0) {
        /* longer than target — truncate to first target_len chars */
        for (int i = 0; i < target_len; i++) buf[i] = src[i];
    } else {
        /* fill leading zeros, then copy src */
        for (int i = 0; i < pad; i++)          buf[i]       = '0';
        for (int i = 0; i < len; i++)           buf[pad + i] = src[i];
    }
    buf[target_len] = '\0';
}

/* ─── clean_gtin ─────────────────────────────────────────────────────────────
 * JS: val.replace(/[^0-9]/g, "")
 * Walk input with two indices: i reads every char, j only advances on digits.
 */
void clean_gtin(const char *input, char *output) {
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit((unsigned char)input[i])) {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

/* ─── compute_check_digit ────────────────────────────────────────────────────
 * JS: iterate reversed digits, multiply alternating by 3 / 1 from rightmost.
 * C:  index from the right with gtin13[len-1-i] — no reverse() needed.
 * The char→int trick: '7' - '0' = 55 - 48 = 7.
 * The int→char trick: '0' + 9 = 57 = '9'. Both replace JS parseInt / String().
 */
char compute_check_digit(const char *gtin13) {
    int len   = (int)strlen(gtin13);
    if (len > 13) len = 13;    /* clamp; pad_left guarantees this won't happen */

    int total = 0;
    for (int i = 0; i < len; i++) {
        int digit = gtin13[len - 1 - i] - '0';           /* rightmost first  */
        total += (i % 2 == 0) ? digit * 3 : digit;       /* weight 3, 1, 3…  */
    }

    return (char)('0' + (10 - (total % 10)) % 10);
}

/* ─── normalize_gtin ─────────────────────────────────────────────────────────
 * JS: digits.padStart(14, "0").substring(0, 13) + "0"
 * Returns a 14-char lookup key where the last digit is always '0'.
 * NOT a valid GTIN — used for dedup / keying.
 */
void normalize_gtin(const char *gtin, char *output) {
    char cleaned[64];
    clean_gtin(gtin, cleaned);

    if (cleaned[0] == '\0') { output[0] = '\0'; return; }

    char padded[16];
    pad_left(cleaned, padded, 14);   /* 14 digits, leading zeros */

    for (int i = 0; i < 13; i++) output[i] = padded[i];
    output[13] = '0';
    output[14] = '\0';
}

/* ─── gtin_with_check_digit ──────────────────────────────────────────────────
 * JS: base = digits.padStart(14,"0").substring(0,13); return base + check;
 * Returns a valid 14-digit GTIN with the correct GS1 check digit appended.
 */
void gtin_with_check_digit(const char *gtin, char *output) {
    char cleaned[64];
    clean_gtin(gtin, cleaned);

    if (cleaned[0] == '\0') { output[0] = '\0'; return; }

    char padded[16];
    pad_left(cleaned, padded, 14);
    padded[13] = '\0';                         /* terminate at 13 — that's our base */

    char check = compute_check_digit(padded);

    for (int i = 0; i < 13; i++) output[i] = padded[i];
    output[13] = check;
    output[14] = '\0';
}

/* ─── validate_gtin ──────────────────────────────────────────────────────────
 * Returns 1 if the GTIN's last digit matches the computed GS1 check digit.
 * Returns 0 for invalid or empty input.
 */
int validate_gtin(const char *gtin) {   
    char cleaned[64];
    clean_gtin(gtin, cleaned);

    if (cleaned[0] == '\0') return 0;

    char padded[16];
    pad_left(cleaned, padded, 14);

    char base[14];
    for (int i = 0; i < 13; i++) base[i] = padded[i];
    base[13] = '\0';

    return padded[13] == compute_check_digit(base) ? 1 : 0;
}