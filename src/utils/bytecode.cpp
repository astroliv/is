#include "bytecode.h"

uint64_t readArg(Instream &in, isize idx) {
    uint64_t result = 0;
    uint8_t tmp, i = 0;
    do {
        tmp = in[idx + i];
        result |= (i < 8 ? tmp & 0x7F : tmp) << (i * 7);
    } while ((tmp >> 7) && (i++ < 8));
    return result;
}

void writeArg(Instream &in, uint64_t arg) {
    uint8_t i = 0;
    do {
        in.append((arg >> 7) && (i < 8) ? arg & 0x7F | 0x80 : arg);
    } while ((arg >>= 7) && (i++ < 8));
}
