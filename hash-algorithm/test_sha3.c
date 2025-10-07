#include "sha3.h"
#include <stdio.h>
#include <string.h>

void test_vector(const char *description, const uint8_t *data, size_t len, sha3_type_t type, const char *expected) {
    uint8_t digest[64];  // Maximum output size for SHA3-512
    char hex_output[129];// 64*2 + 1

    sha3(data, len, digest, type);
    sha3_hex_string(digest, type / 8, hex_output);

    printf("%s\n", description);
    printf("Expected: %s\n", expected);
    printf("Got:      %s\n", hex_output);
    printf("Result:   %s\n\n", strcmp(hex_output, expected) == 0 ? "PASS" : "FAIL");
}

int main() {
    printf("SHA-3 Test Vectors (NIST FIPS 202)\n");
    printf("===================================\n\n");

    // Test vectors from NIST FIPS 202

    // SHA3-224 empty string
    test_vector("SHA3-224(\"\")", (uint8_t *) "", 0, SHA3_224,
                "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7");

    // SHA3-256 empty string
    test_vector("SHA3-256(\"\")", (uint8_t *) "", 0, SHA3_256,
                "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a");

    // SHA3-384 empty string
    test_vector("SHA3-384(\"\")", (uint8_t *) "", 0, SHA3_384,
                "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004");

    // SHA3-512 empty string
    test_vector("SHA3-512(\"\")", (uint8_t *) "", 0, SHA3_512,
                "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f50019"
                "9d95b6d3e301758586281dcd26");

    // SHA3-256 of "abc"
    test_vector("SHA3-256(\"abc\")", (uint8_t *) "abc", 3, SHA3_256,
                "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532");

    // SHA3-512 of "abc"
    test_vector("SHA3-512(\"abc\")", (uint8_t *) "abc", 3, SHA3_512,
                "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b"
                "4cf408d5a56592f8274eec53f0");

    // Test with incremental updates
    printf("Testing incremental API:\n");
    uint8_t digest[64];
    sha3_ctx_t ctx;

    sha3_init(&ctx, SHA3_256);
    sha3_update(&ctx, (uint8_t *) "Hello, ", 7);
    sha3_update(&ctx, (uint8_t *) "World!", 6);
    sha3_final(&ctx, digest);

    char hex_output[65];
    sha3_hex_string(digest, 32, hex_output);
    printf("SHA3-256(\"Hello, World!\") = %s\n", hex_output);

    // Test with longer input
    sha3((uint8_t *) "The quick brown fox jumps over the lazy dog", 43, digest, SHA3_256);
    sha3_hex_string(digest, 32, hex_output);
    printf("SHA3-256(\"The quick brown fox jumps over the lazy dog\") = %s\n", hex_output);

    return 0;
}