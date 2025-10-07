#include "crc32.h"
#include <string.h>

// Reflect data (bit reversal) for specified number of bits
uint32_t crc32_reflect(uint32_t data, int bits) {
    uint32_t reflected = 0;

    for (int i = 0; i < bits; i++) {
        if (data & (1 << i)) {
            reflected |= (1 << (bits - 1 - i));
        }
    }

    return reflected;
}

// Reflect a byte (bit reversal)
uint8_t crc32_reflect_byte(uint8_t data) {
    return (uint8_t) crc32_reflect(data, 8);
}

// Generate standard 8-bit CRC-32 lookup table
void crc32_generate_table(crc32_ctx_t *ctx) {
    uint32_t polynomial = ctx->reflect_input ? crc32_reflect(ctx->polynomial, 32) : ctx->polynomial;

    for (int i = 0; i < 256; i++) {
        uint32_t crc = (uint32_t) i;

        if (ctx->reflect_input) {
            crc = crc32_reflect(crc, 8);
        }

        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }

        if (ctx->reflect_input) {
            crc = crc32_reflect(crc, 32);
        }

        ctx->table[i] = crc;
    }
}

// Generate 16-bit CRC-32 lookup table for faster computation
void crc32_generate_16bit_table(crc32_ctx_t *ctx) {
    // First generate the standard table
    crc32_generate_table(ctx);

    // Then generate 16-bit table by combining two 8-bit lookups
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            ctx->table_16bit[i][j] = (ctx->table[i] >> 8) ^ ctx->table[(ctx->table[i] & 0xFF) ^ j];
        }
    }
}

// Initialize CRC-32 context with predefined polynomial
void crc32_init(crc32_ctx_t *ctx, crc32_poly_t poly, crc32_method_t method) {
    // Set polynomial and default parameters based on standard
    switch (poly) {
        case CRC32_POLY_IEEE_8023:
            ctx->polynomial = 0x04C11DB7;
            ctx->initial_value = 0xFFFFFFFF;
            ctx->final_xor = 0xFFFFFFFF;
            ctx->reflect_input = 1;
            ctx->reflect_output = 1;
            break;

        case CRC32_POLY_CASTAGNOLI:
            ctx->polynomial = 0x1EDC6F41;
            ctx->initial_value = 0xFFFFFFFF;
            ctx->final_xor = 0xFFFFFFFF;
            ctx->reflect_input = 1;
            ctx->reflect_output = 1;
            break;

        case CRC32_POLY_KOOPMAN:
            ctx->polynomial = 0x741B8CD7;
            ctx->initial_value = 0xFFFFFFFF;
            ctx->final_xor = 0xFFFFFFFF;
            ctx->reflect_input = 1;
            ctx->reflect_output = 1;
            break;

        case CRC32_POLY_Q:
            ctx->polynomial = 0x814141AB;
            ctx->initial_value = 0x00000000;
            ctx->final_xor = 0x00000000;
            ctx->reflect_input = 0;
            ctx->reflect_output = 0;
            break;

        default:
            ctx->polynomial = 0x04C11DB7;
            ctx->initial_value = 0xFFFFFFFF;
            ctx->final_xor = 0xFFFFFFFF;
            ctx->reflect_input = 1;
            ctx->reflect_output = 1;
            break;
    }

    ctx->method = method;

    // Generate lookup tables if using table methods
    if (method == CRC32_METHOD_TABLE) {
        crc32_generate_table(ctx);
    } else if (method == CRC32_METHOD_TABLE_16BIT) {
        crc32_generate_16bit_table(ctx);
    }
}

// Initialize CRC-32 context with custom parameters
void crc32_init_custom(crc32_ctx_t *ctx, uint32_t poly, uint32_t init, uint32_t final_xor, uint8_t reflect_in,
                       uint8_t reflect_out) {
    ctx->polynomial = poly;
    ctx->initial_value = init;
    ctx->final_xor = final_xor;
    ctx->reflect_input = reflect_in;
    ctx->reflect_output = reflect_out;
    ctx->method = CRC32_METHOD_TABLE;// Default to table method

    crc32_generate_table(ctx);
}

