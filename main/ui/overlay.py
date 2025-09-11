from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout
from PyQt5.QtCore import Qt, QTimer
from system.fps_monitor_utils import FPSMonitor  # dxcam FPS monitor
from system.cpu_monitor import CPUMonitor
import pynvml


class OverlayWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowFlags(
            Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
        )
        self.setAttribute(Qt.WA_TranslucentBackground)

        # --- Layout ---
        self.layout = QVBoxLayout()
        self.cpu_label = QLabel("CPU: -- % | Temp: -- °C")
        self.gpu_label = QLabel("GPU: -- % | Temp: -- °C")
        self.fps_label = QLabel("FPS: --")

        for lbl in (self.cpu_label, self.gpu_label, self.fps_label):
            lbl.setStyleSheet("color: lime; font-family: Consolas; font-size: 14px;")

        self.layout.addWidget(self.cpu_label)
        self.layout.addWidget(self.gpu_label)
        self.layout.addWidget(self.fps_label)
        self.setLayout(self.layout)

        # --- Monitors ---
        self.cpu_monitor = CPUMonitor()
        self.fps_monitor = FPSMonitor(use_dxcam=True)

        # --- Init GPU (NVIDIA via pynvml) ---
        try:
            pynvml.nvmlInit()
            self.gpu_handle = pynvml.nvmlDeviceGetHandleByIndex(0)
        except Exception:
            self.gpu_handle = None

        # --- Timers ---
        self.stats_timer = QTimer()
        self.stats_timer.timeout.connect(self.update_stats)
        self.stats_timer.start(500)  # 0.5s update

        self.fps_timer = QTimer()
        self.fps_timer.timeout.connect(self.update_fps)
        self.fps_timer.start(16)  # ~60Hz update

        self.resize(220, 100)

    def update_stats(self):
        # --- CPU stats ---
        cpu_percent, cpu_temp = self.cpu_monitor.sample()
        temp_text = f"{cpu_temp:.1f}°C" if cpu_temp is not None else "--°C"
        self.cpu_label.setText(f"CPU: {cpu_percent:.1f}% | Temp: {temp_text}")

        # --- GPU stats ---
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
        fps_mean = int(self.fps_monitor.update())
        self.fps_monitor.update_dxcam()
        fps_real = int(self.fps_monitor.get_real_fps())
        self.fps_label.setText(f"FPS moyen: {fps_mean} | FPS réel: {fps_real}")

    def closeEvent(self, event):
        self.fps_monitor.stop()
        event.accept()
