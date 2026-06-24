#include "gtin.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void) {
    char line[64];
    char rebuilt[16];
    char corrupted[16];

    long count              = 0;
    long valid              = 0;
    long invalid_unexpected = 0;
    long roundtrip_ok       = 0;
    long roundtrip_fail     = 0;
    long corrupt_rejected   = 0;
    long corrupt_accepted   = 0;

    clock_t start = clock();

    while (fgets(line, sizeof(line), stdin)) {
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';
        if (len == 0) continue;

        count++;

        if (validate_gtin(line)) valid++;
        else invalid_unexpected++;

        gtin_with_check_digit(line, rebuilt);
        if (rebuilt[0] != '\0' && validate_gtin(rebuilt)) roundtrip_ok++;
        else roundtrip_fail++;

        memcpy(corrupted, rebuilt, 15);
        corrupted[13] = (char)('0' + (corrupted[13] - '0' + 1) % 10);
        if (!validate_gtin(corrupted)) corrupt_rejected++;
        else corrupt_accepted++;
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n==========================================\n");
    printf("  GTINs processed          %10ld\n", count);
    printf("\n");
    printf("  [inv 1] validated OK     %10ld\n", valid);
    printf("  [inv 1] unexpected fail  %10ld  %s\n", invalid_unexpected,
           invalid_unexpected == 0 ? "" : "<- BUG");
    printf("\n");
    printf("  [inv 2] round-trip OK    %10ld\n", roundtrip_ok);
    printf("  [inv 2] round-trip fail  %10ld  %s\n", roundtrip_fail,
           roundtrip_fail == 0 ? "" : "<- BUG");
    printf("\n");
    printf("  [inv 3] corrupt rejected %10ld\n", corrupt_rejected);
    printf("  [inv 3] corrupt accepted %10ld  %s\n", corrupt_accepted,
           corrupt_accepted == 0 ? "" : "<- BUG");
    printf("\n");
    printf("  Time                       %.3fs\n", elapsed);
    if (elapsed > 0)
        printf("  Throughput           %.0f GTINs/sec\n", (double)count / elapsed);
    printf("==========================================\n");

    long bugs = invalid_unexpected + roundtrip_fail + corrupt_accepted;
    printf("  %s\n", bugs == 0 ? "ALL INVARIANTS HOLD" : "VIOLATIONS DETECTED");
    printf("==========================================\n\n");

    return bugs > 0 ? 1 : 0;
}