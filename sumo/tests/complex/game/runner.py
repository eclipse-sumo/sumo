#!/usr/bin/env python
import os, subprocess, sys, re, pickle

_HIGHSCOREFILE = "scores.pkl"

def loadHighscore():
    try:
        return pickle.load(open(_HIGHSCOREFILE))
    except:
        pass
    return 10*[("", "", -1.)]

def save():
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

        idx = 0
        for n, g, p in high:
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
            high.insert(self.idx, (self.name.get(), self.game, self.points))
            high.pop()
            save()
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
