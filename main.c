#include <stdint.h>
#include <stdbool.h>

//https://stackoverflow.com/questions/12168348/ways-to-do-modulo-multiplication-with-primitive-types
//https://en.wikipedia.org/wiki/Modular_exponentiation


uint64_t mul_modw(uint64_t a, uint64_t b, uint64_t m) {
	//wiki formula, only up to 63 bits?
	// so we want to calculate a * b (mod m)
	long double x;
	uint64_t c;
	int64_t r;
	//https://en.wikipedia.org/wiki/Modular_arithmetic#Properties
	//it follows from the (compatibility with multiplication)
	//rule that you can just mod a and b and get the same thing
	//I think this is almost never relevant for our case, because usually
	//m is enormous
	if (a >= m) a %= m;
	if (b >= m) b %= m;
	//make more space for a
	x = a;
	c = x * b / m;
	r = (int64_t) (a * b - c * m) % (int64_t_) m;
	// r = (a * b - (x * b / m) * m) % m;
	// r = (a * b - (a * b / m) * m) % m;
	// r = (a * b - (a * b )) % m;
	return r < 0 ? r + m : r;
}


uint64_t apowdmodnw(uint64_t a, uint64_t d, uint64_t n) {
	//wiki formula, only up to 63 bits?
    uint64_t r = n == 1 ? 0 : 1;
    while (d > 0) {
        if (d & 1) r = mul_modw(r, a, n);
        d = d >> 1;
        a = mul_modw(a, a, n);
    }
    return r;
}

uint64_t mod_pow(uint64_t a, uint64_t b, uint64_t n) {
	//simple, but b can be enormous, so can be very slow
	if (n == 0) {
		fprintf(stderr, "mod 0 is impossible (in mod_pow)");
	}
	if (n == 1) return 0;
	if (b == 0) return 1;
	uint64_t result = 1;
	for (uint64_t e == 1; e <= b; e++) {
		result * = a;
		result %= n;
	}
	return result;
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
	uint64_t d = (n - 1) / (1 << s);
	uint64_t checks = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37];
	uint64_t len_checks = sizeof(checks) / sizeof(checks[0]);

	for (uint64_t check_i = 0; check_i < len_checks; check_i++) {
		uint64_t a = checks[check_i];
		uint64_t x = mod_pow(a, d, n);
		for (uint64_t i = 0; i < s; i++) {
			uint64_t y = (x * x) % n;
			if (y == 1 && x != 1 and x != n-1) {
				return false;
			}
			x = y;
		}
		if (y != 1) {
			return false;
		}
	}
	return true;
}

int main() {
	//largest 64-bit prime
	//https://www.mersenneforum.org/showthread.php?t=3471
	int prime = 18446744073709551557;
	printf("%d\n", is_prime_miller(prime));
}