// Calculate CRC-32 using bitwise method
static uint32_t crc32_bitwise(const crc32_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint32_t crc = ctx->initial_value;
    uint32_t polynomial = ctx->reflect_input ? crc32_reflect(ctx->polynomial, 32) : ctx->polynomial;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        // Reflect input byte if required
        if (ctx->reflect_input) {
            byte = crc32_reflect_byte(byte);
        }

        // XOR byte with current CRC (LSB for reflected)
        if (ctx->reflect_input) {
            crc ^= ((uint32_t) byte << 24);
        } else {
            crc ^= ((uint32_t) byte << 24);
        }

        // Process 8 bits
        for (int j = 0; j < 8; j++) {
            if (ctx->reflect_input) {
                if (crc & 0x80000000) {
                    crc = (crc << 1) ^ polynomial;
                } else {
                    crc <<= 1;
                }
            } else {
                if (crc & 0x80000000) {
                    crc = (crc << 1) ^ polynomial;
                } else {
                    crc <<= 1;
                }
            }
        }
    }

    // Reflect output if required
    if (ctx->reflect_output) {
        crc = crc32_reflect(crc, 32);
    }

    // Apply final XOR
    crc ^= ctx->final_xor;

    return crc;
}

// Calculate CRC-32 using 8-bit lookup table method
static uint32_t crc32_table_8bit(const crc32_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint32_t crc = ctx->initial_value;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        if (ctx->reflect_input) {
            crc = (crc >> 8) ^ ctx->table[(crc & 0xFF) ^ byte];
        } else {
            crc = (crc << 8) ^ ctx->table[((crc >> 24) & 0xFF) ^ byte];
        }
    }

    // Apply final operations
    if (ctx->reflect_output != ctx->reflect_input) {
        crc = crc32_reflect(crc, 32);
    }

    crc ^= ctx->final_xor;

    return crc;
}

// Calculate CRC-32 using 16-bit lookup table method (faster)
static uint32_t crc32_table_16bit(const crc32_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint32_t crc = ctx->initial_value;
    const uint8_t *current = data;
    size_t remaining = len;

    // Process data in 2-byte chunks when possible
    while (remaining >= 2) {
        uint16_t word = ((uint16_t) current[0] << 8) | current[1];

        if (ctx->reflect_input) {
            uint8_t low_byte = (crc & 0xFF);
            uint8_t high_byte = ((crc >> 8) & 0xFF);
            crc = (crc >> 16) ^ ctx->table_16bit[low_byte][high_byte ^ (word & 0xFF)];
            crc ^= ctx->table_16bit[0][(word >> 8) & 0xFF];
        } else {
            uint8_t high_byte = ((crc >> 24) & 0xFF);
            uint8_t next_byte = ((crc >> 16) & 0xFF);
            crc = (crc << 16) ^ ctx->table_16bit[high_byte][next_byte ^ ((word >> 8) & 0xFF)];
            crc ^= ctx->table_16bit[0][word & 0xFF];
        }

        current += 2;
        remaining -= 2;
    }

    // Process remaining single byte if any
    if (remaining > 0) {
        if (ctx->reflect_input) {
            crc = (crc >> 8) ^ ctx->table[(crc & 0xFF) ^ *current];
        } else {
            crc = (crc << 8) ^ ctx->table[((crc >> 24) & 0xFF) ^ *current];
        }
    }

    // Apply final operations
    if (ctx->reflect_output != ctx->reflect_input) {
        crc = crc32_reflect(crc, 32);
    }

    crc ^= ctx->final_xor;

    return crc;
}

