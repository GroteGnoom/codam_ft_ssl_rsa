#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//https://stackoverflow.com/questions/12168348/ways-to-do-modulo-multiplication-with-primitive-types
//https://en.wikipedia.org/wiki/Modular_exponentiation


uint64_t mulmod(uint64_t a, uint64_t b, uint64_t m);
uint64_t mod_pow(uint64_t b, uint64_t e, uint64_t m) {
	//apparently from bruce schneier
	//https://en.wikipedia.org/wiki/Modular_exponentiation
	if (m == 1) return 0;
	//assert?
	uint64_t res = 1;
	b %= m;
	while (e > 0) {
		if (e % 2 == 1) {
			res = mulmod(res, b, m);
		}
		e >>= 1;
		b = mulmod(b, b, m);
	}
	return res;
}

uint64_t mulmod(uint64_t a, uint64_t b, uint64_t m) {
	//https://stackoverflow.com/a/18680280
    uint64_t res = 0;
    uint64_t temp_b;

    /* Only needed if b may be >= m */
    if (b >= m) {
        if (m > UINT64_MAX / 2u)
            b -= m;
        else
            b %= m;
    }

    while (a != 0) {
        if (a & 1) {
            /* Add b to res, modulo m, without overflow */
            if (b >= m - res) /* Equiv to if (res + b >= m), without overflow */
                res -= m;
            res += b;
        }
        a >>= 1;

        /* Double b, modulo m */
        temp_b = b;
        if (b >= m - b)       /* Equiv to if (2 * b >= m), without overflow */
            temp_b -= m;
        b += temp_b;
    }
    return res;
}

bool is_prime_miller(uint64_t n) {
	if (n < 2) return false;
	if (n == 2) return true;
	if (n % 2 == 0) return false;
	//so from here on n > 2 and n is odd
	uint64_t s = 1;
	while ((n - 1) % (1 << (s + 1)) == 0) {
		s++;
	}
	uint64_t d = (n - 1) / (1UL << s);
	uint64_t checks[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
	uint64_t len_checks = sizeof(checks) / sizeof(checks[0]);

	//printf("n: %lu, s: %lu, d: %lu", n, s, d);
	//printf(" 1<<s: %lu\n", 1UL << s);
	for (uint64_t check_i = 0; check_i < len_checks; check_i++) {
		uint64_t a = checks[check_i];
		if (a >= n -2) {
			break;
		}
		uint64_t x = mod_pow(a, d, n);
		//printf("a: %lu, d: %lu, n: %lu, x: %lu\n", a, d, n, x);
		uint64_t y = 0;
		for (uint64_t i = 0; i < s; i++) {
			y = mulmod(x, x, n);
			//printf("x: %lu, x*x: %lu, y: %lu\n", x, x*x, y);
			if (y == 1 && x != 1 && x != n-1) {
				//printf("hier\n");
				return false;
			}
			x = y;
		}
		if (y != 1) {
			//printf("hier2\n");
			return false;
		}
	}
	return true;
}

int main() {
	//largest 64-bit prime
	//https://www.mersenneforum.org/showthread.php?t=3471
	uint64_t prime = 18446744073709551557UL;
	/* for (uint64_t i = 0; i < 100; i++) { */
	/* 	printf("%lu is %d\n", i, is_prime_miller(i)); */
	/* } */

	printf("%d\n", is_prime_miller(prime));
	char buffer[100];
	int tests = 1000;
	for (uint64_t i = 0xFFFFFFFFFFFFFFFFUL; i > 0xFFFFFFFFFFFFFFFF - tests; i--) {
		sprintf(buffer, "a=$(factor %lu | wc -w); [ $a -eq 2 ]", i);
		if (is_prime_miller(i)) {
			if (system(buffer) != 0) {
				printf("%lu should not be prime!", i);
				exit(1);
			}
		} else {
			if (system(buffer) == 0) {
				printf("%lu should be prime!", i);
				exit(1);
			}
		}
	}
}
//printf("%d\n", is_prime_miller(prime));
