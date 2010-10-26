#!/usr/bin/env python

import os,subprocess,sys,time
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "lib"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "traci"))
import traciControl, testUtil


PORT = 8813
DELTA_T = 1000

netconvertBinary = testUtil.checkBinary('netconvert')
sumoBinary = testUtil.checkBinary('sumo')


def runSingle(addOption):
    step = 0
    timeline = []
    sumoProcess = subprocess.Popen("%s -c %s %s" % (sumoBinary, "sumo.sumo.cfg", addOption), shell=True, stdout=sys.stdout)
    traciControl.initTraCI(PORT)
    while not step>10000:
        try:
            traciControl.cmdSimulationStep2(DELTA_T)
            vehs = traciControl.cmdGetVehicleVariable_idList()
            timeline.append({})
            for v in vehs:
                timeline[-1][v] = traciControl.cmdGetVehicleVariable_speed(v)
            step += 1
        except traciControl.FatalTraCIError:
            print "Closed by SUMO"
            break
    traciControl.cmdClose()
    sys.stdout.flush()
    return timeline

def evalTimeline(timeline):
    ct = 0
    for t in timeline:
        if ct>1000:
            for v in t:
                s = t[v]
                if s<8.9 or s>9.1:
                    print " Mismatching velocity of vehicle %s at time %s (%s)" % (v, ct, s)
        ct = ct + 1

print ">>> Building the network (with internal)"
sys.stdout.flush()
retcode = subprocess.call([netconvertBinary, "-c", "netconvert.netc.cfg"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print ">>> Checking Simulation (network: internal, simulation: internal)"
evalTimeline(runSingle(""))
time.sleep(1)
print ">>> Checking Simulation (network: internal, simulation: no internal)"
evalTimeline(runSingle("--no-internal-links"))
time.sleep(1)

print ""
print ">>> Building the network (without internal)"
sys.stdout.flush()
retcode = subprocess.call([netconvertBinary, "-c", "netconvert.netc.cfg", "--no-internal-links"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print ">>> Checking Simulation (network: no internal, simulation: internal)"
evalTimeline(runSingle(""))
time.sleep(1)
print ">>> Checking Simulation (network: no internal, simulation: no internal)"
evalTimeline(runSingle("--no-internal-links"))



