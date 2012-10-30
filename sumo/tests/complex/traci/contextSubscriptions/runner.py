#!/usr/bin/env python

import os, subprocess, sys, time, math

sumoHome = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import traci

if sys.argv[1]=="sumo":
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = ""
else:
    sumoBinary = os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-S -Q"
PORT = 8813
DELTA_T = 1000


def runSingle(traciEndTime, viewRange):
    seen1 = 0
    seen2 = 0
    step = 0
    sumoProcess = subprocess.Popen("%s -c sumo.sumocfg %s" % (sumoBinary, addOption), shell=True, stdout=sys.stdout)
    traci.init(PORT)
    subscribed = False
    while not step>traciEndTime:
        responses = traci.simulationStep(DELTA_T)
        near1 = set()
        if "ego" in traci.vehicle.getContextSubscriptionResults():
            for v in traci.vehicle.getContextSubscriptionResults()["ego"]:
                near1.add(v)
        vehs = traci.vehicle.getIDList()
        pos = {}
        for v in vehs:
            pos[v] = traci.vehicle.getPosition(v)
        egoPos = pos["ego"]
        near2 = set()
        for v in pos:
            dx = egoPos[0] - pos[v][0]
            dy = egoPos[1] - pos[v][1]
            if math.sqrt(dx*dx + dy*dy)<viewRange:
                near2.add(v)
        
        if not subscribed:
            traci.vehicle.subscribeContext("ego", traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange, [traci.constants.VAR_POSITION] )
            subscribed = True
        else:
            seen1 += len(near1)
            seen2 += len(near2)
            for v in near1:
                if v not in near2:
                    print "timestep %s: %s is missing in subscribed vehicles" % (step, v)
            for v in near2:
                if v not in near1:
                    print "timestep %s: %s is missing in surrounding vehicles" % (step, v)


        step += 1
    print "Print ended at step %s" % (traci.simulation.getCurrentTime() / DELTA_T)
    traci.close()
    sys.stdout.flush()
    print "seen %s vehicles via suscription, %s in surrounding" % (seen1, seen2)
    
print "=========== long route ==========="
fdo = open("input_routes.rou.xml", "w")
print >> fdo, '<routes>"'
print >> fdo, '   <route id="1" edges="2fi 2si 1o 1fi 1si 3o 3fi 3si 4o 4fi 4si 1o 1fi 1si 3o 3fi 3si 4o 4fi 4si 2o 2fi 2si 1o 1fi 1si 3o 3fi 3si 4o 4fi 4si 1o 1fi 1si 3o 3fi 3si 4o 4fi 4si 2o"/>'
print >> fdo, '   <vehicle id="ego" route="1" depart="0"/>'
print >> fdo, '   <route id="2" edges="3fi 3si 4o 4fi 4si 3o 3fi"/>'
print >> fdo, '   <flow id="flow" route="2" begin="0" end="3030" period="10"/>'
print >> fdo, '</routes>'
fdo.close()
print "----------- SUMO end time is not given -----------"
sys.stdout.flush()
runSingle(1000, float(sys.argv[2]))

