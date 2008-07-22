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

def reroute(occupancy, vehicleID):
    slotEdge = ""
    for rowIdx, row in enumerate(occupancy):
        for idx, slot in enumerate(row):
            if slot == -1:
                row[idx] = vehicleID
                dir = "l"
                if rowIdx % 2 == 0:
                    dir = "r"
                slotEdge = "row%sslot%s%sp" % ((rowIdx+1)/2, idx, dir)
                break
        if slotEdge:
            break
    print slotEdge
    command = struct.pack("!BBii", 1+1+4+4+len(slotEdge), CMD_CHANGETARGET, vehicleID, len(slotEdge)) + slotEdge
    command += struct.pack("!BBiBi", 1+1+4+1+4+len(slotEdge)+4+1+4+8, CMD_STOP, vehicleID, POSITION_ROADMAP, len(slotEdge)) + slotEdge
    command += struct.pack("!fBfd", 1., 0, 1., 10000.)
    return command

def main():
    sumoProcess = subprocess.Popen("%s -c %s.sumo.cfg" % (SUMO, PREFIX), shell=True)
    sock = socket.socket()
    for wait in range(10):
        try:
            sock.connect(("localhost", PORT))
	    break
	except socket.error:
            time.sleep(wait)
    occupancy = -numpy.ones((2*DOUBLE_ROWS, SLOTS_PER_ROW), int)
    vehiclePos = {}
    
    for step in range(1, 2000):
        print "step", step
        command = struct.pack("!BBdB", 1+1+8+1, CMD_SIMSTEP, float(step), POSITION_ROADMAP)
        result = sendExact(sock, command)
        rerouteMsg = ""
        while result.ready():
            info = result.read("!BBidB")
            vehicleID = info[2]
#            print info
            edge = result.readString()
            if not vehicleID in vehiclePos:
                rerouteMsg += reroute(occupancy, vehicleID)
            vehiclePos[vehicleID] = edge
            print edge, result.read("!fB")
        if rerouteMsg:
            sendExact(sock, rerouteMsg)
    time.sleep(1)

main()