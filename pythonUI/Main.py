import Networking as nt
import time


def main():
    # Настройки подключения (должны совпадать с настройками ROV)
    ROV_IP = "192.168.1.5"  # IP адрес ROV
    ROV_PORT = 3020         # Порт ROV

    # Создаем клиент
    client = nt.ROVClient(ROV_IP, ROV_PORT)
    # Отправляем hello сообщение
    client.send_hello()
    # Создаем команду управления
    control = nt.RovControl(
        axisX=50,          # Движение вперед (0-100)
        axisY=0,           # Боковое движение (-100 - 100)
        axisZ=0,           # Вертикальное движение (-100 - 100)
        axisW=0,           # Вращение (-100 - 100)
        cameraRotation=(0, 0, 0),  # Поворот камер
        thrusterPower=(0,) * 10,
        debugFlag=0,
        cameraIndex=0       # Выбор камеры 0
    )

    while True:
        # Отправляем команду управления
        if not client.send_control(control):
            print("Failed to send control command")

            # Получаем телеметрию
        telemetry = client.receive_telemetry()
        if telemetry:
            print(f"Depth: {telemetry.depth:.2f}m, "
                  f"Temp:{telemetry.temperature:.1f}C, "
                  f"Voltage: {telemetry.voltmeter:.1f}V, """
                  f"Current: {telemetry.ammeter:.1f}A")

        time.sleep(0.1)  # 10 Hz update rate


if __name__ == "__main__":
    main()
