#!/usr/bin/env python

import os, subprocess, sys, time, shutil

sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import traci

if sys.argv[1]=="sumo":
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = ""
    secondConfig = "sumo.sumocfg"
else:
    sumoBinary = os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-Q"
    secondConfig = "sumo_log.sumocfg"
PORT = 8813

subprocess.Popen("%s -c sumo.sumocfg %s" % (sumoBinary, addOption), shell=True, stdout=sys.stdout, stderr=sys.stderr)
traci.init(PORT)
subprocess.Popen("%s -c %s %s" % (sumoBinary, secondConfig, addOption), shell=True, stdout=sys.stdout, stderr=sys.stderr)
time.sleep(10)
step = 0
while not step>100:
    traci.simulationStep(step)
    vehs = traci.vehicle.getIDList()
    if vehs.index("horiz")<0 or len(vehs)>1:
        print "Something is false"
    step += 1
traci.close()
sys.stdout.flush()
if os.path.exists("lastrun.stderr"):
    f = open("lastrun.stderr")
    shutil.copyfileobj(f, sys.stderr)
    f.close()
