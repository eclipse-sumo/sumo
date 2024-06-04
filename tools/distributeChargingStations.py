#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    distributeChargingStations.py
# @author  Mirko Barthauer
# @date    2024-04-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import random
import math

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def getOptions(args=None):
    argParser = sumolib.options.ArgumentParser(
        description="Add charging stations on parkings and rebuild parkings if necessary")
    argParser.add_argument("-n", "--net-file", category="input", dest="netFile", required=True,
                           help="define the net file (mandatory)")
    argParser.add_argument("-a", "--add-files", category="input", dest="addFiles", required=True,
                           help="define the base parking areas")
    argParser.add_argument("--selection-file", category="input", dest="selectionFile",
                           help="optionally restrict the parking area to the selected net part")
    argParser.add_argument("-o", "--output-file", category="output", dest="outFile",
                           default="parkingCharging.add.xml", help="define the output filename")
    argParser.add_argument("-p", "--probability", category="processing", type=float, default=1,
                           help="Probability for an edge to receive a charging station")
    argParser.add_argument("-d", "--density", category="processing", type=float, default=0.1,
                           help="Share of parking spaces on the edge which should get charging points")
    argParser.add_argument("--power", category="processing", type=float, default=100,
                           help="Charging power of the charging station")
    argParser.add_argument("--efficiency", category="processing", type=float, default=0.95,
                           help="Charging efficiency")
    argParser.add_argument("--min", category="processing", type=int, default=0,
                           help="Minimum number of charging points")
    argParser.add_argument("--max", category="processing", type=int, default=int(1e9),
                           help="Maximum number of charging points")
    argParser.add_argument("--prefix", category="processing", default="cs", help="prefix for the charging station IDs")
    argParser.add_argument("--suffix", category="processing", default="_shift",
                           help="suffix for ID of splitted parkingArea")
    argParser.add_argument("-s", "--seed", category="processing", type=int, default=42, help="random seed")
    argParser.add_argument("--vclass", category="processing", default="passenger",
                           help="only use edges which permit the given vehicle class")
    argParser.add_argument("--include-existing", dest="includeExisting", action="store_false",
                           default=True,
                           help="If set, loaded charging stations from input files will contribute to the density")
    # argParser.add_argument("--no-roadside", dest="noRoadsideCharging", action="store_false",
    # default=True, help="Forbid to use roadside parking for charging points")
    # argParser.add_argument("--no-parking-lot", dest="noParkingLotCharging", action="store_false",
    # default=True, help="Forbid to use parking lots off the road for charging points")
    argParser.add_argument("-v", "--verbose", category="processing", action="store_true",
                           default=False, help="tell me what you are doing")

    options = argParser.parse_args(args=args)
    if not options.netFile or not options.addFiles:
        argParser.print_help()
        sys.exit(1)
    if not options.roadside and not options.parkingLot:
        print("Have to use at least one option of roadside or offroad parking to place charging stations. Please ")
    options.min = max(0, options.min)
    options.max = max(0, options.max)
    if options.min > options.max:
        print("Will swap min and max charging point number due to wrong order (%d > %d)" % (options.min, options.max))
        options.min, options.max = options.max, options.min
    options.density = max(0., min(options.density, 1.))

    return options


def hasOppositeEdge(edge):
    toNode = edge.getToNode()
    fromNode = edge.getFromNode()
    return fromNode in [e.getToNode() for e in toNode.getOutgoing()]


