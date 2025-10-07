#ifndef SHAH_SHA256_H
#define SHAH_SHA256_H

#include <stddef.h>
#include <stdint.h>

// SHA-256 digest size in bytes
#define SHA256_DIGEST_SIZE 32
#define SHA256_BLOCK_SIZE 64

// SHA-256 context structure
typedef struct {
    uint32_t state[8];                // Intermediate hash state (A,B,C,D,E,F,G,H)
    uint32_t count[2];                // Number of bits processed (low, high)
    uint8_t buffer[SHA256_BLOCK_SIZE];// Data buffer for block processing
} sha256_ctx_t;

// Function prototypes
void sha256_init(sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len);
void sha256_final(sha256_ctx_t *ctx, uint8_t digest[SHA256_DIGEST_SIZE]);
void sha256(const uint8_t *data, size_t len, uint8_t digest[SHA256_DIGEST_SIZE]);

// Utility functions
void sha256_hex_string(const uint8_t digest[SHA256_DIGEST_SIZE], char *output);

#endif// SHAH_SHA256_H