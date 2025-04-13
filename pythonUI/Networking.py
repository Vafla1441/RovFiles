import socket
import struct
from enum import IntEnum
from typing import Tuple, NamedTuple
import time


class RovControlErrorCode(IntEnum):
    NoError = 0
    WrongCrc = 1
    WrongDataSize = 2


class RovControl(NamedTuple):
    # Control message to send to ROV
    axisX: int = 0  # -100/100
    axisY: int = 0  # -100/100
    axisZ: int = 0  # -100/100
    axisW: int = 0  # -100/100
    cameraRotation: Tuple[int, int] = (0, 0)  # -100/100 for each
    thrusterPower: Tuple[int, ...] = (0,) * 10  # -100/100 for each
    debugFlag: int = 0  # 0 or 1
    manipulatorRotation: int = 0  # -100/100
    manipulatorOpenClose: int = 0  # -1 close/+1 open
    regulators: int = 0  # bit flags
    desiredDepth: float = 0.0
    desiredYaw: float = 0.0
    cameraIndex: int = 0  # 0 or 1


class RovTelemetry(NamedTuple):
    # Telemetry received from ROV
    depth: float = 0.0
    pitch: float = 0.0  # -180/180
    yaw: float = 0.0  # 0-360
    roll: float = 0.0  # -180/180
    ammeter: float = 0.0
    voltmeter: float = 0.0
    regulatorsFeedback: int = 0
    manipulatorAngle: int = 0
    manipulatorState: int = 0
    cameraIndex: int = 0
    temperature: float = 0.0


class RovHello(NamedTuple):
    # Hello message from ROV
    version: int = 2


