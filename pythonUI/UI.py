import queue
import tkinter as tk
from tkinter import ttk
from TelemetryWidget import TelemetryWidget
from LogWidget import LogConsole
from CameraWidget import CameraWidget

class UI(tk.Tk):
    def __init__(self, client):
        super().__init__()
        self.log_queue = queue.Queue()
        self.client = client
        self._setup_ui_components()
        self._configure_ui_layout()
        self._apply_styles()
        self._start_log_queue_processing()

    def _setup_ui_components(self):
        self.title("RovUI")
        self.geometry("1500x1000")
        self.configure(bg="#353535")

        self.telemetry_frame = TelemetryWidget(self, self.client)
        
        pipeline_config = (
            'rtspsrc location="rtsp://root:12345@192.168.1.6/stream=0" latency=0 ! '
            'rtph264depay ! avdec_h264 ! videoconvert ! autovideosink sync=false'
        )
        self.camera_widget = CameraWidget(self)

        self.log_console_frame = tk.Frame(self, bg="#353535")
        self.log_console = LogConsole(self.log_console_frame)
        self.log_console.log("Система инициализирована")


    def _configure_ui_layout(self):
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=3)
        self.grid_columnconfigure(0, weight=2)
        self.grid_columnconfigure(1, weight=3)
        
        self.telemetry_frame.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
        
        self.log_console_frame.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
        self.log_console_frame.grid_rowconfigure(0, weight=1)
        self.log_console_frame.grid_columnconfigure(0, weight=1)
        self.log_console.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
        
        self.camera_widget.grid(row=0, column=1, rowspan=2, sticky="nsew", padx=5, pady=5)

    def _apply_styles(self):
        style = ttk.Style()
        style.theme_use('clam')
        style.configure('TButton', 
                       background='#555555', 
                       foreground='white')
        style.map('TButton', 
                background=[('active', '#666666'), 
                          ('pressed', '#444444')])

    def _start_log_queue_processing(self):
        self._process_log_queue()
        
    def _process_log_queue(self):
        while not self.log_queue.empty():
            message, level = self.log_queue.get()
            self.log_console.log(message, level)
        self.after(100, self._process_log_queue)

    def run(self):
        self.mainloop()