// Update CRC-32 with new data (useful for streaming)
uint32_t crc32_update(const crc32_ctx_t *ctx, uint32_t crc, const uint8_t *data, size_t len) {
    switch (ctx->method) {
        case CRC32_METHOD_TABLE:
            for (size_t i = 0; i < len; i++) {
                if (ctx->reflect_input) {
                    crc = (crc >> 8) ^ ctx->table[(crc & 0xFF) ^ data[i]];
                } else {
                    crc = (crc << 8) ^ ctx->table[((crc >> 24) & 0xFF) ^ data[i]];
                }
            }
            break;

        case CRC32_METHOD_TABLE_16BIT:
            {
                const uint8_t *current = data;
                size_t remaining = len;

                while (remaining >= 2) {
                    uint16_t word = ((uint16_t) current[0] << 8) | current[1];

                    if (ctx->reflect_input) {
                        uint8_t low_byte = (crc & 0xFF);
                        uint8_t high_byte = ((crc >> 8) & 0xFF);
                        crc = (crc >> 16) ^ ctx->table_16bit[low_byte][high_byte ^ (word & 0xFF)];
                        crc ^= ctx->table_16bit[0][(word >> 8) & 0xFF];
                    } else {
                        uint8_t high_byte = ((crc >> 24) & 0xFF);
                        uint8_t next_byte = ((crc >> 16) & 0xFF);
                        crc = (crc << 16) ^ ctx->table_16bit[high_byte][next_byte ^ ((word >> 8) & 0xFF)];
                        crc ^= ctx->table_16bit[0][word & 0xFF];
                    }

                    current += 2;
                    remaining -= 2;
                }

                if (remaining > 0) {
                    if (ctx->reflect_input) {
                        crc = (crc >> 8) ^ ctx->table[(crc & 0xFF) ^ *current];
                    } else {
                        crc = (crc << 8) ^ ctx->table[((crc >> 24) & 0xFF) ^ *current];
                    }
                }
            }
            break;

        case CRC32_METHOD_BITWISE:
        default:
            {
                uint32_t polynomial = ctx->reflect_input ? crc32_reflect(ctx->polynomial, 32) : ctx->polynomial;

                for (size_t i = 0; i < len; i++) {
                    uint8_t byte = data[i];

                    if (ctx->reflect_input) {
                        byte = crc32_reflect_byte(byte);
                        crc ^= ((uint32_t) byte << 24);
                    } else {
                        crc ^= ((uint32_t) byte << 24);
                    }

                    for (int j = 0; j < 8; j++) {
                        if (crc & 0x80000000) {
                            crc = (crc << 1) ^ polynomial;
                        } else {
                            crc <<= 1;
                        }
                    }
                }
            }
            break;
    }

    return crc;
}

// Calculate CRC-32 for complete data block
uint32_t crc32_calculate(const crc32_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint32_t crc;

    switch (ctx->method) {
        case CRC32_METHOD_BITWISE:
            crc = crc32_bitwise(ctx, data, len);
            break;

        case CRC32_METHOD_TABLE:
            crc = crc32_table_8bit(ctx, data, len);
            break;

        case CRC32_METHOD_TABLE_16BIT:
            crc = crc32_table_16bit(ctx, data, len);
            break;

        default:
            crc = crc32_table_8bit(ctx, data, len);
            break;
    }

    return crc;
}

// One-shot CRC-32 calculation with IEEE polynomial
uint32_t crc32_ieee(const uint8_t *data, size_t len) {
    crc32_ctx_t ctx;
    crc32_init(&ctx, CRC32_POLY_IEEE_8023, CRC32_METHOD_TABLE);
    return crc32_calculate(&ctx, data, len);
}

// One-shot CRC-32 calculation with Castagnoli polynomial
uint32_t crc32_castagnoli(const uint8_t *data, size_t len) {
    crc32_ctx_t ctx;
    crc32_init(&ctx, CRC32_POLY_CASTAGNOLI, CRC32_METHOD_TABLE);
    return crc32_calculate(&ctx, data, len);
}

// One-shot CRC-32 calculation with Koopman polynomial
uint32_t crc32_koopman(const uint8_t *data, size_t len) {
    crc32_ctx_t ctx;
    crc32_init(&ctx, CRC32_POLY_KOOPMAN, CRC32_METHOD_TABLE);
    return crc32_calculate(&ctx, data, len);
}