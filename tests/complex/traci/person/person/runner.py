#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @author  Jakob Erdmann
# @date    2015-02-06


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa


def step():
    s = traci.simulation.getTime()
    traci.simulationStep()
    return s


def print_remaining_plan(personID, comment=""):
    print("remaining stages for '%s' %s" % (personID, comment))
    for i in range(traci.person.getRemainingStages(personID)):
        print("  %s: %s" % (i, traci.person.getStage(personID, i)))


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg", "--fcd-output", "fcd.xml"])
# add walking person
traci.person.add("newPerson", "3si", -10)
traci.person.appendWalkingStage("newPerson", ["3si", "2si"], -20)
traci.person.appendWaitingStage("newPerson", 10, "twiddling thumbs")
traci.person.appendWalkingStage("newPerson", ["2si", "2o"], 20)
traci.person.appendWaitingStage("newPerson", 20, "arrived")
traci.person.setSpeed("newPerson", 1.2)

for i in range(3):
    print("step", step())


# test rerouting
print("detour edges", traci.person.getEdges("detour"))
traci.person.rerouteTraveltime("detour")
print("detour edges after routing", traci.person.getEdges("detour"))


# trip-like rerouting
traci.person.add("tripTest", "2fi", -10)
traci.person.appendWalkingStage("tripTest", ["2fi", "1fi"], -20)
traci.person.removeStage("tripTest", 0)  # proceed from initial waiting-to-depart stage
traci.person.rerouteTraveltime("tripTest")
print("tripTest edges after routing", traci.person.getEdges("tripTest"))

personID = "horiz"
traci.person.setType(personID, "pType2")
traci.person.setLength(personID, 2)
traci.person.setHeight(personID, 1.9)
traci.person.setWidth(personID, 1.11)
traci.person.setMinGap(personID, 2.22)
traci.person.setColor(personID, (0, 0, 255, 255))
print("type", traci.person.getTypeID(personID))


def check(personID):
    print("persons", traci.person.getIDList())
    print("person count", traci.person.getIDCount())
    print("examining", personID)
    print("speed", traci.person.getSpeed(personID))
    print("pos", traci.person.getPosition(personID))
    print("pos3D", traci.person.getPosition3D(personID))
    print("angle", traci.person.getAngle(personID))
    print("slope", traci.person.getSlope(personID))
    print("road", traci.person.getRoadID(personID))
    print("type", traci.person.getTypeID(personID))
    print("lanePos", traci.person.getLanePosition(personID))
    print("color", traci.person.getColor(personID))
    print("length", traci.person.getLength(personID))
    print("MinGap", traci.person.getMinGap(personID))
    print("width", traci.person.getWidth(personID))
    print("waiting time", traci.person.getWaitingTime(personID))
    print("stage", traci.person.getStage(personID))
    print("remainingStages", traci.person.getRemainingStages(personID))
    print("edges", traci.person.getEdges(personID))
    print("vehicle", traci.person.getVehicle(personID))


check(personID)
traci.person.subscribe(personID)
print(traci.person.getSubscriptionResults(personID))
for i in range(3):
    print("step", step())
    print(traci.person.getSubscriptionResults(personID))
check(personID)
try:
    check("bla")
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("recovering from exception after asking for unknown person")
print("step", step())

traci.person.removeStages("newPerson")
traci.person.appendDrivingStage("newPerson", "1o", "B42")

traci.route.add("r0", ["3si", "1o"])
traci.vehicle.addLegacy("veh0", "r0", traci.constants.DEPARTFLAG_TRIGGERED, pos=230)
traci.vehicle.setLine("veh0", "B42")
traci.vehicle.setStop("veh0", "3si", 235, laneIndex=2, startPos=230, duration=1)

print("getIDList", traci.person.getIDList())
print("numVehs=%s, numPersons=%s, minExpected=%s" % (
    traci.vehicle.getIDCount(),
    traci.person.getIDCount(),
    traci.simulation.getMinExpectedNumber()))

for i in range(10):
    print("step", step())
    print(traci.person.getSubscriptionResults(personID))

print("riding in vehicle: '%s'" % traci.vehicle.getParameter("veh0", "device.person.IDList"))
print("riding in vehicle (direct): '%s'" % traci.vehicle.getPersonIDList("veh0"))
print("persons on edge %s at time %s: %s" % (
    traci.person.getRoadID("newPerson"),
    traci.simulation.getTime(),
    traci.edge.getLastStepPersonIDs(traci.person.getRoadID("newPerson"))))

