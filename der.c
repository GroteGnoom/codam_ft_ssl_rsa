#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Function to write an ASN.1 length
void der_write_length(FILE *output, size_t length) {
    //I think we need it because of DER (instead of CER), because then 
    //the write_integer need a write_length
    if (length < 128) {
        fputc(length, output);
    } else {
        size_t num_bytes = (length / 256) + 1;
        fputc(0x80 | num_bytes, output);
        for (size_t i = num_bytes - 1; i < num_bytes; --i) {
            fputc((length >> (8 * i)) & 0xFF, output);
        }
    }
}

// Function to write an ASN.1 INTEGER
void der_write_integer(FILE *output, uint64_t value) {
    // Write the ASN.1 tag for INTEGER
    // defined in X.680, 8.6
    fputc(0x02, output);

    // Compute the number of bytes needed to represent the value
    size_t num_bytes = 1;
    for (uint64_t tmp = value; tmp > 0xFF; tmp >>= 8) {
        ++num_bytes;
    }

    // Write the length
    der_write_length(output, num_bytes);

    // Write the value in big-endian format
    // (big-endian not what intel architectures use, but 
    // is what normal people use.)
    // this starts out with the most significant part
    for (int i = num_bytes - 1; i >= 0; --i) {
        fputc((value >> (8 * i)) & 0xFF, output);
    }
}

// Function to write an ASN.1 SEQUENCE
void der_write_sequence(FILE *output, const unsigned char *content, size_t length) {
    // Write the ASN.1 tag for SEQUENCE
    fputc(0x30, output);

    // Write the length
    der_write_length(output, length);

    // Write the content
    fwrite(content, 1, length, output);
}


// Function to write an ASN.1 OCTET STRING
void der_write_octet_string(FILE *output, const unsigned char *content, size_t length) {
    //Not required for PKCS #1, only #8
    // Write the ASN.1 tag for OCTET STRING
    fputc(0x04, output);

    // Write the length
    der_write_length(output, length);

    // Write the content
    fwrite(content, 1, length, output);
}

// Function to write an ASN.1 NULL
void der_write_null(FILE *output) {
    //Not required for PKCS #1, only #8
    // Write the ASN.1 tag for NULL
    fputc(0x05, output);

    // Write the length
    der_write_length(output, 0);
}

// Function to write an ASN.1 OBJECT IDENTIFIER
void der_write_oid(FILE *output, const unsigned char *oid, size_t length) {
    //Not required for PKCS #1, only #8
    // Write the ASN.1 tag for OBJECT IDENTIFIER
    fputc(0x06, output);

    // Write the length
    der_write_length(output, length);

    // Write the content
    fwrite(oid, 1, length, output);
}

