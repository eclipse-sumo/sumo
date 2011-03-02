#!/usr/bin/env python

import os, subprocess, sys, time, shutil

sumoHome = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools", "traci"))
import traciControl

if sys.argv[1]=="sumo":
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = ""
    secondConfig = "sumo.sumo.cfg"
else:
    sumoBinary = os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-Q"
    secondConfig = "sumo_log.sumo.cfg"
PORT = 8813
DELTA_T = 1000

sumoProcess = subprocess.Popen("%s -c sumo.sumo.cfg %s" % (sumoBinary, addOption), shell=True, stdout=sys.stdout, stderr=sys.stderr)
traciControl.initTraCI(PORT)
sumoProcess = subprocess.Popen("%s -c %s %s" % (sumoBinary, secondConfig, addOption), shell=True, stdout=sys.stdout, stderr=sys.stderr)
time.sleep(10)
step = 0
while not step>100:
    traciControl.cmdSimulationStep2(DELTA_T)
    vehs = traciControl.cmdGetVehicleVariable_idList()
    if vehs.index("horiz")<0 or len(vehs)>1:
        print "Something is false"
    step += 1
traciControl.cmdClose()
sys.stdout.flush()
if os.path.exists("lastrun.stderr"):
    f = open("lastrun.stderr")
    shutil.copyfileobj(f, sys.stderr)
    f.close()
