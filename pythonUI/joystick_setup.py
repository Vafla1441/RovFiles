import pygame
import tkinter as tk
from tkinter import ttk
from collections import defaultdict

class JoystickMonitor:
    def __init__(self, root):
        self.root = root
        self.root.title("Joystick Monitor")
        
        pygame.init()
        pygame.joystick.init()
        
        self.main_frame = ttk.Frame(root, padding="10")
        self.main_frame.pack(fill=tk.BOTH, expand=True)
        
        self.status_label = ttk.Label(self.main_frame, text="Подключите джойстик...")
        self.status_label.pack()
        
        self.axes_frame = ttk.LabelFrame(self.main_frame, text="Оси", padding="5")
        self.axes_frame.pack(fill=tk.X, pady=5)
        
        self.buttons_frame = ttk.LabelFrame(self.main_frame, text="Кнопки", padding="5")
        self.buttons_frame.pack(fill=tk.X, pady=5)
        
        self.hats_frame = ttk.LabelFrame(self.main_frame, text="Хэты", padding="5")
        self.hats_frame.pack(fill=tk.X, pady=5)
        
        self.axis_labels = {}
        self.button_labels = {}
        self.hat_labels = {}
        self.joystick = None
        
        self.update_status()
    
    def update_status(self):
        if pygame.joystick.get_count() == 0:
            self.status_label.config(text="Джойстик не подключен")
            self.clear_all()
            self.root.after(500, self.update_status)
            return
        
        if self.joystick is None:
            self.joystick = pygame.joystick.Joystick(0)
            self.joystick.init()
            self.status_label.config(text=f"Джойстик: {self.joystick.get_name()}")
        
        pygame.event.pump()
        
        self.update_axes()
        self.update_buttons()
        self.update_hats()
        
        self.root.after(50, self.update_status)
    
    def update_axes(self):
        active_axes = set()
        
        for i in range(self.joystick.get_numaxes()):
            value = self.joystick.get_axis(i)
            if abs(value) > 0.1:
                active_axes.add(i)
                if i not in self.axis_labels:
                    self.create_axis_label(i)

                self.axis_labels[i]['value'].config(text=f"{value:.3f}")
                self.axis_labels[i]['progress'].config(value=(value + 1) * 50)
        
        to_remove = set(self.axis_labels.keys()) - active_axes
        for axis_id in to_remove:
            self.axis_labels[axis_id]['frame'].pack_forget()
            del self.axis_labels[axis_id]
    
    def create_axis_label(self, axis_id):
        frame = ttk.Frame(self.axes_frame)
        frame.pack(fill=tk.X, pady=2)
        
        label = ttk.Label(frame, text=f"Ось {axis_id}:", width=10)
        label.pack(side=tk.LEFT)
        
        value_label = ttk.Label(frame, text="0.000", width=10)
        value_label.pack(side=tk.LEFT)
        
        progress = ttk.Progressbar(
            frame, 
            orient=tk.HORIZONTAL, 
            length=200, 
            mode='determinate',
            maximum=100,
            value=50
        )
        progress.pack(side=tk.LEFT, fill=tk.X, expand=True)
        
        self.axis_labels[axis_id] = {
            'frame': frame,
            'value': value_label,
            'progress': progress
        }
    
    def update_buttons(self):
        active_buttons = set()
        
        for i in range(self.joystick.get_numbuttons()):
            state = self.joystick.get_button(i)
            if state:
                active_buttons.add(i)
                if i not in self.button_labels:
                    self.create_button_label(i)
                
                self.button_labels[i]['state'].config(
                    text="НАЖАТА", 
                    foreground='red', 
                    font=('Arial', 9, 'bold')
                )
        
        for btn_id in set(self.button_labels.keys()) - active_buttons:
            self.button_labels[btn_id]['state'].config(
                text="отпущена", 
                foreground='gray', 
                font=('Arial', 9)
            )
    
    def create_button_label(self, button_id):
        frame = ttk.Frame(self.buttons_frame)
        frame.pack(fill=tk.X, pady=2)
        
        label = ttk.Label(frame, text=f"Кнопка {button_id}:", width=15)
        label.pack(side=tk.LEFT)
        
        state_label = ttk.Label(frame, text="отпущена", width=10, foreground='gray')
        state_label.pack(side=tk.LEFT)
        
        self.button_labels[button_id] = {
            'frame': frame,
            'state': state_label
        }
    
    def update_hats(self):
        active_hats = set()
        
        for i in range(self.joystick.get_numhats()):
            hat = self.joystick.get_hat(i)
            if hat != (0, 0):
                active_hats.add(i)
                if i not in self.hat_labels:
                    self.create_hat_label(i)
                
                self.hat_labels[i]['value'].config(
                    text=f"{hat}", 
                    foreground='blue', 
                    font=('Arial', 9, 'bold')
                )
        
        for hat_id in set(self.hat_labels.keys()) - active_hats:
            self.hat_labels[hat_id]['value'].config(
                text="(0, 0)", 
                foreground='gray', 
                font=('Arial', 9)
            )
    
    def create_hat_label(self, hat_id):
        frame = ttk.Frame(self.hats_frame)
        frame.pack(fill=tk.X, pady=2)
        
        label = ttk.Label(frame, text=f"Хэт {hat_id}:", width=15)
        label.pack(side=tk.LEFT)
        
        value_label = ttk.Label(frame, text="(0, 0)", width=10, foreground='gray')
        value_label.pack(side=tk.LEFT)
        
        self.hat_labels[hat_id] = {
            'frame': frame,
            'value': value_label
        }
    
    def clear_all(self):
        for widget in self.axes_frame.winfo_children():
            widget.destroy()
        for widget in self.buttons_frame.winfo_children():
            widget.destroy()
        for widget in self.hats_frame.winfo_children():
            widget.destroy()
        
        self.axis_labels = {}
        self.button_labels = {}
        self.hat_labels = {}

if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("500x600")
    app = JoystickMonitor(root)
    root.mainloop()
    pygame.quit()