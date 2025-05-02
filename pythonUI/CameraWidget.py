import tkinter as tk
from tkinter import ttk
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib
import threading

class CameraWidget(tk.Frame):
    def __init__(self, parent, add_log, *args, **kwargs):
        super().__init__(parent, *args, **kwargs)
        self.log = add_log
        self.configure(
            bg="#2E2E2E", 
            padx=10, 
            pady=10,
            highlightbackground="#263238",
            highlightthickness=1
        )
        
        Gst.init(None)
        pipeline_str = (
            'rtspsrc location="rtsp://root:12345@192.168.1.6/stream=0" latency=0 ! '
            'rtph264depay ! avdec_h264 ! videoconvert ! autovideosink sync=false'
        )
        self.pipeline = Gst.parse_launch(pipeline_str)
        
        self.header = ttk.Label(
            self, 
            text="ВИДЕО С КАМЕРЫ", 
            style='CameraHeader.TLabel'
        )
        self.header.pack(fill='x', pady=(0, 10))
        
        self.video_label = tk.Label(
            self, 
            bg="#263238", 
            bd=2, 
            relief=tk.FLAT
        )
        self.video_label.pack(fill=tk.BOTH, expand=True)
        
        self.control_frame = tk.Frame(
            self, 
            bg="#37474F", 
            padx=5, 
            pady=5
        )
        self.control_frame.pack(fill=tk.X, pady=(10, 0))

        self.btn_style = ttk.Style()
        self.btn_style.configure(
            'Camera.TButton', 
            background="#263238", 
            foreground="#ECEFF1",
            bordercolor="#FF5722",
            focuscolor="#FF5722",
            font=('Segoe UI', 9)
        )
        
        self.connect_btn = ttk.Button(
            self.control_frame, 
            text="Подключить", 
            command=self.connect_camera,
            style='Camera.TButton'
        )
        self.connect_btn.pack(side=tk.LEFT, padx=5)
        
        self.connect_btn = ttk.Button(
            self.control_frame, 
            text="Отключить", 
            command=self.disconnect_camera,
            style='Camera.TButton'
        )
        self.connect_btn.pack(side=tk.LEFT, padx=5)

        self.status_label = tk.Label(
            self.control_frame,
            bg="#37474F",
            fg="#CFD8DC",
            font=('Segoe UI', 9),
            text="Камера отключена"
        )
        self.status_label.pack(side=tk.RIGHT, padx=5)
        
        self._setup_styles()

    def _setup_styles(self):
        style = ttk.Style()
        style.theme_use('clam')
        style.configure(
            'CameraHeader.TLabel', 
            background="#263238",
            foreground="#FF5722",
            font=('Segoe UI', 10, 'bold'),
            padding=4
        )
        style.map(
            'Camera.TButton',
            background=[('active', '#263238'), ('disabled', '#455A64')],
            foreground=[('active', '#FF5722'), ('disabled', '#78909C')]
        )

    def connect_camera(self):
        self.log("Подключаемся к камере")
        threading.Thread(target=self.run, daemon=True).start()

    def disconnect_camera(self):
        if self.pipeline:
            self.log("Отключаемся от камеры")
            self.pipeline.set_state(Gst.State.NULL)
            if self.loop:
                self.loop.quit()

    def run(self):
        self.pipeline.set_state(Gst.State.PLAYING)
        self.loop = GLib.MainLoop()
        try:
            self.loop.run()
        except KeyboardInterrupt:
            self.pipeline.set_state(Gst.State.NULL)
            self.loop.quit()

