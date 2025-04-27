from UI import UI
from Networking import RovClient
import threading
import queue
import sys
import os

class RovUI:
    def __init__(self):
        self.log_queue = queue.Queue()
        self.running = threading.Event()
        self.running.set()

        self.ui = None
        self.client = None
        self.serial = None

    def ui_callback(self, msg, level='INFO'):
        self.log_queue.put((msg, level))

    def start_ui(self):
        self.ui = UI(self.client)
        self.ui.log_queue = self.log_queue
        self.ui.protocol("WM_DELETE_WINDOW", self.shutdown)
        self.ui.mainloop()

    def start_client(self):
        try:
            self.client = RovClient(add_log=self.ui_callback)
            threading.Thread(target=self.start_ui, daemon=True).start()
            self.client.run()
        except Exception as e:
            print(f"Ошибка клиента: {e}", file=sys.stderr)
            self.shutdown()

    def start_serial(self):
        pass

    def shutdown(self):
        if not self.running.is_set():
            return
            
        self.running.clear()

        if self.ui:
            try:
                self.ui.quit()
                self.ui.destroy()
            except:
                pass

        os._exit(0)

    def run(self):
        try:
            client_thread = threading.Thread(target=self.start_client, daemon=True)
            client_thread.start()
            
            while self.running.is_set():
                client_thread.join(timeout=0.1)
                
        except KeyboardInterrupt:
            self.shutdown()
        except Exception as e:
            print(f"Ошибка приложения: {e}", file=sys.stderr)
            self.shutdown()