# overlay.py
from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QFont
from system.fps_monitor_utils import FPSMonitor  # FPSMonitor mis à jour avec dxcam
import psutil
import pynvml


class OverlayWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool)
        self.setAttribute(Qt.WA_TranslucentBackground)

        # Layout
        self.layout = QVBoxLayout()
        self.cpu_label = QLabel("CPU: -- % | Temp: -- °C")
        self.gpu_label = QLabel("GPU: -- % | Temp: -- °C")
        self.fps_label = QLabel("FPS: --")

        # Style du texte
        for lbl in (self.cpu_label, self.gpu_label, self.fps_label):
            lbl.setStyleSheet("color: lime; font-family: Consolas; font-size: 14px;")

        self.layout.addWidget(self.cpu_label)
        self.layout.addWidget(self.gpu_label)
        self.layout.addWidget(self.fps_label)
        self.setLayout(self.layout)

        # Moniteur FPS avec capture dxcam activée
        self.fps_monitor = FPSMonitor(use_dxcam=True)

        # Init GPU (NVIDIA via pynvml)
        try:
            pynvml.nvmlInit()
            self.gpu_handle = pynvml.nvmlDeviceGetHandleByIndex(0)
        except Exception as e:
            self.gpu_handle = None

        # Timer CPU/GPU (0.5s)
        self.stats_timer = QTimer()
        self.stats_timer.timeout.connect(self.update_stats)
        self.stats_timer.start(500)

        # Timer FPS (~60Hz ou plus rapide pour FPS réel)
        self.fps_timer = QTimer()
        self.fps_timer.timeout.connect(self.update_fps)
        self.fps_timer.start(1)  # 1 ms = max vitesse

        # Taille minimale
        self.resize(200, 100)

    def update_stats(self):
        # --- CPU ---
        cpu_usage = psutil.cpu_percent(interval=None)
        cpu_temp = None
        try:
            temps = psutil.sensors_temperatures()
            if "coretemp" in temps:  # Linux mostly
                cpu_temp = temps["coretemp"][0].current
            elif "cpu-thermal" in temps:  # parfois laptops
                cpu_temp = temps["cpu-thermal"][0].current
        except Exception:
            pass

        cpu_temp_text = f"{cpu_temp:.0f}°C" if cpu_temp else "--°C"
        self.cpu_label.setText(f"CPU: {cpu_usage}% | Temp: {cpu_temp_text}")

        # --- GPU (NVIDIA uniquement) ---
        if self.gpu_handle:
            try:
                gpu_util = pynvml.nvmlDeviceGetUtilizationRates(self.gpu_handle).gpu
                gpu_temp = pynvml.nvmlDeviceGetTemperature(
                    self.gpu_handle, pynvml.NVML_TEMPERATURE_GPU
                )
                self.gpu_label.setText(f"GPU: {gpu_util}% | Temp: {gpu_temp}°C")
            except Exception:
                self.gpu_label.setText("GPU: -- % | Temp: -- °C")
        else:
            self.gpu_label.setText("GPU: -- % | Temp: -- °C")

    def update_fps(self):
        """Met à jour FPS moyen et FPS réel si dxcam activé"""
        # FPS moyen (tick)
        fps_mean = int(self.fps_monitor.update())

        if self.fps_monitor.use_dxcam:
            # FPS réel non bloquant
            self.fps_monitor.update_dxcam()
            fps_real = int(self.fps_monitor.get_real_fps())
            self.fps_label.setText(f"FPS moyen: {fps_mean} | FPS réel: {fps_real}")
        else:
            self.fps_label.setText(f"FPS moyen: {fps_mean}")

    def closeEvent(self, event):
        # Stop dxcam proprement
        self.fps_monitor.stop()
        event.accept()