traci.person.removeStages("newPerson")
traci.person.appendWaitingStage(
    "newPerson", 10, "Jumped out of a moving vehicle. Ouch!")

step()
# change plan on junction
internalEdge = traci.person.getRoadID("detour")
print("'detour' on road", internalEdge)
traci.person.removeStages("detour")
traci.person.appendWalkingStage("detour", [internalEdge, "1si"], -20)

# reroute on junction
internalEdge = traci.person.getRoadID("detour2")
print("'detour2' on road", internalEdge)
print("detour2 edges", traci.person.getEdges("detour2"))
traci.person.rerouteTraveltime("detour2")
print("detour2 edges after routing", traci.person.getEdges("detour2"))

for i in range(196):
    step()
    if "detour2" in traci.person.getIDList():
        traci.person.rerouteTraveltime("detour2")

# person should be on a walkingArea in front of a crossing now
print(traci.person.getRoadID(personID))
print(traci.person.getLanePosition(personID))
print(traci.person.getNextEdge(personID))
# retrieve invalid stages
try:
    print(traci.person.getStage(personID, 3))
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("recovering from exception after asking for invalid stage index")
try:
    print(traci.person.getStage(personID, -2))
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("recovering from exception after asking for invalid stage index")

# changing walk edges in the middle of a walk
personTT2 = "tripTest2"
traci.person.add(personTT2, "2fi", 10)
traci.person.appendWalkingStage(personTT2, ["2fi"], -20)
for i in range(5):
    print("%s person=%s edge=%s pos=%s" % (
        traci.simulation.getTime(),
        personTT2,
        traci.person.getRoadID(personTT2),
        traci.person.getLanePosition(personTT2)))
    step()
traci.person.appendWalkingStage(personTT2, ["2fi", "1fi"], 10)
traci.person.removeStage(personTT2, 0)
print("  %s new edges edges=%s" % (personTT2, traci.person.getEdges(personTT2)))
for i in range(5):
    print("%s person=%s edge=%s pos=%s" % (
        traci.simulation.getTime(),
        personTT2,
        traci.person.getRoadID(personTT2),
        traci.person.getLanePosition(personTT2)))
    step()


# appendStage
traci.person.add("p3", "1fi", -10)
stage = traci.simulation.Stage(
    type=traci.constants.STAGE_WALKING,
    vType="", line="", destStop="",
    edges=["1fi", "1si"],
    travelTime=-1, cost=-1, length=-1,
    intended="", depart=-1, departPos=-20, arrivalPos=10,
    description="foo")
stage2 = traci.simulation.Stage(
    type=traci.constants.STAGE_WALKING,
    vType="car", line="", destStop="",
    edges=["1fi", "1o"],
    travelTime=-1, cost=-1, length=-1,
    intended="", depart=-1, departPos=-20, arrivalPos=10,
    description="foo")
stage3 = traci.simulation.Stage(
    type=traci.constants.STAGE_WALKING,
    vType="car", line="", destStop="",
    edges=["1o", "3o"],
    travelTime=-1, cost=-1, length=-1,
    intended="", depart=-1, departPos=-20, arrivalPos=10,
    description="foo")
stage4 = traci.simulation.Stage(
    type=traci.constants.STAGE_WALKING,
    vType="car", line="", destStop="",
    edges=["1o", "4o"],
    travelTime=-1, cost=-1, length=-1,
    intended="", depart=-1, departPos=-20, arrivalPos=10,
    description="foo")

traci.person.appendStage("p3", stage)
for i in range(10):
    traci.simulationStep()

remaining = traci.person.getRemainingStages("p3")
assert(remaining == 1)
# replace current stage
print_remaining_plan("p3", "(before replacement of current stage")
traci.person.replaceStage("p3", 0, stage2)
print_remaining_plan("p3", "(after replacement")
# replace later stage
traci.person.appendStage("p3", stage3)
print_remaining_plan("p3", "(before replacement of next stage")
traci.person.replaceStage("p3", 1, stage4)
print_remaining_plan("p3", "(after replacement")

for i in range(41):
    traci.simulationStep()

traci.close()
