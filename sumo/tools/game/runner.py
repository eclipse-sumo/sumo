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
                    scores[category][idx] = item.split(',')
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
    f = open(_SCOREFILE, 'w')
    pickle.dump(high, f)
    f.close()
    conn = httplib.HTTPConnection(_SCORESERVER)
    conn.request("GET", _SCORESCRIPT + "category=%s&name=%s&instance=%s&points=%s" % (category, name, "_".join(game), points))
    if _DEBUG:
        r1 = conn.getresponse()
        print r1.status, r1.reason, r1.read()

class StartDialog:
    def __init__(self):
        self.root = Tkinter.Tk()
        self.root.title("Traffic Light Game")
        for cfg in glob.glob("*.sumo.cfg"):
            category = cfg[:-9]
            if not category in high:
                high[category] = 10*[("", "", -1.)]
            Tkinter.Button(self.root, text=category, command=lambda:self.ok(cfg)).pack()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        self.root.iconify()
        self.root.update()
        self.root.deiconify()
        self.root.mainloop()

    def ok(self, cfg):
        self.root.destroy()
        self.ret = subprocess.call([guisimBinary, "-G", "-Q", "-c", cfg])
        self.category = cfg[:-9]

class ScoreDialog:
    def __init__(self, game, points, category):
        self.root = Tkinter.Tk()
        self.name = None
        self.game = game
        self.points = points
        self.category = category
        haveHigh = False
        self.root.title("High score")

        idx = 0
        for n, g, p in high[category]:
            if not haveHigh and (p == -1 or p < points):
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


high = loadHighscore()
guisimBinary = "guisim.exe"
if os.name == "posix":
    guisimBinary="sumo-guisim"
guisimBinary = os.environ.get("GUISIM_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', 'bin', guisimBinary))
start = StartDialog()

totalWait = 0
for line in open("netstate.xml"):
    m = re.search('waitingTime="(\d+.\d+)"', line)
    if m:
        totalWait += float(m.group(1))
switch = []
for line in open("tlsstate.xml"):
    m = re.search('tlsstate time="(\d+)"', line)
    if m:
        switch += [m.group(1)]
print switch, totalWait
ScoreDialog(switch, totalWait, start.category)
sys.exit(start.ret)
