#!/usr/bin/env python
"""
@file    agentControl.py
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
from vehicleControl import init

class PassengerAgent:
    def __init__(self, edge):
        self.edge = edge
        self.parking = False

class CyberAgent:
    def __init__(self, edge):
        self.edge = edge
        self.parking = False
        self.load = 0

def reroute(occupancy, vehicleID):
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
    changeTarget(slotEdge, vehicleID)
    stopObject(slotEdge, vehicleID, SLOT_LENGTH-1.)

def reroutePerson(edge, vehicleID):
    row = int(edge[5])
    targetEdge = "footmain%sto%s" % (row, row+1)
    if row == DOUBLE_ROWS-1:
        targetEdge = "footmainout"
    stopObject(edge, vehicleID, 1., 0.)
    changeTarget(targetEdge, vehicleID)
    stopObject(targetEdge, vehicleID, ROW_DIST-10.)

def main():
    init(options.gui)
    occupancy = -numpy.ones((2*DOUBLE_ROWS, SLOTS_PER_ROW), int)
    agents = {}
    persons = {}
    waiting = {}
    
    for step in range(1, 10000):
        if options.verbose:
            print "step", step
        moveNodes = simStep(step)
        for vehicleID, edge, pos in moveNodes:
            if vehicleID in agents:
                agents[vehicleID].edge = edge
            else:
                agents[vehicleID] = CyberAgent(edge)
                if edge == "mainin":
                    reroute(occupancy, vehicleID)
                elif "foot" in edge:
                    stopObject("-"+edge, vehicleID)
                    parkEdge = edge.replace("foot", "slot")
                    if not parkEdge in persons:
                        persons[parkEdge] = []
                    persons[parkEdge].append(vehicleID)
                    agents[vehicleID].parking = True
            if options.verbose:
                print vehicleID, edge
            if edge in persons and pos >= SLOT_LENGTH-1.5:
                if options.verbose:
                    print "destReached", vehicleID, pos
                remaining = []
                for person in persons[edge]:
                    if agents[person].parking:
                        reroutePerson(edge.replace("slot", "-foot"), person)
                        agents[person].parking = False
                    else:
                        remaining.append(person)
                persons[edge] = remaining
            if edge.startswith("footmain"):
                if not agents[vehicleID].parking:
                    if not edge in waiting:
                        waiting[edge] = []
                    waiting[edge].append(vehicleID)
                    agents[vehicleID].parking = True
            if edge.startswith("cyber") and edge != "cyberin":
                if pos < 70:
                    footEdge = edge.replace("cyber", "footmain")
                    if not agents[vehicleID].parking:
                        if agents[vehicleID].load < CYBER_CAPACITY:                
                            if footEdge in waiting and waiting[footEdge]:
                                stopObject(edge, vehicleID, ROW_DIST-15.)
                                agents[vehicleID].parking = True
                    else:
                        if pos >= ROW_DIST - 20:
                            while waiting[footEdge] and agents[vehicleID].load < CYBER_CAPACITY:
                                person = waiting[footEdge].pop(0)
                                stopObject(footEdge, person, ROW_DIST-10., 0.)
                                agents[vehicleID].load += 1
                            stopObject(edge, vehicleID, ROW_DIST-15., 0.)
                            agents[vehicleID].parking = False
            if edge == "cyberout" and pos >= 70 and not agents[vehicleID].parking:
                changeTarget("cyberin", vehicleID)
                stopObject(edge, vehicleID, 90., agents[vehicleID].load * 5.)
                agents[vehicleID].parking = True
            if edge == "cyberin" and agents[vehicleID].parking:
                agents[vehicleID].parking = False
                agents[vehicleID].load = 0
                changeTarget("cyberout", vehicleID)
    close()
optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-g", "--gui", action="store_true", dest="gui",
                     default=False, help="run with GUI")
(options, args) = optParser.parse_args()
main()