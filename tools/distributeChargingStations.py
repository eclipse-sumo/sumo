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
                           help="define the base parking areas and charging stations")
    argParser.add_argument("--selection-file", category="input", dest="selectionFile",
                           help="optionally restrict the parking area to the selected net part")
    argParser.add_argument("-o", "--output-file", category="output", dest="outFile",
                           default="parkingCharging.add.xml",
                           help="define the output filename for charging station definitions "
                                "(and by default parking areas)")
    argParser.add_argument("--output-parking-file", category="output", dest="outParkingFile",
                           help="define the output filename for the separate parking additional file")
    argParser.add_argument("--separate-unused-parkings", dest="separateUnusedParkings", action="store_true",
                           default=False,
                           help="Write parkings with charging stations to the main output file "
                                "and other parkings to the separate one for parkings")
    argParser.add_argument("-p", "--probability", category="processing", type=float, default=1,
                           help="Probability for an edge to receive a charging station")
    argParser.add_argument("-d", "--density", category="processing", type=float, default=0.1,
                           help="Share of parking spaces on the edge which should get charging points")
    argParser.add_argument("--power", category="processing", type=float, default=22000,
                           help="Charging power of the charging station")
    argParser.add_argument("--efficiency", category="processing", type=float, default=0.95,
                           help="Charging efficiency")
    argParser.add_argument("--min", category="processing", type=int, default=1,
                           help="Minimum number of charging points per parking")
    argParser.add_argument("--max", category="processing", type=int, default=100,
                           help="Maximum number of charging points per parking")
    argParser.add_argument("--prefix", category="processing", default="cs",
                           help="prefix for the charging station IDs")
    argParser.add_argument("--suffix", category="processing", default="_shift",
                           help="suffix for ID of splitted parkingArea")
    argParser.add_argument("-s", "--seed", category="processing", type=int, default=42, help="random seed")
    argParser.add_argument("--vclass", category="processing", default="passenger",
                           help="only use edges which permit the given vehicle class")
    argParser.add_argument("--entire-parkings", dest="entireParkings", action="store_true",
                           default=False,
                           help="If set, parkings are not divided if the number of charging stations "
                                "is smaller than the parking capacity")
    argParser.add_argument("--include-existing", dest="includeExisting", action="store_true",
                           default=False,
                           help="If set, loaded charging stations from input files will contribute to the density")
    argParser.add_argument("--skip-equipped-edges", dest="skipEquippedEdges", action="store_true",
                           default=False,
                           help="If set, edges where a charging station already exists are skipped")
    argParser.add_argument("--only-roadside", dest="onlyRoadside", action="store_true",
                           default=False, help="Only use roadside parking for charging points")
    argParser.add_argument("--only-parking-lot", dest="onlyParkingLot", action="store_true",
                           default=False, help="Only use parking lots off the road for charging points")
    argParser.add_argument("-v", "--verbose", category="processing", action="store_true",
                           default=False, help="tell me what you are doing")

    options = argParser.parse_args(args=args)
    if not options.netFile or not options.addFiles:
        argParser.print_help()
        sys.exit(1)
    if options.onlyRoadside and options.onlyParkingLot:
        sys.exit("Please use at most one of the parameters --only-parking-lot and --only-roadside.")
    options.min = max(0, options.min)
    options.max = max(0, options.max)
    if options.min > options.max:
        print("Will swap min and max charging point number due to wrong order (%d > %d)" % (options.min, options.max))
        options.min, options.max = options.max, options.min
    options.density = max(0., min(options.density, 1.))
    if options.separateUnusedParkings and options.outParkingFile is None:
        options.separateUnusedParkings = False
        if options.verbose:
            print("Cannot separate parkings as the output file has not been provided by --output-parking-file).")
    return options


