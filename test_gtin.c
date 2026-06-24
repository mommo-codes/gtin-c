#include "gtin.h"
#include <stdio.h>
#include <string.h>

static int _pass = 0, _fail = 0;

#define ASSERT_STR_EQ(label, want, got) do {                                   \
    if (strcmp((want), (got)) == 0) {                                          \
        _pass++;                                                               \
        printf("  [PASS] %s\n", (label));                                     \
    } else {                                                                   \
        _fail++;                                                               \
        printf("  [FAIL] %s\n"                                                \
               "         want: \"%s\"\n"                                       \
               "         got:  \"%s\"\n", (label), (want), (got));            \
    }                                                                          \
} while(0)

#define ASSERT_CHAR_EQ(label, want, got) do {                                  \
    if ((want) == (got)) {                                                     \
        _pass++;                                                               \
        printf("  [PASS] %s\n", (label));                                     \
    } else {                                                                   \
        _fail++;                                                               \
        printf("  [FAIL] %s\n"                                                \
               "         want: '%c'\n"                                         \
               "         got:  '%c'\n", (label), (want), (got));              \
    }                                                                          \
} while(0)

#define ASSERT_INT_EQ(label, want, got) do {                                   \
    if ((want) == (got)) {                                                     \
        _pass++;                                                               \
        printf("  [PASS] %s\n", (label));                                     \
    } else {                                                                   \
        _fail++;                                                               \
        printf("  [FAIL] %s\n"                                                \
               "         want: %d\n"                                           \
               "         got:  %d\n", (label), (want), (got));                \
    }                                                                          \
} while(0)

static void section(const char *name) {
    printf("\n%s\n", name);
}

static void test_clean_gtin(void) {
    section("clean_gtin ─────────────────────────────────────────");
    char out[64];

    clean_gtin("", out);
    ASSERT_STR_EQ("empty string -> empty", "", out);

    clean_gtin("1234567890123", out);
    ASSERT_STR_EQ("all digits -> unchanged", "1234567890123", out);

    clean_gtin("7-310-12345678", out);
    ASSERT_STR_EQ("strips dashes", "731012345678", out);

    clean_gtin("  012 345 ", out);
    ASSERT_STR_EQ("strips spaces, keeps leading zeros", "012345", out);

    clean_gtin("abc", out);
    ASSERT_STR_EQ("all letters -> empty", "", out);

    clean_gtin("7.350.053.850.019", out);
    ASSERT_STR_EQ("strips dots", "7350053850019", out);

    clean_gtin("EAN:7350053850019", out);
    ASSERT_STR_EQ("strips label prefix", "7350053850019", out);

    clean_gtin("\t9\n", out);
    ASSERT_STR_EQ("strips tab and newline", "9", out);

    clean_gtin("0000000000000", out);
    ASSERT_STR_EQ("all zeros preserved", "0000000000000", out);

    clean_gtin("!!!???###", out);
    ASSERT_STR_EQ("all special chars -> empty", "", out);
}

static void test_compute_check_digit(void) {
    section("compute_check_digit ─────────────────────────────────");

    ASSERT_CHAR_EQ("all zeros -> '0'",
                   '0', compute_check_digit("0000000000000"));

    ASSERT_CHAR_EQ("0735005385001 -> '9' (known EAN-13 base)",
                   '9', compute_check_digit("0735005385001"));

    ASSERT_CHAR_EQ("0001234567890 -> '5' (known UPC-A base)",
                   '5', compute_check_digit("0001234567890"));

    ASSERT_CHAR_EQ("0000001234567 -> '0' (known EAN-8 base)",
                   '0', compute_check_digit("0000001234567"));

    ASSERT_CHAR_EQ("0000000000001 -> '7'",
                   '7', compute_check_digit("0000000000001"));

    ASSERT_CHAR_EQ("9999999999999 -> '7'",
                   '7', compute_check_digit("9999999999999"));
}

