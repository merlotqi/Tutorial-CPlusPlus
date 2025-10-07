#include "crc8.h"

// Reflect a byte (bit reversal)
uint8_t crc8_reflect_byte(uint8_t data) {
    uint8_t reflected = 0;
    for (int i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            reflected |= (1 << (7 - i));
        }
    }
    return reflected;
}

// Generate CRC-8 lookup table
void crc8_generate_table(crc8_ctx_t *ctx) {
    uint8_t polynomial = ctx->reflect_input ? crc8_reflect_byte(ctx->polynomial) : ctx->polynomial;

    for (int i = 0; i < 256; i++) {
        uint8_t crc = (uint8_t) i;

        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }

        ctx->table[i] = crc;
    }
}

// Initialize CRC-8 context with predefined polynomial
void crc8_init(crc8_ctx_t *ctx, crc8_poly_t poly, crc8_method_t method) {
    // Set polynomial and default parameters based on standard
    if (poly == CRC8_POLY_DEFAULT || poly == CRC8_POLY_ITU || poly == CRC8_POLY_ROHC) {
        ctx->polynomial = 0x07;
        ctx->initial_value = 0x00;
        ctx->final_xor = 0x00;
        ctx->reflect_input = 0;
        ctx->reflect_output = 0;
    } else if (poly == CRC8_POLY_CDMA2000 || poly == CRC8_POLY_WCDMA) {
        ctx->polynomial = 0x9B;
        ctx->initial_value = 0xFF;
        ctx->final_xor = 0x00;
        ctx->reflect_input = 0;
        ctx->reflect_output = 0;
    } else if (poly == CRC8_POLY_DARC) {
        ctx->polynomial = 0x39;
        ctx->initial_value = 0x00;
        ctx->final_xor = 0x00;
        ctx->reflect_input = 1;
        ctx->reflect_output = 1;
    } else if (poly == CRC8_POLY_DVB_S2) {
        ctx->polynomial = 0xD5;
        ctx->initial_value = 0x00;
        ctx->final_xor = 0x00;
        ctx->reflect_input = 0;
        ctx->reflect_output = 0;
    } else if (poly == CRC8_POLY_EBU || poly == CRC8_POLY_I_CODE) {
        ctx->polynomial = 0x1D;
        ctx->initial_value = 0xFF;
        ctx->final_xor = 0x00;
        ctx->reflect_input = 1;
        ctx->reflect_output = 1;
    } else if (poly == CRC8_POLY_MAXIM) {
        ctx->polynomial = 0x31;
        ctx->initial_value = 0x00;
        ctx->final_xor = 0x00;
        ctx->reflect_input = 1;
        ctx->reflect_output = 1;
    } else {
        ctx->polynomial = 0x07;
        ctx->initial_value = 0x00;
        ctx->final_xor = 0x00;
        ctx->reflect_input = 0;
        ctx->reflect_output = 0;
    }

    ctx->method = method;

    // Generate lookup table if using table method
    if (method == CRC8_METHOD_TABLE) {
        crc8_generate_table(ctx);
    }
}

// Initialize CRC-8 context with custom parameters
void crc8_init_custom(crc8_ctx_t *ctx, uint8_t poly, uint8_t init, uint8_t final_xor, uint8_t reflect_in,
                      uint8_t reflect_out) {
    ctx->polynomial = poly;
    ctx->initial_value = init;
    ctx->final_xor = final_xor;
    ctx->reflect_input = reflect_in;
    ctx->reflect_output = reflect_out;
    ctx->method = CRC8_METHOD_TABLE;// Default to table method for custom

    crc8_generate_table(ctx);
}

// Calculate CRC-8 using bitwise method
static uint8_t crc8_bitwise(const crc8_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint8_t crc = ctx->initial_value;
    uint8_t polynomial = ctx->polynomial;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        // Reflect input byte if required
        if (ctx->reflect_input) {
            byte = crc8_reflect_byte(byte);
        }

        // XOR byte with current CRC
        crc ^= byte;

        // Process 8 bits
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }

    // Reflect output if required
    if (ctx->reflect_output) {
        crc = crc8_reflect_byte(crc);
    }

    // Apply final XOR
    crc ^= ctx->final_xor;

    return crc;
}

// Calculate CRC-8 using lookup table method
static uint8_t crc8_table(const crc8_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint8_t crc = ctx->initial_value;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        // Reflect input if required and XOR with CRC
        if (ctx->reflect_input) {
            crc = ctx->table[crc ^ crc8_reflect_byte(byte)];
        } else {
            crc = ctx->table[crc ^ byte];
        }
    }

    // Reflect output if required
    if (ctx->reflect_output) {
        crc = crc8_reflect_byte(crc);
    }

    // Apply final XOR
    crc ^= ctx->final_xor;

    return crc;
}

// Update CRC-8 with new data (useful for streaming)
uint8_t crc8_update(const crc8_ctx_t *ctx, uint8_t crc, const uint8_t *data, size_t len) {
    if (ctx->method == CRC8_METHOD_TABLE) {
        for (size_t i = 0; i < len; i++) {
            uint8_t byte = data[i];
            if (ctx->reflect_input) {
                crc = ctx->table[crc ^ crc8_reflect_byte(byte)];
            } else {
                crc = ctx->table[crc ^ byte];
            }
        }
    } else {
        uint8_t polynomial = ctx->polynomial;

        for (size_t i = 0; i < len; i++) {
            uint8_t byte = data[i];

            if (ctx->reflect_input) {
                byte = crc8_reflect_byte(byte);
            }

            crc ^= byte;

            for (int j = 0; j < 8; j++) {
                if (crc & 0x80) {
                    crc = (crc << 1) ^ polynomial;
                } else {
                    crc <<= 1;
                }
            }
        }
    }

    return crc;
}

// Calculate CRC-8 for complete data block
uint8_t crc8_calculate(const crc8_ctx_t *ctx, const uint8_t *data, size_t len) {
    uint8_t crc;

    if (ctx->method == CRC8_METHOD_TABLE) {
        crc = crc8_table(ctx, data, len);
    } else {
        crc = crc8_bitwise(ctx, data, len);
    }

    return crc;
}

// One-shot CRC-8 calculation with standard polynomial
uint8_t crc8_standard(const uint8_t *data, size_t len) {
    crc8_ctx_t ctx;
    crc8_init(&ctx, CRC8_POLY_DEFAULT, CRC8_METHOD_TABLE);
    return crc8_calculate(&ctx, data, len);
}

// One-shot CRC-8 calculation with MAXIM polynomial
uint8_t crc8_maxim(const uint8_t *data, size_t len) {
    crc8_ctx_t ctx;
    crc8_init(&ctx, CRC8_POLY_MAXIM, CRC8_METHOD_TABLE);
    return crc8_calculate(&ctx, data, len);
}

// One-shot CRC-8 calculation with DVB-S2 polynomial
uint8_t crc8_dvb_s2(const uint8_t *data, size_t len) {
    crc8_ctx_t ctx;
    crc8_init(&ctx, CRC8_POLY_DVB_S2, CRC8_METHOD_TABLE);
    return crc8_calculate(&ctx, data, len);
}