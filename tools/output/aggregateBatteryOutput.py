#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    aggregateBatteryOutput.py
# @author  Pablo Alvarez Lopez
# @date    2022-04-25

import os
import sys
from collections import OrderedDict
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import sumolib  # noqa

ATTRS = (
    "energyConsumed",
    "totalEnergyConsumed",
    "totalEnergyRegenerated",
    "energyChargedInTransit",
    "energyChargedStopped",
    "timeStopped",
)
WRITE_ATTRS = ATTRS + ("aggregateNumber",)


def parseTimeSteps(inputFile):
    result = {}
    for timeStep in sumolib.xml.parse(inputFile, 'timestep'):
        timestepFloat = float(timeStep.time)
        result[timestepFloat] = {}
        for vehicle in timeStep.vehicle or []:
            result[timestepFloat][vehicle.id] = {attr: float(getattr(vehicle, attr)) for attr in ATTRS}
    return result


def writeTimeSteps(result, outputFile):
    with sumolib.openz(outputFile, "w") as f:
        sumolib.xml.writeHeader(f)
        f.write("<battery-export>\n")
        Step = sumolib.xml.compound_object('timestep', ['interval'])
        for start, end, vehicles in result:
            step = Step(["%s-%s" % (start, end)])
            for vehicleID, vehicleAttributes in vehicles.items():
                attrValues = OrderedDict(
                    [("id", vehicleID)] + [(a, str(vehicleAttributes[a])) for a in WRITE_ATTRS]
                )
                step.addChild('vehicle', attrValues, sortAttrs=False)
            f.write(step.toXML("    "))
        f.write("</battery-export>\n")


def processMatrix(matrix, timeToSplit):
    if not matrix:
        return []
    result = []
    lastValue = int(list(matrix.keys())[-1])
    if lastValue == 0:
        result.append([0, 0, {}])
    else:
        for timeStep in range(0, lastValue, timeToSplit):
            endStep = min(timeStep + timeToSplit - 1, lastValue)
            result.append([timeStep, endStep, {}])
    timeStepCounter = 0
    for timeStep, stepVehicles in matrix.items():
        while timeStepCounter < len(result) and result[timeStepCounter][1] < timeStep:
            timeStepCounter += 1
        if not stepVehicles:
            continue
        if timeStepCounter >= len(result):
            startStep = result[-1][1] + 1
            result.append([startStep, max(startStep, int(timeStep)), {}])
        intervalVehicles = result[timeStepCounter][2]
        for vehicleID, vehicleMatrix in stepVehicles.items():
            if vehicleID in intervalVehicles:
                vehicleResult = intervalVehicles[vehicleID]
                for attr in ATTRS:
                    vehicleResult[attr] += vehicleMatrix[attr]
                vehicleResult["aggregateNumber"] += 1.0
            else:
                newVehicleAttributes = {attr: vehicleMatrix[attr] for attr in ATTRS}
                newVehicleAttributes["aggregateNumber"] = 1.0
                intervalVehicles[vehicleID] = newVehicleAttributes
    return result


def get_options(args=None):
    op = sumolib.options.ArgumentParser()
    op.add_argument("-i", "--input", type=op.file, category="input", help="battery input file")
    op.add_argument("-o", "--output", type=op.file, category="output", help="battery merged output file")
    op.add_argument("-t", "--time", type=op.time, help="time to merge")
    return op.parse_args(args=args)


def main(args=None):
    opts = get_options(args)
    matrix = parseTimeSteps(opts.input)
    matrix = processMatrix(matrix, int(opts.time))
    writeTimeSteps(matrix, opts.output)


if __name__ == "__main__":
    main()
