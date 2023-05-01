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
	//https://www.khanacademy.org/computing/computer-science/cryptography/modarithmetic/a/fast-modular-exponentiation
	//So suppose the base is 7, the mod is 13
	//if e is 1, then you multiply res(1) by base(7), and do mod(13), so you get 7, then 
	//e becomes 0, the base becomes 7*7 mod 13 = 10, but that doesn't matter, because the loop
	//stops there, end you just end up with a res of 7
	//if e  is 2, then you shift it right, and set the b to 10
	//then you go through the loop again and multiply res with the base to get 10.
	//if e is 3, then you multiply res by 7, then on the next step you multiply it by 10
	//
	//it's just an exponentiation loop with mulmod instead of multiply
	//
	//and the exponentiation loop just keeps track of the exponent by squaring the base and shifting e to the right, 
	//and then if you see a 1, then res gets multiplied by the correct exponent
	//
	//or another way to look at it: check first that it's correct for a 1 with some number of zero's behind it, like exp = 2^256. And then see that it's correct if there are multiple 1's in there, because it just multiplies then correct factors.
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
	//
	//this is just basic multiplication saying that 57 * 49 = (5 * 10 + 7) * 49 = 7 * 49 + 5 * 10 * 49, but then using binary
	//so 101 * 11 = (1 * 2 * 2 + 1) * 11 = 1 * 11 + 0 * 2 * 11 + 1 * 2 * 2 * 11
	//you just double the b (11) every time you shift a(101) right, and then add the b if you see 1 in a.
	//
	//the nice thing here is that doubling is just addition and there's a way to check if you're going to overflow by 
	//first underflowing and then going back......
	//wait, but doesn't the reverse also work? first overflowing and then going back? The problem there
	//is that after the overflow you don't know what's going on anymore and the 'if' doesn't work. 
    uint64_t res = 0;
    uint64_t temp_b;

	/* we could say that this just never happens, but we might re-use this function
	* in other places, which would mess everything up.
	*/
    if (b >= m) {
        if (m > UINT64_MAX / 2u)
		//in this case you know m is not multiple times in b, because if it was, b would be larger
		// than the uint64 max, which is impossible. So since m is in b only once, you can just do subtraction
		// instead of mod, which should be cheaper. 
            b -= m;
        else
            b %= m;
    }

    while (a != 0) {
        if (a & 1) {
            /* Add b to res, modulo m, without overflow */
            if (b >= m - res) /* Equiv to if (res + b >= m), without overflow */
                res -= m;
				/* of course, this can go below zero (for example for res == 0)
				* but that's allowed, not undefined behavour. Then end result will always
				* end up as a proper positibe number, because you do res - m + b
				* and res + b >= m*/
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

#ifdef RSA_TEST
int main() {
	//largest 64-bit prime
	//https://www.mersenneforum.org/showthread.php?t=3471
	uint64_t prime = 18446744073709551557UL;
	/* for (uint64_t i = 0; i < 100; i++) { */
	/* 	printf("%lu is %d\n", i, is_prime_miller(i)); */
	/* } */

	printf("%d\n", is_prime_miller(prime));
	int tests = 1000;
	/* for (uint64_t i = 0xFFFFFFFFFFFFFFFFUL; i > 0xFFFFFFFFFFFFFFFF - tests; i--) { */
	/* 	printf("%d\n", is_prime_miller(i)); */
	/* } */
	char buffer[100];
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
#endif // RSA_TEST
//printf("%d\n", is_prime_miller(prime));
