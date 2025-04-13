import Networking as nt
client = nt.ROVClient(rov_ip="192.168.1.5", rov_port=3020,
                      local_ip="0.0.0.0", local_port=3010)

# Send control command
control = nt.RovControl(
    axisX=50,  # -100 to 100
    axisY=0,
    axisZ=-30,
    axisW=0,
    cameraRotation=(20, -10),  # Two camera angles
    thrusterPower=(50, 50, 0, 0, 0, 0, 0, 0, 0, 0),
    debugFlag=0,
    manipulatorRotation=50,
    manipulatorOpenClose=1,  # 1 = open, -1 = close
    regulators=1,  # Bit flags
    desiredDepth=2.5,  # meters
    desiredYaw=180.0,  # degrees
    cameraIndex=1  # 0 or 1
)

# Send command
client.send_control(control)

# Receive telemetry
telemetry, success = client.receive_telemetry()
if success:
    print(f"Current depth: {telemetry.depth} meters")
    print(f"Battery voltage: {telemetry.voltmeter}V")

# Close when done
client.close()