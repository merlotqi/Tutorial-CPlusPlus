#include "sha3.h"
#include <stdio.h>
#include <string.h>

// Keccak constants
static const uint64_t keccakf_round_constants[24] = {
        0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
        0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
        0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
        0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
        0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL, 0x800000008000000aULL,
        0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL};

// Rotation constants for Keccak
static const int keccakf_rotc[24] = {1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
                                     27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44};

// Rotation constants for Keccak (pi indices)
static const int keccakf_piln[24] = {10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
                                     15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1};

// Keccak-f[1600] permutation function
static void keccakf(uint64_t state[25]) {
    uint64_t t;
    uint64_t bc[5];

    // 24 rounds of Keccak-f
    for (int round = 0; round < 24; round++) {
        // Theta step
        for (int i = 0; i < 5; i++) {
            bc[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
        }

        for (int i = 0; i < 5; i++) {
            t = bc[(i + 4) % 5] ^ ((bc[(i + 1) % 5] << 1) | (bc[(i + 1) % 5] >> 63));
            for (int j = 0; j < 25; j += 5) {
                state[j + i] ^= t;
            }
        }

        // Rho and Pi steps
        t = state[1];
        for (int i = 0; i < 24; i++) {
            int j = keccakf_piln[i];
            bc[0] = state[j];
            state[j] = (t << keccakf_rotc[i]) | (t >> (64 - keccakf_rotc[i]));
            t = bc[0];
        }

        // Chi step
        for (int j = 0; j < 25; j += 5) {
            for (int i = 0; i < 5; i++) {
                bc[i] = state[j + i];
            }
            for (int i = 0; i < 5; i++) {
                state[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
            }
        }

        // Iota step
        state[0] ^= keccakf_round_constants[round];
    }
}

// Convert little-endian bytes to 64-bit word
static uint64_t load64(const uint8_t *bytes) {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result |= ((uint64_t) bytes[i]) << (8 * i);
    }
    return result;
}

// Convert 64-bit word to little-endian bytes
static void store64(uint8_t *bytes, uint64_t value) {
    for (int i = 0; i < 8; i++) {
        bytes[i] = (uint8_t) (value >> (8 * i));
    }
}

// Initialize SHA-3 context for specific hash type
void sha3_init(sha3_ctx_t *ctx, sha3_type_t type) {
    // Clear state
    memset(ctx->state, 0, sizeof(ctx->state));

    // Set parameters based on hash type
    size_t output_bits = (size_t) type;
    ctx->capacity = 2 * output_bits;
    ctx->rate = (1600 - ctx->capacity) / 8;
    ctx->block_size = ctx->rate;
    ctx->output_size = output_bits / 8;

    // Clear buffer
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

// Absorb data into SHA-3 state
void sha3_update(sha3_ctx_t *ctx, const uint8_t *data, size_t len) {
    size_t block_size = ctx->block_size;
    size_t buffer_pos = 0;

    // Process existing buffer content if any
    if (buffer_pos < len) {
        size_t to_copy = block_size - buffer_pos;
        if (to_copy > len) {
            to_copy = len;
        }

        memcpy(ctx->buffer + buffer_pos, data, to_copy);
        buffer_pos += to_copy;
        data += to_copy;
        len -= to_copy;

        if (buffer_pos == block_size) {
            // Process full block
            for (size_t i = 0; i < block_size; i += 8) {
                ctx->state[i / 8] ^= load64(ctx->buffer + i);
            }
            keccakf(ctx->state);
            buffer_pos = 0;
        }
    }

    // Process full blocks from input data
    while (len >= block_size) {
        for (size_t i = 0; i < block_size; i += 8) {
            ctx->state[i / 8] ^= load64(data + i);
        }
        keccakf(ctx->state);
        data += block_size;
        len -= block_size;
    }

    // Copy remaining data to buffer
    if (len > 0) {
        memcpy(ctx->buffer + buffer_pos, data, len);
    }
}

// Finalize SHA-3 hash and produce digest
void sha3_final(sha3_ctx_t *ctx, uint8_t *digest) {
    size_t block_size = ctx->block_size;
    size_t buffer_pos = 0;

    // Find position of last byte in buffer
    while (buffer_pos < block_size && ctx->buffer[buffer_pos] != 0) {
        buffer_pos++;
    }

    // Apply padding: 0x06 || 0x80 for SHA-3
    ctx->buffer[buffer_pos] = 0x06;// SHA-3 padding
    memset(ctx->buffer + buffer_pos + 1, 0, block_size - buffer_pos - 1);
    ctx->buffer[block_size - 1] |= 0x80;// End marker

    // Absorb final block
    for (size_t i = 0; i < block_size; i += 8) {
        ctx->state[i / 8] ^= load64(ctx->buffer + i);
    }

    // Final Keccak permutation
    keccakf(ctx->state);

    // Squeeze output
    for (size_t i = 0; i < ctx->output_size; i += 8) {
        store64(digest + i, ctx->state[i / 8]);
    }

    // Clear sensitive data
    memset(ctx, 0, sizeof(sha3_ctx_t));
}

// One-shot SHA-3 hash function
void sha3(const uint8_t *data, size_t len, uint8_t *digest, sha3_type_t type) {
    sha3_ctx_t ctx;
    sha3_init(&ctx, type);
    sha3_update(&ctx, data, len);
    sha3_final(&ctx, digest);
}

// Convert digest to hexadecimal string
void sha3_hex_string(const uint8_t *digest, size_t len, char *output) {
    for (size_t i = 0; i < len; i++) {
        sprintf(output + 2 * i, "%02x", digest[i]);
    }
    output[2 * len] = '\0';
}