# fps_monitor_utils.py
import time
from collections import deque
import dxcam

class FPSMonitor:
    def __init__(self, max_samples=1000, use_dxcam=False, capture_region=None):
        """
        max_samples    : nombre de mesures pour FPS moyen
        use_dxcam      : True pour capturer le FPS réel via dxcam
        capture_region : tuple (x, y, w, h) ou None pour tout l'écran
        """
        self.frame_times = deque(maxlen=max_samples)
        self.last_time = time.time()

        self.use_dxcam = use_dxcam
        self.capture_region = capture_region

        # dxcam variables
        self.camera = None
        self.frame_count = 0
        self.fps_real = 0.0
        self.fps_last_time = time.time()

        if self.use_dxcam:
            # Crée la caméra avec capture en continu, pas de limite target_fps
            if self.capture_region:
                self.camera = dxcam.create(region=self.capture_region)
            else:
                self.camera = dxcam.create()
            self.camera.start(target_fps=0)  # 0 = capture aussi vite que possible

    # --- FPS moyen ---
    def tick(self):
        """Enregistre le temps écoulé pour FPS moyen"""
        now = time.time()
        delta = now - self.last_time
        self.last_time = now
        self.frame_times.append(delta)

    def get_fps(self):
        """Retourne FPS moyen basé sur tick()"""
        if not self.frame_times:
            return 0.0
        avg_frame_time = sum(self.frame_times) / len(self.frame_times)
        return 1.0 / avg_frame_time if avg_frame_time > 0 else 0.0

    def update(self):
        """Tick + retourne FPS moyen"""
        self.tick()
        return self.get_fps()

    # --- FPS réel via dxcam ---
    def update_dxcam(self):
        """À appeler régulièrement (timer) pour calculer FPS réel"""
        if not self.camera:
            return

        frame = self.camera.get_latest_frame()
        if frame is not None:
            self.frame_count += 1

        now = time.time()
        elapsed = now - self.fps_last_time
        if elapsed >= 0.5:  # calcul toutes les 0.5s
            self.fps_real = self.frame_count / elapsed
            self.frame_count = 0
            self.fps_last_time = now

    def get_real_fps(self):
        """Retourne le dernier FPS réel calculé par update_dxcam()"""
        return self.fps_real

    def stop(self):
        """Arrête la capture dxcam si activée"""
        if self.camera is not None:
            self.camera.stop()
