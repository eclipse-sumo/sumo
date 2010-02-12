#!/usr/bin/env python
import os, subprocess, sys, re

guisimBinary = "guisim.exe"
if os.name == "posix":
    guisimBinary="sumo-guisim"
ret = subprocess.call([guisimBinary, "-Q", "-c", "sumo.sumo.cfg"])
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
sys.exit(ret)