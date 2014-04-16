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

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, subprocess, sys, re, pickle, httplib, glob, Tkinter
from xml.dom import pulldom

_SCOREFILE = "scores.pkl"
_SCORESERVER = "sumo-sim.org"
_SCORESCRIPT = "/scores.php?game=TLS&"
_DEBUG = False
_SCORES= 30

def loadHighscore():
    try:
        conn = httplib.HTTPConnection(_SCORESERVER)
        conn.request("GET", _SCORESCRIPT + "top=" + str(_SCORES))
        response = conn.getresponse()
        if response.status == httplib.OK:
            scores = {}
            for line in response.read().splitlines():
                category, values = line.split()
                scores[category] = _SCORES*[("", "", -1.)]
                for idx, item in enumerate(values.split(':')):
                    name, game, points = item.split(',')
                    scores[category][idx] = (name, game, int(float(points)))
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
    
class StartDialog:
    def __init__(self):
        # variables for changing language
        self._language = "english"
        self._language_text = {'english':['Interactive Traffic Light','Simple Junction','Four Junctions','Highway Scenario','Highscore','Reset Highscore','deutsch','Quit'],'deutsch':['Interaktives Ampelspiel','Einfache Kreuzung','Vier Kreuzungen','Autobahn Auffahrt','Highscore','Highscore zurück setzen','english','Beenden']}  
        self.buttons= []
        self.textIDs= []
        # misc variables
        self.name = ''
        # setup gui
        self.root = Tkinter.Tk()
        self.root.title(self._language_text[self._language][0])
        self.root.minsize(250, 50)

        # we use a grid layout with 4 columns
        COL_DLRLOGO, COL_START, COL_HIGH, COL_SUMOLOGO = range(4)
        # there is one column for every config, +2 more columns for control buttons
        configs = glob.glob(os.path.join(base, "*.sumocfg"))
        numButtons = len(configs) + 3
        # button dimensions
        bWidth_start = 15
        bWidth_high = 7
        bWidth_control = 26   

        self.gametime = 0
        self.ret = 0
        # some pretty images
        dlrLogo = Tkinter.PhotoImage(file='dlr.gif') 
        sumoLogo = Tkinter.PhotoImage(file='logo.gif')
        Tkinter.Label(self.root, image=dlrLogo).grid(row=0, rowspan=numButtons, column=COL_DLRLOGO)
        Tkinter.Label(self.root, image=sumoLogo).grid(row=0, rowspan=numButtons, column=COL_SUMOLOGO)

        # 2 button for each config (start, highscore)
        for row, cfg in enumerate(configs):
            text = category = os.path.basename(cfg)[:-8]
            if text == "cross":
                text = self._language_text[self._language][1]
                self.textIDs.append(1)
            elif text == "square":
                text = self._language_text[self._language][2]
                self.textIDs.append(2)
            elif text == "kuehne":
                text = "Prof. Kühne" 
            elif text == "ramp":
                text = self._language_text[self._language][3] 
                self.textIDs.append(3)
            # lambda must make a copy of cfg argument
            button=Tkinter.Button(self.root, text=text, width=bWidth_start, 
                    command=lambda cfg=cfg:self.start_cfg(cfg))
            self.buttons.append(button)
            button.grid(row=row, column=COL_START)
            
            button=Tkinter.Button(self.root, text=self._language_text[self._language][4], width=bWidth_high,
                    command=lambda cfg=cfg:ScoreDialog([], None, self.category_name(cfg)))#.grid(row=row, column=COL_HIGH)
            button.grid(row=row, column=COL_HIGH)
            self.buttons.append(button)
            self.textIDs.append(4)

        # control buttons
        button=Tkinter.Button(self.root, text=self._language_text[self._language][5], width=bWidth_control,
                       command=high.clear)
        self.buttons.append(button)
        button.grid(row=numButtons - 3, column=COL_START, columnspan=2)
        self.textIDs.append(5)
        
        button=Tkinter.Button(self.root, text=self._language_text[self._language][7], width=bWidth_control,
                       command=sys.exit)
        self.buttons.append(button)
        button.grid(row=numButtons - 1, column=COL_START, columnspan = 2)
        self.textIDs.append(7)
        
        button=Tkinter.Button(self.root, text=self._language_text[self._language][6], width=bWidth_control, command=lambda cfg=cfg:self.change_language())
        self.buttons.append(button)
        button.grid(row=numButtons - 2, column=COL_START, columnspan=2) 
        self.textIDs.append(6)
        
        self.root.grid()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        self.root.iconify()
        self.root.update()
        self.root.deiconify()
        self.root.mainloop()      
        
    def change_language(self):      
        if self._language== 'deutsch':
            self._language= 'english'
        else:
            self._language= 'deutsch'
        i = 0
        for button in self.buttons:
            button["text"]= self._language_text[self._language][self.textIDs[i]]  
            i+=1
            
    
    def category_name(self, cfg):
        return os.path.basename(cfg)[:-8]

    def start_cfg(self, cfg):
        self.root.destroy()
        if _DEBUG:
            print "starting", cfg
        self.gametime = parseEndTime(cfg)
        self.ret = subprocess.call([guisimPath, "-S", "-G", "-Q", "-c", cfg])
        self.category = self.category_name(cfg) # remember which which cfg was launched



