import os,sys
import re, time
import threading
import configparser
from datetime import datetime

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QMessageBox, QButtonGroup
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from Ui_PlayerWeight import Ui_FreePlayer
import pygame
from BaseModules import ConfigParseHandle, GetCurrentFolder

def GetMediaByIndex(playList, mediaIndex):
    filePath = playList[mediaIndex]
    mediName = filePath.split("\\")[-1]
    return mediName

class AudioPlayer:
    def __init__(self, playlist, mediaIndex):
        pygame.init()
        pygame.mixer.init()
        self.playlist = playlist
        self.current_index = mediaIndex
        self.is_paused = False
        self.is_playing = False
        self.end_event = pygame.USEREVENT + 1
        pygame.mixer.music.set_endevent(self.end_event)

    def play_audio(self):
        if self.current_index >= len(self.playlist):
            return
        file_path = self.playlist[self.current_index]
        if file_path.endswith(".wma"):
            pass
        else:
            pygame.mixer.music.load(file_path)
            pygame.mixer.music.play()
        self.is_playing = True

        UpdateConfig()
        ui.lineEdit.setText(GetMediaByIndex(g_playList ,g_audio_player.current_index))

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == self.end_event:
                self.current_index += 1
                self.play_audio()   
    
    def pause_audio(self):
        if not self.is_paused:
            pygame.mixer.music.pause()
            self.is_paused = True
            self.is_playing = False    

    def resume_audio(self):
        if self.is_paused:
            pygame.mixer.music.unpause()
            self.is_paused = False
            self.is_playing = True

    def run(self):
        self.play_audio()
        while self.current_index < len(self.playlist):
            self.handle_events()
            time.sleep(0.1)
        self.is_playing = False


def player_thread(player):
    player.run()

global g_playList
global g_mediaIndex
global g_folderIndex
global g_audio_player
global g_config_parse

@pyqtSlot()
def SlotPlay():
    global g_audio_player
    if g_audio_player.is_paused:
        g_audio_player.resume_audio()  
    else:
        thread = threading.Thread(target=player_thread, args=(g_audio_player,))
        thread.start()
    
@pyqtSlot()
def SlotPause():
    global g_audio_player
    if g_audio_player.is_playing:
        g_audio_player.pause_audio()

@pyqtSlot()
def SlotDelete():
    GenerateDeletFiles()

@pyqtSlot()
def SlotBtnPop():
    global g_config_parse
    GenerateTypeFiles(g_config_parse.style_popular)

@pyqtSlot()
def SlotBtnBlues():
    global g_config_parse
    GenerateTypeFiles(g_config_parse.style_blues)

@pyqtSlot()
def SlotBtnCountry():
    global g_config_parse
    GenerateTypeFiles(g_config_parse.style_country)

@pyqtSlot()
def SlotBtnMetal():
    global g_config_parse
    GenerateTypeFiles(g_config_parse.style_metal)

@pyqtSlot()
def SlotBtnPunk():
    global g_config_parse
    GenerateTypeFiles(g_config_parse.style_punk)

@pyqtSlot()
def SlotBtnHardRock():
    global g_config_parse
    GenerateTypeFiles(g_config_parse.style_hardRock)

@pyqtSlot()
def SlotNextSong():
    global g_audio_player
    global g_playList
    g_audio_player.current_index += 1
    g_audio_player.play_audio()
    
@pyqtSlot()
def SlotPreSong():
    global g_audio_player
    global g_playList
    g_audio_player.current_index -= 1
    g_audio_player.play_audio()

def GenerateTypeFiles(type):
    type_file = type + ".txt"
    global g_audio_player
    global g_playList
    fileNameContent = g_playList[g_audio_player.current_index]
    global g_config_parse
    g_config_parse.AddStyleList(type_file, fileNameContent)


def GenerateDeletFiles():
    global g_audio_player
    global g_playList
    global g_config_parse
    fileNameContent = g_playList[g_audio_player.current_index]
    g_config_parse.AddDeleteFileList(fileNameContent)

def UpdateConfig():
    global g_audio_player
    global g_config_parse
    g_config_parse.setMediaIndex(g_audio_player.current_index)

def GetIniList():
    global g_config_parse
    playList = []
    for root, dirs, files in os.walk(g_config_parse._media_folder_val):
        for file in files:
            play_file = root + os.sep + file
            playList.append(play_file)
    return playList, g_config_parse._media_folder_val, int(g_config_parse._media_index_val)


if __name__ == '__main__':
    CONFIG = "config.ini"
    g_config_parse = ConfigParseHandle(CONFIG)
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_FreePlayer()
    ui.setupUi(MainWindow)
    ui.btnPlay.clicked.connect(SlotPlay)
    ui.btnPause.clicked.connect(SlotPause)
    ui.btnDel.clicked.connect(SlotDelete)
    ui.btnBlues.clicked.connect(SlotBtnBlues)
    ui.btnCountry.clicked.connect(SlotBtnCountry)
    ui.btnMetal.clicked.connect(SlotBtnMetal)
    ui.btnHard.clicked.connect(SlotBtnHardRock)
    ui.btnPunk.clicked.connect(SlotBtnPunk)
    ui.btnPop.clicked.connect(SlotBtnPop)
    ui.btnNext.clicked.connect(SlotNextSong)
    ui.btnPre.clicked.connect(SlotPreSong)
    g_playList = []
    g_mediaIndex = -1
    g_folderIndex = -1
    g_playList, g_folderIndex, g_mediaIndex = GetIniList()
    g_audio_player = AudioPlayer(g_playList, g_mediaIndex)
    ui.lineEdit.setText(GetMediaByIndex(g_playList, g_mediaIndex))
    MainWindow.show()
    sys.exit(app.exec_())