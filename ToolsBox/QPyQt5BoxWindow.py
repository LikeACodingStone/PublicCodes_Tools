# -*- coding: utf-8 -*-

from PyQt5 import QtCore, QtGui, QtWidgets

class ClickableLabel(QtWidgets.QLabel):
    clicked = QtCore.pyqtSignal(int, int)

    def __init__(self, row, col, parent=None):
        super().__init__(parent)
        self.row = row
        self.col = col

    def mousePressEvent(self, event):
        self.clicked.emit(self.row, self.col)

def get_screen_size(self):
    screen = QtWidgets.QApplication.primaryScreen()
    size = screen.size()
    return size.width(), size.height()

class Ui_MainWindow(object):
    def __init__(self):
        self.selected_row = None
        self.selected_col = None

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("HundredBoxTool")
        screen_width, screen_height = get_screen_size(self)
        real_width = int(screen_width * 0.5)
        real_height = int(real_width * 0.8)
        MainWindow.resize(real_width, real_height)

        self.PyQt5BoxWindow = QtWidgets.QWidget(MainWindow)
        self.PyQt5BoxWindow.setObjectName("PyQt5BoxWindow")
        self.PyQt5BoxWindow.setStyleSheet("background-color: rgb(176, 216, 230);")
        MainWindow.setCentralWidget(self.PyQt5BoxWindow)

        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.add_widgets(real_width, real_height)
        self.set_button_label_colors()

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        self.selected_row = 0
        self.selected_col = 0
        self.highlight_selected_label()

        # Add these lines for ClearDisplay and Exit button functionality
        self.extraButtons[2].clicked.connect(self.clear_display)
        self.extraButtons[5].clicked.connect(MainWindow.close)

    def add_widgets(self, real_width, real_height):
        self.buttonwidth = int(real_width * 0.1)
        self.buttonheight = int(real_height * 0.06)
        self.labelheight = int(real_width * 0.02)
        self.btnHirozonSpace = int((real_width - 40 - (self.buttonwidth*8)) / 7)
        self.varticalSpace = int(real_height * 0.03)
        self.pushButtons = []
        self.labels = []
        for row in range(7):
            y_offset = 20 + row * (self.buttonheight + self.labelheight + self.varticalSpace)
            row_buttons = []
            for i in range(8):
                btn = QtWidgets.QPushButton(self.PyQt5BoxWindow)
                btn.setText("ERR" + str(i + 1))
                x = 20 + i * (self.buttonwidth + self.btnHirozonSpace)
                btn.setGeometry(QtCore.QRect(x, y_offset, self.buttonwidth, self.buttonheight))
                row_buttons.append(btn)
            self.pushButtons.append(row_buttons)
            row_labels = []
            y_label = y_offset + self.buttonheight
            for i in range(8):
                lbl = ClickableLabel(row, i, self.PyQt5BoxWindow)
                lbl.setText(f"{str(i+1)*1}")
                x = 20 + i * (self.buttonwidth + self.btnHirozonSpace)
                lbl.setGeometry(QtCore.QRect(x, y_label, self.buttonwidth, self.labelheight))
                lbl.setAlignment(QtCore.Qt.AlignCenter)
                lbl.clicked.connect(self.label_clicked)
                row_labels.append(lbl)
            self.labels.append(row_labels)
        area_height = 20 + 7 * (self.buttonheight + self.labelheight + self.varticalSpace)
        display_width = int(real_width * 0.45)
        display_height = min(int(real_height * 0.25), real_height - area_height - 40)
        display_x = 20
        display_y = area_height + 20

        self.displayBox = QtWidgets.QTextBrowser(self.PyQt5BoxWindow)
        self.displayBox.setGeometry(QtCore.QRect(display_x, display_y, display_width, display_height))
        self.displayBox.setObjectName("displayBox")

        btn_group_width = int(real_width * 0.45)
        btn_group_height = display_height
        btn_group_x = 40 + display_width
        btn_group_y = display_y

        btn_width = int(btn_group_width * 0.28)
        btn_height = int(btn_group_height * 0.45)
        btn_space_x = int((btn_group_width - 3 * btn_width) / 3)
        btn_space_y = int((btn_group_height - 2 * btn_height) / 3) + 2

        self.extraButtons = []
        self.ButtonTexts = [
            "MoveForward", "MoveNo.1", "ClearDisplay", "MoveBack", "MoveLast.1", "Exit"]
        for row in range(2):
            for col in range(3):
                btn = QtWidgets.QPushButton(self.PyQt5BoxWindow)
                btn.setText(self.ButtonTexts[row * 3 + col])
                x = btn_group_x + btn_space_x + col * (btn_width + btn_space_x)
                y = btn_group_y + btn_space_y + row * (btn_height + btn_space_y)
                if row == 0:
                    y = y - 2
                btn.setGeometry(QtCore.QRect(x, y, btn_width, btn_height))
                self.extraButtons.append(btn)

    def set_button_label_colors(self):
        button_colors = [
            "background-color: rgb(160, 160, 160); color: black;",
            "background-color: rgb(160, 160, 160); color: black;",
            "background-color: rgb(160, 160, 160); color: black;",
            "background-color: rgb(160, 160, 160); color: black;",
            "background-color: rgb(160, 160, 160); color: black;"
        ]
        label_colors = [
            "background-color: rgb(189, 183, 107); color: black;",
            "background-color: rgb(143, 188, 143); color: black;",
            "background-color: rgb(0, 191, 255); color: black;",
            "background-color: rgb(230, 230, 250); color: black;",
            "background-color: rgb(216, 191, 216); color: black;"
        ]
        color_idx = 0
        for row in self.pushButtons:
            for btn in row:
                btn.setStyleSheet(button_colors[color_idx % 5])
                color_idx += 1
        # Use label_colors for the bottom 6 buttons
        for idx, btn in enumerate(getattr(self, "extraButtons", [])):
            btn.setStyleSheet(label_colors[idx % 5])
        label_idx = 0
        for row in self.labels:
            for lbl in row:
                lbl.setStyleSheet(label_colors[label_idx % 5])
                label_idx += 1

    def highlight_selected_label(self):
        for r, row in enumerate(self.labels):
            for c, lbl in enumerate(row):
                if r == self.selected_row and c == self.selected_col:
                    lbl.setStyleSheet("background-color: yellow; color: black; border: 2px solid red;")
                else:
                    idx = (r * 8 + c) % 5
                    label_colors = [
                        "background-color: rgb(189, 183, 107); color: black;",
                        "background-color: rgb(143, 188, 143); color: black;",
                        "background-color: rgb(0, 191, 255); color: black;",
                        "background-color: rgb(230, 230, 250); color: black;",
                        "background-color: rgb(216, 191, 216); color: black;"
                    ]
                    lbl.setStyleSheet(label_colors[idx])

    def label_clicked(self, row, col):
        self.selected_row = row
        self.selected_col = col
        self.highlight_selected_label()

    def clear_display(self):
        if hasattr(self, "displayBox"):
            self.displayBox.clear()

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("HundredBoxTool", "HundredBoxTool"))