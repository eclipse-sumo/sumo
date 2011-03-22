# -*- coding: utf-8 -*-
"""
@file    polygon.py
@author  Michael.Behrisch@dlr.de
@date    2011-03-16
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct, traci
import traci.constants as tc

def readShape(result):
    length = result.read("!B")[0]
    return [result.read("!ff") for i in range(length)]


RETURN_VALUE_FUNC = {tc.ID_LIST:   traci.Storage.readStringList,
                     tc.VAR_TYPE:  traci.Storage.readString,
                     tc.VAR_SHAPE: readShape,
                     tc.VAR_COLOR: lambda(result): result.read("!BBBB")}
subscriptionResults = {}

def _getUniversal(varID, polygonID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_POLYGON_VARIABLE, varID, polygonID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getType(polygonID):
    return _getUniversal(tc.VAR_TYPE, polygonID)

def getShape(polygonID):
    return _getUniversal(tc.VAR_SHAPE, polygonID)

def getColor(polygonID):
    return _getUniversal(tc.VAR_COLOR, polygonID)


def subscribe(polygonID, varIDs=(tc.VAR_SHAPE,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_POLYGON_VARIABLE, begin, end, polygonID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(polygonID, varID, data):
    if polygonID not in subscriptionResults:
        subscriptionResults[polygonID] = {}
    subscriptionResults[polygonID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(polygonID=None):
    if polygonID == None:
        return subscriptionResults
    return subscriptionResults.get(polygonID, None)


def setType(polygonID, polygonType):
    traci._beginMessage(tc.CMD_SET_POLYGON_VARIABLE, tc.VAR_TYPE, polygonID, 1+4+len(polygonType))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(polygonType)) + polygonType
    traci._sendExact()

def setShape(polygonID, shape):
    traci._beginMessage(tc.CMD_SET_POLYGON_VARIABLE, tc.VAR_SHAPE, polygonID, 1+1+8*len(shape))
    traci._message.string += struct.pack("!BB", tc.TYPE_POLYGON, len(shape))
    for p in shape:
        traci._message.string += struct.pack("!ff", p)
    traci._sendExact()

def setColor(polygonID, color):
    traci._beginMessage(tc.CMD_SET_POLYGON_VARIABLE, tc.VAR_COLOR, polygonID, 1+1+1+1+1)
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._sendExact()

def add(polygonID, shape, color, fill=False, polygonType="", layer=0):
    traci._beginMessage(tc.CMD_SET_POLYGON_VARIABLE, tc.ADD, polygonID, 1+4 + 1+4+len(polygonType) + 1+1+1+1+1 + 1+1 + 1+4 + 1+1+len(shape)*(4+4))
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 5)
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(polygonType)) + polygonType
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._message.string += struct.pack("!BB", tc.TYPE_UBYTE, int(fill))
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
    traci._message.string += struct.pack("!BB", tc.TYPE_POLYGON, len(shape))
    for p in shape:
        traci._message.string += struct.pack("!ff", *p)
    traci._sendExact()

def remove(polygonID, layer=0):
    traci._beginMessage(tc.CMD_SET_POLYGON_VARIABLE, tc.REMOVE, polygonID, 1+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
    traci._sendExact()
