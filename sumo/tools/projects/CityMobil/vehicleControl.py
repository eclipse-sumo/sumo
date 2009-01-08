"""
@file    vehicleControl.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-21
@version $Id$

Control the CityMobil parking lot via TraCI.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import subprocess, random, sys
from optparse import OptionParser

import statistics
from constants import *
from traciControl import initTraCI, simStep, stopObject, changeTarget, close

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

class Setting:
    step = 0
    manager = None 
    verbose = False
    cyber = False

setting = Setting()
occupancy = {}
vehicleStatus = {}
persons = {}
waiting = {}

def init(manager):
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-g", "--gui", action="store_true", dest="gui",
                         default=False, help="run with GUI")
    optParser.add_option("-c", "--cyber", action="store_true", dest="cyber",
                         default=False, help="use small cybercars instead of big busses")
    optParser.add_option("-d", "--demand", type="int", dest="demand",
                         default=15, help="period with which the persons are emitted")
    optParser.add_option("-b", "--break", type="int", dest="breakstep", metavar="TIMESTEP",
                         help="let a vehicle break for %s seconds at TIMESTEP" % BREAK_DELAY)
    (options, args) = optParser.parse_args()
    sumoExe = SUMO
    if options.gui:
        sumoExe = SUMOGUI
    sumoConfig = "%s%02i.sumo.cfg" % (PREFIX, options.demand)
    if options.cyber:
        sumoConfig = "%s%02i_cyber.sumo.cfg" % (PREFIX, options.demand)
    sumoProcess = subprocess.Popen("%s -c %s" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)
    initTraCI(PORT)
    setting.manager = manager
    setting.verbose = options.verbose
    setting.cyber = options.cyber
    setting.breakstep = options.breakstep
    try:
        while setting.step < 100 or statistics.personsRunning > 0:
            doStep()
        statistics.evaluate()
    finally:
        close()

def getCapacity():
    if setting.cyber:
        return CYBER_CAPACITY
    return BUS_CAPACITY

def getStep():
    return setting.step

def getPosition(vehicleID):
    return vehicleStatus[vehicleID].edge

def stopAt(vehicleID, edge, pos=None):
    if setting.verbose:
        print "stopAt", vehicleID, edge
    if pos == None:
        pos = ROW_DIST-15.
        if edge.endswith("out") or edge.endswith("in"):
            pos = 90.
    changeTarget(edge, vehicleID)
    stopObject(edge, vehicleID, pos)
    vehicleStatus[vehicleID].target = edge
    vehicleStatus[vehicleID].targetPos = pos

def leaveStop(vehicleID, newTarget=None, delay=0.):
    v = vehicleStatus[vehicleID]
    if newTarget:
        changeTarget(newTarget, vehicleID)
    stopObject(v.target, vehicleID, v.targetPos, delay)
    v.target = None
    v.targetPos = None
    v.parking = False

def _rerouteCar(vehicleID):
    slotEdge = ""
    for rowIdx in range(DOUBLE_ROWS):
        for idx in range(SLOTS_PER_ROW):
            for dir in ["l", "r"]:
                slotEdge = "slot%s-%s%s" % (rowIdx, idx, dir)
                if not slotEdge in occupancy:
                    occupancy[slotEdge] = vehicleID
                    stopAt(vehicleID, slotEdge, SLOT_LENGTH-1.)
                    return

def _reroutePersons(edge):
    if edge in persons:
        for person in persons[edge]:
            if not vehicleStatus[person].slot:
                row = int(edge[4])
                targetEdge = "footmain%sto%s" % (row, row+1)
                stopObject(edge.replace("slot", "-foot"), person, 1., 0.)
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
            stopObject("-"+edge, vehicleID)
            parkEdge = edge.replace("foot", "slot")
            if not parkEdge in persons:
                persons[parkEdge] = []
            persons[parkEdge].append(vehicleID)
            vehicleStatus[vehicleID].parking = True
        elif edge.startswith("slot"):
            stopAt(vehicleID, edge, SLOT_LENGTH-1.)
            occupancy[edge] = vehicleID
    _reroutePersons(edge)

def doStep():
    setting.step += 1
    if setting.verbose:
        print "step", setting.step
    moveNodes = simStep(setting.step)
    for vehicleID, edge, pos in moveNodes:
        _checkInitialPositions(vehicleID, edge, pos)
        vehicle = vehicleStatus[vehicleID]
        if edge == vehicle.target and not vehicle.parking:
            if edge.startswith("footmain"):
                vehicle.parking = True
                target = "footmainout"
                if edge == "footmainout":
                    row = random.randrange(0, DOUBLE_ROWS)
                    target = "footmain%sto%s" % (row, row+1)
                statistics.personArrived(vehicleID, edge, target, setting.step)
                setting.manager.personArrived(vehicleID, edge, target)
            if edge.startswith("cyber"):
                if vehicle.delay:
                    vehicle.delay -= 1
                elif setting.breakstep and setting.step >= setting.breakstep and edge != "cyberin":
                    if setting.verbose:
                        print "broken", vehicleID, edge
                    setting.breakstep = None
                    setting.manager.cyberCarBroken(vehicleID, edge)
                    vehicle.delay = BREAK_DELAY
                else:
                    if setting.verbose:
                        print "arrived", vehicleID, edge
                    vehicle.parking = True
                    setting.manager.cyberCarArrived(vehicleID, edge)
    setting.manager.setNewTargets()
