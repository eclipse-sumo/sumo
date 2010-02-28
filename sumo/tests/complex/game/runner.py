#!/usr/bin/env python
import os, subprocess, sys, re, pickle

_HIGHSCOREFILE = "scores.pkl"

def loadHighscore():
    try:
        return pickle.load(open(_HIGHSCOREFILE))
    except:
        pass
    return 10*[("", "", -1.)]

def checkHigh(points):
    idx = 0
    while idx < len(high) and high[idx][2] < points and high[idx][2] != -1:
        idx += 1
    return idx

def save(name, game, points):
    idx = checkHigh(points)
    high.insert(idx, (name, game, points))
    high.pop()
    f = open(_HIGHSCOREFILE, 'w')
    pickle.dump(high, f)
    f.close()

import Tkinter
class TKDialog:
    def __init__(self, game, points):
        self.root = Tkinter.Tk()
        self.name = None
        self.game = game
        self.points = points
        haveHigh = False
        self.root.title("High score")

        idx = 1
        for n, g, p in high:
            if not haveHigh and (p == -1 or p < points):
                self.name = Tkinter.Entry(self.root)
                self.name.grid(row=idx, sticky=Tkinter.W)
                p = points
                haveHigh = True
                self.root.title("Congratulations")
            else:
                Tkinter.Label(self.root, text=n).grid(row=idx, sticky=Tkinter.W)
            Tkinter.Label(self.root, text=str(p)).grid(row=idx, column=1)
            idx += 1
        Tkinter.Button(self.root, text="OK", command=self.ok).grid(row=idx)
        self.root.grid()
#        self.root.bind("&lt;Return>", self.ok)
#        self.root.grab_set()
        # The following three commands are needed so the window pops
        # up on top on Windows...
        self.root.iconify()
        self.root.update()
        self.root.deiconify()
        self.root.mainloop()

    def ok(self):
        if self.name != None:
            save(self.name.get(), self.game, self.points)
        self.root.destroy()


high = loadHighscore()
guisimBinary = "guisim.exe"
if os.name == "posix":
    guisimBinary="sumo-guisim"
guisimBinary = os.environ.get("GUISIM_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', 'bin', guisimBinary))
ret = subprocess.call([guisimBinary, "-G", "-Q", "-c", "sumo.sumo.cfg"])
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
#TKDialog(switch, totalWait)
sys.exit(ret)
