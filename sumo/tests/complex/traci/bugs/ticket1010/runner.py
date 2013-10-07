#!/usr/bin/env python

import os, subprocess, sys, time, math

sumoHome = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import traci

if sys.argv[1]=="sumo":
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = ""
else:
    sumoBinary = os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-S -Q"
PORT = 8813

def run():
    """execute the TraCI control loop"""
    traci.init(PORT)
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0 and step < 100:
        traci.simulationStep()
        step += 1
        if step == 4:
            traci.trafficlights.setProgram("center", "0")
    traci.close()
    sys.stdout.flush()

sumoProcess = subprocess.Popen([sumoBinary,
    "-n", "input_net.net.xml", 
    "-r", "input_routes.rou.xml",
    "-a", "input_additional.add.xml",
    "--no-step-log", 
    "--verbose", 
    "--remote-port", str(PORT)], 
    stdout=sys.stdout, stderr=sys.stderr)
run()
sumoProcess.wait()

