# main.py
from PyQt5.QtWidgets import QApplication
import sys
from ui.overlay import OverlayWindow

if __name__ == "__main__":
    app = QApplication(sys.argv)
    overlay = OverlayWindow()
    overlay.show()
    sys.exit(app.exec_())
