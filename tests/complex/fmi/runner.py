#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Matthias Schwamborn
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import shutil
import sys

from fmpy import read_model_description, extract
from fmpy.fmi2 import FMU2Slave
from fmpy.validation import validate_fmu

VERBOSE = True

sumoHome = os.path.abspath(os.environ['SUMO_HOME'])
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa

egoID = "ego"


def determineFMUFilename():
    FMU_FILENAME = 'sumo-fmi2.fmu'
    BITS = '64' if sys.maxsize > 2**32 else '32'
    if os.name == 'posix':
        FMU_FILENAME = 'sumo-fmi2-linux' + BITS + '.fmu'
    elif os.name == 'nt':
        FMU_FILENAME = 'sumo-fmi2-win' + BITS + '.fmu'
    else:
        FMU_FILENAME = 'sumo-fmi2-darwin' + BITS + '.fmu'
    return FMU_FILENAME


def runSingle(startTime, endTime, validate, scalarVariable):
    callOptions = "-n input_net.net.xml -r input_routes.rou.xml"

    fmuLocation = os.path.join(sumoHome, "bin", determineFMUFilename())
    modelDescription = read_model_description(fmuLocation, validate=False)

    if validate:
        try:
            problems = validate_fmu(fmuLocation)
        except Exception as e:
            problems = [str(e)]
        if problems:
            print("Problems found during FMU validation:")
            for p in problems:
                print("\t", p)
            sys.exit(1)
        else:
            print("FMU validation successful")

    # collect the value references
    valueRefs = {}
    for var in modelDescription.modelVariables:
        valueRefs[var.name] = var.valueReference

    # print("value references:")
    # for vr in valueRefs:
    #     print("\t%d %s" % (valueRefs[vr], vr))

    unzipDir = extract(fmuLocation)

    fmu = FMU2Slave(guid=modelDescription.guid,
                    unzipDirectory=unzipDir,
                    modelIdentifier=modelDescription.coSimulation.modelIdentifier,
                    instanceName='instance_01')

    # initialize FMU
    fmu.instantiate()
    fmu.setupExperiment(startTime=startTime, stopTime=endTime)
    fmu.setString([valueRefs['simulation.libsumo.callOptions']], [callOptions])
    fmu.enterInitializationMode()
    fmu.exitInitializationMode()

    time = startTime
    stepSize = modelDescription.defaultExperiment.stepSize

    print("Simulating %s (model name '%s', FMI v%s)..." % (
        modelDescription.coSimulation.modelIdentifier, modelDescription.modelName, modelDescription.fmiVersion))

    if scalarVariable == "vehicle.getParameterWithKey":
        fmu.setString([valueRefs['setGetterParameters']], ["ego meaningOfLife"])
        resultList = fmu.getString([valueRefs['vehicle.getParameterWithKey']])
        key = resultList[0].decode('UTF-8').split()[0]
        value = resultList[0].decode('UTF-8').split()[1]
        print("key, value: \'%s\', \'%s\'" % (key, value))

    while time < endTime:
        if scalarVariable == "vehicle.moveToXY":
            # get current laneID of ego vehicle
            fmu.setString([valueRefs['setGetterParameters']], ["ego"])
            resultList = fmu.getString([valueRefs['vehicle.getLaneID']])
            laneID = resultList[0].decode('UTF-8')
            print("TIME %s: laneID = \'%s\'" % (time, laneID))
            if laneID is not None and len(laneID) > 0:
                edgeID = laneID[:laneID.rfind('_')]
                laneIndex = laneID[laneID.rfind('_') + 1:]
                resultList = fmu.getString([valueRefs['vehicle.getPosition']])
                currentPos = [float(x) for x in resultList[0].decode('UTF-8').split()]
                targetPos = (currentPos[0] + 10, currentPos[1])
                print("currentPos =", currentPos)
                print("targetPos =", targetPos)
                fmu.setString([valueRefs['vehicle.moveToXY']],
                              ["ego " + edgeID + " " + laneIndex + " " + str(targetPos[0]) + " " + str(targetPos[1])])

        # perform one step
        fmu.doStep(currentCommunicationPoint=time, communicationStepSize=stepSize)
        # advance the time
        time += stepSize

        if scalarVariable == "vehicle.getIDCount":
            vehIDCount = fmu.getInteger([valueRefs['vehicle.getIDCount']])[0]
            print("TIME %s: vehicle.getIDCount = %s" % (time, vehIDCount))

    # clean up
    fmu.terminate()
    fmu.freeInstance()
    shutil.rmtree(unzipDir, ignore_errors=True)

    print("Print ended at step %s" % time)
    sys.stdout.flush()


if len(sys.argv) < 5:
    print("Usage: runner <startTime> <endTime> <validateFMU> <ScalarVariable>")
    sys.exit("")
sys.stdout.flush()

startTime = int(sys.argv[1])
endTime = int(sys.argv[2])
validate = sumolib.miscutils.parseBool(sys.argv[3])
scalarVariable = sys.argv[4]

runSingle(startTime, endTime, validate, scalarVariable)
