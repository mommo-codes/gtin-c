#include "gtin.h"
#include <stdio.h>

/* Quick helper so test output isn't just 1/0 */
static void pass_fail(const char *label, int result) {
    printf("  [%s] %s\n", result ? "PASS" : "FAIL", label);
}

int main(void) {
    char out[32];

    /* ── clean_gtin ─────────────────────────── */
    printf("clean_gtin:\n");
    clean_gtin("7-310-12345678", out);
    pass_fail("strips dashes:  7-310-12345678 → 731012345678",
              __builtin_strcmp(out, "731012345678") == 0);

    clean_gtin("  012 345 ", out);
    pass_fail("strips spaces:  '  012 345 '  → 012345",
              __builtin_strcmp(out, "012345") == 0);

    clean_gtin("", out);
    pass_fail("empty in → empty out",
              out[0] == '\0');

    /* ── compute_check_digit ────────────────── */
    printf("\ncompute_check_digit:\n");
    /* EAN-13 7350053850019:  pad to 14 → 07350053850019, base = 0735005385001 → check '9' */
    char cd = compute_check_digit("0735005385001");
    pass_fail("0735005385001 → check digit '9'", cd == '9');

    /* GTIN with all zeros: base 0000000000000 → check '0' */
    cd = compute_check_digit("0000000000000");
    pass_fail("0000000000000 → check digit '0'", cd == '0');

    /* ── gtin_with_check_digit ──────────────── */
    printf("\ngtin_with_check_digit:\n");
    gtin_with_check_digit("7350053850019", out);
    pass_fail("7350053850019 → 07350053850019",
              __builtin_strcmp(out, "07350053850019") == 0);

    gtin_with_check_digit("7-350-053-850-019", out);   /* 13 digits + dashes */
    pass_fail("messy input same result",
              __builtin_strcmp(out, "07350053850019") == 0);

    gtin_with_check_digit("", out);
    pass_fail("empty in → empty out", out[0] == '\0');

    /* ── normalize_gtin ─────────────────────── */
    printf("\nnormalize_gtin:\n");
    normalize_gtin("7350053850019", out);
    pass_fail("7350053850019 → 07350053850010 (key, last digit '0')",
              __builtin_strcmp(out, "07350053850010") == 0);

    normalize_gtin("", out);
    pass_fail("empty in → empty out", out[0] == '\0');

    /* ── validate_gtin ──────────────────────── */
    printf("\nvalidate_gtin:\n");
    pass_fail("7350053850019  is valid",    validate_gtin("7350053850019")  == 1);
    pass_fail("07350053850019 is valid",    validate_gtin("07350053850019") == 1);
    pass_fail("7350053850018  is invalid",  validate_gtin("7350053850018")  == 0);
    pass_fail("empty string   is invalid",  validate_gtin("")               == 0);
    pass_fail("'abc'          is invalid",  validate_gtin("abc")            == 0);

    return 0;
}