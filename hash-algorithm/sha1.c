#include "sha1.h"
#include <string.h>

#define SHA1_ROTL(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

#define SHA1_F1(b, c, d) (((b) & (c)) | (~(b) & (d)))
#define SHA1_F2(b, c, d) ((b) ^ (c) ^ (d))
#define SHA1_F3(b, c, d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))
#define SHA1_F4(b, c, d) ((b) ^ (c) ^ (d))

#define SHA1_K1 (0x5A827999)
#define SHA1_K2 (0x6ED9EBA1)
#define SHA1_K3 (0x8F1BBCDC)
#define SHA1_K4 (0xCA62C1D6)


static void sha1_transform(uint32_t state[5], const uint8_t block[SHA1_BLOCK_SIZE]) {
    uint32_t w[80];
    uint32_t a, b, c, d, e, t;

    // Copy the block into the message schedule array
    for (t = 0; t < 16; t++) {
        w[t] = ((uint32_t) block[t * 4]) << 24;
        w[t] |= ((uint32_t) block[t * 4 + 1]) << 16;
        w[t] |= ((uint32_t) block[t * 4 + 2]) << 8;
        w[t] |= ((uint32_t) block[t * 4 + 3]);
    }

    // Expand the message schedule array
    for (t = 16; t < 80; t++) {
        w[t] = SHA1_ROTL(1, w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16]);
    }

    // Initialize the working variables
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // Perform the main compression loop
    for (t = 0; t < 80; t++) {
        uint32_t temp = SHA1_ROTL(5, a);

        if (t < 20) {
            temp += SHA1_F1(b, c, d) + SHA1_K1;
        } else if (t < 40) {
            temp += SHA1_F2(b, c, d) + SHA1_K2;
        } else if (t < 60) {
            temp += SHA1_F3(b, c, d) + SHA1_K3;
        } else {
            temp += SHA1_F4(b, c, d) + SHA1_K4;
        }

        temp += e + w[t];
        e = d;
        d = c;
        c = SHA1_ROTL(30, b);
        b = a;
        a = temp;
    }

    // Update the state variables
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void sha1_init(sha1_ctx_t *ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count[0] = ctx->count[1] = 0;
}

void sha1_update(sha1_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint32_t i, index, partlen;

    // calculate how many bytes into the 64-byte chunk we are
    index = (uint32_t) ((ctx->count[0] >> 3) & 0x3F);

    // update the number of bits
    if ((ctx->count[0] += ((uint32_t) len << 3)) < ((uint32_t) len << 3)) {
        ctx->count[1]++;
    }
    ctx->count[1] += ((uint32_t) len >> 29);

    partlen = SHA1_BLOCK_SIZE - index;

    if (len >= partlen) {
        memcpy(&ctx->buffer[index], data, partlen);
        sha1_transform(ctx->state, ctx->buffer);

        for (i = partlen; i + SHA1_BLOCK_SIZE <= len; i += SHA1_BLOCK_SIZE) {
            sha1_transform(ctx->state, &data[i]);
        }

        index = 0;
    } else {
        i = 0;
    }

    memcpy(&ctx->buffer[index], &data[i], len - i);
}

void sha1_final(sha1_ctx_t *ctx, uint8_t digest[SHA1_DIGEST_SIZE]) {
    uint8_t bits[8];
    uint32_t index, padlen;
    int i;

    for (i = 0; i < 8; i++) {
        bits[i] = (uint8_t) ((ctx->count[(i >= 4) ? 0 : 1] >> ((3 - (i & 3)) * 8)) & 0xFF);
    }

    index = (uint32_t) ((ctx->count[0] >> 3) & 0x3F);
    padlen = (index < 56) ? (56 - index) : (120 - index);

    uint8_t padding[SHA1_BLOCK_SIZE];
    memset(padding, 0, sizeof(padding));
    padding[0] = 0x80;

    sha1_update(ctx, padding, padlen);
    sha1_update(ctx, bits, 8);

    for (i = 0; i < 5; i++) {
        digest[i * 4] = (uint8_t) ((ctx->state[i] >> 24) & 0xFF);
        digest[i * 4 + 1] = (uint8_t) ((ctx->state[i] >> 16) & 0xFF);
        digest[i * 4 + 2] = (uint8_t) ((ctx->state[i] >> 8) & 0xFF);
        digest[i * 4 + 3] = (uint8_t) (ctx->state[i] & 0xFF);
    }

    memset(ctx, 0, sizeof(*ctx));
}

void sha1(const uint8_t *data, size_t len, uint8_t digest[SHA1_DIGEST_SIZE]) {
    sha1_ctx_t ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, data, len);
    sha1_final(&ctx, digest);
}