#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2010-01-30
@version $Id$

This script runs the gaming GUI for the LNdW traffic light game.
It checks for possible scenarios in the current working directory
and lets the user start them as a game. Furthermore it
saves highscores to local disc and to the central highscore server.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import subprocess
import sys
import re
import pickle
import httplib
import glob
import Tkinter
from optparse import OptionParser
from xml.dom import pulldom

_SCOREFILE = "scores.pkl"
_SCORESERVER = "sumo.dlr.de"
_SCORESCRIPT = "/scores.php?game=TLS&"
_DEBUG = False
_SCORES = 30

_LANGUAGE_EN = {'title': 'Interactive Traffic Light',
                'cross': 'Simple Junction',
                'cross_demo': 'Simple Junction (Demo)',
                'square': 'Four Junctions',
                'grid6': 'Six Junctions',
                'kuehne': 'Prof. Kühne',
                'bs3d': '3D Junction Virtual World',
                'bs3Dosm': '3D Junction OpenStreetMap',
                'ramp': 'Highway Ramp',
                'high': 'Highscore',
                'reset': 'Reset Highscore',
                'lang': 'Deutsch',
                'quit': 'Quit',
                'Highscore': 'Highscore',
                'Congratulations': 'Congratulations!',
                'your score': 'Your Score',
                'Continue': 'Continue',
                }
_LANGUAGE_DE = {'title': 'Interaktives Ampelspiel',
                'cross': 'Einfache Kreuzung',
                'cross_demo': 'Einfache Kreuzung (Demo)',
                'square': 'Vier Kreuzungen',
                'grid6': 'Sechs Kreuzungen',
                'kuehne': 'Prof. Kühne',
                'bs3d': '3D Forschungskreuzung Virtuelle Welt',
                'bs3Dosm': '3D Forschungskreuzung OpenStreetMap',
                'ramp': 'Autobahnauffahrt',
                'high': 'Bestenliste',
                'reset': 'Bestenliste zurücksetzen',
                'lang': 'Englisch',
                'quit': 'Beenden',
                'Highscore': 'Bestenliste',
                'Congratulations': 'Gratulation!',
                'your score': 'Deine Punkte',
                'Continue': 'Weiter',
                }


def loadHighscore():
    try:
        conn = httplib.HTTPConnection(_SCORESERVER)
        conn.request("GET", _SCORESCRIPT + "top=" + str(_SCORES))
        response = conn.getresponse()
        if response.status == httplib.OK:
            scores = {}
            for line in response.read().splitlines():
                category, values = line.split()
                scores[category] = _SCORES * [("", "", -1.)]
                for idx, item in enumerate(values.split(':')):
                    name, game, score = item.split(',')
                    scores[category][idx] = (name, game, int(float(score)))
            return scores
    except:
        pass

    try:
        return pickle.load(open(_SCOREFILE))
    except:
        pass
    return {}


def parseEndTime(cfg):
    cfg_doc = pulldom.parse(cfg)
    for event, parsenode in cfg_doc:
        if event == pulldom.START_ELEMENT and parsenode.localName == 'end':
            return float(parsenode.getAttribute('value'))
            break


class IMAGE:
    pass


