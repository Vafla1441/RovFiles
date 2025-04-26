import socket
import struct
from enum import IntEnum
from typing import Tuple, NamedTuple
import time
from Joystick import Joystick

class RovControlErrorCode(IntEnum):
    NoError = 0
    WrongDataSize = 1
    ConnectionError = 2
    CRCError = 3

class RovControl(NamedTuple):
    axisX: float = 0
    axisY: float = 0
    axisZ: float = 0
    axisW: float = 0
    cameraRotation: Tuple[float, float, float] = (0, 0, 0)
    thrusterPower: Tuple[float, ...] = (0,) * 10
    debugFlag: int = 0
    manipulatorRotation: float = 0
    manipulatorOpenClose: int = 0
    pumpPower: int = 0
    regulators: int = 0
    desiredDepth: float = 0.0
    desiredYaw: float = 0.0
    cameraIndex: int = 0

class RovTelemetry(NamedTuple):
    depth: float = 0.0
    pitch: float = 0.0
    yaw: float = 0.0
    roll: float = 0.0
    voltmeter: float = 0.0
    ampermeter: float = 0.0
    regulatorsFeedback: int = 0
    manipulatorAngle: int = 0
    manipulatorState: int = 0
    cameraIndex: int = 0
    temperature: float = 0.0

