import socket
import struct
from enum import IntEnum
from dataclasses import dataclass
from typing import Tuple, Optional


class RovControlErrorCode(IntEnum):
    NoError = 0
    WrongCrc = 1
    WrongDataSize = 2


@dataclass
class RovControl:
    header: int = 0xAC
    version: int = 2

    # v1 fields
    axisX: int = 0
    axisY: int = 0
    axisZ: int = 0
    axisW: int = 0
    cameraRotation: Tuple[int, int] = (0, 0)
    thrusterPower: Tuple[int, ...] = (0,) * 10
    debugFlag: int = 0
    manipulatorRotation: int = 0
    manipulatorOpenClose: int = 0
    regulators: int = 0
    desiredDepth: float = 0.0
    desiredYaw: float = 0.0

    # v2 fields
    cameraIndex: int = 0


@dataclass
class RovTelemetry:
    header: int = 0xAE
    version: int = 2

    # v1 fields
    depth: float = 0.0
    pitch: float = 0.0
    yaw: float = 0.0
    roll: float = 0.0
    ammeter: float = 0.0
    voltmeter: float = 0.0
    regulatorsFeedback: int = 0
    manipulatorAngle: int = 0
    manipulatorState: int = 0

    # v2 fields
    cameraIndex: int = 0
    temperature: float = 0.0


class ROVClient:
    def __init__(self, remote_ip: str, remote_port: int, local_ip:
                 str = "0.0.0.0", local_port: int = 0):
        self.remote_addr = (remote_ip, remote_port)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((local_ip, local_port))
        self.sock.settimeout(1.0)  # 1 second timeout

    def close(self):
        self.sock.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def _swap_endian(self, data: bytes) -> bytes:
        """Swap endianness for multi-byte values"""
        if len(data) == 2:
            return struct.pack('<h', struct.unpack('>h', data)[0])
        elif len(data) == 4:
            return struct.pack('<f', struct.unpack('>f', data)[0])
        return data

    def _calculate_crc(self, data: bytes) -> int:
        """Simple CRC calculation (should match the ROV implementation)"""
        crc = 0
        for byte in data:
            crc ^= byte << 8
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ 0x1021
                else:
                    crc <<= 1
                crc &= 0xFFFF
        return crc

    def send_control(self, control: RovControl) -> bool:
        """Send control command to ROV"""
        buffer = bytearray()

        # Pack common fields
        buffer.extend(struct.pack('>B', control.header))
        buffer.extend(struct.pack('>b', control.version))

        # Pack control data
        buffer.extend(struct.pack('>b', control.axisX))
        buffer.extend(struct.pack('>b', control.axisY))
        buffer.extend(struct.pack('>b', control.axisZ))
        buffer.extend(struct.pack('>b', control.axisW))
        buffer.extend(struct.pack('>B', control.debugFlag))

        for power in control.thrusterPower[:10]:
            buffer.extend(struct.pack('>b', power))

        buffer.extend(struct.pack('>b', control.manipulatorRotation))
        buffer.extend(struct.pack('>b', control.cameraRotation[0]))
        buffer.extend(struct.pack('>b', control.cameraRotation[1]))
        buffer.extend(struct.pack('>b', control.manipulatorOpenClose))
        buffer.extend(struct.pack('>B', control.regulators))
        buffer.extend(struct.pack('>f', control.desiredDepth))
        buffer.extend(struct.pack('>f', control.desiredYaw))
        buffer.extend(struct.pack('>B', control.cameraIndex))

        # Calculate CRC
        crc = self._calculate_crc(buffer)
        buffer.extend(struct.pack('>H', crc))

        # Send data
        try:
            self.sock.sendto(buffer, self.remote_addr)
            return True
        except socket.error:
            return False

    def receive_telemetry(self) -> Optional[RovTelemetry]:
        """Receive telemetry data from ROV"""
        try:
            data, _ = self.sock.recvfrom(1024)
        except socket.timeout:
            return None
        except socket.error:
            return None

        if len(data) < 2:
            return None

        # Check header
        header = data[0]
        if header == 0xAE:  # Telemetry
            return self._parse_telemetry_v2(data)
        elif header == 0xAA:  # Hello message
            return None  # Ignore hello messages
        else:
            return None  # Unknown message type

    def _parse_telemetry_v2(self, data: bytes) -> Optional[RovTelemetry]:
        """Parse telemetry message version 2"""
        if len(data) < 40:  # Minimum size for v2 telemetry
            return None

        # Verify CRC
        crc_received = struct.unpack('>H', data[-2:])[0]
        crc_calculated = self._calculate_crc(data[:-2])
        if crc_received != crc_calculated:
            return None

        # Parse fields
        idx = 0
        header = data[idx]
        idx += 1
        version = struct.unpack_from('>b', data, idx)[0]
        idx += 1

        telemetry = RovTelemetry()
        telemetry.header = header
        telemetry.version = version

        # Unpack and swap endianness for float values
        telemetry.depth = struct.unpack('>f', data[idx:idx+4])[0]
        idx += 4
        telemetry.pitch = struct.unpack('>f', data[idx:idx+4])[0]
        idx += 4
        telemetry.yaw = struct.unpack('>f', data[idx:idx+4])[0]
        idx += 4
        telemetry.roll = struct.unpack('>f', data[idx:idx+4])[0]
        idx += 4
        telemetry.ammeter = struct.unpack('>f', data[idx:idx+4])[0]
        idx += 4
        telemetry.voltmeter = struct.unpack('>f', data[idx:idx+4])[0]
        idx += 4

        telemetry.regulatorsFeedback = data[idx]
        idx += 1
        telemetry.manipulatorAngle = struct.unpack_from('>b', data, idx)[0]
        idx += 1
        telemetry.manipulatorState = struct.unpack_from('>b', data, idx)[0]
        idx += 1
        telemetry.cameraIndex = data[idx]
        idx += 1
        telemetry.temperature = struct.unpack('>f', data[idx:idx+4])[0]

        return telemetry

    def send_hello(self) -> bool:
        """Send hello message to ROV"""
        buffer = bytearray()
        buffer.extend(struct.pack('>B', 0xAA))  # Header
        buffer.extend(struct.pack('>b', 2))     # Version

        # Calculate CRC
        crc = self._calculate_crc(buffer)
        buffer.extend(struct.pack('>H', crc))

        try:
            self.sock.sendto(buffer, self.remote_addr)
            return True
        except socket.error:
            return False
