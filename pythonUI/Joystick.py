import pygame
import json

class Joystick:
    def __init__(self, add_log):
        self.log = add_log

        pygame.init()
        pygame.joystick.init()

        self.is_joystick = True

        self.mappings = {
            "axis_x": 0,
            "axis_y": 0,
            "axis_z": 0,
            "axis_w": 0,
            "manipulator_rotation": 0,
            "camera_rotation": 0,
            "manipulator_open": 0,
            "manipulator_close": 0,
            "twenty_power": 0,
            "fifty_power": 0,
            "pump": 0,
            "laser": 0,
            "stop_polnagr": 0
        }
        self.values = {
            "axis_x": 0,
            "axis_y": 0,
            "axis_z": 0,
            "axis_w": 0,
            "manipulator_rotation": 0,
            "camera_rotation": [0, 0],
            "scale_power": 1.0,
            "manipulator_open_close": 0,
            "pump_laser": 0
        }

        if pygame.joystick.get_count() == 0:
            self.is_joystick = False
            self.log("Не обнаружено джойстика!", "ERROR")
            self.update_buttons()
            return

        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        self.update_buttons()

    def update_buttons(self):
        try:
            with open("Joystick.settings", "r") as f:
                self.mappings = json.load(f)
        except (FileNotFoundError, json.JSONDecodeError):
            self.log("Невозможно загрузить настройки джойстика", "ERROR")

    def update(self):
        if not self.is_joystick:
            return 
        pygame.event.pump() 

        self.values['axis_x'] = self.joystick.get_axis(self.mappings['axis_x'])
        self.values['axis_y'] = self.joystick.get_axis(self.mappings['axis_y'])
        self.values['axis_z'] = self.joystick.get_axis(self.mappings['axis_z']) 
        self.values['axis_w'] = self.joystick.get_axis(self.mappings['axis_w'])
        
        for event in pygame.event.get():
            if event.type == pygame.JOYHATMOTION:
                hat_x, hat_y = event.value
                self.values['manipulator_rotation'] = hat_x
                self.values['camera_rotation'][0] = hat_y
                self.values['camera_rotation'][1] = hat_y
                self.log(self.values['camera_rotation'][0], self.values['camera_rotation'][1])

        power_buttons_pressed = False
        button_actions = [
            "manipulator_open", "manipulator_close", 
            "pump", "laser", "stop_polnagr", 
            "twenty_power", "fifty_power"
        ]


        for action in button_actions:
            button_id = self.mappings[action]
            if self.joystick.get_button(button_id):
                self._handle_button_press(action)
                if action in ("twenty_power", "fifty_power"):
                    power_buttons_pressed = True
        
        if not power_buttons_pressed:
            self.values["scale_power"] = 1.0


    def _handle_button_press(self, button_name):
        if button_name == "manipulator_close":
            self.values["manipulator_open_close"] = -1
        elif button_name == "manipulator_open":
            self.values["manipulator_open_close"] = 1

        if button_name == "stop_polnagr":
            self.values["pump_laser"] = 0
            self.log("Полезная нагрузка остановлена")
        elif button_name == "pump":
            self.values["pump_laser"] = 1
            self.log("Насос качает")
        elif button_name == "laser":
            self.values["pump_laser"] = 2
            self.log("Лазер вкл")

        if button_name == "twenty_power":
            self.values["scale_power"] = 0.25
            self.log("Мощность двигателей установлена в 25%")
        elif button_name == "fifty_power":
            self.values["scale_power"] = 0.5
            self.log("Мощность двигателей установлена в 50%")
