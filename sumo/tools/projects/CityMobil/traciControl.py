#!/usr/bin/env python
"""
@file    traciControl.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-21
@version $Id$

Control the CityMobil parking lot via TraCI.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import subprocess, socket, time, struct, numpy
from optparse import OptionParser

from constants import *

class Storage:

    def __init__(self, content):
        self._content = content
        self._pos = 0

    def read(self, format):
        oldPos = self._pos
        self._pos += struct.calcsize(format)
        return struct.unpack(format, self._content[oldPos:self._pos])

    def readString(self):
        length = self.read("!i")[0]
        return self.read("!%ss" % length)[0]
    
    def ready(self):
        return self._pos < len(self._content) 

class Status:

    def __init__(self, edge):
        self.edge = edge
        self.parking = False
        self.load = 0


def recvExact(sock):
    result = ""
    while len(result) < 4:
        result += sock.recv(4 - len(result))
    length = struct.unpack("!i", result)[0] - 4
    result = ""
    while len(result) < length:
        result += sock.recv(length - len(result))
    return Storage(result)

def sendExact(sock, message):
    length = struct.pack("!i", len(message)+4)
    sock.send(length)
    sock.send(message)
    result = recvExact(sock)
    prefix = result.read("!BBB")
    err = result.readString()
    if prefix[2] or err:
        print prefix, RESULTS[prefix[2]], err
    return result

def stopObject(edge, objectID, pos=1., duration=10000.):
    command = struct.pack("!BBiBi", 1+1+4+1+4+len(edge)+4+1+4+8, CMD_STOP, objectID, POSITION_ROADMAP, len(edge)) + edge
    command += struct.pack("!fBfd", pos, 0, 1., duration)
    return command

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
    command = struct.pack("!BBii", 1+1+4+4+len(slotEdge), CMD_CHANGETARGET, vehicleID, len(slotEdge)) + slotEdge
    command += stopObject(slotEdge, vehicleID, SLOT_LENGTH-1.)
    return command

def reroutePerson(edge, vehicleID):
    row = int(edge[5])
    targetEdge = "footmain%sto%s" % (row, row+1)
    if row == DOUBLE_ROWS-1:
        targetEdge = "footmainout"
    command = stopObject(edge, vehicleID, 1., 0.)
    command += struct.pack("!BBii", 1+1+4+4+len(targetEdge), CMD_CHANGETARGET, vehicleID, len(targetEdge)) + targetEdge
    command += stopObject(targetEdge, vehicleID, ROW_DIST-10.)
    return command

def main():
    sumoExe = SUMO
    if options.gui:
        sumoExe = SUMOGUI
    sumoProcess = subprocess.Popen("%s -c %s.sumo.cfg" % (sumoExe, PREFIX), shell=True)
    sock = socket.socket()
    for wait in range(10):
        try:
            sock.connect(("localhost", PORT))
	    break
	except socket.error:
            time.sleep(wait)
    occupancy = -numpy.ones((2*DOUBLE_ROWS, SLOTS_PER_ROW), int)
    vehicleStatus = {}
    persons = {}
    waiting = {}
    
    for step in range(1, 2000):
        if options.verbose:
            print "step", step
        command = struct.pack("!BBdB", 1+1+8+1, CMD_SIMSTEP, float(step), POSITION_ROADMAP)
        result = sendExact(sock, command)
        rerouteMsg = ""
        while result.ready():
            info = result.read("!BBidB")
            vehicleID = info[2]
#            print info
            edge = result.readString()
            if vehicleID in vehicleStatus:
                vehicleStatus[vehicleID].edge = edge
            else:
                vehicleStatus[vehicleID] = Status(edge)
                if edge == "mainin":
                    rerouteMsg += reroute(occupancy, vehicleID)
                elif "foot" in edge:
                    rerouteMsg += stopObject("-"+edge, vehicleID)
                    parkEdge = edge.replace("foot", "slot")
                    if not parkEdge in persons:
                        persons[parkEdge] = []
                    persons[parkEdge].append(vehicleID)
                    vehicleStatus[vehicleID].parking = True
            if options.verbose:
                print vehicleID, edge
            pos = result.read("!fB")[0]
            if edge in persons and pos >= SLOT_LENGTH-1.5:
                if options.verbose:
                    print "destReached", vehicleID, pos
                remaining = []
                for person in persons[edge]:
                    if vehicleStatus[person].parking:
                        rerouteMsg += reroutePerson(edge.replace("slot", "-foot"), person)
                        vehicleStatus[person].parking = False
                    else:
                        remaining.append(person)
                persons[edge] = remaining
            if edge.startswith("footmain"):
                if not vehicleStatus[vehicleID].parking:
                    if not edge in waiting:
                        waiting[edge] = []
                    waiting[edge].append(vehicleID)
                    vehicleStatus[vehicleID].parking = True
            if edge.startswith("cyber"):
                footEdge = edge.replace("cyber", "footmain")
                if not vehicleStatus[vehicleID].parking:
                    if vehicleStatus[vehicleID].load < CYBER_CAPACITY:                
                        if footEdge in waiting and waiting[footEdge]:
                            rerouteMsg += stopObject(edge, vehicleID, ROW_DIST-5.)
                            vehicleStatus[vehicleID].parking = True
                else:
                    if pos >= ROW_DIST - 10:
                        while waiting[footEdge] and vehicleStatus[vehicleID].load < CYBER_CAPACITY:
                            person = waiting[footEdge].pop(0)
                            rerouteMsg += stopObject(footEdge, person, ROW_DIST-10., 0.)
                            vehicleStatus[vehicleID].load += 1
                        rerouteMsg += stopObject(edge, vehicleID, ROW_DIST-5., 0.)
                        vehicleStatus[vehicleID].parking = False
            
        if rerouteMsg:
            sendExact(sock, rerouteMsg)
    time.sleep(1)

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-g", "--gui", action="store_true", dest="gui",
                     default=False, help="run with GUI")
(options, args) = optParser.parse_args()
main()