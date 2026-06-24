CC     = cc
CFLAGS = -Wall -Wextra -std=c11 -O2

all: test_gtin gen_gtins stress_test

test_gtin: test_gtin.c gtin.c gtin.h
	$(CC) $(CFLAGS) -o test_gtin test_gtin.c gtin.c

gen_gtins: gen_gtins.c gtin.c gtin.h
	$(CC) $(CFLAGS) -o gen_gtins gen_gtins.c gtin.c

stress_test: stress_test.c gtin.c gtin.h
	$(CC) $(CFLAGS) -o stress_test stress_test.c gtin.c

.PHONY: test
test: test_gtin
	./test_gtin

N = 1000000
.PHONY: stress
stress: gen_gtins stress_test
	./gen_gtins $(N) | ./stress_test

.PHONY: generate
generate: gen_gtins
	./gen_gtins $(N) > gtins.txt

.PHONY: clean
clean:
	rm -f test_gtin gen_gtins stress_test gtins.txt