class ScoreDialog:
    def __init__(self, game, points, category):
        self.root = Tkinter.Tk()
        self.name = None
        self.game = game
        self.points = points
        self.category = category
        haveHigh = False
        self.root.title("Highscore")
        self.root.minsize(250, 50)

        if not category in high:
            high[category] = _SCORES*[("", "", -1.)]
        idx = 0
        for n, g, p in high[category]:
            if not haveHigh and p < points:
                Tkinter.Label(self.root, text=(str(idx + 1) + '. ')).grid(row=idx)
                self.name = Tkinter.Entry(self.root)
                self.name.grid(row=idx, sticky=Tkinter.W, column=1)
                self.scoreLabel = Tkinter.Label(self.root, text=str(points),
                                                bg="pale green").grid(row=idx, column=2)
                self.idx = idx
                haveHigh = True
                self.root.title("Congratulations!")
                idx += 1
            if p == -1 or idx == _SCORES:
                break
            Tkinter.Label(self.root, text=(str(idx + 1) + '. ')).grid(row=idx)
            Tkinter.Label(self.root, text=n, padx=5).grid(row=idx, sticky=Tkinter.W, column=1)
            Tkinter.Label(self.root, text=str(p)).grid(row=idx, column=2)
            idx += 1
        if not haveHigh:
            if points != None: # not called from the main menue
                Tkinter.Label(self.root, text='your score', padx=5,
                              bg="indian red").grid(row=idx, sticky=Tkinter.W, column=1)
                Tkinter.Label(self.root, text=str(points),
                              bg="indian red").grid(row=idx, column=2)
                idx += 1
        else:
            self.saveBut = Tkinter.Button(self.root, text="Save", command=self.save)
            self.saveBut.grid(row=idx, column=1)
        Tkinter.Button(self.root, text="Continue", command=self.quit).grid(row=idx, column=2)
        self.root.grid()
        self.root.bind("<Return>", self.save)
        # self.root.wait_visibility() 
        # self.root.grab_set()
        if self.name:
            self.name.focus_set()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        self.root.iconify()
        self.root.update()
        self.root.deiconify()
        self.root.mainloop()

    def save(self, event=None):
        if self.name:
            name = self.name.get()
            high[self.category].insert(self.idx, (name, self.game, self.points))
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
                    self.category, name, "_".join(self.game), self.points))
                if _DEBUG:
                    r1 = conn.getresponse()
                    print r1.status, r1.reason, r1.read()
            except:
                pass
        else:
            self.quit()


    def quit(self, event=None):
        self.root.destroy()


base = os.path.dirname(sys.argv[0])
high = loadHighscore()
guisimBinary = "sumo-gui"
if os.name != "posix":
    guisimBinary += ".exe"
if os.path.exists(os.path.join(base, guisimBinary)):
    guisimPath = os.path.join(base, guisimBinary)
else:
    guisimPath = os.environ.get("GUISIM_BINARY", os.path.join(base, '..', '..', 'bin', guisimBinary))
if not os.path.exists(guisimPath):
    guisimPath = guisimBinary
  
    
while True:
    start = StartDialog()
    totalDistance = 0
    totalFuel = 0
    totalArrived = 0
    complete = True
    for line in open(os.path.join(base, "netstate.xml")):
        m = re.search('<interval begin="0(.00)?" end="([^"]*)"', line)
        if m and float(m.group(2)) != start.gametime:
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
    switch = []
    lastProg = {}
    for line in open(os.path.join(base, "tlsstate.xml")):
        m = re.search('tlsstate time="(\d+(.\d+)?)" id="([^"]*)" programID="([^"]*)"', line)
        if m:
            tls = m.group(3)
            program = m.group(4)
            if tls not in lastProg or lastProg[tls] != program:
                lastProg[tls] = program
                switch += [m.group(3), m.group(1)]
    score = totalArrived
    if _DEBUG:
        print switch, score, totalArrived, complete
    if complete:
        ScoreDialog(switch, score, start.category)
    if start.ret != 0:
        sys.exit(start.ret)
