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

# @file    vehicleControl.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Lena Kalleske
# @date    2008-07-21

from __future__ import absolute_import
from __future__ import print_function
import random
import sys
import os
from optparse import OptionParser

from constants import PREFIX, DOUBLE_ROWS, STOP_POS, SLOTS_PER_ROW, SLOT_LENGTH, BUS_CAPACITY, BREAK_DELAY
from constants import CYBER_CAPACITY
import statistics

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci  # noqa
import traci.constants as tc  # noqa
import sumolib  # noqa


class Manager:

    def personArrived(self, vehicleID, edge, target):
        raise NotImplementedError

    def cyberCarArrived(self, vehicleID, edge):
        raise NotImplementedError

    def cyberCarBroken(self, vehicleID, edge):
        pass

    def setNewTargets(self):
        pass


class Status:

    def __init__(self, edge, pos):
        self.edge = edge
        self.pos = pos
        self.parking = False
        self.target = None
        self.targetPos = None
        self.slot = None
        self.delay = None

    def __repr__(self):
        return "%s,%s" % (self.edge, self.pos)


class Setting:
    step = 0
    manager = None
    verbose = False
    cyber = False
    breakstep = None


setting = Setting()
occupancy = {}
vehicleStatus = {}
persons = {}
waiting = {}


def init(manager):
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-g", "--gui", action="store_true",
                         default=False, help="run with GUI")
    optParser.add_option("-c", "--cyber", action="store_true",
                         default=False, help="use small cybercars instead of big busses")
    optParser.add_option("-d", "--demand", type="int",
                         default=15, help="period with which the persons are emitted")
    optParser.add_option("-b", "--break", type="int", dest="breakstep", metavar="TIMESTEP",
                         help="let a vehicle break for %s seconds at TIMESTEP" % BREAK_DELAY)
    optParser.add_option("-t", "--test", action="store_true",
                         default=False, help="Run in test mode")
    options, _ = optParser.parse_args()
    sumoExe = sumolib.checkBinary('sumo')
    if options.gui:
        sumoExe = sumolib.checkBinary('sumo-gui')
    sumoConfig = "%s%02i.sumocfg" % (PREFIX, options.demand)
    if options.cyber:
        sumoConfig = "%s%02i_cyber.sumocfg" % (PREFIX, options.demand)
    traci.start([sumoExe, "-c", sumoConfig])
    traci.simulation.subscribe()
    setting.manager = manager
    setting.verbose = options.verbose
    setting.cyber = options.cyber
    setting.breakstep = options.breakstep
    try:
        while setting.step < 100 or statistics.personsRunning > 0:
            doStep()
        statistics.evaluate(options.test)
    finally:
        traci.close()


def getCapacity():
    if setting.cyber:
        return CYBER_CAPACITY
    return BUS_CAPACITY


def getStep():
    return setting.step


def getPosition(vehicleID):
    return vehicleStatus[vehicleID].edge


def stopAt(vehicleID, edge, pos=None):
    if pos is None:
        pos = STOP_POS
        if edge.endswith("out") or edge.endswith("in"):
            pos = 90.
    traci.vehicle.changeTarget(vehicleID, edge)
    if setting.verbose:
        print("stopAt", vehicleID, edge, pos)
#        print vehicleStatus[vehicleID]
#        print traci.vehicle.getRoute(vehicleID)
    traci.vehicle.setStop(vehicleID, edge, pos)
    vehicleStatus[vehicleID].target = edge
    vehicleStatus[vehicleID].targetPos = pos


def leaveStop(vehicleID, newTarget=None, delay=0):
    v = vehicleStatus[vehicleID]
    if newTarget:
        traci.vehicle.changeTarget(vehicleID, newTarget)
    traci.vehicle.setStop(vehicleID, v.target, v.targetPos, 0, delay)
    v.target = None
    v.targetPos = None
    v.parking = False


