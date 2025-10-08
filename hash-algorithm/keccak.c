#include "keccak.h"
#include <string.h>

// Keccak-f[1600] round constants
static const uint64_t keccakf_round_constants[24] = {
        0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
        0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
        0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
        0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
        0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL, 0x800000008000000aULL,
        0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL};

// Rotation offsets for Keccak-f[1600]
static const int keccakf_rotation_offsets[5][5] = {
        { 0,  1, 62, 28, 27},
        {36, 44,  6, 55, 20},
        { 3, 10, 43, 25, 39},
        {41, 45, 15, 21,  8},
        {18,  2, 61, 56, 14}
};

// Pi indices for Keccak-f[1600] permutation
static const int keccakf_pi_indices[5][5] = {
        {0, 3, 1, 4, 2},
        {1, 4, 2, 0, 3},
        {2, 0, 3, 1, 4},
        {3, 1, 4, 2, 0},
        {4, 2, 0, 3, 1}
};

// Rotate left for 64-bit values
static inline uint64_t keccak_rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

// Keccak-f[1600] permutation function
static void keccakf_permute(uint64_t state[25]) {
    uint64_t bc[5];
    uint64_t temp;
    int i, j, round;

    // 24 rounds of Keccak-f permutation
    for (round = 0; round < 24; round++) {
        // Theta step: Compute parity of columns
        for (i = 0; i < 5; i++) {
            bc[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
        }

        for (i = 0; i < 5; i++) {
            temp = bc[(i + 4) % 5] ^ keccak_rotl64(bc[(i + 1) % 5], 1);
            for (j = 0; j < 25; j += 5) {
                state[j + i] ^= temp;
            }
        }

        // Rho and Pi steps: Rotate and permute lanes
        temp = state[1];
        for (i = 0; i < 24; i++) {
            j = keccakf_pi_indices[i / 5][i % 5];
            bc[0] = state[j];
            state[j] = keccak_rotl64(temp, keccakf_rotation_offsets[i / 5][i % 5]);
            temp = bc[0];
        }

        // Chi step: Non-linear mixing
        for (j = 0; j < 25; j += 5) {
            for (i = 0; i < 5; i++) {
                bc[i] = state[j + i];
            }
            for (i = 0; i < 5; i++) {
                state[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
            }
        }

        // Iota step: Add round constant
        state[0] ^= keccakf_round_constants[round];
    }
}

// Convert little-endian bytes to 64-bit word
static uint64_t load64_le(const uint8_t *bytes) {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result |= ((uint64_t) bytes[i]) << (8 * i);
    }
    return result;
}

// Convert 64-bit word to little-endian bytes
static void store64_le(uint8_t *bytes, uint64_t value) {
    for (int i = 0; i < 8; i++) {
        bytes[i] = (uint8_t) (value >> (8 * i));
    }
}

// Initialize Keccak context for specific hash type
void keccak_init(keccak_ctx_t *ctx, keccak_type_t type) {
    // Clear state
    memset(ctx->state, 0, sizeof(ctx->state));

    // Set parameters based on hash type
    size_t output_bits = (size_t) type;
    size_t security_level = output_bits;

    // Capacity is 2 * security level for sponge construction
    ctx->capacity = 2 * security_level;
    ctx->rate = (1600 - ctx->capacity) / 8;// Rate in bytes
    ctx->output_size = output_bits / 8;

    // Set delimiter based on whether it's SHA3 or original Keccak
    switch (type) {
        case SHA3_224:
        case SHA3_256:
        case SHA3_384:
        case SHA3_512:
            ctx->delimiter = 0x06;// SHA3 uses 0x06 delimiter
            break;
        case KECCAK_224:
        case KECCAK_256:
        case KECCAK_384:
        case KECCAK_512:
        default:
            ctx->delimiter = 0x01;// Original Keccak uses 0x01
            break;
    }

    // Clear buffer and position
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    ctx->buffer_pos = 0;
}

// Absorb data into Keccak state
void keccak_absorb(keccak_ctx_t *ctx, const uint8_t *data, size_t len) {
    size_t block_size = ctx->rate;
    const uint8_t *data_ptr = data;
    size_t remaining = len;

    // Process data in blocks
    while (remaining > 0) {
        size_t to_process = block_size - ctx->buffer_pos;
        if (to_process > remaining) {
            to_process = remaining;
        }

        // Copy data to buffer
        memcpy(ctx->buffer + ctx->buffer_pos, data_ptr, to_process);
        ctx->buffer_pos += to_process;
        data_ptr += to_process;
        remaining -= to_process;

        // If buffer is full, process the block
        if (ctx->buffer_pos == block_size) {
            // XOR buffer into state
            for (size_t i = 0; i < block_size; i += 8) {
                ctx->state[i / 8] ^= load64_le(ctx->buffer + i);
            }

            // Apply Keccak permutation
            keccakf_permute(ctx->state);

            // Reset buffer position
            ctx->buffer_pos = 0;
        }
    }
}

// Finalize absorption and apply padding
void keccak_finalize(keccak_ctx_t *ctx) {
    size_t block_size = ctx->rate;

    // Apply padding: 1 || 0* || 1
    // Add delimiter (first 1)
    ctx->buffer[ctx->buffer_pos] = ctx->delimiter;
    ctx->buffer_pos++;

    // Fill the rest with zeros
    memset(ctx->buffer + ctx->buffer_pos, 0, block_size - ctx->buffer_pos);

    // Add final 1 at the end of the rate
    ctx->buffer[block_size - 1] ^= 0x80;

    // XOR padded buffer into state
    for (size_t i = 0; i < block_size; i += 8) {
        ctx->state[i / 8] ^= load64_le(ctx->buffer + i);
    }

    // Apply final permutation
    keccakf_permute(ctx->state);

    // Reset buffer for possible squeezing
    ctx->buffer_pos = 0;
}

// Squeeze output from Keccak state
void keccak_squeeze(keccak_ctx_t *ctx, uint8_t *output, size_t len) {
    size_t block_size = ctx->rate;
    size_t remaining = len;
    uint8_t *output_ptr = output;

    while (remaining > 0) {
        // If buffer is empty, squeeze a new block
        if (ctx->buffer_pos == 0) {
            // Copy state to buffer
            for (size_t i = 0; i < block_size; i += 8) {
                store64_le(ctx->buffer + i, ctx->state[i / 8]);
            }

            // Apply permutation for next squeeze (except for last block)
            if (remaining > block_size) {
                keccakf_permute(ctx->state);
            }
        }

        // Copy from buffer to output
        size_t to_copy = block_size - ctx->buffer_pos;
        if (to_copy > remaining) {
            to_copy = remaining;
        }

        memcpy(output_ptr, ctx->buffer + ctx->buffer_pos, to_copy);
        output_ptr += to_copy;
        ctx->buffer_pos += to_copy;
        remaining -= to_copy;

        // If buffer is consumed, reset position
        if (ctx->buffer_pos == block_size) {
            ctx->buffer_pos = 0;
        }
    }
}

// Complete Keccak hash operation
void keccak(keccak_ctx_t *ctx, const uint8_t *data, size_t len, uint8_t *output) {
    keccak_absorb(ctx, data, len);
    keccak_finalize(ctx);
    keccak_squeeze(ctx, output, ctx->output_size);
}

// One-shot Keccak hash function
void keccak_hash(const uint8_t *data, size_t len, uint8_t *output, keccak_type_t type) {
    keccak_ctx_t ctx;
    keccak_init(&ctx, type);
    keccak(&ctx, data, len, output);
}

// One-shot SHA3 hash function
void sha3_hash(const uint8_t *data, size_t len, uint8_t *output, keccak_type_t type) {
    keccak_ctx_t ctx;

    // Convert SHA3 type to equivalent Keccak type
    keccak_type_t keccak_type;
    switch (type) {
        case SHA3_224:
            keccak_type = KECCAK_224;
            break;
        case SHA3_256:
            keccak_type = KECCAK_256;
            break;
        case SHA3_384:
            keccak_type = KECCAK_384;
            break;
        case SHA3_512:
            keccak_type = KECCAK_512;
            break;
        default:
            keccak_type = type;
            break;
    }

    keccak_init(&ctx, keccak_type);
    ctx.delimiter = 0x06;// SHA3 uses different padding
    keccak(&ctx, data, len, output);
}

// Convert digest to hexadecimal string
void keccak_hex_string(const uint8_t *digest, size_t len, char *output) {
    static const char hex_digits[] = "0123456789abcdef";

    for (size_t i = 0; i < len; i++) {
        output[i * 2] = hex_digits[(digest[i] >> 4) & 0x0F];
        output[i * 2 + 1] = hex_digits[digest[i] & 0x0F];
    }
    output[len * 2] = '\0';
}