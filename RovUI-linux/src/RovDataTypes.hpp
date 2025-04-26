import struct
from enum import Enum, IntEnum
from typing import NamedTuple, Tuple

class RovTelimetryErrorCode(IntEnum):
    NoError = 0
    WrongCrc = 1
    WrongDataSize = 2

class RovControl(NamedTuple):
    header_control: int = 0xAC  # only v2 and later
    version: int = 0
    axisX: int = 0       # -100/100
    axisY: int = 0       # -100/100
    axisZ: int = 0       # -100/100
    axisW: int = 0       # -100/100
    desiredDepth: float = 0.0
    desiredYaw: float = 0.0
    cameraRotation: Tuple[int, int, int] = (0, 0, 0)  # qint8[3]
    thrusterPower: Tuple[int, ...] = (0,) * 10        # qint8[10]
    debugFlag: int = 0       # quint8
    manipulatorRotation: int = 0  # -100/100
    manipulatorOpenClose: int = 0  # -1 close/+1 open
    pumpLazerButton: int = 0
    regulators: int = 0     # 1st bit - depth
    cameraIndex: int = 0

    def to_ranger_control_msg_v1(self) -> bytes:
        data = bytearray()
        
        # Pack basic control data
        data.extend(struct.pack('>b', self.axisX))
        data.extend(struct.pack('>b', self.axisY))
        data.extend(struct.pack('>b', self.axisZ))
        data.extend(struct.pack('>b', self.axisW))
        data.extend(struct.pack('>B', self.debugFlag))
        
        # Pack thruster power (first 6)
        for t in self.thrusterPower[:6]:
            data.extend(struct.pack('>b', t))
        
        # Pack manipulator and camera data
        data.extend(struct.pack('>b', self.manipulatorRotation))
        for c in self.cameraRotation:
            data.extend(struct.pack('>b', c))
        
        data.extend(struct.pack('>b', self.manipulatorOpenClose))
        data.extend(struct.pack('>b', self.pumpLazerButton))
        data.extend(struct.pack('>B', self.regulators))
        data.extend(struct.pack('>f', self.desiredDepth))
        
        # Calculate and append CRC
        crc = self._calculate_crc(data)
        data.extend(struct.pack('>H', crc))
        
        return bytes(data)

    def to_ranger_control_msg_v2(self) -> bytes:
        data = bytearray()
        self.version = 2
        
        # Header and version
        data.extend(struct.pack('>B', self.header_control))
        data.extend(struct.pack('>b', self.version))
        
        # Basic control data (same as v1)
        data.extend(struct.pack('>b', self.axisX))
        data.extend(struct.pack('>b', self.axisY))
        data.extend(struct.pack('>b', self.axisZ))
        data.extend(struct.pack('>b', self.axisW))
        data.extend(struct.pack('>B', self.debugFlag))
        
        # All 10 thrusters
        for t in self.thrusterPower:
            data.extend(struct.pack('>b', t))
        
        # Manipulator and camera data
        data.extend(struct.pack('>b', self.manipulatorRotation))
        for c in self.cameraRotation:
            data.extend(struct.pack('>b', c))
        
        data.extend(struct.pack('>b', self.manipulatorOpenClose))
        data.extend(struct.pack('>b', self.pumpLazerButton))
        data.extend(struct.pack('>B', self.regulators))
        data.extend(struct.pack('>f', self.desiredDepth))
        
        # V2 additions
        data.extend(struct.pack('>f', self.desiredYaw))
        data.extend(struct.pack('>b', self.cameraIndex))
        
        # Calculate and append CRC
        crc = self._calculate_crc(data)
        data.extend(struct.pack('>H', crc))
        
        return bytes(data)

    @staticmethod
    def _calculate_crc(data: bytes) -> int:
        """CRC-16/CCITT-FALSE implementation matching the C++ version"""
        crc = 0xFFFF
        poly = 0x1021
        
        for byte in data:
            crc ^= (byte << 8)
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ poly
                else:
                    crc <<= 1
                crc &= 0xFFFF
        
        return crc

