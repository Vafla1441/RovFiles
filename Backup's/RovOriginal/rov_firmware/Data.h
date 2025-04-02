// RovDataTypes.h

#ifndef _ROVDATATYPES_h
#define _ROVDATATYPES_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <stdint.h>
#include "Crc.h"


namespace rov {
    struct RovControl;
    struct RovTelimetry;
    struct RovHello;

    namespace prvt {
        template <typename T>
        inline T swap_endian(T u) {

            union
            {
                T u;
                unsigned char u8[sizeof(T)];
            } source, dest;

            source.u = u;

            for (size_t k = 0; k < sizeof(T); k++)
                dest.u8[k] = source.u8[sizeof(T) - k - 1];

            return dest.u;
        }

        void swap_endian_for(RovControl &rc);

        void swap_endian_for(RovTelimetry &rt);

        void swap_endian_for(RovHello &rt);
    };

    template<typename T>
    inline void write_bytes(uint8_t *msg, size_t &i, T &data){
        memcpy(msg + i, &data, sizeof(data));
        i += sizeof(data);
    }

    template<typename T>
    inline void read_bytes(const uint8_t *msg, size_t &i, T &data){
        memcpy(&data, msg + i, sizeof(data));
        i += sizeof(data);
    }

    struct RovControl {
        const uint8_t header_control = 0xAC; // only v2 or later

        enum class RovControlErrorCode : int {
            NoError = 0,
            WrongCrc = 1,
            WrongDataSize = 2,
        };

        uint8_t header = 0;
        int8_t version = 0; // protocol version

        // v1 begin
        int8_t axisX = 0; //! -100/100;
        int8_t axisY = 0; //! -100/100;
        int8_t axisZ = 0; //! -100/100;
        int8_t axisW = 0; //! -100/100;
        int8_t cameraRotation[2] = { 0, 0 };
        int8_t thrusterPower[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t debugFlag = 0;
        int8_t manipulatorRotation = 0; //! -100/100
        int8_t manipulatorOpenClose = 0; //! -1 close/+1 open;
        uint8_t regulators = 0; //! 1st bit - depth;
        float desiredDepth = 0.0f;
        float desiredYaw = 0.0f;

        // v2 begin
        uint8_t cameraIndex = 0;

        void resetData() {
            header = 0;
            version = 0;
            axisX = 0;
            axisY = 0;
            axisZ = 0;
            axisW = 0;
            debugFlag = 0;
            thrusterPower[0] = 0;
            thrusterPower[1] = 0;
            thrusterPower[2] = 0;
            thrusterPower[3] = 0;
            thrusterPower[4] = 0;
            thrusterPower[5] = 0;
            thrusterPower[6] = 0;
            thrusterPower[7] = 0;
            thrusterPower[8] = 0;
            thrusterPower[9] = 0;
            manipulatorRotation = 0;
            cameraRotation[0] = 0;
            cameraRotation[1] = 0;
            manipulatorOpenClose = 0;
            regulators = 0;
            desiredDepth = 0;
            desiredYaw = 0;
            cameraIndex = 0;
        }

        RovControlErrorCode fromRovControlMsg(const uint8_t *msg, unsigned long size) {
            size_t i = 0;
            read_bytes(msg, i, header);
            read_bytes(msg, i, version);
            prvt::swap_endian_for(*this);

            if (size == 22) {
                return fromRovControlMsgV1(msg, size);
            } else if (version == 2) {
                return fromRovControlMsgV2(msg, size);
            }

            // return fromRovControlMsgV2(msg, size);
        }

        RovControlErrorCode fromRovControlMsgV1(const uint8_t *msg, unsigned long size) {
            size_t i = 0;
            version = 1;

            read_bytes(msg, i, axisX);
            read_bytes(msg, i, axisY);
            read_bytes(msg, i, axisZ);
            read_bytes(msg, i, axisW);
            read_bytes(msg, i, debugFlag);
            read_bytes(msg, i, thrusterPower[0]);
            read_bytes(msg, i, thrusterPower[1]);
            read_bytes(msg, i, thrusterPower[2]);
            read_bytes(msg, i, thrusterPower[3]);
            read_bytes(msg, i, thrusterPower[4]);
            read_bytes(msg, i, thrusterPower[5]);
            read_bytes(msg, i, manipulatorRotation);
            read_bytes(msg, i, cameraRotation[0]);
            read_bytes(msg, i, cameraRotation[1]);
            read_bytes(msg, i, manipulatorOpenClose);
            read_bytes(msg, i, regulators);
            read_bytes(msg, i, desiredDepth);

            uint16_t crc = 0;
            read_bytes(msg, i, crc);
            crc = prvt::swap_endian<int16_t>(crc);

            prvt::swap_endian_for(*this);

            int16_t currentCrc = calculateCRC((const char *)msg, i);
            if (crc != currentCrc) {
                resetData();
                return RovControlErrorCode::WrongCrc;
            }
            return RovControlErrorCode::NoError;
        }

        RovControlErrorCode fromRovControlMsgV2(const uint8_t *msg, unsigned long size) {
            size_t i = 2; // skip header and version
            version = 2;

            read_bytes(msg, i, axisX);
            read_bytes(msg, i, axisY);
            read_bytes(msg, i, axisZ);
            read_bytes(msg, i, axisW);
            read_bytes(msg, i, debugFlag);
            read_bytes(msg, i, thrusterPower[0]);
            read_bytes(msg, i, thrusterPower[1]);
            read_bytes(msg, i, thrusterPower[2]);
            read_bytes(msg, i, thrusterPower[3]);
            read_bytes(msg, i, thrusterPower[4]);
            read_bytes(msg, i, thrusterPower[5]);
            read_bytes(msg, i, thrusterPower[6]);
            read_bytes(msg, i, thrusterPower[7]);
            read_bytes(msg, i, thrusterPower[8]);
            read_bytes(msg, i, thrusterPower[9]);
            read_bytes(msg, i, manipulatorRotation);
            read_bytes(msg, i, cameraRotation[0]);
            read_bytes(msg, i, cameraRotation[1]);
            read_bytes(msg, i, manipulatorOpenClose);
            read_bytes(msg, i, regulators);
            read_bytes(msg, i, desiredDepth);
            read_bytes(msg, i, desiredYaw);
            read_bytes(msg, i, cameraIndex);

            uint16_t currentCrc = calculateCRC((const char *)msg, i);

            uint16_t crc = 0;
            read_bytes(msg, i, crc);
            crc = prvt::swap_endian<uint16_t>(crc);

            prvt::swap_endian_for(*this);

            if (crc != currentCrc) {
                resetData();
                return RovControlErrorCode::WrongCrc;
            }
            return RovControlErrorCode::NoError;
        }

        static String fromErrorToString(RovControlErrorCode ec) {
            switch (ec) {
            case RovControlErrorCode::NoError: return "No error";
            case RovControlErrorCode::WrongCrc: return "CRC missmatch";
            case RovControlErrorCode::WrongDataSize: return "Wrong data size";
            }
        }

    };


