"""
@file    vehicleControl.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-21
@version $Id$

Control the CityMobil parking lot via TraCI.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import subprocess, numpy
from optparse import OptionParser

from constants import *
from traciControl import initTraCI, simStep, stopObject, changeTarget, close

class Manager:
    def personArrived(self, vehicleID, edge):
        raise NotImplementedError
    def cyberCarArrived(self, vehicleID, edge, pos):
        raise NotImplementedError

class Status:

    def __init__(self, edge, pos):
        self.edge = edge
        self.pos = pos
        self.parking = False
        self.target = None
        self.targetPos = None

class Setting:
    step = 0
    manager = None 
    verbose = False

setting = Setting()
occupancy = -numpy.ones((2*DOUBLE_ROWS, SLOTS_PER_ROW), int)
vehicleStatus = {}
persons = {}
waiting = {}

def init(gui, manager, verbose):
    sumoExe = SUMO
    if gui:
        sumoExe = SUMOGUI
    sumoProcess = subprocess.Popen("%s -c %s.sumo.cfg" % (sumoExe, PREFIX), shell=True)
    initTraCI(PORT)
    setting.manager = manager
    setting.verbose = verbose

def stopAt(vehicleID, edge, pos=1.):
    changeTarget(edge, vehicleID)
    stopObject(edge, vehicleID, pos)
    vehicleStatus[vehicleID].target = edge
    vehicleStatus[vehicleID].targetPos = pos

def leaveStop(vehicleID, newTarget=None):
    v = vehicleStatus[vehicleID]
    if newTarget:
        changeTarget(newTarget, vehicleID)
    stopObject(v.target, vehicleID, v.targetPos, 0.)
    v.target = None
    v.targetPos = None

def _rerouteCar(vehicleID):
    slotEdge = ""
    for rowIdx, row in enumerate(occupancy):
        for idx, slot in enumerate(row):
            if slot == -1:
                row[idx] = vehicleID
                dir = "l"
                if rowIdx % 2 == 1:
                    dir = "r"
                slotEdge = "slot%s-%s%s" % (rowIdx/2, idx, dir)
                break
        if slotEdge:
            break
    stopAt(vehicleID, slotEdge, SLOT_LENGTH-1.)

def _reroutePerson(edge, vehicleID):
    row = int(edge[5])
    targetEdge = "footmain%sto%s" % (row, row+1)
    if row == DOUBLE_ROWS-1:
        targetEdge = "footmainout"
    stopObject(edge, vehicleID, 1., 0.)
    stopAt(vehicleID, targetEdge, ROW_DIST-10.)

def _checkInitialPositions(vehicleID, edge, pos):
    if vehicleID in vehicleStatus:
        vehicleStatus[vehicleID].edge = edge
        vehicleStatus[vehicleID].pos = pos
    else:
        vehicleStatus[vehicleID] = Status(edge, pos)
        if edge == "mainin":
            _rerouteCar(vehicleID)
        elif "foot" in edge:
            stopObject("-"+edge, vehicleID)
            parkEdge = edge.replace("foot", "slot")
            if not parkEdge in persons:
                persons[parkEdge] = []
            persons[parkEdge].append(vehicleID)
            vehicleStatus[vehicleID].parking = True
    if setting.verbose:
        print vehicleID, edge
    if edge in persons and pos >= SLOT_LENGTH-1.5:
        if setting.verbose:
            print "destReached", vehicleID, pos
        remaining = []
        for person in persons[edge]:
            if vehicleStatus[person].parking:
                _reroutePerson(edge.replace("slot", "-foot"), person)
                vehicleStatus[person].parking = False
            else:
                remaining.append(person)
        persons[edge] = remaining

def doStep():
    setting.step += 1
    if setting.verbose:
        print "step", setting.step
    moveNodes = simStep(setting.step)
    for vehicleID, edge, pos in moveNodes:
        _checkInitialPositions(vehicleID, edge, pos)
        if edge.startswith("footmain"):
            if not vehicleStatus[vehicleID].parking:
                setting.manager.personArrived(vehicleID, edge)
                vehicleStatus[vehicleID].parking = True
        if edge.startswith("cyber"):
            setting.manager.cyberCarArrived(vehicleID, edge, pos)