class RovTelimetry(NamedTuple):
    header_telemetry: int = 0xAE  # only v2 and later
    header: int = 0
    version: int = 0
    depth: float = 0.0
    temperature: float = 0.0
    pitch: float = 0.0    # -180/180
    yaw: float = 0.0      # 0-360
    roll: float = 0.0     # -180/180
    ammeter: float = 0.0
    voltmeter: float = 0.0
    regulatorsFeedback: int = 0  # quint8
    manipulatorAngle: int = 0    # ???
    manipulatorState: int = 0    # -1 close/+1 open
    cameraIndex: int = 0

    @classmethod
    def from_ranger_telimetry_msg_v1(cls, data: bytes) -> Tuple['RovTelimetry', RovTelimetryErrorCode]:
        if len(data) < 34:  # Minimum expected size
            return cls(), RovTelimetryErrorCode.WrongDataSize
        
        try:
            # Unpack basic telemetry data
            depth = struct.unpack('>f', data[0:4])[0]
            pitch = struct.unpack('>f', data[4:8])[0]
            yaw = struct.unpack('>f', data[8:12])[0]
            roll = struct.unpack('>f', data[12:16])[0]
            ammeter = struct.unpack('>f', data[16:20])[0]
            voltmeter = struct.unpack('>f', data[20:24])[0]
            regulatorsFeedback = data[24]
            manipulatorAngle = struct.unpack('>b', data[25:26])[0]
            manipulatorState = struct.unpack('>b', data[26:27])[0]
            
            # Check CRC
            received_crc = struct.unpack('>H', data[-2:])[0]
            calculated_crc = RovControl._calculate_crc(data[:-2])
            
            if received_crc != calculated_crc:
                return cls(), RovTelimetryErrorCode.WrongCrc
            
            return cls(
                depth=depth,
                pitch=pitch,
                yaw=yaw,
                roll=roll,
                ammeter=ammeter,
                voltmeter=voltmeter,
                regulatorsFeedback=regulatorsFeedback,
                manipulatorAngle=manipulatorAngle,
                manipulatorState=manipulatorState
            ), RovTelimetryErrorCode.NoError
            
        except struct.error:
            return cls(), RovTelimetryErrorCode.WrongDataSize

    @classmethod
    def from_ranger_telimetry_msg_v2(cls, data: bytes) -> Tuple['RovTelimetry', RovTelimetryErrorCode]:
        if len(data) < 36:  # Minimum expected size
            return cls(), RovTelimetryErrorCode.WrongDataSize
        
        try:
            pos = 0
            header = data[pos]
            pos += 1
            version = struct.unpack('>b', data[pos:pos+1])[0]
            pos += 1
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
            regulatorsFeedback = data[pos]
            pos += 1
            manipulatorAngle = struct.unpack('>b', data[pos:pos+1])[0]
            pos += 1
            manipulatorState = struct.unpack('>b', data[pos:pos+1])[0]
            pos += 1
            cameraIndex = data[pos]
            pos += 1
            temperature = struct.unpack('>f', data[pos:pos+4])[0]
            pos += 4
            
            # Check CRC
            received_crc = struct.unpack('>H', data[-2:])[0]
            calculated_crc = RovControl._calculate_crc(data[:-2])
            
            if received_crc != calculated_crc:
                return cls(), RovTelimetryErrorCode.WrongCrc
            
            return cls(
                header=header,
                version=version,
                depth=depth,
                temperature=temperature,
                pitch=pitch,
                yaw=yaw,
                roll=roll,
                ammeter=ammeter,
                voltmeter=voltmeter,
                regulatorsFeedback=regulatorsFeedback,
                manipulatorAngle=manipulatorAngle,
                manipulatorState=manipulatorState,
                cameraIndex=cameraIndex
            ), RovTelimetryErrorCode.NoError
            
        except struct.error:
            return cls(), RovTelimetryErrorCode.WrongDataSize

    @staticmethod
    def from_error_to_string(ec: RovTelimetryErrorCode) -> str:
        error_map = {
            RovTelimetryErrorCode.NoError: "No error",
            RovTelimetryErrorCode.WrongCrc: "CRC mismatch",
            RovTelimetryErrorCode.WrongDataSize: "Wrong data size"
        }
        return error_map.get(ec, "Unknown error")

class RovHello:
    header_hello: int = 0xAA
    header: int = 0
    version: int = 0

    @staticmethod
    def get_version(data: bytes) -> int:
        if len(data) < 4:  # header + version + crc
            return 0
        
        try:
            header = data[0]
            version = struct.unpack('>b', data[1:2])[0]
            
            # Check CRC
            received_crc = struct.unpack('>H', data[-2:])[0]
            calculated_crc = RovControl._calculate_crc(data[:-2])
            
            if received_crc == calculated_crc:
                return version
            return 0
        except struct.error:
            return 0