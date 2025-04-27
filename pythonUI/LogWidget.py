import tkinter as tk
from tkinter import ttk
from datetime import datetime
from tkinter.scrolledtext import ScrolledText
import glob
import serial
import threading

class LogConsole(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        super().__init__(parent, *args, **kwargs)
        self.configure(
            bg="#2E2E2E",
            padx=10,
            pady=10,
            highlightbackground="#263238",
            highlightthickness=1
        )

        ttk.Label(
            self,
            text="КОНСОЛЬ",
            style='ConsoleHeader.TLabel'
        ).pack(fill='x', pady=(0, 10))

        self.text_area = ScrolledText(
            self,
            state='disabled',
            wrap='word',
            bg="#263238",
            fg="#CFD8DC",
            insertbackground='#FF5722',
            selectbackground='#455A64',
            selectforeground='#FFFFFF',
            width=60,
            height=15,
            font=('Segoe UI', 9),
            relief=tk.FLAT,
            borderwidth=0,
            padx=4,
            pady=4
        )
        self.text_area.pack(expand=True, fill='both')

        self.text_area.tag_config('TIME', foreground='#78909C')
        self.text_area.tag_config('INFO', foreground='#8BC34A')
        self.text_area.tag_config('WARNING', foreground='#FFC107')
        self.text_area.tag_config('ERROR', foreground='#FF5252')
        self.text_area.tag_config('DEBUG', foreground='#B39DDB')
        self.text_area.tag_config('SERIAL', foreground='#4FC3F7')

        self.toolbar_frame = tk.Frame(self, bg="#37474F", padx=6, pady=4)
        self.toolbar_frame.pack(fill='x', pady=(10, 0))

        tk.Label(
            self.toolbar_frame,
            text="Порт:",
            bg="#37474F",
            fg="#CFD8DC",
            font=('Segoe UI', 9)
        ).pack(side='left', padx=(0, 4))

        self.port_var = tk.StringVar()
        self.port_combobox = ttk.Combobox(
            self.toolbar_frame,
            textvariable=self.port_var,
            state='readonly',
            width=18,
            font=('Segoe UI', 9)
        )
        self.port_combobox.pack(side='left', padx=4)

        ttk.Button(
            self.toolbar_frame,
            text="Подключить",
            command=self.toggle_serial_connection,
            style='Console.TButton'
        ).pack(side='left', padx=4)
        
        ttk.Button(
            self.toolbar_frame,
            text="Очистить",
            command=self.clear_logs,
            style='Console.TButton'
        ).pack(side='left', padx=4)
        
        ttk.Button(
            self.toolbar_frame,
            text="Экспорт",
            command=self.export_logs,
            style='Console.TButton'
        ).pack(side='left', padx=4)

        self.status_label = tk.Label(
            self.toolbar_frame,
            text=" Готово ",
            bg="#37474F",
            fg="#78909C",
            font=('Segoe UI', 8),
            anchor='w',
            width=20
        )
        self.status_label.pack(side='right', padx=4)

        self._setup_styles()

        self.refresh_ports()

        self.serial_connection = None
        self.serial_thread = None
        self.running = False

    def _setup_styles(self):
        style = ttk.Style()
        style.theme_use('clam')
        
        style.configure(
            'ConsoleHeader.TLabel', 
            background="#263238",
            foreground="#FF5722",
            font=('Segoe UI', 10, 'bold'),
            padding=4
        )
        
        style.configure(
            'Console.TButton',
            background="#263238",
            foreground="#ECEFF1",
            bordercolor="#455A64",
            focuscolor="#FF5722",
            font=('Segoe UI', 9),
            padding=4
        )
        
        style.map(
            'Console.TButton',
            background=[
                ('active', '#263238'), 
                ('disabled', '#455A64')
            ],
            foreground=[
                ('active', '#FF5722'), 
                ('disabled', '#78909C')
            ]
        )
        
        style.configure(
            'TCombobox',
            fieldbackground="#263238",
            background="#263238",
            foreground="#ECEFF1",
            selectbackground="#455A64",
            selectforeground="#FFFFFF",
            bordercolor="#455A64",
            arrowsize=12
        )
        
        style.map(
            'TCombobox',
            fieldbackground=[('readonly', '#263238')],
            background=[('readonly', '#263238')]
        )
    
    def refresh_ports(self):
        ports = glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*')
        self.port_combobox['values'] = ports
        if ports:
            self.port_var.set(ports[0])
        else:
            self.port_var.set('')
            self.log("Не найдены доступные последовательные порты", 'WARNING')
    
    def toggle_serial_connection(self):
        if self.serial_connection and self.serial_connection.is_open:
            self.disconnect_serial()
        else:
            self.connect_serial()
    
    def connect_serial(self):
        port = self.port_var.get()
        if not port:
            self.log("Порт не выбран", 'ERROR')
            return
            
        try:
            self.serial_connection = serial.Serial(
                port=port,
                baudrate=115200,
                timeout=1
            )
            self.running = True
            self.serial_thread = threading.Thread(
                target=self.read_serial,
                daemon=True
            )
            self.serial_thread.start()
            self.connect_button.config(text="Отключиться")
            self.log(f"Подключено к {port}")
            self.update_status(f"Подключено: {port}")
        except Exception as e:
            self.log(f"Ошибка подключения: {str(e)}", 'ERROR')
            self.update_status("Ошибка подключения")
    
    def disconnect_serial(self):
        self.running = False
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
        self.connect_button.config(text="Подключиться")
        self.log("Отключено от последовательного порта")
        self.update_status("Отключено")
    
    def read_serial(self):
        while self.running and self.serial_connection and self.serial_connection.is_open:
            try:
                line = self.serial_connection.readline().decode('utf-8').strip()
                if line:
                    self.after(0, self.display_serial_data, line)
            except Exception as e:
                self.after(0, self.log, f"Ошибка чтения: {str(e)}", 'ERROR')
                self.after(0, self.disconnect_serial)
                break
    
    def display_serial_data(self, data):
        self.text_area.configure(state='normal')
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.text_area.insert('end', f"[{timestamp}] ", 'TIME')
        self.text_area.insert('end', f"{data}\n", 'SERIAL')
        self.text_area.configure(state='disabled')
        self.text_area.see('end')
    
    def log(self, message, level='INFO'):
        self.text_area.configure(state='normal')
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.text_area.insert('end', f"[{timestamp}] ", 'TIME')
        self.text_area.insert('end', f"{message}\n", level)
        self.text_area.configure(state='disabled')
        self.text_area.see('end')
        self.update_status(f"Добавлено: {level}")

    def export_logs(self):
        try:
            filename = f'logs_{datetime.now().strftime("%d.%m.%Y_%H:%M:%S")}.txt'
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(self.text_area.get('1.0', 'end'))
            self.log(f"Логи экспортированы в {filename}", 'INFO')
            self.update_status(f"Экспорт в {filename}")
        except Exception as e:
            self.log(f"Ошибка экспорта: {str(e)}", 'ERROR')

    def clear_logs(self):
        self.text_area.configure(state='normal')
        self.text_area.delete('1.0', 'end')
        self.text_area.configure(state='disabled')
        self.log("Логи очищены")
        self.update_status("Очищено")

    def update_status(self, message):
        self.status_label.config(text=f" {message} ")
        self.after(1000, lambda: self.status_label.config(text=" Готово "))
    
    def destroy(self):
        self.disconnect_serial()
        super().destroy()