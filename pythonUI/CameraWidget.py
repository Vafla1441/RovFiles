import tkinter as tk
import cv2 as cv
from tkinter import ttk
from PIL import Image, ImageTk

class CameraWidget(tk.Frame):
    def __init__(self, parent, video_source='rtsp://10.42.0.100/stream=0', *args, **kwargs):
        super().__init__(parent, *args, **kwargs)
        
        self.video_source = video_source
        self.cap = None
        self.video_label = tk.Label(self)
        self.video_label.pack(fill=tk.BOTH, expand=True)
        
        self.control_frame = tk.Frame(self)
        self.control_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.connect_btn = ttk.Button(self.control_frame, text="Подключить", command=self.connect_camera)
        self.connect_btn.pack(side=tk.LEFT, padx=5)
        
        self.disconnect_btn = ttk.Button(self.control_frame, text="Отключить", command=self.disconnect_camera, state=tk.DISABLED)
        self.disconnect_btn.pack(side=tk.LEFT, padx=5)
        
        self.status_label = ttk.Label(self.control_frame, text="Камера отключена")
        self.status_label.pack(side=tk.LEFT, padx=5)
        
        self.current_image = None
        
        # self.connect_camera()

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