/*
void generate_and_output_key(FILE *output) {
    uint64_t n = 123; // Replace with your actual value
    uint64_t e = 65537; // Replace with your actual value
    uint64_t d = 456; // Replace with your actual value
    uint64_t p = 789; // Replace with your actual value
    uint64_t q = 101112; // Replace with your actual value
    uint64_t dp = d % (p - 1); // Replace with your actual value
    uint64_t dq = d % (q - 1); // Replace with your actual value
    uint64_t qi = q; // Replace with your actual value, this is (inverse of q) mod p

    // Create the PrivateKeyInfo structure
    unsigned char *private_key_info;
    size_t private_key_info_length;
    // The rest of the function...

    // The RSA encryption OID
    unsigned char rsa_oid[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01 };

    // Write the PrivateKeyAlgorithmIdentifier
    FILE *private_key_algorithm_identifier = tmpfile();
    der_write_oid(private_key_algorithm_identifier, rsa_oid, sizeof(rsa_oid));
    der_write_null(private_key_algorithm_identifier);

    // Compute the length of the PrivateKeyAlgorithmIdentifier
    size_t private_key_algorithm_identifier_length = ftell(private_key_algorithm_identifier);

    // Write the RSAPrivateKey
    FILE *rsa_private_key = tmpfile();
    der_write_integer(rsa_private_key, 0); // version
    der_write_integer(rsa_private_key, n);
    der_write_integer(rsa_private_key, e);
    der_write_integer(rsa_private_key, d);
    der_write_integer(rsa_private_key, p);
    der_write_integer(rsa_private_key, q);
    der_write_integer(rsa_private_key, dp);
    der_write_integer(rsa_private_key, dq);
    der_write_integer(rsa_private_key, qi);

    // Compute the length of the RSAPrivateKey
    size_t rsa_private_key_length = ftell(rsa_private_key);

    // Write the PrivateKey
    FILE *private_key = tmpfile();
    der_write_octet_string(private_key, rsa_private_key, rsa_private_key_length);

    // Compute the length of the PrivateKey
    size_t private_key_length = ftell(private_key);

    // Write the PrivateKeyInfo
    FILE *private_key_info = tmpfile();
    der_write_integer(private_key_info, 0); // version
    rewind(private_key_algorithm_identifier);
    der_write_sequence(private_key_info, private_key_algorithm_identifier, private_key_algorithm_identifier_length);
    rewind(private_key);
    der_write_octet_string(private_key_info, private_key, private_key_length);

    // Compute the length of the PrivateKeyInfo
    size_t private_key_info_length = ftell(private_key_info);

    // Write the PrivateKeyInfo to the output
    rewind(private_key_info);
    der_write_sequence(output, private_key_info, private_key_info_length);

    // Close the temporary files
    fclose(private_key_algorithm_identifier);
    fclose(rsa_private_key);
    fclose(private_key);
    fclose(private_key_info);
}
*/

unsigned char* read_file_to_string(FILE *fp) {
    /* FILE* fp = fopen(filename, "rb"); // open file in binary mode */
    /* if (fp == NULL) { */
    /*     return NULL; // error opening file */
    /* } */

    // determine file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // allocate buffer to hold file contents
    unsigned char* buffer = malloc(size + 1);
    if (buffer == NULL) {
        fclose(fp);
        return NULL; // error allocating memory
    }

    // read file contents into buffer
    size_t bytes_read = fread(buffer, 1, size, fp);
    if (bytes_read < size) {
        fclose(fp);
        //free(buffer);
        printf("error reading file\n");
        exit(1);
        return NULL; // error reading file
    }
    buffer[size] = '\0'; // add null terminator to end of buffer

    fclose(fp);
    return buffer;
}

void generate_and_output_key_pkcs1(FILE *output) {
    // Generate the RSA parameters
    uint64_t n = 123; // Replace with your actual value
    uint64_t e = 65537; // Replace with your actual value
    uint64_t d = 456; // Replace with your actual value
    uint64_t p = 789; // Replace with your actual value
    uint64_t q = 101112; // Replace with your actual value
    uint64_t dp = d % (p - 1); // Replace with your actual value
    uint64_t dq = d % (q - 1); // Replace with your actual value
    uint64_t qi = q; // Replace with your actual value, this is (inverse of q) mod p

    // Write the RSAPrivateKey
    FILE *rsa_private_key = tmpfile();
    der_write_integer(rsa_private_key, 0); // version
    der_write_integer(rsa_private_key, n);
    der_write_integer(rsa_private_key, e);
    der_write_integer(rsa_private_key, d);
    der_write_integer(rsa_private_key, p);
    der_write_integer(rsa_private_key, q);
    der_write_integer(rsa_private_key, dp);
    der_write_integer(rsa_private_key, dq);
    der_write_integer(rsa_private_key, qi);

    // Compute the length of the RSAPrivateKey
    size_t rsa_private_key_length = ftell(rsa_private_key);

    // Write the RSAPrivateKey to the output
    rewind(rsa_private_key);
    unsigned char *tmp = read_file_to_string(rsa_private_key);
    der_write_sequence(output, tmp, rsa_private_key_length);
    //free(tmp);

    // Close the temporary file
    //fclose(rsa_private_key);
}

#ifdef DER_TEST
int main() {
    generate_and_output_key_pkcs1(stdout);
}
#endif

