#include "sha1.h"
#include <stdio.h>
#include <string.h>

void print_digest(const uint8_t digest[SHA1_DIGEST_SIZE]) {
    for (int i = 0; i < SHA1_DIGEST_SIZE; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}

int main() {
    uint8_t digest[SHA1_DIGEST_SIZE];

    printf("SHA-1 Test Vectors:\n\n");

    // csae 1: ""
    sha1((uint8_t *) "", 0, digest);
    printf("SHA1(\"\") = ");
    print_digest(digest);

    // case 2: "abc"
    sha1((uint8_t *) "abc", 3, digest);
    printf("SHA1(\"abc\") = ");
    print_digest(digest);

    // case 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    sha1((uint8_t *) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56, digest);
    printf("SHA1(\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\") = ");
    print_digest(digest);

    // case 4: long string
    sha1((uint8_t *) "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsm"
                     "nopqrstnopqrstu",
         112, digest);
    printf("SHA1("
           "\"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstn"
           "opqrstu\") = ");
    print_digest(digest);

    // case 5: "Hello, World!"
    sha1_ctx_t ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, (uint8_t *) "Hello, ", 7);
    sha1_update(&ctx, (uint8_t *) "World!", 6);
    sha1_final(&ctx, digest);
    printf("SHA1(\"Hello, World!\") = ");
    print_digest(digest);

    // case 6: "The quick brown fox jumps over the lazy dog"
    sha1((uint8_t *) "The quick brown fox jumps over the lazy dog", 43, digest);
    printf("SHA1(\"The quick brown fox jumps over the lazy dog\") = ");
    print_digest(digest);

    // case 7: "The quick brown fox jumps over the lazy cog"
    sha1((uint8_t *) "The quick brown fox jumps over the lazy cog", 43, digest);
    printf("SHA1(\"The quick brown fox jumps over the lazy cog\") = ");
    print_digest(digest);

    return 0;
}