def main(options):
    random.seed(options.seed)

    net = sumolib.net.readNet(options.netFile)
    checkSelection = False
    if options.selectionFile is not None:
        net.loadSelection(options.selectionFile)
        checkSelection = True

    # read data of predefined parkingAreas and charging stations
    edge2parkingArea = {}
    edge2chargingPointCount = {}
    additionals = []
    for addFile in options.addFiles.split(","):
        additionals.extend(list(sumolib.xml.parse(addFile, "additional"))[0])
    totalCapacity = 0
    paCount = 0
    existingChargingStations = []
    for node in additionals.getChildList():
        if node.name == "chargingStation" and node.hasAttribute("parkingArea"):
            existingChargingStations.append(node)
        elif node.name == "parkingArea":
            edge = net.getLane(node.lane).getEdge()
            capacity = sum(determineParkingCapacity(node))
            if edge not in edge2parkingArea:
                edge2parkingArea[edge] = []
            edge2parkingArea[edge].append([node, capacity])
            paCount += 1
            totalCapacity += capacity
    if options.verbose:
        print("Loaded %d parkings (%d already equipped with charging stations) with a total of %d parking lots." %
              (paCount, len(equippedParkings), totalCapacity))

    # count already existing charging points per edge
    if options.includeExisting:
        for cs in existingChargingStations:
            edge = net.getLane(node.lane).getEdge()
            for item in edge2parkingArea[edge]:
                if item[0].getAttribute("id") == cs.getAttribute("parkingArea"):
                    if edge not in edge2chargingPointCount:
                        edge2chargingPointCount = 0
                    edge2chargingPointCount += item[1]
                    item[1] = 0
                    break

    # iterate edges with parkingArea groups and randomly select a charging point count
    totalChargingPoints = math.floor(totalCapacity * options.probability * options.density)
    if options.verbose:
        print("Charging points to distribute: %.0f" % totalChargingPoints)
    csIndex = 0
    with open(options.outFile, 'w') as outf:
        sumolib.xml.writeHeader(outf)
        alreadyChecked = []
        for edge in edge2parkingArea:
            if (checkSelection and not edge.isSelected()) or edge in alreadyChecked:
                continue
            randomNumber = random.random()
            if randomNumber < options.probability:
                capacities = [p[1] for p in edge2parkingArea[edge]]
                parkingSum = sum(capacities)
                if parkingSum < options.min:
                    continue
                randomChargingPointCount = options.min + \
                    round((options.max - options.min) * randomNumber/options.probability)
                if options.includeExisting and edge in edge2chargingPointCount:
                    randomChargingPointCount -= edge2chargingPointCount[edge]
                chargingPointCount = min(totalChargingPoints, parkingSum, randomChargingPointCount)

                # first check if the charging point fits exactly one parkingArea
                remainingChargingPoints = chargingPointCount
                for i in range(len(capacities)):
                    if capacities[i] >= remainingChargingPoints:
                        addChargingStation(options, additionals, edge, edge2parkingArea[edge][i][0],
                                           remainingChargingPoints, "%s%d" % (options.prefix, csIndex))
                        csIndex += 1
                        remainingChargingPoints = 0
                        edge2parkingArea[edge].remove(edge2parkingArea[edge][i])
                        break
                # then distribute across the parkingAreas in definition order
                capacities = [p[1] for p in edge2parkingArea[edge]]
                for i in range(len(capacities)):
                    installChargingPoints = min(remainingChargingPoints, capacities[i])
                    addChargingStation(options, additionals, edge, edge2parkingArea[edge][i][0],
                                       installChargingPoints, "%s%d" % (options.prefix, csIndex))
                    # print("added charging station with %d points on parkingArea %s %s" %
                    #       (installChargingPoints, edge2parkingArea[edge][i][0].id, result))
                    csIndex += 1
                    remainingChargingPoints -= installChargingPoints
                    if remainingChargingPoints == 0:
                        break
                totalChargingPoints -= chargingPointCount
                if totalChargingPoints <= 0:
                    break
        outf.write(additionals.toXML())
        if options.verbose:
            print("Final charging point balance: %.0f" % totalChargingPoints)


def addChargingStation(options, root, edge, parkingArea, chargingPoints, csID):
    parkingCapacity = determineParkingCapacity(parkingArea)
    if chargingPoints <= sum(parkingCapacity):
        # downsize parkingArea and create a new one for the remaining parking spaces
        chargingRoadSide = min(parkingCapacity[0], chargingPoints)
        chargingOnSpaces = chargingPoints - chargingRoadSide
        shiftRoadSideCapacity = parkingCapacity[0] - chargingRoadSide
        shiftSpaces = parkingCapacity[1] - chargingOnSpaces
        startPos = float(parkingArea.startPos) if parkingArea.startPos is not None else 0
        endPos = float(parkingArea.endPos) if parkingArea.endPos is not None else edge.getLength()
        posDownSize = (startPos, endPos) if chargingOnSpaces > 0 else (
            startPos, startPos + (endPos - startPos)*chargingRoadSide/parkingCapacity[0])
        parkingArea.roadsideCapacity = str(chargingRoadSide)

        if shiftRoadSideCapacity + shiftSpaces > 0:
            parkingArea.setAttribute("startPos", str(posDownSize[0]))
            parkingArea.setAttribute("endPos", str(posDownSize[1]))
            posShift = (posDownSize[0], posDownSize[1]
                        ) if chargingRoadSide == parkingCapacity[0] else (posDownSize[1], endPos)
            spacesToShift = []
            if shiftSpaces > 0:
                spacesToShift.extend(parkingArea.getChild("space")[chargingOnSpaces:])
            shiftedPaDict = {t[0]: t[1] for t in parkingArea.getAttributes()}
            shiftedPaDict["id"] = "%s%s" % (shiftedPaDict["id"], options.suffix)
            shiftedPaDict["startPos"] = str(posShift[0])
            shiftedPaDict["endPos"] = str(posShift[1])
            shiftedPaDict["roadsideCapacity"] = str(shiftRoadSideCapacity)
            shiftedParkingArea = root.addChild(parkingArea.name, shiftedPaDict, sortAttrs=False)
            for spaceToShift in spacesToShift:
                parkingArea.removeChild(spaceToShift)
                spaceDict = {t[0]: t[1] for t in spaceToShift.getAttributes()}
                shiftedParkingArea.addChild(spaceToShift.name, spaceDict)
        root.addChild("chargingStation", {"id": csID,
                                          "lane": parkingArea.lane,
                                          "startPos": parkingArea.startPos,
                                          "endPos": parkingArea.endPos,
                                          "power": str(options.power),
                                          "efficiency": str(options.efficiency),
                                          "parkingArea": parkingArea.id}, sortAttrs=False)
        return True
    return False


def determineParkingCapacity(parkingArea):
    roadSide = int(parkingArea.roadsideCapacity)
    spaces = 0
    if parkingArea.hasChild("space"):
        spaces += len(parkingArea.space)
    return (roadSide, spaces)


if __name__ == "__main__":
    if not main(getOptions()):
        sys.exit(1)
