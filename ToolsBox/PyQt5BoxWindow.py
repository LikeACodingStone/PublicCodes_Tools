from PyQt5 import QtCore, QtGui, QtWidgets
from QPyQt5BoxWindow import Ui_MainWindow
import configparser
import os
import subprocess

def apply_tools_config(ui, ini_path):
    """
    Traverse and read config\ToolsNames.ini, and update UI buttons and labels.
    - x, y: position to replace button/label
    - icon: set button icon and background
    - description: set label text
    - rgb: set button/label color if not 'default'
    """
    import configparser
    import os
    from PyQt5 import QtGui

    config = configparser.ConfigParser()
    config.read(ini_path, encoding='utf-8')

    # icons folder is config/icons under current ini_path folder
    icons_folder = os.path.join(os.path.dirname(ini_path), "icons")
    tools_folder = os.path.join(os.path.dirname(__file__), "tools")

    configured_positions = set()
    for section in config.sections():
        x = int(config.get(section, 'x', fallback='0'))
        y = int(config.get(section, 'y', fallback='0'))
        configured_positions.add((y, x))
        icon_file = config.get(section, 'icon', fallback=None)
        exec_file = config.get(section, 'exec', fallback=None)
        label_text = section
        description = config.get(section, 'description', fallback=label_text)
        rgbBtn = config.get(section, 'rgbBtn', fallback='default')
        rgblbl = config.get(section, 'rgblbl', fallback='default')

        btn = ui.pushButtons[y][x]
        lbl = ui.labels[y][x]

        # Set button icon if icon exists
        if icon_file:
            icon_path = os.path.join(icons_folder, icon_file)
            if os.path.exists(icon_path):
                btn.setIcon(QtGui.QIcon(icon_path))
                btn.setIconSize(btn.size())
                btn.setText("")
                btn.setFlat(True)
                btn.setAutoDefault(False)
                btn.setDefault(False)
                btn.setStyleSheet("""
                    QPushButton {
                        border: none;
                        border-image: none;
                        background: transparent;
                        outline: none;
                        box-shadow: none;
                        padding: 0px;
                    }
                    QPushButton:focus { outline: none; border: none; }
                    QPushButton:default { border: none; }
                """)
          # Always set button color if rgbBtn is not 'default'

        if rgbBtn.lower() != 'default':
            rgb_tuple = rgbBtn.strip('()').split(',')
            if len(rgb_tuple) == 3:
                r, g, b = [int(v) for v in rgb_tuple]
                color_style = f"background-color: rgb({r}, {g}, {b});"
                btn.setStyleSheet(color_style)

        lbl.setText(label_text)
        # Set label font to bold (黑体)
        font = lbl.font()
        font.setBold(True)
        lbl.setFont(font)

        # Set label color if rgblbl is not 'default'
        if rgblbl.lower() != 'default':
            rgb_tuple = rgblbl.strip('()').split(',')
            if len(rgb_tuple) == 3:
                r, g, b = [int(v) for v in rgb_tuple]
                color_style = f"background-color: rgb({r}, {g}, {b});"
                lbl.setStyleSheet(color_style)
        # Optionally set tooltip or description
        btn.setToolTip(description)
        lbl.setToolTip(description)

        def make_label_click_handler(desc):
            def handler(event):
                if hasattr(ui, "displayBox"):
                    ui.displayBox.append(desc)
            return handler
        lbl.mousePressEvent = make_label_click_handler(description)
        lbl.setCursor(QtCore.Qt.PointingHandCursor)

        if exec_file and isinstance(exec_file, str) and exec_file.lower() != 'none':
            exec_path = os.path.join(tools_folder, exec_file)
            def run_exec(patxec_pat):
                try:
                    if isinstance(exec_path, str) and os.path.exists(exec_path):
                        subprocess.Popen([exec_path], shell=True)
                    else:
                        error_msg = f"Executable not found or invalid: {exec_path}"
                        if hasattr(ui, "displayBox"):
                            ui.displayBox.append(error_msg)
                        else:
                            print(error_msg)
                except Exception as e:
                    error_msg = f"Error running {exec_path}: {e}"
                    if hasattr(ui, "displayBox"):
                        ui.displayBox.append(error_msg)
                    else:
                        print(error_msg)
            btn.clicked.connect(run_exec)

    for row_idx, row in enumerate(ui.pushButtons):
        for col_idx, btn in enumerate(row):
            if (row_idx, col_idx) not in configured_positions:
                btn.setEnabled(False)

    # Set displayBox font to SimHei (or Heiti), and enlarge it 3x for better visibility
    if hasattr(ui, "displayBox"):
        font = ui.displayBox.font()
        font.setFamily("黑体")  # SimHei or Microsoft YaHei are also acceptable
        font.setPointSize(font.pointSize() * 2)
        ui.displayBox.setFont(font)

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    ini_path = os.path.join(os.path.dirname(__file__), "config", "ToolsNames.ini")
    apply_tools_config(ui, ini_path)
    MainWindow.show()
    sys.exit(app.exec_())
