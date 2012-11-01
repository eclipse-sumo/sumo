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


def runSingle(traciEndTime, viewRange, module, objID):
    seen1 = 0
    seen2 = 0
    step = 0
    sumoProcess = subprocess.Popen("%s -c sumo.sumocfg %s" % (sumoBinary, addOption), shell=True, stdout=sys.stdout)
#    time.sleep(20)
    traci.init(PORT)
    traci.poi.add("poi", 400, 500, (1,0,0,0))
    traci.polygon.add("poly", ((400, 400), (450, 400), (450, 400)), (1,0,0,0))
    subscribed = False
    while not step>traciEndTime:
        responses = traci.simulationStep(DELTA_T)
        near1 = set()
        if objID in module.getContextSubscriptionResults():
            for v in module.getContextSubscriptionResults()[objID]:
                near1.add(v)
        vehs = traci.vehicle.getIDList()
        pos = {}
        for v in vehs:
            pos[v] = traci.vehicle.getPosition(v)
        shape = None
        egoPos = None
        if hasattr(module, "getPosition"):
            egoPos = module.getPosition(objID)
        elif hasattr(module, "getShape"):
            shape = module.getShape(objID)
        near2 = set()
        for v in pos:
            if egoPos:
                dx = egoPos[0] - pos[v][0]
                dy = egoPos[1] - pos[v][1]
                if math.sqrt(dx*dx + dy*dy) < viewRange:
                    near2.add(v)
            if shape:
                for x, y in shape:
                    dx = x - pos[v][0]
                    dy = y - pos[v][1]
                    if math.sqrt(dx*dx + dy*dy) < viewRange:
                        near2.add(v)
        
        if not subscribed:
            module.subscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange, [traci.constants.VAR_POSITION] )
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

sys.stdout.flush()
if sys.argv[3] == "vehicle":
    runSingle(1000, float(sys.argv[2]), traci.vehicle, "ego")
elif sys.argv[3] == "edge":
    runSingle(1000, float(sys.argv[2]), traci.edge, "1fi")
elif sys.argv[3] == "lane":
    runSingle(1000, float(sys.argv[2]), traci.lane, "2si_0")
elif sys.argv[3] == "junction":
    runSingle(1000, float(sys.argv[2]), traci.junction, "0")
elif sys.argv[3] == "poi":
    runSingle(1000, float(sys.argv[2]), traci.poi, "poi")
elif sys.argv[3] == "polygon":
    runSingle(1000, float(sys.argv[2]), traci.polygon, "poly")
