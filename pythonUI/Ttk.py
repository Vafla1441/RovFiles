from tkinter import *
 
root = Tk()     # создаем корневой объект - окно
root.title("PyUI")     # устанавливаем заголовок окна
root.geometry("800x600")    # устанавливаем размеры окна
 
label = Label(text="Hello METANIT.COM") # создаем текстовую метку
label.pack()    # размещаем метку в окне
 
root.mainloop()