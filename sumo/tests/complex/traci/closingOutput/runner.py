#!/usr/bin/env python

import os, subprocess, sys, time, shutil

sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import traci

if sys.argv[1]=="sumo":
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = ""
else:
    sumoBinary = os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-S -Q"
PORT = 8813

sumoProcess = subprocess.Popen("%s -c sumo.sumocfg %s" % (sumoBinary, addOption), shell=True, stdout=sys.stdout, stderr=sys.stderr)
traci.init(PORT)
time.sleep(10)
step = 0
while not step>100:
    traci.simulationStep()
    vehs = traci.vehicle.getIDList()
    if vehs.index("horiz")<0 or len(vehs)>1:
        print "Something is false"
    step += 1
traci.close()
sumoProcess.wait()
sys.stdout.flush()
