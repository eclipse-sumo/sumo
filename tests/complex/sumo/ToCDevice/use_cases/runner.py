#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    runner.py
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Leonhard Luecken
# @date    2009-03-26
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import optparse
import subprocess
import random
from optparse import OptionParser
# we need to import python modules from the $SUMO_HOME/tools directory
try:
    sys.path.append(os.path.join(os.path.dirname(
        __file__), '..', '..', '..', '..', "tools"))  # tutorial in tests
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
        os.path.dirname(__file__), "..", "..", "..")), "tools"))  # tutorial in docs
    from sumolib import checkBinary  # noqa
except ImportError:
    sys.exit(
        "please declare environment variable 'SUMO_HOME' as the root directory of your sumo installation (it should contain folders 'bin', 'tools' and 'docs')")

import traci

ToC_vehicles_identifier = "AVflow"

def doToC(vehSet,toRemove,timeUntilMRM,edgeID,distance):
    for vehID in vehSet:
        distToTOR = traci.vehicle.getDrivingDistance(vehID, edgeID, distance)
        print ("distToTOR = %s"%distToTOR)
        print ("timeToMRM = %s"%timeUntilMRM)
        if distToTOR < 0.:
            toRemove.add(vehID)
            if traci.vehicle.getTypeID(vehID).startswith("toc"):
                requestToC(vehID, timeUntilMRM)
                if options.verbose:
                    t = traci.simulation.getCurrentTime()/1000.
                    print("Requested ToC of %s at t=%s (until t=%s)"%(vehID, t,t + float(timeUntilMRM)))
                    printToCParams(vehID, True)
            elif traci.vehicle.getTypeID(vehID).startswith("auto"):
                traci.vehicle.setVehicleClass(vehID, "passenger")
    return toRemove
    
def run(timeUntilMRM,edgeID,distance):
    """execute the TraCI control loop"""
    AVsOnRoad = set(traci.vehicle.getIDList())
    onlyUpward = set()

    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep()
        # Keep book in a set AVsOnRoad
        AVsOnRoad = AVsOnRoad.difference([vehID for vehID in traci.simulation.getArrivedIDList() if vehID.startswith(ToC_vehicles_identifier)])
        AVsOnRoad.update([vehID for vehID in traci.simulation.getDepartedIDList() if vehID.startswith(ToC_vehicles_identifier)])

        # provide the ToCService at the traffic sign for informing the lane closure
        toRemove=set()
        toRemove = doToC(AVsOnRoad,toRemove,timeUntilMRM,edgeID,distance)
        AVsOnRoad = AVsOnRoad.difference(toRemove)
        onlyUpward.update(toRemove)
        
        print ("onlyUpward:%s" %onlyUpward)

        # provide ToCService to the upwardTransitions
        toRemove=set()
        toRemove = doToC(onlyUpward,toRemove,0.,edgeID,distance)
        onlyUpward.difference(toRemove)

def requestToC(vehID, timeUntilMRM):
    traci.vehicle.setParameter(vehID, "device.toc.requestToC", str(timeUntilMRM))

def printToCParams(vehID, only_dynamic = False):
    holder = traci.vehicle.getParameter(vehID, "device.toc.holder")
    manualType = traci.vehicle.getParameter(vehID, "device.toc.manualType")
    automatedType = traci.vehicle.getParameter(vehID, "device.toc.automatedType")
    responseTime = traci.vehicle.getParameter(vehID, "device.toc.responseTime")
    recoveryRate = traci.vehicle.getParameter(vehID, "device.toc.recoveryRate")
    initialAwareness = traci.vehicle.getParameter(vehID, "device.toc.initialAwareness")
    mrmDecel = traci.vehicle.getParameter(vehID, "device.toc.mrmDecel")
    currentAwareness = traci.vehicle.getParameter(vehID, "device.toc.currentAwareness")
    state = traci.vehicle.getParameter(vehID, "device.toc.state")
    speed = traci.vehicle.getSpeed(vehID)
    
    print("time step %s"%traci.simulation.getCurrentTime())
    print("ToC device infos for vehicle '%s'"%vehID)
    if not only_dynamic:
        print("Static parameters:")
        print("  holder = %s"%holder)
        print("  manualType = %s"%manualType)
        print("  automatedType = %s"%automatedType)
        print("  responseTime = %s"%responseTime)
        print("  recoveryRate = %s"%recoveryRate)
        print("  initialAwareness = %s"%initialAwareness)
        print("  mrmDecel = %s"%mrmDecel)
        print("Dynamic parameters:")
    print("  currentAwareness = %s"%currentAwareness)
    print("  currentSpeed = %s"%speed)
    print("  state = %s"%state)
    
def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("-c", "--code", dest="code", help="scenario code")
    optParser.add_option("-n", "--net-file", dest="netfile", help="network file")
    optParser.add_option("-r", "--route-file", dest="routefile", help="route file")
    optParser.add_option("-a", "--add-file", dest="addfile", help="additional file")
    optParser.add_option("-o", "--output-file", dest="outputfile", help="fcd output filename")
    optParser.add_option("-g", "--view-file", dest="viewfile", help="sumo-gui view setting file")
    optParser.add_option("-t", "--time-MRM", dest="timeUntilMRM", default= 8, help="time until MRM")
    optParser.add_option("--nogui", action="store_false",
                         default=True, help="run the commandline version of sumo")
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose", 
                         default=False, help="tell me what you are doing")
    options, args = optParser.parse_args()
    return options


# this is the main entry point of this script
if __name__ == "__main__":
    options = get_options()
    timeUntilMRM = float(options.timeUntilMRM)
    code = options.code
    edgeID = None
    distance = None
    if options.verbose:
        print ("time to MRM: %s" %(timeUntilMRM))
    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run
    if options.nogui:
        sumoBinary = checkBinary('sumo')
    else:
        sumoBinary = checkBinary('sumo-gui')

    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs
    traci.start([sumoBinary, "-n", options.netfile, "-r", options.routefile, "-a", options.addfile, "--ignore-route-errors", "--step-method.ballistic", "--fcd-output", options.outputfile, "--gui-settings-file", options.viewfile, "--no-step-log", "true"])
     
    if code=="UC1_1":
        edgeID = "approach2"
        distance = 245.
    elif code == "UC5_1":
        edgeID = "e0"
        distance = 3000.
    
    if edgeID and distance:
        run(timeUntilMRM, edgeID, distance)
    
    traci.close()
    sys.stdout.flush()


