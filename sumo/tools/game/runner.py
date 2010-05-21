#!/usr/bin/env python
"""
@file    runner.py
@author  Michael.Behrisch@dlr.de
@date    2010-01-30
@version $Id$

This script runs the gaming GUI for the LNdW traffic light game.
It checks for possible scenarios in the current working directory
and lets the user start them as a game. Furthermore it
saves highscores to local disc and to the central highscore server.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""
import os, subprocess, sys, re, pickle, httplib, glob, Tkinter

_SCOREFILE = "scores.pkl"
_SCORESERVER = "sumo.sourceforge.net"
_SCORESCRIPT = "/scores.php?game=TLS&"
_DEBUG = True

def loadHighscore():
    try:
        conn = httplib.HTTPConnection(_SCORESERVER)
        conn.request("GET", _SCORESCRIPT + "top=10")
        response = conn.getresponse()
        if response.status == httplib.OK:
            scores = {}
            for line in response.read().splitlines():
                category, values = line.split()
                scores[category] = 10*[("", "", -1.)]
                for idx, item in enumerate(values.split(':')):
                    name, game, points = item.split(',')
                    scores[category][idx] = (name, game, float(points))
            return scores
    except:
        pass

    try:
        return pickle.load(open(_SCOREFILE))
    except:
        pass
    return {}

def save(idx, category, name, game, points):
    high[category].insert(idx, (name, game, points))
    high[category].pop()
    try:
        f = open(_SCOREFILE, 'w')
        pickle.dump(high, f)
        f.close()
    except:
        pass
    try:
        conn = httplib.HTTPConnection(_SCORESERVER)
        conn.request("GET", _SCORESCRIPT + "category=%s&name=%s&instance=%s&points=%s" % (category, name, "_".join(game), points))
        if _DEBUG:
            r1 = conn.getresponse()
            print r1.status, r1.reason, r1.read()
    except:
        pass

class StartDialog:
    def __init__(self):
        self.root = Tkinter.Tk()
        self.root.title("Traffic Light Game")
        for cfg in glob.glob(os.path.join(base, "*.sumo.cfg")):
            category = os.path.basename(cfg)[:-9]
            Tkinter.Button(self.root, text=category, command=lambda cfg=cfg:self.ok(cfg)).pack()
        Tkinter.Button(self.root, text="Clear Highscore", command=high.clear).pack()
        Tkinter.Button(self.root, text="Quit", command=sys.exit).pack()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        self.root.iconify()
        self.root.update()
        self.root.deiconify()
        self.root.mainloop()

    def ok(self, cfg):
        self.root.destroy()
        print "starting", cfg
        self.ret = subprocess.call([guisimPath, "-G", "-Q", "-c", cfg])
        self.category = os.path.basename(cfg)[:-9]

class ScoreDialog:
    def __init__(self, game, points, category):
        self.root = Tkinter.Tk()
        self.name = None
        self.game = game
        self.points = points
        self.category = category
        haveHigh = False
        self.root.title("High score")

        if not category in high:
            high[category] = 10*[("", "", -1.)]
        idx = 0
        for n, g, p in high[category]:
            if not haveHigh and p < points:
                self.name = Tkinter.Entry(self.root)
                self.name.grid(row=idx, sticky=Tkinter.W)
                self.idx = idx
                p = points
                haveHigh = True
                self.root.title("Congratulations")
            else:
                if p == -1:
                    break
                Tkinter.Label(self.root, text=n).grid(row=idx, sticky=Tkinter.W)
            Tkinter.Label(self.root, text=str(p)).grid(row=idx, column=1)
            idx += 1
        Tkinter.Button(self.root, text="OK", command=self.ok).grid(row=idx)
        self.root.grid()
        self.root.bind("<Return>", self.ok)
        self.root.grab_set()
        if self.name:
            self.name.focus_set()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        self.root.iconify()
        self.root.update()
        self.root.deiconify()
        self.root.mainloop()

    def ok(self, event=None):
        if self.name:
            save(self.idx, self.category, self.name.get(), self.game, self.points)
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
        if m and float(m.group(2)) != 180:
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
    score = 25000 - 100000 * totalFuel / totalDistance
    if _DEBUG:
        print switch, score, totalArrived
    if complete:
        ScoreDialog(switch, score, start.category)
    if start.ret != 0:
        sys.exit(start.ret)
