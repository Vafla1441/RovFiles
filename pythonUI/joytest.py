import pygame
from time import sleep

pygame.init()
pygame.joystick.init()
joy = 0
while True:
    for event in pygame.event.get():
        if event.type == pygame.JOYDEVICEADDED:
            joy = pygame.joystick.Joystick(event.device_index)
    for i in range(joy.get_numbuttons()):
        print(f'Ось {i + 1} = {joy.get_button(i)}')
    print()
    sleep(0.1)