def _rerouteCar(vehicleID):
    slotEdge = ""
    for rowIdx in range(DOUBLE_ROWS):
        for idx in range(SLOTS_PER_ROW):
            for dir in ["l", "r"]:
                slotEdge = "slot%s-%s%s" % (rowIdx, idx, dir)
                if slotEdge not in occupancy:
                    occupancy[slotEdge] = vehicleID
                    stopAt(vehicleID, slotEdge, SLOT_LENGTH - 5.)
                    return


def _reroutePersons(edge):
    if edge in persons:
        for person in persons[edge]:
            if not vehicleStatus[person].slot:
                row = int(edge[4])
                targetEdge = "footmain%sto%s" % (row, row + 1)
                traci.vehicle.setStop(
                    person, edge.replace("slot", "-foot"), 1., 0, 0)
                stopAt(person, targetEdge)
                vehicleStatus[person].parking = False
                vehicleStatus[person].slot = edge


def _checkInitialPositions(vehicleID, edge, pos):
    if vehicleID in vehicleStatus:
        vehicleStatus[vehicleID].edge = edge
        vehicleStatus[vehicleID].pos = pos
    else:
        vehicleStatus[vehicleID] = Status(edge, pos)
        if edge == "mainin":
            _rerouteCar(vehicleID)
        elif edge == "cyberin":
            stopAt(vehicleID, "cyberin")
        elif edge == "footfairin":
            stopAt(vehicleID, "footmainout")
        elif "foot" in edge:
            traci.vehicle.setStop(vehicleID, "-" + edge)
            parkEdge = edge.replace("foot", "slot")
            if parkEdge not in persons:
                persons[parkEdge] = []
            persons[parkEdge].append(vehicleID)
            vehicleStatus[vehicleID].parking = True
        elif edge.startswith("slot"):
            stopAt(vehicleID, edge, SLOT_LENGTH - 5.)
            occupancy[edge] = vehicleID
    _reroutePersons(edge)


def doStep():
    setting.step += 1
    if setting.verbose:
        print("step", setting.step)
    traci.simulationStep()
    moveNodes = []
    for veh, subs in traci.vehicle.getAllSubscriptionResults().items():
        moveNodes.append((veh, subs[tc.VAR_ROAD_ID], subs[tc.VAR_LANEPOSITION]))
    departed = traci.simulation.getSubscriptionResults()[tc.VAR_DEPARTED_VEHICLES_IDS]
    for v in departed:
        traci.vehicle.subscribe(v)
        subs = traci.vehicle.getSubscriptionResults(v)
        moveNodes.append((v, subs[tc.VAR_ROAD_ID], subs[tc.VAR_LANEPOSITION]))
    for vehicleID, edge, pos in moveNodes:
        _checkInitialPositions(vehicleID, edge, pos)
        vehicle = vehicleStatus[vehicleID]
        if edge == vehicle.target and not vehicle.parking:
            if edge.startswith("footmain"):
                vehicle.parking = True
                target = "footmainout"
                if edge == "footmainout":
                    row = random.randrange(0, DOUBLE_ROWS)
                    target = "footmain%sto%s" % (row, row + 1)
                statistics.personArrived(vehicleID, edge, target, setting.step)
                setting.manager.personArrived(vehicleID, edge, target)
            if edge.startswith("cyber"):
                if vehicle.delay:
                    vehicle.delay -= 1
                elif setting.breakstep and setting.step >= setting.breakstep and edge != "cyberin":
                    if setting.verbose:
                        print("broken", vehicleID, edge)
                    setting.breakstep = None
                    setting.manager.cyberCarBroken(vehicleID, edge)
                    vehicle.delay = BREAK_DELAY
                else:
                    if setting.verbose:
                        print("arrived", vehicleID, edge)
                    vehicle.parking = True
                    setting.manager.cyberCarArrived(vehicleID, edge)
    setting.manager.setNewTargets()
