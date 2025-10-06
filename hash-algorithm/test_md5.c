#include "hash.h"
#include <stdio.h>
#include <string.h>


void print_digest(const uint8_t digest[MD5_DIGEST_SIZE]) {
    for (int i = 0; i < MD5_DIGEST_SIZE; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}

int main() {
    uint8_t digest[MD5_DIGEST_SIZE];

    printf("MD5 Test Vectors:\n");

    md5((uint8_t *) "", 0, digest);
    printf("MD5(\"\") = ");
    print_digest(digest);

    // "a"
    md5((uint8_t *) "a", 1, digest);
    printf("MD5(\"a\") = ");
    print_digest(digest);

    // "abc"
    md5((uint8_t *) "abc", 3, digest);
    printf("MD5(\"abc\") = ");
    print_digest(digest);

    // "message digest"
    md5((uint8_t *) "message digest", 14, digest);
    printf("MD5(\"message digest\") = ");
    print_digest(digest);

    md5_ctx_t ctx;
    md5_init(&ctx);
    md5_update(&ctx, (uint8_t *) "Hello, ", 7);
    md5_update(&ctx, (uint8_t *) "World!", 6);
    md5_final(&ctx, digest);
    printf("MD5(\"Hello, World!\") = ");
    print_digest(digest);

    return 0;
}