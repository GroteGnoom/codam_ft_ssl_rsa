#include "ft_ssl_rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
