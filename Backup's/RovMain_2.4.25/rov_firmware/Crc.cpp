#include "Crc.h"

namespace  {

    //! CRC values
    static const int16_t poly_val = 0x1021;
    static const int16_t seed_val = 0xFFFF;

    int16_t updateCRC(int16_t acc, const int8_t input) {
        acc ^= (input << 8);
        for (int8_t i = 0; i < 8; i++) {
            if ((acc & 0x8000) == 0x8000) {
                acc <<= 1;
                acc ^= poly_val;
            }
            else {
                acc <<= 1;
            }
        }
        return acc;
    }

}

int16_t calculateCRC(const char* data, const unsigned long len) {
    int16_t crcout = ::seed_val;
    unsigned long i;
    for (i = 0; i < len; ++i) {
        crcout = ::updateCRC(crcout, data[i]);
    }

    return crcout;
}