class StartDialog(Tkinter.Frame):

    def __init__(self, parent, lang):
        Tkinter.Frame.__init__(self, parent)
        # variables for changing language
        self.parent = parent
        self._language_text = lang
        self.buttons = []
        # misc variables
        self.name = ''
        # setup gui
        self.parent.title(self._language_text['title'])
        self.parent.minsize(250, 50)
        self.category = None

        # we use a grid layout with 4 columns
        COL_DLRLOGO, COL_START, COL_HIGH, COL_SUMOLOGO = range(4)
        # there is one column for every config, +2 more columns for control
        # buttons
        configs = sorted(glob.glob(os.path.join(base, "*.sumocfg")))
        numButtons = len(configs) + 3
        # button dimensions
        bWidth_start = 30
        bWidth_high = 10
        bWidth_control = 41

        self.gametime = 0
        self.ret = 0
        # some pretty images
        Tkinter.Label(self, image=IMAGE.dlrLogo).grid(
            row=0, rowspan=numButtons, column=COL_DLRLOGO)
        Tkinter.Label(self, image=IMAGE.sumoLogo).grid(
            row=0, rowspan=numButtons, column=COL_SUMOLOGO)

        # 2 button for each config (start, highscore)
        for row, cfg in enumerate(configs):
            if "bs3" in cfg and not haveOSG:
                continue
            category = self.category_name(cfg)
            # lambda must make a copy of cfg argument
            button = Tkinter.Button(self, width=bWidth_start,
                                    command=lambda cfg=cfg: self.start_cfg(cfg))
            self.addButton(button, category)
            button.grid(row=row, column=COL_START)

            button = Tkinter.Button(self, width=bWidth_high,
                                    command=lambda cfg=cfg: ScoreDialog(self, [],
                                                                        None, self.category_name(cfg), self._language_text))  # .grid(row=row, column=COL_HIGH)
            self.addButton(button, 'high')
            button.grid(row=row, column=COL_HIGH)

        # control buttons
        button = Tkinter.Button(
            self, width=bWidth_control, command=high.clear)
        self.addButton(button, 'reset')
        button.grid(row=numButtons - 3, column=COL_START, columnspan=2)

        button = Tkinter.Button(
            self, width=bWidth_control, command=sys.exit)
        self.addButton(button, 'quit')
        button.grid(row=numButtons - 1, column=COL_START, columnspan=2)

        button = Tkinter.Button(
            self, width=bWidth_control, command=lambda: self.change_language())
        self.addButton(button, 'lang')
        button.grid(row=numButtons - 2, column=COL_START, columnspan=2)

        self.grid()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        self.parent.iconify()
        self.parent.update()
        self.parent.deiconify()

    def addButton(self, button, text):
        button["text"] = self._language_text.get(text, text)
        self.buttons.append((text, button))

    def change_language(self):
        if self._language_text == _LANGUAGE_DE:
            self._language_text = _LANGUAGE_EN
        else:
            self._language_text = _LANGUAGE_DE
        for text, button in self.buttons:
            button["text"] = self._language_text[text]

    def category_name(self, cfg):
        return os.path.basename(cfg)[:-8]

    def start_cfg(self, cfg):
        # remember which which cfg was launched
        self.category = self.category_name(cfg)
        if _DEBUG:
            print("starting", cfg)
        self.gametime = parseEndTime(cfg)
        self.ret = subprocess.call(
            [guisimPath, "-S", "-G", "-Q", "-c", cfg, '-l', 'log'], stderr=sys.stderr)
        if _DEBUG:
            print("ended", cfg)

        # compute score
        totalDistance = 0
        totalFuel = 0
        totalArrived = 0
        totalWaitingTime = 0
        complete = True
        for line in open(os.path.join(base, "%s.netstate.xml" % start.category)):
            m = re.search('<interval begin="0(.00)?" end="([^"]*)"', line)
            if m and float(m.group(2)) != start.gametime:
                print("error: incomplete output")
                complete = False
            m = re.search('sampledSeconds="([^"]*)".*speed="([^"]*)"', line)
            if m:
                totalDistance += float(m.group(1)) * float(m.group(2))
            m = re.search('fuel_abs="([^"]*)"', line)
            if m:
                totalFuel += float(m.group(1))
            m = re.search('arrived="([^"]*)"', line)
            if m:
                totalArrived += float(m.group(1))
            m = re.search('waitingTime="([^"]*)"', line)
            if m:
                totalWaitingTime += float(m.group(1))
        switch = []
        lastProg = {}
        for line in open(os.path.join(base, "%s.tlsstate.xml" % start.category)):
            m = re.search(
                'tlsstate time="(\d+(.\d+)?)" id="([^"]*)" programID="([^"]*)"', line)
            if m:
                tls = m.group(3)
                program = m.group(4)
                if tls not in lastProg or lastProg[tls] != program:
                    lastProg[tls] = program
                    switch += [m.group(3), m.group(1)]
        # doing nothing gives a waitingTime of 6033 for cross and 6700 for
        # square
        score = 10000 - totalWaitingTime
        lang = start._language_text
        if _DEBUG:
            print(switch, score, totalArrived, complete)
        if complete:
            ScoreDialog(self, switch, score, self.category, lang)
        # if ret != 0:
        # quit on error
        #    sys.exit(start.ret)


