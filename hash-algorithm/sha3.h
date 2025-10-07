#ifndef HASH_SHA3_H
#define HASH_SHA3_H

#include <stddef.h>
#include <stdint.h>

// SHA-3 variants
typedef enum {
    SHA3_224 = 224,
    SHA3_256 = 256,
    SHA3_384 = 384,
    SHA3_512 = 512
} sha3_type_t;

typedef struct {
    uint64_t state[25]; // 5x5 state matrix of 64-bit words
    size_t rate;        // rate in bytes (1600 - 2*output_bits) / 8
    size_t capacity;    // capacity in bits (2 * output_bits)2
    size_t block_size;  // block size in bytes
    size_t output_size; // output size in bytes
    uint8_t buffer[200];// input buffer (max block size for SHA3-224)
} sha3_ctx_t;

void sha3_init(sha3_ctx_t *ctx, sha3_type_t type);
void sha3_update(sha3_ctx_t *ctx, const uint8_t *data, size_t len);
void sha3_final(sha3_ctx_t *ctx, uint8_t *digest);
void sha3(const uint8_t *data, size_t len, uint8_t *digest, sha3_type_t type);

void sha3_hex_string(const uint8_t *digest, size_t len, char *output);

#endif// HASH_SHA3_H