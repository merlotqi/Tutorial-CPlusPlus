#include "sha256.h"
#include <string.h>

// SHA-256 constants (first 32 bits of fractional parts of cube roots of first 64 primes)
static const uint32_t k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

// Initial hash values (first 32 bits of fractional parts of square roots of first 8 primes)
static const uint32_t h0[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                               0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

// Rotate right (circular right shift)
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

// Shift right
#define SHR(x, n) ((x) >> (n))

// SHA-256 functions
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

// Convert big-endian bytes to 32-bit word
static uint32_t be32(const uint8_t *bytes) {
    return ((uint32_t) bytes[0] << 24) | ((uint32_t) bytes[1] << 16) | ((uint32_t) bytes[2] << 8) |
           ((uint32_t) bytes[3]);
}

// Convert 32-bit word to big-endian bytes
static void store32(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t) (value >> 24);
    bytes[1] = (uint8_t) (value >> 16);
    bytes[2] = (uint8_t) (value >> 8);
    bytes[3] = (uint8_t) (value);
}

// SHA-256 transformation function - process one 512-bit block
static void sha256_transform(sha256_ctx_t *ctx, const uint8_t block[SHA256_BLOCK_SIZE]) {
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t w[64];
    int i;

    // Prepare message schedule
    for (i = 0; i < 16; i++) {
        w[i] = be32(block + i * 4);
    }

    // Extend the first 16 words into the remaining 48 words
    for (i = 16; i < 64; i++) {
        w[i] = SIG1(w[i - 2]) + w[i - 7] + SIG0(w[i - 15]) + w[i - 16];
    }

    // Initialize working variables with current hash value
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    // Main compression loop - 64 rounds
    for (i = 0; i < 64; i++) {
        uint32_t t1 = h + EP1(e) + CH(e, f, g) + k[i] + w[i];
        uint32_t t2 = EP0(a) + MAJ(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // Add the compressed chunk to the current hash value
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

// Initialize SHA-256 context
void sha256_init(sha256_ctx_t *ctx) {
    // Initialize state with initial hash values
    memcpy(ctx->state, h0, sizeof(h0));

    // Initialize bit count
    ctx->count[0] = 0;
    ctx->count[1] = 0;

    // Clear buffer
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

// Update SHA-256 context with new data
void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint32_t i, index, partlen;

    // Compute number of bytes mod 64
    index = (uint32_t) ((ctx->count[0] >> 3) & 0x3F);

    // Update number of bits
    if ((ctx->count[0] += ((uint32_t) len << 3)) < ((uint32_t) len << 3)) {
        ctx->count[1]++;
    }
    ctx->count[1] += ((uint32_t) len >> 29);

    partlen = SHA256_BLOCK_SIZE - index;

    // Transform as many times as possible
    if (len >= partlen) {
        memcpy(&ctx->buffer[index], data, partlen);
        sha256_transform(ctx, ctx->buffer);

        for (i = partlen; i + SHA256_BLOCK_SIZE <= len; i += SHA256_BLOCK_SIZE) {
            sha256_transform(ctx, &data[i]);
        }

        index = 0;
    } else {
        i = 0;
    }

    // Buffer remaining input
    memcpy(&ctx->buffer[index], &data[i], len - i);
}

// Finalize SHA-256 hash and produce digest
void sha256_final(sha256_ctx_t *ctx, uint8_t digest[SHA256_DIGEST_SIZE]) {
    uint8_t bits[8];
    uint32_t index, padlen;
    int i;

    // Save number of bits (big-endian)
    for (i = 0; i < 8; i++) {
        bits[i] = (uint8_t) ((ctx->count[(i >= 4) ? 0 : 1] >> ((3 - (i & 3)) * 8)) & 0xFF);
    }

    // Pad out to 56 mod 64
    index = (uint32_t) ((ctx->count[0] >> 3) & 0x3F);
    padlen = (index < 56) ? (56 - index) : (120 - index);

    uint8_t padding[SHA256_BLOCK_SIZE];
    memset(padding, 0, sizeof(padding));
    padding[0] = 0x80;// Append 1 bit followed by zeros

    sha256_update(ctx, padding, padlen);

    // Append length (before padding)
    sha256_update(ctx, bits, 8);

    // Store final state in digest (big-endian)
    for (i = 0; i < 8; i++) {
        store32(digest + (i * 4), ctx->state[i]);
    }

    // Clear sensitive data
    memset(ctx, 0, sizeof(*ctx));
}

// One-shot SHA-256 hash function
void sha256(const uint8_t *data, size_t len, uint8_t digest[SHA256_DIGEST_SIZE]) {
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, digest);
}

// Convert digest to hexadecimal string
void sha256_hex_string(const uint8_t digest[SHA256_DIGEST_SIZE], char *output) {
    static const char hex_digits[] = "0123456789abcdef";

    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        output[i * 2] = hex_digits[(digest[i] >> 4) & 0x0F];
        output[i * 2 + 1] = hex_digits[digest[i] & 0x0F];
    }
    output[SHA256_DIGEST_SIZE * 2] = '\0';
}