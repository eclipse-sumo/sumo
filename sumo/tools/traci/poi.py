# -*- coding: utf-8 -*-
"""
@file    poi.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct, traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:      traci.Storage.readStringList,
                     tc.VAR_TYPE:     traci.Storage.readString,
                     tc.VAR_POSITION: lambda(result): result.read("!ff"),
                     tc.VAR_COLOR:    lambda(result): result.read("!BBBB")}
subscriptionResults = {}

def _getUniversal(varID, poiID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_POI_VARIABLE, varID, poiID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getType(poiID):
    return _getUniversal(tc.VAR_TYPE, poiID)

def getPosition(poiID):
    return _getUniversal(tc.VAR_POSITION, poiID)

def getColor(poiID):
    return _getUniversal(tc.VAR_COLOR, poiID)


def subscribe(poiID, varIDs=(tc.VAR_POSITION,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_POI_VARIABLE, begin, end, poiID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(poiID, varID, data):
    if poiID not in subscriptionResults:
        subscriptionResults[poiID] = {}
    subscriptionResults[poiID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(poiID=None):
    if poiID == None:
        return subscriptionResults
    return subscriptionResults.get(poiID, None)


def setType(poiID, poiType):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.VAR_TYPE, poiID, 1+4+len(poiType))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(poiType)) + poiType
    traci._sendExact()

def setPosition(poiID, x, y):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.VAR_POSITION, poiID, 1+4+4)
    traci._message.string += struct.pack("!Bff", tc.POSITION_2D, x, y)
    traci._sendExact()

def setColor(poiID, color):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.VAR_COLOR, poiID, 1+1+1+1+1)
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._sendExact()

def add(poiID, x, y, color, poiType="", layer=0):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.ADD, poiID, 1+4 + 1+4+len(poiType) + 1+1+1+1+1 + 1+4 + 1+4+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(poiType)) + poiType
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
    traci._message.string += struct.pack("!Bff", tc.POSITION_2D, x, y)
    traci._sendExact()

def remove(poiID, layer=0):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.REMOVE, poiID, 1+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
    traci._sendExact()
