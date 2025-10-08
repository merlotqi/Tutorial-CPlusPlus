#ifndef HASH_KECCAK_H
#define HASH_KECCAK_H

#include <stddef.h>
#include <stdint.h>

// Keccak variants (SHA-3 and original Keccak)
typedef enum {
    KECCAK_224,// Keccak-224
    KECCAK_256,// Keccak-256
    KECCAK_384,// Keccak-384
    KECCAK_512,// Keccak-512
    SHA3_224,  // SHA3-224
    SHA3_256,  // SHA3-256
    SHA3_384,  // SHA3-384
    SHA3_512   // SHA3-512
} keccak_type_t;

// Keccak context structure
typedef struct {
    uint64_t state[25]; // 5x5 state matrix of 64-bit words
    size_t rate;        // Rate in bytes (1600 - capacity) / 8
    size_t capacity;    // Capacity in bits (2 * security_level)
    size_t output_size; // Output size in bytes
    uint8_t delimiter;  // Delimiter for padding (0x01 for SHA3, 0x06 for Keccak)
    uint8_t buffer[144];// Input buffer (max block size for Keccak-224)
    size_t buffer_pos;  // Current position in buffer
} keccak_ctx_t;

// Function prototypes
void keccak_init(keccak_ctx_t *ctx, keccak_type_t type);
void keccak_absorb(keccak_ctx_t *ctx, const uint8_t *data, size_t len);
void keccak_finalize(keccak_ctx_t *ctx);
void keccak_squeeze(keccak_ctx_t *ctx, uint8_t *output, size_t len);
void keccak(keccak_ctx_t *ctx, const uint8_t *data, size_t len, uint8_t *output);

// One-shot functions
void keccak_hash(const uint8_t *data, size_t len, uint8_t *output, keccak_type_t type);
void sha3_hash(const uint8_t *data, size_t len, uint8_t *output, keccak_type_t type);

// Utility functions
void keccak_hex_string(const uint8_t *digest, size_t len, char *output);

#endif