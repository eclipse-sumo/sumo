#!/usr/bin/env python
import os, subprocess, sys, re

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
sys.exit(ret)