class ROVClient:
    def __init__(self, rov_ip: str = "192.168.1.5", rov_port: int = 3020,
                 local_ip: str = "192.168.1.4", local_port: int = 3010):
        self.rov_addr = (rov_ip, rov_port)
        self.local_addr = (local_ip, local_port)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(self.local_addr)
        self.sock.settimeout(0.1)  # 100ms timeout

        # Protocol version (should match ROV code)
        self.version = 2

        # Last received telemetry
        self.last_telemetry = RovTelemetry()

        # Sequence counter for debugging
        self.sequence = 0

    def close(self):
        self.sock.close()

    def _calculate_crc(self, data: bytes) -> int:
        """Calculate CRC-16 (CCITT) for the given data"""
        poly = 0x1021
        crc = 0xFFFF

        for byte in data:
            crc ^= (byte << 8)
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ poly
                else:
                    crc <<= 1
                crc &= 0xFFFF  # Ensure it stays 16-bit

        return crc

    def _swap_endian(self, value: int, size: int) -> int:
        """Swap endianness of the given value"""
        return int.from_bytes(
            value.to_bytes(size, byteorder='little'),
            byteorder='big', signed=size > 1
        )

    def send_control(self, control: RovControl) -> RovControlErrorCode:
        """
        Send control message to ROV
        Returns RovControlErrorCode indicating success/failure
        """
        # Prepare the message buffer
        msg = bytearray()

        # Header and version (V2 protocol)
        msg.extend(struct.pack('>B', 0xAC))  # header_control
        msg.extend(struct.pack('>b', self.version))

        # Pack all control fields
        msg.extend(struct.pack('>b', control.axisX))
        msg.extend(struct.pack('>b', control.axisY))
        msg.extend(struct.pack('>b', control.axisZ))
        msg.extend(struct.pack('>b', control.axisW))
        msg.extend(struct.pack('>B', control.debugFlag))

        for power in control.thrusterPower[:10]:
            msg.extend(struct.pack('>b', power))

        msg.extend(struct.pack('>b', control.manipulatorRotation))
        msg.extend(struct.pack('>b', control.cameraRotation[0]))
        msg.extend(struct.pack('>b', control.cameraRotation[1]))
        msg.extend(struct.pack('>b', control.manipulatorOpenClose))
        msg.extend(struct.pack('>B', control.regulators))
        msg.extend(struct.pack('>f', control.desiredDepth))
        msg.extend(struct.pack('>f', control.desiredYaw))
        msg.extend(struct.pack('>B', control.cameraIndex))

        # Calculate and append CRC
        crc = self._calculate_crc(msg)
        msg.extend(struct.pack('>H', crc))

        # Send the message
        try:
            self.sock.sendto(msg, self.rov_addr)
            return RovControlErrorCode.NoError
        except Exception as e:
            print(f"Error sending control: {e}")
            return RovControlErrorCode.WrongDataSize

    def receive_telemetry(self, timeout: float = 0.1):
        """
        Receive telemetry from ROV
        Returns (telemetry, success) tuple
        """
        try:
            data, _ = self.sock.recvfrom(1024)
            if not data:
                return self.last_telemetry, False

            # Check message type by header
            header = data[0]

            if header == 0xAE:  # Telemetry V2
                return self._parse_telemetry_v2(data), True
            elif header == 0xAA:  # Hello message
                return self._parse_hello(data), True
            else:  # Assume Telemetry V1
                return self._parse_telemetry_v1(data), True

        except socket.timeout:
            return self.last_telemetry, False
        except Exception as e:
            print(f"Error receiving telemetry: {e}")
            return self.last_telemetry, False

    def _parse_telemetry_v1(self, data: bytes) -> RovTelemetry:
        """Parse V1 telemetry message"""
        if len(data) < 30:  # Minimum expected size
            return self.last_telemetry

        # Unpack fields
        depth = struct.unpack('>f', data[0:4])[0]
        pitch = struct.unpack('>f', data[4:8])[0]
        yaw = struct.unpack('>f', data[8:12])[0]
        roll = struct.unpack('>f', data[12:16])[0]
        ammeter = struct.unpack('>f', data[16:20])[0]
        voltmeter = struct.unpack('>f', data[20:24])[0]
        regulators = data[24]
        manip_angle = struct.unpack('>b', data[25:26])[0]
        manip_state = struct.unpack('>b', data[26:27])[0]

        # CRC is last 2 bytes
        received_crc = struct.unpack('>H', data[-2:])[0]
        calculated_crc = self._calculate_crc(data[:-2])

        if received_crc != calculated_crc:
            print("CRC mismatch in telemetry V1")
            return self.last_telemetry

        # Update last telemetry
        self.last_telemetry = RovTelemetry(
            depth=depth,
            pitch=pitch,
            yaw=yaw,
            roll=roll,
            ammeter=ammeter,
            voltmeter=voltmeter,
            regulatorsFeedback=regulators,
            manipulatorAngle=manip_angle,
            manipulatorState=manip_state,
            cameraIndex=0,  # Not in V1
            temperature=0.0  # Not in V1
        )

        return self.last_telemetry

    def _parse_telemetry_v2(self, data: bytes) -> RovTelemetry:
        """Parse V2 telemetry message"""
        if len(data) < 36:  # Minimum expected size
            return self.last_telemetry

        # Skip header (1 byte) and version (1 byte)
        pos = 2

        # Unpack fields
        depth = struct.unpack('>f', data[pos:pos+4])[0]
        pos += 4
        pitch = struct.unpack('>f', data[pos:pos+4])[0]
        pos += 4
        yaw = struct.unpack('>f', data[pos:pos+4])[0]
        pos += 4
        roll = struct.unpack('>f', data[pos:pos+4])[0]
        pos += 4
        ammeter = struct.unpack('>f', data[pos:pos+4])[0]
        pos += 4
        voltmeter = struct.unpack('>f', data[pos:pos+4])[0]
        pos += 4
        regulators = data[pos]
        pos += 1
        manip_angle = struct.unpack('>b', data[pos:pos+1])[0]
        pos += 1
        manip_state = struct.unpack('>b', data[pos:pos+1])[0]
        pos += 1
        cam_index = data[pos]
        pos += 1
        temp = struct.unpack('>f', data[pos:pos+4])[0]
        pos += 4

        # CRC is last 2 bytes
        received_crc = struct.unpack('>H', data[-2:])[0]
        calculated_crc = self._calculate_crc(data[:-2])

        if received_crc != calculated_crc:
            print("CRC mismatch in telemetry V2")
            return self.last_telemetry

        # Update last telemetry
        self.last_telemetry = RovTelemetry(
            depth=depth,
            pitch=pitch,
            yaw=yaw,
            roll=roll,
            ammeter=ammeter,
            voltmeter=voltmeter,
            regulatorsFeedback=regulators,
            manipulatorAngle=manip_angle,
            manipulatorState=manip_state,
            cameraIndex=cam_index,
            temperature=temp
        )

        return self.last_telemetry

    def _parse_hello(self, data: bytes) -> RovTelemetry:
        """Parse hello message (just prints version info)"""
        if len(data) < 4:  # header (1) + version (1) + CRC (2)
            return self.last_telemetry

        version = struct.unpack('>b', data[1:2])[0]
        print(f"Received HELLO from ROV, protocol version: {version}")
        return self.last_telemetry

    def run_test_sequence(self):
        """Test sequence that sends control commands and prints telemetry"""
        print("Starting ROV test sequence...")

        try:
            while True:
                self.sequence += 1
                axis_val = 50 if (self.sequence // 10) % 2 else -50

                control = RovControl(
                    axisX=axis_val,
                    axisY=axis_val,
                    axisZ=0,
                    axisW=0,
                    cameraRotation=(0, 0),
                    thrusterPower=(axis_val, axis_val, 0, 0, 0, 0, 0, 0, 0, 0),
                    debugFlag=0,
                    manipulatorRotation=0,
                    manipulatorOpenClose=1 if self.sequence % 20 < 10 else -1,
                    regulators=1 if self.sequence % 40 < 20 else 0,
                    desiredDepth=1.0,
                    desiredYaw=0.0,
                    cameraIndex=self.sequence % 2
                )

                # Send control command
                result = self.send_control(control)
                if result != RovControlErrorCode.NoError:
                    print(f"Error sending control: {result}")

                # Receive telemetry
                telemetry, success = self.receive_telemetry()
                if success:
                    print("\nTelemetry received:")
                    print(f"Depth: {telemetry.depth:.2f} m")
                    print(
                        f"Attitude: Yaw={telemetry.yaw:.1f}°, "
                        f"Pitch={telemetry.pitch:.1f}°, "
                        f"Roll={telemetry.roll:.1f}°")
                    print(
                        f"Power: Voltage={telemetry.voltmeter:.2f} V, "
                        f"Current={telemetry.ammeter:.2f} A")
                    print(f"Temperature: {telemetry.temperature:.1f}°C")
                    print(
                        f"Manipulator: Angle={telemetry.manipulatorAngle}, "
                        f"State={telemetry.manipulatorState}")
                    print(f"Camera: Index={telemetry.cameraIndex}")

                time.sleep(0.1)  # ~10Hz loop

        except KeyboardInterrupt:
            print("\nTest sequence stopped")
        finally:
            self.close()
