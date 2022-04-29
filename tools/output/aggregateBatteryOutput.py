#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
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

import xml.etree.ElementTree as ET
import xml.dom.minidom
import sys
import getopt

"""
@brief parse time steps
"""
def parseTimeSteps(tree):
    # create matrix for result
    result = {float : {str : {str : float}}}
    # iterate over timeSteps
    for timeStep in tree.getroot():
        # get timeStep in float format
        timestepFloat = float(timeStep.attrib["time"])
        # create substructure
        result[timestepFloat] = {str : {str : float}}
        for vehicle in timeStep:
            # get vehicle ID
            vehicleID = vehicle.attrib["id"]
            # add vehicle
            result[timestepFloat][vehicleID] = {str : float}
            # add vehicle values
            result[timestepFloat][vehicleID]["energyConsumed"] = float(vehicle.attrib["energyConsumed"])
            result[timestepFloat][vehicleID]["totalEnergyConsumed"] = float(vehicle.attrib["totalEnergyConsumed"])
            result[timestepFloat][vehicleID]["totalEnergyRegenerated"] = float(vehicle.attrib["totalEnergyRegenerated"])
            result[timestepFloat][vehicleID]["energyChargedInTransit"] = float(vehicle.attrib["energyChargedInTransit"])
            result[timestepFloat][vehicleID]["energyChargedStopped"] = float(vehicle.attrib["energyChargedStopped"])
            result[timestepFloat][vehicleID]["timeStopped"] = float(vehicle.attrib["timeStopped"])
    # return result
    return result

"""
@brief save time steps
"""
def saveTimeSteps(result):
    # convert result in xml format
    outputRoot = ET.Element('battery-export')
    # iterate over result
    for timeStep in result:
        # filter type float
        if (type(timeStep) is not type(float)):
            # write timeSteps
            timeStepOutput = ET.SubElement(outputRoot, 'timestep')
            timeStepOutput.set("time", str(timeStep))
            # iterate over vehicles
            for vehicle in result[timeStep]:
                if (type(vehicle) is not type(str)):
                    vehicleOutput = ET.SubElement(timeStepOutput, 'vehicle')
                    # write vehicle values
                    vehicleOutput.set("id", vehicle)
                    vehicleOutput.set("energyConsumed", str(result[timeStep][vehicle]["energyConsumed"]))
                    vehicleOutput.set("totalEnergyConsumed", str(result[timeStep][vehicle]["totalEnergyConsumed"]))
                    vehicleOutput.set("totalEnergyRegenerated", str(result[timeStep][vehicle]["totalEnergyRegenerated"]))
                    vehicleOutput.set("energyChargedInTransit", str(result[timeStep][vehicle]["energyChargedInTransit"]))
                    vehicleOutput.set("energyChargedStopped", str(result[timeStep][vehicle]["energyChargedStopped"]))
                    vehicleOutput.set("timeStopped", str(result[timeStep][vehicle]["timeStopped"]))
    # write Output
    outputRootStr = ET.tostring(outputRoot, encoding="utf-8", method="xml")
    dom = xml.dom.minidom.parseString(outputRootStr)
    prettyDom = dom.toprettyxml()
    with open(outputFile, "w") as f:
        f.write(prettyDom)

"""
@brief process matrix
"""
def processMatrix(matrix)
    # create matrix for result
    result = {float : {str : {str : float}}}


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
        
# declare timeStep counter
timeStepCounter = 0
currentTimeStep = 0

# load battery outuput
tree = ET.parse(inputFile)

# create matrix with timeSteps
matrix = parseTimeSteps(tree)

# process matrix
matrix = processMatrix(matrix)

#write matrix
saveTimeSteps(matrix)