def addChildToParent(parentEl, childEl, secondChildTags=[]):
    addedChildEl = parentEl.addChild(childEl.name, {t[0]: t[1] for t in childEl.getAttributes()}, sortAttrs=False)
    for secondChildEl in childEl.getChildList():
        if secondChildEl.name in secondChildTags:
            addedChildEl.addChild(secondChildEl.name, {t[0]: t[1]
                                  for t in secondChildEl.getAttributes()}, sortAttrs=False)
    return addedChildEl


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
        if options.verbose:
            print("Load additional file %s" % addFile)
        additionals.extend(list(sumolib.xml.parse(addFile, "additional"))[0].getChildList())
    totalCapacity = 0
    paCount = 0
    existingChargingStations = []
    unusedParkings = []
    for node in additionals:
        if node.name == "chargingStation" and node.hasAttribute("parkingArea"):
            existingChargingStations.append(node)
        elif node.name == "parkingArea":
            if int(node.getAttributeSecure("roadsideCapacity", 0)) > 0 and options.onlyParkingLot:
                unusedParkings.append(node)
                continue
            if node.hasChild("space") and options.onlyRoadside:
                unusedParkings.append(node)
                continue
            edge = net.getLane(node.lane).getEdge()
            if not edge.allows(options.vclass):
                continue
            capacity = sum(determineParkingCapacity(node))
            if edge not in edge2parkingArea:
                edge2parkingArea[edge] = []
            edge2parkingArea[edge].append([node, capacity])
            paCount += 1
            totalCapacity += capacity
    if options.verbose:
        print("Loaded %d parkings (%d already equipped with charging stations) with a total of %d parking lots." %
              (paCount, len(existingChargingStations), totalCapacity))
        print("%d unused parkings due to settings." % len(unusedParkings))

    # count already existing charging points per edge
    if options.includeExisting:
        for cs in existingChargingStations:
            edge = net.getLane(cs.getAttribute("lane")).getEdge()
            for item in edge2parkingArea[edge]:
                if item[0].getAttribute("id") == cs.getAttribute("parkingArea"):
                    if edge not in edge2chargingPointCount:
                        edge2chargingPointCount[edge] = 0
                    edge2chargingPointCount[edge] += item[1]
                    item[1] = 0
                    break

    # iterate edges with parkingArea groups and randomly select a charging point count
    totalChargingPoints = math.floor(totalCapacity * options.probability * options.density)
    if options.verbose:
        print("Charging points to distribute using a density of %.2f: %.0f on %.0f parking spaces (%.2f%%)" %
              (options.density, totalChargingPoints, totalCapacity,
               totalChargingPoints/totalCapacity*100 if totalCapacity > 0 else 0))
    csIndex = 0
    rootParking = None
    with sumolib.openz(options.outFile, 'w') as outf:
        rootCharging = sumolib.xml.create_document("additional")
        rootParking = sumolib.xml.create_document("additional") if options.outParkingFile else rootCharging
        for unusedParking in unusedParkings:
            addChildToParent(rootParking, unusedParking)
        unchangedParkings = []
        unvisitedEdges = []
        for edge, parkingAreas in edge2parkingArea.items():
            if  ((checkSelection and not edge.isSelected()) or len(parkingAreas) == 0 or
                 (options.skipEquippedEdges and edge in edge2chargingPointCount and edge2chargingPointCount[edge] > 0)):
                if len(parkingAreas) > 0:
                    unchangedParkings.extend([pa[0] for pa in parkingAreas])
                continue
            unvisitedEdges.append(edge)

        assignBalance = 0
        while totalChargingPoints > 0 and len(unvisitedEdges) > 0:
            # select edge
            randomNumber = random.random()
            selectedEdge = unvisitedEdges[int(randomNumber * len(unvisitedEdges))]
            unvisitedEdges.remove(selectedEdge)
            usedParkingAreas = []
            parkingAreas = edge2parkingArea[selectedEdge]
            capacities = [p[1] for p in parkingAreas]
            parkingSum = sum(capacities)
            chargingPointDiscount = edge2chargingPointCount[selectedEdge] if options.includeExisting and selectedEdge in edge2chargingPointCount else 0  # noqa
            wishedChargingPointCount = max(0, math.floor(options.density * parkingSum) - chargingPointDiscount)
            if parkingSum < options.min:
                assignBalance -= wishedChargingPointCount
                continue
            chargingPointCount = min(parkingSum, min(options.max, max(options.min, wishedChargingPointCount)))
            if options.verbose:
                print("Charging points before balancing: cp %d wished %d discount %d" %
                      (chargingPointCount, wishedChargingPointCount, chargingPointDiscount))

            openDelta = wishedChargingPointCount - chargingPointCount
            if openDelta > 0 and assignBalance < 0:
                addPoints = max(1, int(min(-assignBalance, parkingSum - chargingPointCount) * randomNumber))
                chargingPointCount += addPoints
                assignBalance += addPoints
            if chargingPointCount < wishedChargingPointCount:
                assignBalance -= wishedChargingPointCount - chargingPointCount

            if options.verbose:
                print("\tDistribute %d charging points on edge %s" % (chargingPointCount, str(selectedEdge.getID())))

            # first check if the charging point fits exactly one parkingArea
            remainingChargingPoints = chargingPointCount

            # optional: do not divide parkings
            # and just choose the one which matches best the select charging point number
            if options.entireParkings:
                closestCapacityIndex = min(range(len(capacities)), key=lambda i: abs(
                    capacities[i]-remainingChargingPoints))
                addChargingStation(options, rootCharging, rootParking, selectedEdge,
                                   parkingAreas[closestCapacityIndex][0],
                                   capacities[closestCapacityIndex], "%s%d" % (options.prefix, csIndex))
                csIndex += 1
                remainingChargingPoints = 0
                usedParkingAreas.append(parkingAreas[closestCapacityIndex][0])
            else:
                for i in range(len(capacities)):
                    if capacities[i] >= remainingChargingPoints:
                        if options.verbose:
                            print("Add charging station to parking %s with %d spaces." %
                                  (parkingAreas[i][0].getAttribute("id"), parkingAreas[i][1]))
                        addChargingStation(options, rootCharging, rootParking, selectedEdge, parkingAreas[i][0],
                                           remainingChargingPoints, "%s%d" % (options.prefix, csIndex))
                        csIndex += 1
                        remainingChargingPoints = 0
                        usedParkingAreas.append(parkingAreas[i][0])
                        break
                # then distribute across the parkingAreas in definition order
                if remainingChargingPoints > 0:
                    capacities = [p[1] for p in parkingAreas]
                    for i in range(len(capacities)):
                        if parkingAreas[i][0] in usedParkingAreas:
                            continue
                        if options.verbose:
                            print("Add charging station to parking %s with %d spaces." %
                                  (parkingAreas[i][0].getAttribute("id"), parkingAreas[i][1]))
                        installChargingPoints = min(remainingChargingPoints, capacities[i])
                        addChargingStation(options, rootCharging, rootParking, selectedEdge, parkingAreas[i][0],
                                           installChargingPoints, "%s%d" % (options.prefix, csIndex))
                        csIndex += 1
                        remainingChargingPoints -= installChargingPoints
                        usedParkingAreas.append(parkingAreas[i][0])
                        if remainingChargingPoints == 0:
                            break
            totalChargingPoints -= chargingPointCount

            # write unchanged parkings
            for node, _ in parkingAreas:
                if node not in usedParkingAreas:
                    unchangedParkings.append(node)

        # add more unchanged parkings
        for edge in unvisitedEdges:
            if edge in edge2parkingArea:
                unchangedParkings.extend([pa[0] for pa in edge2parkingArea[edge]])

        for node in unchangedParkings:
            addChildToParent(rootParking, node, secondChildTags=["param", "space"])

        # write existing charging stations
        for csNode in existingChargingStations:
            addChildToParent(rootCharging, csNode)
        outf.write(rootCharging.toXML())
        if options.verbose:
            print("Final charging point balance: %.0f" % totalChargingPoints)
    if options.outParkingFile:
        outfParking = open(options.outParkingFile, 'w')
        outfParking.write(rootParking.toXML())
        outfParking.close()


