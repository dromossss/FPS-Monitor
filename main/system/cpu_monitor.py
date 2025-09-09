import time
import psutil
from collections import deque
import wmi

class CPUMonitor:
    def __init__(self, window_size=10):
        self.samples = deque(maxlen=window_size)
        # Init WMI for Windows temps (requires OpenHardwareMonitor running)
        try:
            self.w = wmi.WMI(namespace="root\\OpenHardwareMonitor")
        except Exception:
            self.w = None

    def get_temp(self):
        """Get CPU temperature (Windows via OpenHardwareMonitor, else psutil)."""
        temp = None

        # --- Windows path (WMI + OHM) ---
        if self.w:
            try:
                sensors = self.w.Sensor()
                for sensor in sensors:
                    if sensor.SensorType == u"Temperature" and "CPU" in sensor.Name:
                        temp = float(sensor.Value)
                        break
            except Exception:
                pass

        # --- Linux / others via psutil ---
        if temp is None:
            try:
                temps = psutil.sensors_temperatures()
                if "coretemp" in temps:  # Linux
                    temp = temps["coretemp"][0].current
                elif "cpu-thermal" in temps:  # ARM laptops
                    temp = temps["cpu-thermal"][0].current
            except Exception:
                pass

        return temp

    def sample(self):
        # Task Manager-like CPU % (short interval)
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
        temp_text = f"{temp:.0f}°C" if temp else "N/A°C"
        print(f"CPU Instant: {cpu:.1f}% | Avg: {monitor.get_avg():.1f}% | Temp: {temp_text}")
        time.sleep(0.5)  # refresh rate
