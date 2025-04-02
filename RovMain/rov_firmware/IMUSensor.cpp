#include "IMUSensor.h"
#include <Arduino.h>
#include "wiring_private.h"

Uart SerialImu (&sercom1, 12, 11, SERCOM_RX_PAD_3, UART_TX_PAD_0);

void SERCOM1_Handler() {
    SerialImu.IrqHandler();
    IMUSensor::inst().update();
}

void IMUSensor::crc16_update(uint16_t *currectCrc, const uint8_t *src, uint32_t lengthInBytes)
{
    uint32_t crc = *currectCrc;
    uint32_t j;
    for (j = 0; j < lengthInBytes; ++j)
    {
        uint32_t i;
        uint32_t byte = src[j];
        crc ^= byte << 8;
        for (i = 0; i < 8; ++i)
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    }
    *currectCrc = crc;
}


uint32_t IMUSensor::packetDecode(uint8_t c)
{
    static uint16_t CRCReceived = 0; /* CRC value received from a frame */
    static uint16_t CRCCalculated = 0;  /* CRC value caluated from a frame */
    static uint8_t status = kStatus_Idle; /* state machine */
    static uint8_t crc_header[4] = { 0x5A, 0xA5, 0x00, 0x00 };

    switch (status)
    {
    case kStatus_Idle:
        if (c == 0x5A)
            status = kStatus_Cmd;
        break;
    case kStatus_Cmd:
        RxPkt.type = c;
        if (c == 0xA5) {
            status = kStatus_LenLow;
        } else {
            status = kStatus_Idle;
        }
        break;
    case kStatus_LenLow:
        RxPkt.payload_len = c;
        crc_header[2] = c;
        status = kStatus_LenHigh;
        break;
    case kStatus_LenHigh:
        RxPkt.payload_len |= (c << 8);
        crc_header[3] = c;
        status = kStatus_CRCLow;
        break;
    case kStatus_CRCLow:
        CRCReceived = c;
        status = kStatus_CRCHigh;
        break;
    case kStatus_CRCHigh:
        CRCReceived |= (c << 8);
        RxPkt.ofs = 0;
        CRCCalculated = 0;
        status = kStatus_Data;
        break;
    case kStatus_Data:
        if (RxPkt.ofs >= IMU_MAX_PACKET_LEN) {
            status = kStatus_Idle;
            RxPkt.ofs = 0;
            CRCCalculated = 0;
            break;
        }

        RxPkt.buf[RxPkt.ofs++] = c;

        if (RxPkt.ofs >= RxPkt.payload_len && RxPkt.type == 0xA5)
        {
            crc16_update(&CRCCalculated, crc_header, 4);
            crc16_update(&CRCCalculated, RxPkt.buf, RxPkt.ofs);

            /* CRC match */
            if (CRCCalculated == CRCReceived)
            {
                updateEuler(&RxPkt);
            } else {
                // SerialUSB.print("[IMU CRC FAIL] ");
                // SerialUSB.print("excepted: ");
                // SerialUSB.print(CRCReceived, HEX);
                // SerialUSB.print(", got: ");
                // SerialUSB.print(CRCCalculated, HEX);
                // SerialUSB.print("\n\t");
            }

            status = kStatus_Idle;
        }
        break;
    default:
        status = kStatus_Idle;
        break;
    }
}
void IMUSensor::updateEuler(Packet_t * pkt)
{
    if (pkt->buf[0] == kItemID) /* user ID */
    {
        ID = pkt->buf[1];
    }
    if (pkt->buf[2] == kItemAccRaw)  /* Acc raw value */
    {
        memcpy(AccRaw, (uint8_t *)pkt->buf[3], 6);
    }

    if (pkt->buf[9] == kItemGyoRaw)  /* gyro raw value */
    {
        memcpy(GyoRaw, (uint8_t *)pkt->buf[10], 6);
    }

    if (pkt->buf[16] == kItemMagRaw)  /* mag raw value */
    {
        memcpy(MagRaw, (uint8_t *)pkt->buf[17], 6);
    }
    if (pkt->buf[23] == kItemAtdE)  /* atd E */
    {
        float val = ((float)(int16_t)(pkt->buf[24] + (pkt->buf[25] << 8))) / 100;
        if (!isnan(val)) {
            Eular[0] = ((float)(int16_t)(pkt->buf[24] + (pkt->buf[25] << 8))) / 100;
        }
        val = ((float)(int16_t)(pkt->buf[26] + (pkt->buf[27] << 8))) / 100;
        if (!isnan(val)) {
            Eular[1] = ((float)(int16_t)(pkt->buf[26] + (pkt->buf[27] << 8))) / 100;
        }
        val = ((float)(int16_t)(pkt->buf[28] + (pkt->buf[29] << 8))) / 10;
        if (!isnan(val)) {
            Eular[2] = ((float)(int16_t)(pkt->buf[28] + (pkt->buf[29] << 8))) / 10;
        }
        m_isUpdated = true;
    }

}

IMUSensor::IMUSensor()
{
}

void IMUSensor::init()
{
    SerialImu.begin(115200);
    pinPeripheral(11, PIO_SERCOM);
    pinPeripheral(12, PIO_SERCOM);
    // SERIAL_BUFFER_SIZE
}
void IMUSensor::update()
{
    if (SerialImu.available()) {
        while (SerialImu.available())
        {
            char ch = SerialImu.read();
            inst().packetDecode(ch);
        }
    }
}

float IMUSensor::getPitch()
{
    return inst().Eular[0];
}
float IMUSensor::getRoll()
{
    return inst().Eular[1];
}
float IMUSensor::getYaw()
{
    return inst().Eular[2];
}
void IMUSensor::update_int()
{


}

IMUSensor & IMUSensor::inst() {
    static IMUSensor imu;
    return imu;
}
