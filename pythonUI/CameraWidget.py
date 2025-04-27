import tkinter as tk
import cv2 as cv
from tkinter import ttk
from PIL import Image, ImageTk

class CameraWidget(tk.Frame):
    def __init__(self, parent, video_source, *args, **kwargs):
        super().__init__(parent, *args, **kwargs)
        self.configure(
            bg="#2E2E2E", 
            padx=10, 
            pady=10,
            highlightbackground="#263238",
            highlightthickness=1
        )
        
        self.video_source = video_source
        self.cap = None
        
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
        
        self.disconnect_btn = ttk.Button(
            self.control_frame, 
            text="Отключить", 
            command=self.disconnect_camera, 
            state=tk.DISABLED,
            style='Camera.TButton'
        )
        self.disconnect_btn.pack(side=tk.LEFT, padx=5)
        
        self.status_label = tk.Label(
            self.control_frame,
            bg="#37474F",
            fg="#CFD8DC",
            font=('Segoe UI', 9),
            text="Камера отключена"
        )
        self.status_label.pack(side=tk.RIGHT, padx=5)
        
        self.current_image = None
        
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
        try:
            self.cap = cv.VideoCapture(self.video_source)
            if not self.cap.isOpened():
                raise ValueError("Не удалось открыть видеопоток")
            
            self.connect_btn.config(state=tk.DISABLED)
            self.disconnect_btn.config(state=tk.NORMAL)
            self.status_label.config(text="Камера подключена")
            
            self.update_frame()
            
        except Exception as e:
            self.status_label.config(text=f"Ошибка: {str(e)}")
            if self.cap:
                self.cap.release()

    def disconnect_camera(self):
        if self.cap:
            self.cap.release()
            self.cap = None
        
        self.connect_btn.config(state=tk.NORMAL)
        self.disconnect_btn.config(state=tk.DISABLED)
        self.status_label.config(text="Камера отключена")
        
        self.video_label.config(image='')

    def update_frame(self):
        if self.cap and self.cap.isOpened():
            ret, frame = self.cap.read()
            
            if ret:
                frame = cv.cvtColor(frame, cv.COLOR_BGR2RGB)
                img = Image.fromarray(frame)
                
                img_width = self.video_label.winfo_width()
                img_height = self.video_label.winfo_height()
                
                if img_width > 0 and img_height > 0:
                    img.thumbnail((img_width, img_height))
                
                imgtk = ImageTk.PhotoImage(image=img)
                
                self.video_label.imgtk = imgtk
                self.video_label.config(image=imgtk)
            
            self.after(10, self.update_frame)
        else:
            self.disconnect_camera()

    def __del__(self):
        if self.cap:
            self.cap.release()