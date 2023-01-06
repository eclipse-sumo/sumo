#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

# @file    aggregateBatteryOutput.py
# @author  Pablo Alvarez Lopez
# @date    2022-04-25

import os
import sys
import getopt
from collections import OrderedDict
sys.path += [os.path.join(os.environ["SUMO_HOME"], "tools")]
import sumolib  # noqa


def parseTimeSteps(inputFile):
    # create matrix for result
    result = {}
    # iterate over timeSteps
    for timeStep in sumolib.xml.parse(inputFile, 'timestep'):
        timestepFloat = float(timeStep.time)
        # create substructure
        result[timestepFloat] = {}
        for vehicle in timeStep.vehicle or []:
            # add vehicle
            result[timestepFloat][vehicle.id] = {}
            # add vehicle values
            result[timestepFloat][vehicle.id]["energyConsumed"] = float(vehicle.energyConsumed)
            result[timestepFloat][vehicle.id]["totalEnergyConsumed"] = float(vehicle.totalEnergyConsumed)
            result[timestepFloat][vehicle.id]["totalEnergyRegenerated"] = float(vehicle.totalEnergyRegenerated)
            result[timestepFloat][vehicle.id]["energyChargedInTransit"] = float(vehicle.energyChargedInTransit)
            result[timestepFloat][vehicle.id]["energyChargedStopped"] = float(vehicle.energyChargedStopped)
            result[timestepFloat][vehicle.id]["timeStopped"] = float(vehicle.timeStopped)
    # return result
    return result


def writeTimeSteps(result):
    with open(outputFile, "w") as f:
        sumolib.xml.writeHeader(f)
        f.write("<battery-export>\n")
        Step = sumolib.xml.compound_object('timestep', ['interval'])
        for timeStep in result:
            # create ET subElement (node) for timeStep
            step = Step([str(timeStep[0]) + "-" + str(timeStep[1])])
            # iterate over timeStep's vehicles
            for vehicleID in timeStep[2]:
                vehicleAttributes = timeStep[2][vehicleID]
                attrs = ("energyConsumed", "totalEnergyConsumed", "totalEnergyRegenerated",
                         "energyChargedInTransit", "energyChargedStopped", "timeStopped", "aggregateNumber")
                attrValues = OrderedDict([("id", vehicleID)] + [(a, str(vehicleAttributes[a])) for a in attrs])
                # create ET sub element (node) for vehicle
                step.addChild('vehicle', attrValues, sortAttrs=False)
            # write Output
            f.write(step.toXML("    "))
        f.write("</battery-export>\n")


def processMatrix(matrix, timeToSplit):
    # create matrix for result
    result = []
    # get last
    lastValue = int(list(matrix.keys())[-1])
    # fill timesteps
    for timeStep in range(0, lastValue, timeToSplit):
        # check if this is the last interval
        if ((timeStep + timeToSplit - 1) > lastValue):
            result.append([timeStep, lastValue, {}])
        else:
            result.append([timeStep, timeStep + timeToSplit - 1, {}])
    # declare timeStep counter
    timeStepCounter = 0
    # now copy values from matrix to result
    for timeStep in matrix:
        # check if update counter
        if (result[timeStepCounter][1] < timeStep):
            timeStepCounter += 1
        # copy vehicle information
        for vehicleID in matrix[timeStep]:
            # get vehicle from Matrix
            vehicleMatrix = matrix[timeStep][vehicleID]
            # declare flag for find
            found = False
            # iterate over all vehicle IDs that there is already in result
            for vehicleIDResult in result[timeStepCounter][2]:
                # check if was already inserted and we have only to update
                if ((vehicleIDResult == vehicleID) and not found):
                    # get vehicle from Result
                    vehicleResult = result[timeStepCounter][2][vehicleIDResult]
                    # declare new vehicle attributes
                    newVehicleAttributes = {}
                    # update vehicle attributes
                    newVehicleAttributes["energyConsumed"] = float(
                        vehicleResult["energyConsumed"]) + float(vehicleMatrix["energyConsumed"])
                    newVehicleAttributes["totalEnergyConsumed"] = float(
                        vehicleResult["totalEnergyConsumed"]) + float(vehicleMatrix["totalEnergyConsumed"])
                    newVehicleAttributes["totalEnergyRegenerated"] = float(
                        vehicleResult["totalEnergyRegenerated"]) + float(vehicleMatrix["totalEnergyRegenerated"])
                    newVehicleAttributes["energyChargedInTransit"] = float(
                        vehicleResult["energyChargedInTransit"]) + float(vehicleMatrix["energyChargedInTransit"])
                    newVehicleAttributes["energyChargedStopped"] = float(
                        vehicleResult["energyChargedStopped"]) + float(vehicleMatrix["energyChargedStopped"])
                    newVehicleAttributes["timeStopped"] = float(
                        vehicleResult["timeStopped"]) + float(vehicleMatrix["timeStopped"])
                    newVehicleAttributes["aggregateNumber"] = float(vehicleResult["aggregateNumber"]) + 1
                    # add new vehicle attributes in result
                    result[timeStepCounter][2][vehicleIDResult] = newVehicleAttributes
                    # update flag
                    found = True
            # if vehicle wasn't found add it
            if not found:
                # declare new vehicle attributes
                newVehicleAttributes = {}
                # add vehicle attributes
                newVehicleAttributes["energyConsumed"] = float(vehicleMatrix["energyConsumed"])
                newVehicleAttributes["totalEnergyConsumed"] = float(vehicleMatrix["totalEnergyConsumed"])
                newVehicleAttributes["totalEnergyRegenerated"] = float(vehicleMatrix["totalEnergyRegenerated"])
                newVehicleAttributes["energyChargedInTransit"] = float(vehicleMatrix["energyChargedInTransit"])
                newVehicleAttributes["energyChargedStopped"] = float(vehicleMatrix["energyChargedStopped"])
                newVehicleAttributes["timeStopped"] = float(vehicleMatrix["timeStopped"])
                newVehicleAttributes["aggregateNumber"] = 1.0
                # add new vehicle attributes in result
                result[timeStepCounter][2][vehicleID] = newVehicleAttributes

    # return  matrix
    return result


"""
@brief main
"""

# parse input
opts, args = getopt.getopt(sys.argv[1:], "i:o:t:v:", ["input=", "output=", "time="])

# check arguments
if len(opts) == 0:
    print('usage: aggregateBatteryOutput.py -i <battery input file> '
          '-o <battery merged output file> -t <time to merge>')
    sys.exit()

# parse arguments
for opt, arg in opts:
    if opt in ("-i", "--input"):
        inputFile = arg
    elif opt in ("-o", "--output"):
        outputFile = arg
    elif opt in ("-t", "--time"):
        timeToSplit = int(arg)

# create matrix with timeSteps
matrix = parseTimeSteps(inputFile)

matrix = processMatrix(matrix, timeToSplit)

# write matrix
writeTimeSteps(matrix)
