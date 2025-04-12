#include "Crc.hpp"

namespace prvt {

static const qint16 poly_val = 0x1021;
static const qint16 seed_val = 0xFFFF;

qint16 updateCRC(qint16 acc, const qint8 input)
{
    acc ^= (input << 8);
    for (qint8 i = 0; i < 8; i++) {
        if ((acc & 0x8000) == 0x8000) {
            acc <<= 1;
            acc ^= poly_val;
        } else {
            acc <<= 1;
        }
    }
    return acc;
}

} // namespace prvt

qint16 calculateCRC(const char* data, const size_t len)
{
    qint16 crcout = prvt::seed_val;

    for (size_t i = 0; i < len; ++i) {
        crcout = prvt::updateCRC(crcout, data[i]);
    }

    return crcout;
}
