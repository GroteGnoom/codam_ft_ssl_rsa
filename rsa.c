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

struct eea_result{
    uint64_t x;
    uint64_t y;
    uint64_t gcd;
};

// Extended Euclidean Algorithm
struct eea_result extended_gcd(uint64_t a, uint64_t b) {
    if (b == 0) {
        struct eea_result result = {1, 0, a};
        return result;
    }

    struct eea_result next = extended_gcd(b, a % b);
    struct eea_result result;
    result.gcd = next.gcd;
    result.x = next.y;
    result.y = next.x - (a / b) * next.y;
    return result;
}

uint64_t mod_inverse(uint64_t a, uint64_t m) {
    struct eea_result er = extended_gcd(a, m);
    return er.x;
}

/*
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
    */

const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* encode_base64(uint64_t num) {
    char* output = (char*)malloc(11 * sizeof(char));
    int i, j;
    for (i = 0, j = 58; i < 10; i++, j -= 6) {
        output[i] = base64_table[(num >> j) & 0x3f];
    }
    output[10] = '\0';
    return output;
}

unsigned char* encode_base64_str(const unsigned char* input, int input_len) {
    int output_len = ((input_len + 2) / 3) * 4; // calculate output length
    unsigned char* output = malloc(output_len + 1); // allocate memory for output string
    if (output == NULL) {
        return NULL;
    }

    int i, j;
    for (i = 0, j = 0; i < input_len; i += 3, j += 4) {
        unsigned char b1 = input[i];
        unsigned char b2 = (i + 1 < input_len) ? input[i + 1] : 0;
        unsigned char b3 = (i + 2 < input_len) ? input[i + 2] : 0;

        unsigned char c1 = b1 >> 2;
        unsigned char c2 = ((b1 & 0x03) << 4) | (b2 >> 4);
        unsigned char c3 = ((b2 & 0x0f) << 2) | (b3 >> 6);
        unsigned char c4 = b3 & 0x3f;

        output[j] = base64_table[c1];
        output[j + 1] = base64_table[c2];
        output[j + 2] = (i + 1 < input_len) ? base64_table[c3] : '=';
        output[j + 3] = (i + 2 < input_len) ? base64_table[c4] : '=';
    }

    output[output_len] = '\0'; // add null terminator to end of string
    return output;
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
    fprintf(output, "phi / ct: %lu\n", phi / carm_tot);
    FILE *der_encoded = tmpfile();
    generate_and_output_key_pkcs1(der_encoded, e, d, p, q);
    rewind(der_encoded);
    unsigned char *tmp = read_file_to_string(der_encoded);
    unsigned char *enc = encode_base64_str(tmp, strlen((char *)tmp));
    //fprintf(output, "n, 64-bit encoded: %s\n", enc);
    fprintf(output, "base64 encoded der: %s\n", enc);
    free(enc);
}