def addChargingStation(options, rootCharging, rootParking, edge, parkingArea, chargingPoints, csID):
    parkingCapacity = determineParkingCapacity(parkingArea)
    if chargingPoints <= sum(parkingCapacity) and chargingPoints > 0:
        # downsize parkingArea and create a new one for the remaining parking spaces
        chargingRoadSide = min(parkingCapacity[0], chargingPoints)
        chargingOnSpaces = chargingPoints - chargingRoadSide
        shiftRoadSideCapacity = parkingCapacity[0] - chargingRoadSide
        shiftSpaces = parkingCapacity[1] - chargingOnSpaces
        startPos = float(parkingArea.startPos) if parkingArea.startPos is not None else 0
        endPos = float(parkingArea.endPos) if parkingArea.endPos is not None else edge.getLength()
        posDownSize = (startPos, endPos) if chargingOnSpaces > 0 else (
            startPos, startPos + (endPos - startPos)*chargingRoadSide/sum(parkingCapacity))
        parkingArea.roadsideCapacity = str(chargingRoadSide)
        remainingSpaces = []

        if shiftRoadSideCapacity + shiftSpaces > 0:
            parkingArea.setAttribute("startPos", str(posDownSize[0]))
            parkingArea.setAttribute("endPos", str(posDownSize[1]))
            posShift = (posDownSize[0], posDownSize[1]
                        ) if chargingRoadSide == parkingCapacity[0] else (posDownSize[1], endPos)
            spacesToShift = []
            if shiftSpaces > 0:
                spacesToShift.extend(parkingArea.getChild("space")[shiftSpaces:])
                remainingSpaces.extend(parkingArea.getChild("space")[:shiftSpaces])
            shiftedPaDict = {t[0]: t[1] for t in parkingArea.getAttributes()}
            shiftedPaDict["id"] = "%s%s" % (shiftedPaDict["id"], options.suffix)
            shiftedPaDict["startPos"] = str(posShift[0])
            shiftedPaDict["endPos"] = str(posShift[1])
            shiftedPaDict["roadsideCapacity"] = str(shiftRoadSideCapacity)
            shiftedParkingArea = rootParking.addChild(parkingArea.name, shiftedPaDict, sortAttrs=False)
            for spaceToShift in spacesToShift:
                addChildToParent(shiftedParkingArea, spaceToShift)
            if parkingArea.hasChild("param"):
                for paramEl in parkingArea.getChild("param"):
                    addChildToParent(shiftedParkingArea, paramEl)
        elif parkingArea.hasChild("space"):
            remainingSpaces.extend(parkingArea.getChild("space"))
        remainingParking = addChildToParent(
            rootCharging if options.separateUnusedParkings else rootParking, parkingArea, secondChildTags=["param"])
        if len(remainingSpaces) > 0:
            for remainingSpace in remainingSpaces:
                addChildToParent(remainingParking, remainingSpace)
        rootCharging.addChild("chargingStation", {"id": csID,
                                                  "lane": parkingArea.lane,
                                                  "startPos": parkingArea.startPos,
                                                  "endPos": parkingArea.endPos,
                                                  "power": str(options.power),
                                                  "efficiency": str(options.efficiency),
                                                  "parkingArea": parkingArea.id}, sortAttrs=False)
        return True
    return False


def determineParkingCapacity(parkingArea):
    roadSide = int(parkingArea.getAttributeSecure("roadsideCapacity", 0))
    spaces = 0
    if parkingArea.hasChild("space"):
        spaces += len(parkingArea.getChild("space"))
    return (roadSide, spaces)


if __name__ == "__main__":
    if not main(getOptions()):
        sys.exit(1)
