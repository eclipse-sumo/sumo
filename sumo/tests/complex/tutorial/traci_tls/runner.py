#!/usr/bin/env python
"""
@file    runner.py
@author  Lena Kalleske
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2009-03-26
@version $Id$

Tutorial for traffic light control via the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2009-2012 DLR/TS, Germany
All rights reserved
"""

import os, sys
import optparse
import subprocess
import random

# we need to import python modules from the $SUMO_HOME/tools directory
try:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools")) # tutorial in tests
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), "..", "..", "..")), "tools")) # tutorial in docs
    from sumolib import checkBinary
except ImportError:
    sys.exit("please declare environment variable 'SUMO_HOME' as the root directory of your sumo installation (it should contain folders 'bin', 'tools' and 'docs')")

import traci
# the port used for communicating with your sumo instance
PORT = 8813

NSGREEN = "GrGr" 
NSYELLOW = "yryr"
WEGREEN = "rGrG" 
WEYELLOW = "ryry"

PROGRAM = [WEYELLOW,WEYELLOW,WEYELLOW,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSYELLOW,NSYELLOW,WEGREEN]

def generate_routefile():
    random.seed(42) # make tests reproducible
    N = 3600 # number of time steps
    # demand per second from different directions
    pWE = 1./10 
    pEW = 1./11
    pNS = 1./30
    with open("data/cross.rou.xml", "w") as routes:
        print >> routes, """<routes>
        <vType id="typeWE" accel="0.8" decel="4.5" sigma="0.5" length="5" minGap="2.5" maxSpeed="16.67" guiShape="passenger"/>
        <vType id="typeNS" accel="0.8" decel="4.5" sigma="0.5" length="17" minGap="3" maxSpeed="25" guiShape="bus"/>

        <route id="right" edges="51o 1i 2o 52i" />
        <route id="left" edges="52o 2i 1o 51i" />
        <route id="down" edges="54o 4i 3o 53i" />"""
        lastVeh = 0
        vehNr = 0
        for i in range(N):
            if random.uniform(0,1) < pWE:
                print >> routes, '    <vehicle id="%i" type="typeWE" route="right" depart="%i" />' % (vehNr, i)
                vehNr += 1
                lastVeh = i
            if random.uniform(0,1) < pEW:
                print >> routes, '    <vehicle id="%i" type="typeWE" route="left" depart="%i" />' % (vehNr, i)
                vehNr += 1
                lastVeh = i
            if random.uniform(0,1) < pNS:
                print >> routes, '    <vehicle id="%i" type="typeNS" route="down" depart="%i" color="1,0,0"/>' % (vehNr, i)
                vehNr += 1
                lastVeh = i
        print >> routes, "</routes>"

def run():
    """execute the TraCI control loop"""
    traci.init(PORT)
    programPointer = len(PROGRAM)-1
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep()
        programPointer = min(programPointer+1, len(PROGRAM)-1)
        no = traci.inductionloop.getLastStepVehicleNumber("0")
        if no > 0:
            programPointer = (0 if programPointer == len(PROGRAM)-1 else 3)
        traci.trafficlights.setRedYellowGreenState("0", PROGRAM[programPointer])
        step += 1
    traci.close()
    sys.stdout.flush()

def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("--nogui", action="store_true", default=False, help="run the commandline version of sumo")
    options, args = optParser.parse_args()
    return options


# this is the main entry point of this script
if __name__ == "__main__":
    options = get_options()

    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run
    if options.nogui:
        sumoBinary = checkBinary('sumo')
    else:
        sumoBinary = checkBinary('sumo-gui')

    # first, generate the route file for this simulation
    generate_routefile()

    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs
    sumoProcess = subprocess.Popen([sumoBinary, "-c", "data/cross.sumocfg", "--tripinfo-output", "tripinfo.xml"], stdout=sys.stdout, stderr=sys.stderr)
    run()
    sumoProcess.wait()