static void test_normalize_gtin(void) {
    section("normalize_gtin ─────────────────────────────────────");
    char out[32];

    normalize_gtin("", out);
    ASSERT_STR_EQ("empty -> empty", "", out);

    normalize_gtin("7350053850019", out);
    ASSERT_STR_EQ("EAN-13 -> 14-char key, last digit '0'", "07350053850010", out);

    normalize_gtin("7-350-053-850-019", out);
    ASSERT_STR_EQ("messy EAN-13 -> same key", "07350053850010", out);

    normalize_gtin("07350053850019", out);
    ASSERT_STR_EQ("GTIN-14 -> last digit zeroed", "07350053850010", out);

    normalize_gtin("123", out);
    ASSERT_STR_EQ("short input -> heavily padded", "00000000000120", out);

    normalize_gtin("9999999999997", out);
    ASSERT_INT_EQ("result is always exactly 14 chars", 14, (int)strlen(out));
    ASSERT_INT_EQ("last char is always '0'", '0', (int)out[13]);
}

static void test_gtin_with_check_digit(void) {
    section("gtin_with_check_digit ──────────────────────────────");
    char out[32];

    gtin_with_check_digit("", out);
    ASSERT_STR_EQ("empty -> empty", "", out);

    gtin_with_check_digit("7350053850019", out);
    ASSERT_STR_EQ("EAN-13 -> correct GTIN-14", "07350053850019", out);

    gtin_with_check_digit("012345678905", out);
    ASSERT_STR_EQ("UPC-A -> correct GTIN-14", "00012345678905", out);

    gtin_with_check_digit("12345670", out);
    ASSERT_STR_EQ("EAN-8 -> correct GTIN-14", "00000012345670", out);

    gtin_with_check_digit("7350053850018", out);
    ASSERT_STR_EQ("wrong check digit -> corrected", "07350053850019", out);

    gtin_with_check_digit("7-350-053-850-019", out);
    ASSERT_STR_EQ("messy input -> clean GTIN-14", "07350053850019", out);

    gtin_with_check_digit("0000000000000", out);
    ASSERT_STR_EQ("all zeros -> 00000000000000", "00000000000000", out);

    ASSERT_INT_EQ("result always 14 chars", 14, (int)strlen(out));
}

static void test_validate_gtin(void) {
    section("validate_gtin ──────────────────────────────────────");

    ASSERT_INT_EQ("EAN-13 7350053850019",     1, validate_gtin("7350053850019"));
    ASSERT_INT_EQ("GTIN-14 07350053850019",   1, validate_gtin("07350053850019"));
    ASSERT_INT_EQ("UPC-A 012345678905",       1, validate_gtin("012345678905"));
    ASSERT_INT_EQ("EAN-8 12345670",           1, validate_gtin("12345670"));
    ASSERT_INT_EQ("all zeros 00000000000000", 1, validate_gtin("00000000000000"));
    ASSERT_INT_EQ("messy but valid input",    1, validate_gtin("7-350-053-850-019"));

    ASSERT_INT_EQ("wrong check digit (off by 1)", 0, validate_gtin("7350053850018"));
    ASSERT_INT_EQ("wrong check digit (off by 2)", 0, validate_gtin("7350053850017"));
    ASSERT_INT_EQ("check '0' on known '9' base",  0, validate_gtin("7350053850010"));
    ASSERT_INT_EQ("empty string",                  0, validate_gtin(""));
    ASSERT_INT_EQ("all letters",                   0, validate_gtin("abcdefghijklm"));
    ASSERT_INT_EQ("only dashes",                   0, validate_gtin("-------------"));
    ASSERT_INT_EQ("check '1' on all-zeros base",   0, validate_gtin("00000000000001"));
}

int main(void) {
    printf("+=================================+\n");
    printf("|    GTIN Unit Test Suite         |\n");
    printf("+=================================+\n");

    test_clean_gtin();
    test_compute_check_digit();
    test_normalize_gtin();
    test_gtin_with_check_digit();
    test_validate_gtin();

    printf("\n-----------------------------------\n");
    printf("  %d passed  |  %d failed\n", _pass, _fail);
    printf("-----------------------------------\n");
    printf("  %s\n", _fail == 0 ? "ALL GOOD" : "FAILURES DETECTED");
    printf("-----------------------------------\n");

    return _fail > 0 ? 1 : 0;
}