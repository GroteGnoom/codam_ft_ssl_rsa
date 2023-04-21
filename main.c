#include "ft_ssl_rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to calculate the greatest common divisor
uint64_t gcd(uint64_t a, uint64_t b) {
    if (b == 0) {
        return a;
    } else {
        return gcd(b, a % b);
    }
}

uint64_t lcm(uint64_t a, uint64_t b) {
    return a * b / gcd(a,b);
}

int is_prime(uint64_t num) {
    if (num <= 1) {
        return 0;
    }
    for (uint64_t i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

// Function to generate a random prime number in the range [min, max]
uint64_t generate_random_prime(uint64_t min, uint64_t max) {
    uint64_t prime = 0;
    do {
        prime = min + rand() % (max - min + 1);
    } while (!is_prime_miller(prime));
    return prime;
}

// Function to compute the modular inverse of a number
uint64_t mod_inverse(uint64_t a, uint64_t m) {
    uint64_t m0 = m, t, q;
    uint64_t x0 = 0, x1 = 1;

    if (m == 1) {
        return 0;
    }

    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0; //overflow?
        x1 = t;
    }

    if (x1 < 0) {
        x1 += m0;
    }

    return x1;
}

void generate_and_output_key(FILE *output) {
    srand(time(NULL));
    uint64_t p = generate_random_prime(1ULL << 31, (1ULL << 32) - 1);
    uint64_t q = generate_random_prime(1ULL << 31, (1ULL << 32) - 1);
    uint64_t n = p * q;
    uint64_t carm_tot = lcm((p - 1),(q - 1));
    //the Carmichael function λ(n) of a positive integer n is the smallest positive integer m such that 
    // a^m === 1 (mod n)
    uint64_t phi = (p - 1) * (q - 1);

    uint64_t e = 0;
    do {
        e = 2 + rand() % (phi - 2);
    } while (gcd(e, phi) != 1);

    uint64_t d = mod_inverse(e, phi);

    fprintf(output, "RSA Key:\n");
    fprintf(output, "p: %lu\n", p);
    fprintf(output, "q: %lu\n", q);
    fprintf(output, "n: %lu\n", n);
    fprintf(output, "e: %lu\n", e);
    fprintf(output, "d: %lu\n", d);
    fprintf(output, "phi: %lu\n", phi);
    fprintf(output, "Carmichael's totient: %lu\n", carm_tot);
}

int main(int argc, char *argv[]) {
    int genrsa = 0;
    int output_to_file = 0;
    char *output_filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "genrsa") == 0) {
            genrsa = 1;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_to_file = 1;
            output_filename = argv[++i];
        }
    }

    if (genrsa) {
        if (output_to_file) {
            FILE *output_file = fopen(output_filename, "w");
            if (output_file == NULL) {
                printf("Error opening output file\n");
                return 1;
            }
            generate_and_output_key(output_file);
            fclose(output_file);
        } else {
            generate_and_output_key(stdout);
        }
    } else {
        printf("Usage: %s genrsa [-o output_file]\n", argv[0]);
    }

    return 0;
}
