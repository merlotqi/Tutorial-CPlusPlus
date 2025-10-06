#ifndef HASH_MD5_H
#define HASH_MD5_H

#include <stddef.h>
#include <stdint.h>

#define MD5_BLOCK_SIZE (64)
#define MD5_DIGEST_SIZE (16)

typedef struct md5_ctx_s {
    uint32_t state[4];
    uint32_t count[2];
    uint8_t buffer[64];
} md5_ctx_t;

void md5_init(md5_ctx_t *ctx);
void md5_update(md5_ctx_t *ctx, const uint8_t *data, size_t len);
void md5_final(md5_ctx_t *ctx, uint8_t digest[MD5_DIGEST_SIZE]);
void md5(const uint8_t *data, size_t len, uint8_t digest[MD5_DIGEST_SIZE]);

#endif// HASH_MD5_H


#ifndef HASH_SHA1_H
#define HASH_SHA1_H

#endif// HASH_SHA1_H