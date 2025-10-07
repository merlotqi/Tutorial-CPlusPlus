#include "sha256.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_vector(const char *description, const uint8_t *data, size_t len, const char *expected) {
    uint8_t digest[SHA256_DIGEST_SIZE];
    char hex_output[65];

    sha256(data, len, digest);
    sha256_hex_string(digest, hex_output);

    printf("%s\n", description);
    printf("Expected: %s\n", expected);
    printf("Got:      %s\n", hex_output);
    printf("Result:   %s\n\n", strcmp(hex_output, expected) == 0 ? "PASS" : "FAIL");
}

void test_incremental(const char *description, const uint8_t *parts[], const size_t lengths[], int num_parts,
                      const char *expected) {
    sha256_ctx_t ctx;
    uint8_t digest[SHA256_DIGEST_SIZE];
    char hex_output[65];

    sha256_init(&ctx);
    for (int i = 0; i < num_parts; i++) {
        sha256_update(&ctx, parts[i], lengths[i]);
    }
    sha256_final(&ctx, digest);
    sha256_hex_string(digest, hex_output);

    printf("%s\n", description);
    printf("Expected: %s\n", expected);
    printf("Got:      %s\n", hex_output);
    printf("Result:   %s\n\n", strcmp(hex_output, expected) == 0 ? "PASS" : "FAIL");
}

void performance_test(const char *description, const uint8_t *data, size_t len, int iterations) {
    uint8_t digest[SHA256_DIGEST_SIZE];
    char hex_output[65];

    printf("%s (%zu bytes, %d iterations):\n", description, len, iterations);

    // Time measurement would be implemented here
    for (int i = 0; i < iterations; i++) {
        sha256(data, len, digest);
    }

    sha256_hex_string(digest, hex_output);
    printf("Hash: %s\n\n", hex_output);
}

int main() {
    printf("SHA-256 Implementation Tests\n");
    printf("============================\n\n");

    // NIST test vectors
    printf("NIST Test Vectors:\n");
    printf("==================\n");

    // Test vector 1: Empty string
    test_vector("SHA-256(\"\")", (uint8_t *) "", 0, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

    // Test vector 2: "abc"
    test_vector("SHA-256(\"abc\")", (uint8_t *) "abc", 3,
                "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

    // Test vector 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    test_vector("SHA-256(\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\")",
                (uint8_t *) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56,
                "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");

    // Test vector 4: Long string of 'a' characters
    printf("Long string test (1,000,000 'a' characters):\n");
    uint8_t long_string[1000];
    memset(long_string, 'a', sizeof(long_string));

    sha256_ctx_t ctx;
    uint8_t digest[SHA256_DIGEST_SIZE];
    char hex_output[65];

    sha256_init(&ctx);
    for (int i = 0; i < 1000; i++) {
        sha256_update(&ctx, long_string, sizeof(long_string));
    }
    sha256_final(&ctx, digest);
    sha256_hex_string(digest, hex_output);

    printf("Expected: cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0\n");
    printf("Got:      %s\n", hex_output);
    printf("Result:   %s\n\n",
           strcmp(hex_output, "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0") == 0 ? "PASS"
                                                                                                       : "FAIL");

    // Incremental hashing test
    printf("Incremental Hashing Test:\n");
    printf("=========================\n");

    const uint8_t *parts[] = {(uint8_t *) "Hello, ", (uint8_t *) "world! ", (uint8_t *) "This is ",
                              (uint8_t *) "incremental hashing."};
    const size_t lengths[] = {7, 7, 8, 19};

    test_incremental("SHA-256(\"Hello, world! This is incremental hashing.\")", parts, lengths, 4,
                     "a591a6d40bf420404a011733cfb7b190d62c65bf0bcda32b57b277d9ad9f146e");

    // Common strings test
    printf("Common Strings Test:\n");
    printf("====================\n");

    test_vector("SHA-256(\"Hello, World!\")", (uint8_t *) "Hello, World!", 13,
                "dffd6021bb2bd5b0af676290809ec3a53191dd81c7f70a4b28688a362182986f");

    test_vector("SHA-256(\"The quick brown fox jumps over the lazy dog\")",
                (uint8_t *) "The quick brown fox jumps over the lazy dog", 43,
                "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");

    test_vector("SHA-256(\"The quick brown fox jumps over the lazy cog\")",
                (uint8_t *) "The quick brown fox jumps over the lazy cog", 43,
                "e4c4d8f3bf76b692de791a173e05321150f7a345b46484fe427f6acc7ecc81be");

    // Performance test with moderate data
    uint8_t test_data[1024];
    for (size_t i = 0; i < sizeof(test_data); i++) {
        test_data[i] = (uint8_t) (i & 0xFF);
    }
    performance_test("Performance test", test_data, sizeof(test_data), 10000);

    // Edge case tests
    printf("Edge Case Tests:\n");
    printf("================\n");

    // Single byte
    test_vector("SHA-256(single byte 0x00)", (uint8_t *) "\x00", 1,
                "6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d");

    // Exactly one block (64 bytes)
    uint8_t one_block[64];
    memset(one_block, 0x61, sizeof(one_block));// All 'a'
    test_vector("SHA-256(64 bytes of 'a')", one_block, sizeof(one_block),
                "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb");

    printf("All tests completed!\n");

    return 0;
}