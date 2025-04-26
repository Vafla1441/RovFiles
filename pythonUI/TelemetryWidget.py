import tkinter as tk
from tkinter import ttk

class TelemetryWidget(tk.Frame):
    def __init__(self, parent, client, *args, **kwargs):
        super().__init__(parent, *args, **kwargs)
        self.client = client
        self.configure(bg="#2E2E2E", padx=10, pady=10)

        self.label_style = {
            'bg': "#2E2E2E",
            'fg': "#ECEFF1",
            'font': ('Segoe UI', 10)
        }
        
        self.value_style = {
            'bg': "#2E2E2E",
            'fg': "#FFFFFF",
            'font': ('Segoe UI', 10, 'bold')
        }
        
        self.section_style = {
            'bg': "#263238",
            'fg': "#CFD8DC",
            'font': ('Segoe UI', 9, 'bold'),
            'relief': tk.FLAT,
            'padx': 8,
            'pady': 4
        }
        
        self.telemetry_vars = {
            'depth': tk.StringVar(value="0.0 м"),
            'pitch': tk.StringVar(value="0.0"),
            'yaw': tk.StringVar(value="0.0"),
            'roll': tk.StringVar(value="0.0"),
            'ampermeter': tk.StringVar(value="0.0 А"),
            'voltmeter': tk.StringVar(value="0.0"),
            'regulatorsFeedback': tk.StringVar(value="0"),
            'manipulatorAngle': tk.StringVar(value="0"),
            'manipulatorState': tk.StringVar(value="0"),
            'cameraIndex': tk.StringVar(value="0"),
            'temperature': tk.StringVar(value="0.0 °C")
        }
        
        self._create_widgets()
        self._update_telemetry()
    
    def _create_widgets(self):
        ttk.Label(self, text="ТЕЛЕМЕТРИЯ ТНПА", style='Telemetry.TLabel').pack(fill='x', pady=(0,10))
        
        # Датчики
        self._create_section("Датчики", [
            ("Глубина", self.telemetry_vars['depth']),
            # ("Тангаж", self.telemetry_vars['pitch']),
            # ("Рыскание", self.telemetry_vars['yaw']),
            # ("Крен", self.telemetry_vars['roll']),
            ("Уровень pH", self.telemetry_vars['voltmeter']),
            ("Температура", self.telemetry_vars['temperature'])
        ])
        
        # Электрика
        self._create_section("Электрика", [
            # ("Напряжение", self.telemetry_vars['voltmeter']),
            ("Ток", self.telemetry_vars['ampermeter'])
        ])

        # Отладка
        # self._create_section("Отладка", [
        #     ("regulatorsFeedback", self.telemetry_vars['regulatorsFeedback']),
        #     ("manipulatorAngle", self.telemetry_vars['manipulatorAngle']),
        #     ("cameraIndex", self.telemetry_vars['cameraIndex'])
        # ])

        self._setup_styles()
    
    def _create_section(self, title, items):
        frame = tk.Frame(self, bg="#263238", relief=tk.RAISED, bd=1)
        frame.pack(fill='x', pady=(0,8))
        
        tk.Label(frame, text=title, **self.section_style).pack(fill='x')
        
        inner_frame = tk.Frame(frame, bg="#37474F")
        inner_frame.pack(fill='x', padx=2, pady=(0,2))
        
        for name, var in items:
            row = tk.Frame(inner_frame, bg="#37474F")
            row.pack(fill='x', pady=1)
            
            tk.Label(row, text=name+":", **self.label_style).pack(side='left', padx=8)
            tk.Label(row, textvariable=var, **self.value_style).pack(side='right', padx=8)
    
    def _setup_styles(self):
        style = ttk.Style()
        style.theme_use('clam')
        style.configure('Telemetry.TLabel', 
                      background="#263238",
                      foreground="#FF5722",
                      font=('Segoe UI', 10, 'bold'),
                      padding=4)
    
    def _update_telemetry(self):
        data = self.client.receive_telemetry()
        # if data:
        #     self.telemetry_vars['depth'].set(f"{data.depth:.2f} м")
        #     self.telemetry_vars['pitch'].set(f"{data.pitch:.2f}")
        #     self.telemetry_vars['yaw'].set(f"{data.yaw:.2f}")
        #     self.telemetry_vars['roll'].set(f"{data.roll:.2f}")
        #     self.telemetry_vars['voltmeter'].set(f"{data.voltmeter:.2f}")
        #     self.telemetry_vars['ampermeter'].set(f"{data.ampermeter:.2f} А")
        #     self.telemetry_vars['regulatorsFeedback'].set(f"{data.regulatorsFeedback}")
        #     self.telemetry_vars['manipulatorAngle'].set(f"{data.manipulatorAngle}")
        #     self.telemetry_vars['manipulatorState'].set(f"{data.manipulatorState}")
        #     self.telemetry_vars['cameraIndex'].set(f"{data.cameraIndex}")
        #     self.telemetry_vars['temperature'].set(f"{data.temperature:.1f} °C")
        self.after(500, self._update_telemetry)