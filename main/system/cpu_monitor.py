import time
import psutil
from collections import deque
import clr
import sys
import os

# --- LibreHardwareMonitor ---
use_lhm = False
try:
    dll_path = r"C:\Users\ryadk\OneDrive\Documents\XCODING\codes\fps_tracker\LibreHardwareMonitor-net472"
    sys.path.append(dll_path)
    clr.AddReference("LibreHardwareMonitorLib")
    import LibreHardwareMonitor.Hardware as Hardware
    use_lhm = True
except Exception:
    pass

# --- WMI (OpenHardwareMonitor) ---
use_wmi = False
try:
    import wmi
    w = wmi.WMI(namespace="root\\OpenHardwareMonitor")
    use_wmi = True
except Exception:
    w = None


class CPUMonitor:
    def __init__(self, window_size=10):
        self.samples = deque(maxlen=window_size)

        # Init LibreHardwareMonitor
        if use_lhm:
            self.computer = Hardware.Computer()
            self.computer.IsCpuEnabled = True
            self.computer.Open()
        else:
            self.computer = None

    def _get_temp_lhm(self):
        try:
            for hw in self.computer.Hardware:
                if hw.HardwareType == Hardware.HardwareType.Cpu:
                    hw.Update()
                    for sensor in hw.Sensors:
                        if sensor.SensorType == Hardware.SensorType.Temperature:
                            if "Package" in sensor.Name or "Core #1" in sensor.Name:
                                return sensor.Value
        except Exception:
            pass
        return None

    def _get_temp_wmi(self):
        try:
            if w:
                sensors = w.Sensor()
                for sensor in sensors:
                    if sensor.SensorType == u"Temperature" and "CPU" in sensor.Name:
                        return float(sensor.Value)
        except Exception:
            pass
        return None

    def _get_temp_psutil(self):
        try:
            temps = psutil.sensors_temperatures()
            if "coretemp" in temps:
                return temps["coretemp"][0].current
            elif "cpu-thermal" in temps:
                return temps["cpu-thermal"][0].current
        except Exception:
            pass
        return None

    def get_temp(self):
        if use_lhm and self.computer:
            return self._get_temp_lhm()
        elif use_wmi:
            return self._get_temp_wmi()
        else:
            return self._get_temp_psutil()

    def sample(self):
        cpu_percent = psutil.cpu_percent(interval=0.1, percpu=False)
        self.samples.append(cpu_percent)
        cpu_temp = self.get_temp()
        return cpu_percent, cpu_temp

    def get_avg(self):
        return sum(self.samples) / len(self.samples) if self.samples else 0


# === Demo ===
if __name__ == "__main__":
    monitor = CPUMonitor()
    while True:
        cpu, temp = monitor.sample()
        temp_text = f"{temp:.1f}°C" if temp else "N/A°C"
        print(f"CPU Instant: {cpu:.1f}% | Avg: {monitor.get_avg():.1f}% | Temp: {temp_text}")
        time.sleep(0.5)
