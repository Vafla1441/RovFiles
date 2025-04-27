import Networking as nt
import pygame

pygame.init()
pygame.joystick.init()
joysticks = []
client = nt.ROVClient(rov_ip="192.168.1.5", rov_port=3020,
                      local_ip="0.0.0.0", local_port=3010)
axis_x, axis_y, axis_z, axis_w = 1, 0, 2, 5
mr, mo, lz, ca = 0, 0, 0, 0
# Send control command
control = nt.RovControl(
    axisX=0,  # -100 to 100
    axisY=0,
    axisZ=0,
    axisW=0,
    cameraRotation=(0, 0, 0),  # Two camera angles
    thrusterPower=(0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    debugFlag=0,
    manipulatorRotation=0,
    manipulatorOpenClose=0,  # 1 = open, -1 = close
    pumpPower=0,
    regulators=0,  # Bit flags
    desiredDepth=0,  # meters
    desiredYaw=0,  # degrees
    cameraIndex=1  # 0 or 1
)
while True:
    for event in pygame.event.get():
        if event.type == pygame.JOYDEVICEADDED:
            joy = pygame.joystick.Joystick(event.device_index)
            joysticks.append(joy)
    client.send_control(control)
    if joy.get_button(14):
        mr = 1
    if joy.get_button(5):
        mr = -1
    if joy.get_button(20):
        mo = 1
    if joy.get_button(22):
        mo = -1
    if joy.get_button(19):
        lz = 2
    if joy.get_button(21):
        lz = 0
    if joy.get_button(2):
        ca = 1
    if joy.get_button(3):
        ca = -1
    if joy.get_button(6):
        lz = 1
    for joystick in joysticks:
        control = nt.RovControl(
            axisX=int(joystick.get_axis(axis_x) * (-100)),  # -100 to 100
            axisY=int(joystick.get_axis(axis_y) * 100),
            axisZ=int(joystick.get_axis(axis_z) * 100),
            axisW=int(joystick.get_axis(axis_w) * (-100)),
            cameraRotation=(0, ca, 0),  # Two camera angles
            thrusterPower=(0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
            debugFlag=0,
            manipulatorRotation=mo,
            manipulatorOpenClose=mr,  # 1 = open, -1 = close
            pumpPower=lz,
            regulators=0,  # Bit flags
            desiredDepth=0,  # meters
            desiredYaw=0,  # degrees
            cameraIndex=1  # 0 or 1
        )
    mr = 0
    mo = 0
    ca = 0
    client.send_control(control)
    telemetry, success = client.receive_telemetry()
    if success:
        print(f'Глубина {telemetry.depth}')
        print(f'Глубина {telemetry.voltmeter}')