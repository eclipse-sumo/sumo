"""
@file    traciControl.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-21
@version $Id$

Capsule for the TraCI interface.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import socket, time, struct

CMD_SIMSTEP = 0x01
CMD_STOP = 0x12
CMD_CHANGETARGET = 0x31
CMD_MOVENODE = 0x80

POSITION_ROADMAP = 0x04

RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}

SOCKET = socket.socket()

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


def _recvExact():
    result = ""
    while len(result) < 4:
        result += SOCKET.recv(4 - len(result))
    length = struct.unpack("!i", result)[0] - 4
    result = ""
    while len(result) < length:
        result += SOCKET.recv(length - len(result))
    return Storage(result)

def _sendExact(message):
    length = struct.pack("!i", len(message)+4)
    SOCKET.send(length)
    SOCKET.send(message)
    result = _recvExact()
    prefix = result.read("!BBB")
    err = result.readString()
    if prefix[2] or err:
        print prefix, RESULTS[prefix[2]], err
    return result

def initTraCI(port):
    for wait in range(10):
        try:
            SOCKET.connect(("localhost", port))
            break
        except socket.error:
            time.sleep(wait)

def simStep(step, message=""):
    message += struct.pack("!BBdB", 1+1+8+1, CMD_SIMSTEP, float(step), POSITION_ROADMAP)
    return _sendExact(message)

def stopObject(edge, objectID, pos=1., duration=10000.):
    command = struct.pack("!BBiBi", 1+1+4+1+4+len(edge)+4+1+4+8, CMD_STOP, objectID, POSITION_ROADMAP, len(edge)) + edge
    command += struct.pack("!fBfd", pos, 0, 1., duration)
    return command

def changeTarget(edge, objectID):
    return struct.pack("!BBii", 1+1+4+4+len(edge), CMD_CHANGETARGET, objectID, len(edge)) + edge
