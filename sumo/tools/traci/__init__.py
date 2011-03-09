# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import socket, time, struct

RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}

class FatalTraCIError:
    def __init__(self, desc):
        self._desc = desc

class Message:
    string = ""
    queue = []

class Storage:
    def __init__(self, content):
        self._content = content
        self._pos = 0

    def read(self, format):
        oldPos = self._pos
        self._pos += struct.calcsize(format)
        return struct.unpack(format, self._content[oldPos:self._pos])

    def readInt(self):
        return self.read("!i")[0]

    def readFloat(self):
        return self.read("!f")[0]

    def readDouble(self):
        return self.read("!d")[0]

    def readLength(self):
        length = self.read("!B")[0]
        if length > 0:
            return length
        return self.read("!i")[0]

    def readString(self):
        length = self.read("!i")[0]
        return self.read("!%ss" % length)[0]

    def readStringList(self):
        n = self.read("!i")[0]
        list = []
        for i in range(n):
            list.append(self.readString())
        return list

    def ready(self):
        return self._pos < len(self._content) 


import constants, vehicle, simulation, route
_socket = None
_message = Message()

def _recvExact():
    global _socket
    try:
        result = ""
        while len(result) < 4:
            t = _socket.recv(4 - len(result))
            if not t:
                return None
            result += t
        length = struct.unpack("!i", result)[0] - 4
        result = ""
        while len(result) < length:
            t = _socket.recv(length - len(result))
            if not t:
                return None
            result += t
        return Storage(result)
    except socket.error:
        return None

def _sendExact():
    global _socket
    length = struct.pack("!i", len(_message.string)+4)
    _socket.send(length)
    _socket.send(_message.string)
    _message.string = ""
    result = _recvExact()
    if not result:
        _socket.close()
        _socket = None
        raise FatalTraCIError("connection closed by SUMO")
    for command in _message.queue:
        prefix = result.read("!BBB")
        err = result.readString()
        if prefix[2] or err:
            print prefix, RESULTS[prefix[2]], err
        elif prefix[1] != command:
            print "Error! Received answer %s for command %s." % (prefix[1], command)
        elif prefix[1] == constants.CMD_STOP:
            length = result.read("!B")[0] - 1
            result.read("!%sx" % length)
    _message.queue = []
    return result

def _beginMessage(cmdID, varID, objID, length=0):
    _message.queue.append(cmdID)
    length += 1+1+1+4+len(objID)
    if length<=255:
        _message.string += struct.pack("!BBBi", length, cmdID, varID, len(objID)) + objID
    else:
        _message.string += struct.pack("!BiBBi", 0, length+4, cmdID, varID, len(objID)) + objID

def _sendReadOneStringCmd(cmdID, varID, objID):
    _beginMessage(cmdID, varID, objID)
    result = _sendExact()
    result.readLength()
    response, retVarID = result.read("!BB")
    objectID = result.readString()
    if response - cmdID != 16 or retVarID != varID or objectID != objID:
        print "Error! Received answer %s,%s,%s for command %s,%s,%s."\
              % (response, retVarID, objectID, cmdID, varID, objID)
    result.read("!B")     # Return type of the variable
    return result

def _readSubscription(result):
    result.readLength()
    response = result.read("!B")[0]
    objectID = result.readString()
    numVars = result.read("!B")[0]
    while numVars > 0:
        varID = result.read("!B")[0]
        status, varType = result.read("!BB")
        if status:
            print "Error!", result.readString()
        elif response == constants.RESPONSE_SUBSCRIBE_VEHICLE_VARIABLE:
            vehicle._addSubscriptionResult(objectID, varID, result)
        elif response == constants.RESPONSE_SUBSCRIBE_SIM_VARIABLE:
            simulation._addSubscriptionResult(varID, result)
        elif response == constants.RESPONSE_SUBSCRIBE_ROUTE_VARIABLE:
            route._addSubscriptionResult(varID, result)
        numVars -= 1
    return response, objectID

def _subscribe(cmdID, begin, end, objID, varIDs):
    _message.queue.append(cmdID)
    length = 1+1+4+4+4+len(objID)+1+len(varIDs)
    if length<=255:
        _message.string += struct.pack("!B", length)
    else:
        _message.string += struct.pack("!Bi", 0, length+4)
    _message.string += struct.pack("!Biii", cmdID, begin, end, len(objID)) + objID
    _message.string += struct.pack("!B", len(varIDs))
    for v in varIDs:
        _message.string += struct.pack("!B", v)
    result = _sendExact()
    response, objectID = _readSubscription(result)
    if response - cmdID != 16 or objectID != objID:
        print "Error! Received answer %s,%s for subscription command %s,%s."\
              % (response, objectID, cmdID, objID)

def init(port, numRetries=10):
    global _socket
    _socket = socket.socket()
    for wait in range(numRetries):
        try:
            _socket.connect(("localhost", port))
            _socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            break
        except socket.error:
            time.sleep(wait)

def simulationStep(step):
    """
    Make simulation step and simulate up to "step" second in sim time.
    """
    _message.queue.append(constants.CMD_SIMSTEP2)
    _message.string += struct.pack("!BBi", 1+1+4, constants.CMD_SIMSTEP2, step)
    result = _sendExact()
    vehicle._resetSubscriptionResults()
    simulation._resetSubscriptionResults()
    route._resetSubscriptionResults()
    numSubs = result.readInt()
    while numSubs > 0:
        _readSubscription(result)
        numSubs -= 1

def close():
    global _socket
    if _socket:
        _message.queue.append(constants.CMD_CLOSE)
        _message.string += struct.pack("!BB", 1+1, constants.CMD_CLOSE)
        _sendExact()
        _socket.close()
        _socket = None