class RovClient:
    def __init__(self, add_log, rov_ip: str = "192.168.1.5", rov_port: int = 3020,
                 local_ip: str = "", local_port: int = 3010):
        self.log = add_log
        self.joystick = Joystick(add_log)
        self.rov_addr = (rov_ip, rov_port)
        self.local_addr = (local_ip, local_port)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(self.local_addr)
        self.sock.settimeout(0.1)
        self.version = 2
        self.last_telemetry = RovTelemetry()

    def close(self):
        self.sock.close()

    def _calculate_crc(self, data: bytes) -> int:
        poly = 0x1021
        crc = 0xFFFF

        for byte in data:
            crc ^= (byte << 8)
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ poly
                else:
                    crc <<= 1
                crc &= 0xFFFF

        return crc

    def send_control(self, control: RovControl) -> RovControlErrorCode:
        msg = bytearray()
        
        msg.extend(struct.pack('>B', 0xAC))
        msg.extend(struct.pack('>b', self.version))

        msg.extend(struct.pack('>f', control.axisX))
        msg.extend(struct.pack('>f', control.axisY))
        msg.extend(struct.pack('>f', control.axisZ))
        msg.extend(struct.pack('>f', control.axisW))
        msg.extend(struct.pack('>B', control.debugFlag))

        for power in control.thrusterPower[:10]:
            msg.extend(struct.pack('>b', power))

        msg.extend(struct.pack('>f', control.manipulatorRotation))
        msg.extend(struct.pack('>f', control.cameraRotation[0]))
        msg.extend(struct.pack('>f', control.cameraRotation[1]))
        msg.extend(struct.pack('>f', control.cameraRotation[2]))
        msg.extend(struct.pack('>b', control.manipulatorOpenClose))
        msg.extend(struct.pack('>b', control.pumpPower))
        msg.extend(struct.pack('>B', control.regulators))
        msg.extend(struct.pack('>f', control.desiredDepth))
        msg.extend(struct.pack('>f', control.desiredYaw))
        msg.extend(struct.pack('>B', control.cameraIndex))

        crc = self._calculate_crc(msg)
        msg.extend(struct.pack('>H', crc))

        try:
            self.sock.sendto(msg, self.rov_addr)
            return RovControlErrorCode.NoError
        except Exception as e:
            self.log(f"Ошибка пакета: {e}", "ERROR")
            return RovControlErrorCode.ConnectionError

    def receive_telemetry(self) -> Tuple[RovTelemetry, bool]:
        try:
            data, _ = self.sock.recvfrom(1024)
            if not data:
                return self.last_telemetry, False

            header = data[0]

            if header == 0xAE:
                return self._parse_telemetry_v2(data), True
            elif header == 0xAA:
                return self._parse_hello(data), True

        except socket.timeout:
            return self.last_telemetry, False
        except Exception as e:
            self.log(f"Ошибка пакета: {e}", "ERROR")
            return self.last_telemetry, False

        except struct.error as e:
            self.log(f"Ошибка пакета: {e}", "ERROR")
            return self.last_telemetry

    def _parse_telemetry_v2(self, data: bytes) -> RovTelemetry:
        if len(data) < 36:
            self.log("Пакет слишком мал", "ERROR")
            return self.last_telemetry

        try:
            pos = 2

            depth = struct.unpack('>f', data[pos:pos+4])[0]
            pos += 4
            pitch = struct.unpack('>f', data[pos:pos+4])[0]
            pos += 4
            yaw = struct.unpack('>f', data[pos:pos+4])[0]
            pos += 4
            roll = struct.unpack('>f', data[pos:pos+4])[0]
            pos += 4
            ampermeter = struct.unpack('>f', data[pos:pos+4])[0]
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

            # Verify CRC
            received_crc = struct.unpack('>H', data[-2:])[0]
            calculated_crc = self._calculate_crc(data[:-2])

            if received_crc != calculated_crc:
                self.log("Пакет лос", "ERROR")
                return self.last_telemetry

            self.last_telemetry = RovTelemetry(
                depth=depth,
                pitch=pitch,
                yaw=yaw,
                roll=roll,
                ampermeter=ampermeter,
                voltmeter=voltmeter,
                regulatorsFeedback=regulators,
                manipulatorAngle=manip_angle,
                manipulatorState=manip_state,
                cameraIndex=cam_index,
                temperature=temp
            )
            return self.last_telemetry

        except struct.error as e:
            self.log(f"Ошибка парсинга пакета: {e}")
            return self.last_telemetry

    def _parse_hello(self, data: bytes) -> RovTelemetry:
        if len(data) >= 4:
            version = struct.unpack('>b', data[1:2])[0]
            # self.log(f"ROV HELLO message received, protocol version: {version}", "WARNING")
        return self.last_telemetry

    def run(self):
        self.log("Инициализация обмена пакетами")

        try:
            while True:
                self.joystick.update()
                control = RovControl(
                    axisX=(self.joystick.values["axis_x"] * (-100)),
                    axisY=(self.joystick.values["axis_y"] * 100),
                    axisZ=(self.joystick.values["axis_z"] * 100),
                    axisW=(self.joystick.values["axis_w"] * (-100)),
                    cameraRotation=(self.joystick.values["camera_rotation"][0], 
                                    self.joystick.values["camera_rotation"][1],
                                    self.joystick.values["camera_rotation"][2]),
                    thrusterPower=(0,0,0,0,0,0,0,0,0,0),
                    debugFlag=0,
                    manipulatorRotation=self.joystick.values["manipulator_rotation"],
                    manipulatorOpenClose=self.joystick.values["manipulator_open_close"],
                    pumpPower=self.joystick.values["pump_laser"],
                    regulators=0,
                    desiredDepth=1.0,
                    desiredYaw=0.0,
                    cameraIndex=0
                )
                result = self.send_control(control)
                if result != RovControlErrorCode.NoError:
                    self.log(f"Ошибка отправки пакета: {result.name}")
                time.sleep(0.1)
        finally:
            self.close()

    def _display_telemetry(self, telemetry: RovTelemetry):
        self.log("\n=== ROV Telemetry ===")
        self.log(f"Depth:       {telemetry.depth:7.2f} m")
        self.log(f"Attitude:    Yaw={telemetry.yaw:6.1f}° Pitch={telemetry.pitch:6.1f}° Roll={telemetry.roll:6.1f}°")
        self.log(f"Voltage:     {telemetry.voltmeter:7.2f} V")
        self.log(f"Current:     {telemetry.ampermeter:7.2f} A")
        self.log(f"Temperature: {telemetry.temperature:7.1f} °C")
        self.log(f"Manipulator: Angle={telemetry.manipulatorAngle:3} State={telemetry.manipulatorState:2}")
        self.log(f"Camera:      Index={telemetry.cameraIndex}")
        self.log(f"Regulators:  {bin(telemetry.regulatorsFeedback)}")
        self.log("====================")