class ScoreDialog:

    def __init__(self, parent, switch, score, category, lang):
        self.root = Tkinter.Toplevel(parent)
        # self.root.transient(parent)
        self.name = None
        self.switch = switch
        self.score = score
        self.category = category
        self.root.title(lang["Highscore"])
        self.root.minsize(250, 50)
        haveHigh = False

        if not category in high:
            high[category] = _SCORES * [("", "", -1.)]
        idx = 0
        for n, g, p in high[category]:
            if not haveHigh and p < score:
                Tkinter.Label(
                    self.root, text=(str(idx + 1) + '. ')).grid(row=idx)
                self.name = Tkinter.Entry(self.root)
                self.name.grid(row=idx, sticky=Tkinter.W, column=1)
                self.scoreLabel = Tkinter.Label(self.root, text=str(score),
                                                bg="pale green").grid(row=idx, column=2)
                self.idx = idx
                haveHigh = True
                self.root.title(lang["Congratulations"])
                idx += 1
            if p == -1 or idx == _SCORES:
                break
            Tkinter.Label(self.root, text=(str(idx + 1) + '. ')).grid(row=idx)
            Tkinter.Label(self.root, text=n, padx=5).grid(
                row=idx, sticky=Tkinter.W, column=1)
            Tkinter.Label(self.root, text=str(p)).grid(row=idx, column=2)
            idx += 1
        if not haveHigh:
            if score != None:  # not called from the main menue
                Tkinter.Label(self.root, text=lang['your score'], padx=5,
                              bg="indian red").grid(row=idx, sticky=Tkinter.W, column=1)
                Tkinter.Label(self.root, text=str(score),
                              bg="indian red").grid(row=idx, column=2)
                idx += 1
        else:
            self.saveBut = Tkinter.Button(
                self.root, text="Save", command=self.save)
            self.saveBut.grid(row=idx, column=1)
        Tkinter.Button(self.root, text=lang["Continue"], command=self.quit).grid(
            row=idx, column=2)

        # add QR-code for LNDW
        Tkinter.Label(self.root, image=IMAGE.qrCode).grid(
            row=1, column=3, rowspan=22)

        self.root.grid()
        self.root.bind("<Return>", self.save)
        # self.root.wait_visibility()
        # self.root.grab_set()
        if self.name:
            self.name.focus_set()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        # self.root.iconify()
        # self.root.update()
        # self.root.deiconify()
        # self.root.mainloop()

    def save(self, event=None):
        if self.name:
            name = self.name.get()
            high[self.category].insert(
                self.idx, (name, self.switch, self.score))
            high[self.category].pop()
            self.saveBut.config(state=Tkinter.DISABLED)
            self.name.destroy()
            self.name = None
            Tkinter.Label(self.root, text=name, padx=5,
                          bg="pale green").grid(row=self.idx, sticky=Tkinter.W, column=1)
            try:
                f = open(_SCOREFILE, 'w')
                pickle.dump(high, f)
                f.close()
            except:
                pass
            try:
                conn = httplib.HTTPConnection(_SCORESERVER)
                conn.request("GET", _SCORESCRIPT + "category=%s&name=%s&instance=%s&points=%s" % (
                    self.category, name, "_".join(self.switch), self.score))
                if _DEBUG:
                    r1 = conn.getresponse()
                    print(r1.status, r1.reason, r1.read())
            except:
                pass
        else:
            self.quit()

    def quit(self, event=None):
        self.root.destroy()

stereoModes = (
    'ANAGLYPHIC', 'QUAD_BUFFER', 'VERTICAL_SPLIT', 'HORIZONTAL_SPLIT')
optParser = OptionParser()
optParser.add_option("-s", "--stereo", metavar="OSG_STEREO_MODE",
                     help="Defines the stereo mode to use for 3D output; unique prefix of %s" % (", ".join(stereoModes)))
options, args = optParser.parse_args()

base = os.path.dirname(sys.argv[0])
high = loadHighscore()


def findSumoBinary(guisimBinary):
    if os.name != "posix":
        guisimBinary += ".exe"
    if os.path.exists(os.path.join(base, guisimBinary)):
        guisimPath = os.path.join(base, guisimBinary)
    else:
        guisimPath = os.environ.get(
            "GUISIM_BINARY", os.path.join(base, '..', '..', 'bin', guisimBinary))
    if not os.path.exists(guisimPath):
        guisimPath = guisimBinary
    return guisimPath


guisimPath = findSumoBinary("sumo-gui")
haveOSG = "OSG" in subprocess.check_output(findSumoBinary("sumo"))

if options.stereo:
    for m in stereoModes:
        if m.lower().startswith(options.stereo.lower()):
            os.environ["OSG_STEREO_MODE"] = m
            os.environ["OSG_STEREO"] = "ON"
            break

lang = _LANGUAGE_EN
if "OSG_FILE_PATH" in os.environ:
    os.environ["OSG_FILE_PATH"] += os.pathsep + \
        os.path.join(os.environ.get("SUMO_HOME", ""), "data", "3D")
else:
    os.environ["OSG_FILE_PATH"] = os.path.join(
        os.environ.get("SUMO_HOME", ""), "data", "3D")

root = Tkinter.Tk()
IMAGE.dlrLogo = Tkinter.PhotoImage(file='dlr.gif')
IMAGE.sumoLogo = Tkinter.PhotoImage(file='logo.gif')
IMAGE.qrCode = Tkinter.PhotoImage(file='dlr_lndw_15_ts_4.gif')
start = StartDialog(root, lang)
root.mainloop()
