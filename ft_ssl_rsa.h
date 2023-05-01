#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool is_prime_miller(uint64_t n);
uint64_t mod_inverse(uint64_t a, uint64_t m);
void generate_and_output_key_pkcs1(FILE *output, uint64_t e, uint64_t d, uint64_t p, uint64_t q);
unsigned char* read_file_to_string(FILE *fp);
void generate_and_output_key(FILE *output);
