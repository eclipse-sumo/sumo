# -*- coding: utf-8 -*-
"""
@file    __init__.py
@author  Michael Behrisch
@author  Lena Kalleske
@author  Mario Krumnow
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
import socket, time, struct
try:
    import traciemb
    _embedded = True
except ImportError:
    _embedded = False

_RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}
_DEBUG = False

def isEmbedded():
    return _embedded

def _STEPS2TIME(step):
    """Conversion from time steps in milliseconds to seconds as float"""
    return step/1000.

def _TIME2STEPS(time):
    """Conversion from (float) time in seconds to milliseconds as int"""
    return int(time*1000)

class TraCIException(Exception):
    """Exception class for all TraCI errors which keep the connection intact"""
    def __init__(self, command, errorType, desc):
        Exception.__init__(self, desc)
        self._command = command
        self._type = errorType
    
    def getCommand(self):
        return self._command

    def getType(self):
        return self._type

class FatalTraCIError(Exception):
    """Exception class for all TraCI errors which do not allow for continuation"""
    def __init__(self, desc):
        Exception.__init__(self, desc)

class Message:
    """ A named tuple for internal usage.
    
    Simple "struct" for the composed message string
    together with a list of TraCI commands which are inside.
    """
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

    def readShape(self):
        length = self.read("!B")[0]
        return [self.read("!dd") for i in range(length)]

    def ready(self):
        return self._pos < len(self._content) 

    def printDebug(self):
        if _DEBUG:
            for char in self._content[self._pos:]:
                print("%03i %02x %s" % (ord(char), ord(char), char))

class SubscriptionResults:
    def __init__(self, valueFunc):
        self._results = {}
        self._contextResults = {}
        self._valueFunc = valueFunc

    def _parse(self, varID, data):
        if not varID in self._valueFunc:
            raise FatalTraCIError("Unknown variable %02x." % varID)
        return self._valueFunc[varID](data)

    def reset(self):
        self._results.clear()
        self._contextResults.clear()

    def add(self, refID, varID, data):
        if refID not in self._results:
            self._results[refID] = {}
        self._results[refID][varID] = self._parse(varID, data)

    def get(self, refID=None):
        if refID == None:
            return self._results
        return self._results.get(refID, None)

    def addContext(self, refID, domain, objID, varID=None, data=None):
        if refID not in self._contextResults:
            self._contextResults[refID] = {}
        if objID not in self._contextResults[refID]:
            self._contextResults[refID][objID] = {}
        if varID != None and data != None:
            self._contextResults[refID][objID][varID] = domain._parse(varID, data)
        
    def getContext(self, refID=None):
        if refID == None:
            return self._contextResults
        return self._contextResults.get(refID, None)
    
    def __repr__(self):
        return "<%s, %s>" % (self._results, self._contextResults)


from . import constants
from . import inductionloop, multientryexit, trafficlights
from . import lane, vehicle, vehicletype, route, areal
from . import poi, polygon, junction, edge, simulation, gui

_modules = {constants.RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE: inductionloop,
            constants.RESPONSE_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE:\
            multientryexit,
            constants.RESPONSE_SUBSCRIBE_AREAL_DETECTOR_VARIABLE: areal,
            constants.RESPONSE_SUBSCRIBE_TL_VARIABLE: trafficlights,
            constants.RESPONSE_SUBSCRIBE_LANE_VARIABLE: lane,
            constants.RESPONSE_SUBSCRIBE_VEHICLE_VARIABLE: vehicle,
            constants.RESPONSE_SUBSCRIBE_VEHICLETYPE_VARIABLE: vehicletype,
            constants.RESPONSE_SUBSCRIBE_ROUTE_VARIABLE: route,
            constants.RESPONSE_SUBSCRIBE_POI_VARIABLE: poi,
            constants.RESPONSE_SUBSCRIBE_POLYGON_VARIABLE: polygon,
            constants.RESPONSE_SUBSCRIBE_JUNCTION_VARIABLE: junction,
            constants.RESPONSE_SUBSCRIBE_EDGE_VARIABLE: edge,
            constants.RESPONSE_SUBSCRIBE_SIM_VARIABLE: simulation,
            constants.RESPONSE_SUBSCRIBE_GUI_VARIABLE: gui,
            
            constants.RESPONSE_SUBSCRIBE_INDUCTIONLOOP_CONTEXT: inductionloop,
            constants.RESPONSE_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_CONTEXT:\
            multientryexit,
            constants.RESPONSE_SUBSCRIBE_AREAL_DETECTOR_CONTEXT: areal,
            constants.RESPONSE_SUBSCRIBE_TL_CONTEXT: trafficlights,
            constants.RESPONSE_SUBSCRIBE_LANE_CONTEXT: lane,
            constants.RESPONSE_SUBSCRIBE_VEHICLE_CONTEXT: vehicle,
            constants.RESPONSE_SUBSCRIBE_VEHICLETYPE_CONTEXT: vehicletype,
            constants.RESPONSE_SUBSCRIBE_ROUTE_CONTEXT: route,
            constants.RESPONSE_SUBSCRIBE_POI_CONTEXT: poi,
            constants.RESPONSE_SUBSCRIBE_POLYGON_CONTEXT: polygon,
            constants.RESPONSE_SUBSCRIBE_JUNCTION_CONTEXT: junction,
            constants.RESPONSE_SUBSCRIBE_EDGE_CONTEXT: edge,
            constants.RESPONSE_SUBSCRIBE_SIM_CONTEXT: simulation,
            constants.RESPONSE_SUBSCRIBE_GUI_CONTEXT: gui,
            
            constants.CMD_GET_INDUCTIONLOOP_VARIABLE: inductionloop,
            constants.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE:\
            multientryexit,
            constants.CMD_GET_AREAL_DETECTOR_VARIABLE: areal,
            constants.CMD_GET_TL_VARIABLE: trafficlights,
            constants.CMD_GET_LANE_VARIABLE: lane,
            constants.CMD_GET_VEHICLE_VARIABLE: vehicle,
            constants.CMD_GET_VEHICLETYPE_VARIABLE: vehicletype,
            constants.CMD_GET_ROUTE_VARIABLE: route,
            constants.CMD_GET_POI_VARIABLE: poi,
            constants.CMD_GET_POLYGON_VARIABLE: polygon,
            constants.CMD_GET_JUNCTION_VARIABLE: junction,
            constants.CMD_GET_EDGE_VARIABLE: edge,
            constants.CMD_GET_SIM_VARIABLE: simulation,
            constants.CMD_GET_GUI_VARIABLE: gui}
_connections = {}
_message = Message()

def _recvExact():
    try:
        result = ""
        while len(result) < 4:
            t = _connections[""].recv(4 - len(result))
            if not t:
                return None
            result += t
        length = struct.unpack("!i", result)[0] - 4
        result = ""
        while len(result) < length:
            t = _connections[""].recv(length - len(result))
            if not t:
                return None
            result += t
        return Storage(result)
    except socket.error:
        return None

def _sendExact():
    if _embedded:
        result = Storage(traciemb.execute(_message.string))
    else:
        length = struct.pack("!i", len(_message.string)+4)
        _connections[""].send(length + _message.string)
        result = _recvExact()
    if not result:
        _connections[""].close()
        del _connections[""]
        raise FatalTraCIError("connection closed by SUMO")
    for command in _message.queue:
        prefix = result.read("!BBB")
        err = result.readString()
        if prefix[2] or err:
            _message.string = ""
            _message.queue = []
            raise TraCIException(prefix[1], _RESULTS[prefix[2]], err)
        elif prefix[1] != command:
            raise FatalTraCIError("Received answer %s for command %s." % (prefix[1],
                                                                 command))
        elif prefix[1] == constants.CMD_STOP:
            length = result.read("!B")[0] - 1
            result.read("!%sx" % length)
    _message.string = ""
    _message.queue = []
    return result

def _beginMessage(cmdID, varID, objID, length=0):
    _message.queue.append(cmdID)
    length += 1+1+1+4+len(objID)
    if length<=255:
        _message.string += struct.pack("!BBBi", length,
                                       cmdID, varID, len(objID)) + objID
    else:
        _message.string += struct.pack("!BiBBi", 0, length+4,
                                       cmdID, varID, len(objID)) + objID

def _sendReadOneStringCmd(cmdID, varID, objID):
    _beginMessage(cmdID, varID, objID)
    return _checkResult(cmdID, varID, objID)

def _sendIntCmd(cmdID, varID, objID, value):
    _beginMessage(cmdID, varID, objID, 1+4)
    _message.string += struct.pack("!Bi", constants.TYPE_INTEGER, value)
    _sendExact()

def _sendDoubleCmd(cmdID, varID, objID, value):
    _beginMessage(cmdID, varID, objID, 1+8)
    _message.string += struct.pack("!Bd", constants.TYPE_DOUBLE, value)
    _sendExact()

def _sendByteCmd(cmdID, varID, objID, value):
    _beginMessage(cmdID, varID, objID, 1+1)
    _message.string += struct.pack("!BB", constants.TYPE_BYTE, value)
    _sendExact()

def _sendStringCmd(cmdID, varID, objID, value):
    _beginMessage(cmdID, varID, objID, 1+4+len(value))
    _message.string += struct.pack("!Bi", constants.TYPE_STRING,
                                   len(value)) + value
    _sendExact()

def _checkResult(cmdID, varID, objID):
    result = _sendExact()
    result.readLength()
    response, retVarID = result.read("!BB")
    objectID = result.readString()
    if response - cmdID != 16 or retVarID != varID or objectID != objID:
        raise FatalTraCIError("Received answer %s,%s,%s for command %s,%s,%s."\
              % (response, retVarID, objectID, cmdID, varID, objID))
    result.read("!B")     # Return type of the variable
    return result

def _readSubscription(result):
    result.printDebug() # to enable this you also need to set _DEBUG to True
    result.readLength()
    response = result.read("!B")[0]
    isVariableSubscription = response>=constants.RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE and response<=constants.RESPONSE_SUBSCRIBE_GUI_VARIABLE
    objectID = result.readString()
    if not isVariableSubscription:
        domain = result.read("!B")[0]
    numVars = result.read("!B")[0]
    if isVariableSubscription:
        while numVars > 0:
            varID = result.read("!B")[0]
            status, varType = result.read("!BB")
            if status:
                print("Error!", result.readString())
            elif response in _modules:
                _modules[response].subscriptionResults.add(objectID, varID, result)
            else:
                raise FatalTraCIError("Cannot handle subscription response %02x for %s." % (response, objectID))
            numVars -= 1
    else:
        objectNo = result.read("!i")[0]
        for o in range(objectNo):
            oid = result.readString()
            if numVars == 0:
                _modules[response].subscriptionResults.addContext(objectID, _modules[domain].subscriptionResults, oid)
            for v in range(numVars):
                varID = result.read("!B")[0]
                status, varType = result.read("!BB")
                if status:
                    print("Error!", result.readString())
                elif response in _modules:
                    _modules[response].subscriptionResults.addContext(objectID, _modules[domain].subscriptionResults, oid, varID, result)
                else:
                    raise FatalTraCIError("Cannot handle subscription response %02x for %s." % (response, objectID))
    return objectID, response

def _subscribe(cmdID, begin, end, objID, varIDs, parameters=None):
    _message.queue.append(cmdID)
    length = 1+1+4+4+4+len(objID)+1+len(varIDs)
    if parameters:
        for v in varIDs:
            if v in parameters:
                length += len(parameters[v])
    if length <= 255:
        _message.string += struct.pack("!B", length)
    else:
        _message.string += struct.pack("!Bi", 0, length+4)
    _message.string += struct.pack("!Biii", cmdID, begin, end, len(objID)) + objID
    _message.string += struct.pack("!B", len(varIDs))
    for v in varIDs:
        _message.string += struct.pack("!B", v)
        if parameters and v in parameters:
            _message.string += parameters[v]
    result = _sendExact()
    objectID, response = _readSubscription(result)
    if response - cmdID != 16 or objectID != objID:
        raise FatalTraCIError("Received answer %02x,%s for subscription command %02x,%s." % (response, objectID, cmdID, objID))

def _subscribeContext(cmdID, begin, end, objID, domain, dist, varIDs):
    _message.queue.append(cmdID)
    length = 1+1+4+4+4+len(objID)+1+8+1+len(varIDs)
    if length<=255:
        _message.string += struct.pack("!B", length)
    else:
        _message.string += struct.pack("!Bi", 0, length+4)
    _message.string += struct.pack("!Biii", cmdID, begin, end, len(objID)) + objID
    _message.string += struct.pack("!BdB", domain, dist, len(varIDs))
    for v in varIDs:
        _message.string += struct.pack("!B", v)
    result = _sendExact()
    objectID, response = _readSubscription(result)
    if response - cmdID != 16 or objectID != objID:
        raise FatalTraCIError("Received answer %02x,%s for context subscription command %02x,%s." % (response, objectID, cmdID, objID))

def init(port=8813, numRetries=10, host="localhost", label="default"):
    if _embedded:
        return getVersion()
    _connections[""] = _connections[label] = socket.socket()
    for wait in range(numRetries):
        try:
            _connections[label].connect((host, port))
            _connections[label].setsockopt(socket.IPPROTO_TCP,
                                           socket.TCP_NODELAY, 1)
            break
        except socket.error:
            time.sleep(wait)
    return getVersion()

def simulationStep(step=0):
    """
    Make simulation step and simulate up to "step" second in sim time.
    """
    _message.queue.append(constants.CMD_SIMSTEP2)
    _message.string += struct.pack("!BBi", 1+1+4, constants.CMD_SIMSTEP2, step)
    result = _sendExact()
    for module in _modules.values():
        module.subscriptionResults.reset()
    numSubs = result.readInt()
    responses = []
    while numSubs > 0:
        responses.append(_readSubscription(result))
        numSubs -= 1
    return responses

def getVersion():
    command = constants.CMD_GETVERSION
    _message.queue.append(command)
    _message.string += struct.pack("!BB", 1+1, command)
    result = _sendExact()
    result.readLength()
    response = result.read("!B")[0]
    if response != command:
        raise FatalTraCIError("Received answer %s for command %s." % (response, command))
    return result.readInt(), result.readString()

def close():
    if "" in _connections:
        _message.queue.append(constants.CMD_CLOSE)
        _message.string += struct.pack("!BB", 1+1, constants.CMD_CLOSE)
        _sendExact()
        _connections[""].close()
        del _connections[""]

def switch(label):
    _connections[""] = _connections[label]