    struct RovTelimetry {
        const uint8_t header_telemetry = 0xAE; // only v2 or later

        // v1 begin
        float depth = 0.0f;
        float pitch = 0; //! -180/180;
        float yaw = 0; //! 0 - 360;
        float roll = 0; //! -180/180;
        float ammeter = 0.0f;
        float voltmeter = 0.0f;
        uint8_t regulatorsFeedback = 0;
        int8_t manipulatorAngle = 0; //! ???
        int8_t manipulatorState = 0; //! -1 close/+1 open;

        // v2 begin
        int8_t cameraIndex = 0; //! 0 / 1 video multiplexer
        float temperature = 0.0f;

        size_t toRovTelemetryMsgV1(uint8_t *msg) {
            size_t i = 0;
            RovTelimetry rt = *this;
            prvt::swap_endian_for(rt);

            write_bytes(msg, i, rt.depth);
            write_bytes(msg, i, rt.pitch);
            write_bytes(msg, i, rt.yaw);
            write_bytes(msg, i, rt.roll);
            write_bytes(msg, i, rt.ammeter);
            write_bytes(msg, i, rt.voltmeter);
            write_bytes(msg, i, rt.regulatorsFeedback);
            write_bytes(msg, i, rt.manipulatorAngle);
            write_bytes(msg, i, rt.manipulatorState);

            uint16_t crc = prvt::swap_endian<int16_t>(calculateCRC((const char *)msg, i));
            write_bytes(msg, i, crc);

            return i;
        }

        size_t toRovTelemetryMsgV2(uint8_t *msg) {
            size_t i = 0;
            RovTelimetry rt = *this;
            prvt::swap_endian_for(rt);
            constexpr int8_t version = 2;

            write_bytes(msg, i, header_telemetry);
            write_bytes(msg, i, version);

            write_bytes(msg, i, rt.depth);
            write_bytes(msg, i, rt.pitch);
            write_bytes(msg, i, rt.yaw);
            write_bytes(msg, i, rt.roll);
            write_bytes(msg, i, rt.ammeter);
            write_bytes(msg, i, rt.voltmeter);
            write_bytes(msg, i, rt.regulatorsFeedback);
            write_bytes(msg, i, rt.manipulatorAngle);
            write_bytes(msg, i, rt.manipulatorState);

            write_bytes(msg, i, rt.cameraIndex);
            write_bytes(msg, i, rt.temperature);

            uint16_t crc = prvt::swap_endian<int16_t>(calculateCRC((const char *)msg, i));
            write_bytes(msg, i, crc);

            return i;
        }
    };

    struct RovHello {
        uint8_t header_hello = 0xAA;
        int8_t version = 2; // latest supported protocol version

        static size_t toRovHelloMsg(uint8_t *msg) {
            size_t i = 0;

            RovHello rh;
            prvt::swap_endian_for(rh);

            write_bytes(msg, i, rh.header_hello);
            write_bytes(msg, i, rh.version);

            int16_t crc = prvt::swap_endian<int16_t>(calculateCRC((const char *)msg, i));
            write_bytes(msg, i, crc);

            return i;
        }
    };

}
#endif
