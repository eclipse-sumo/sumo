# -*- coding: utf-8 -*-
"""
@file    vehicletype.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct, traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:           traci.Storage.readStringList,
                     tc.VAR_LENGTH:        traci.Storage.readFloat,
                     tc.VAR_MAXSPEED:      traci.Storage.readFloat,
                     tc.VAR_ACCEL:         traci.Storage.readFloat,
                     tc.VAR_DECEL:         traci.Storage.readFloat,
                     tc.VAR_TAU:           traci.Storage.readFloat,
                     tc.VAR_VEHICLECLASS:  traci.Storage.readString,
                     tc.VAR_EMISSIONCLASS: traci.Storage.readString,
                     tc.VAR_SHAPECLASS:    traci.Storage.readString,
                     tc.VAR_GUIOFFSET:     traci.Storage.readFloat,
                     tc.VAR_WIDTH:         traci.Storage.readFloat,
                     tc.VAR_COLOR:         lambda(result): result.read("!BBBB")}
subscriptionResults = {}

def _getUniversal(varID, vehID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_VEHICLETYPE_VARIABLE, varID, vehID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getLength(typeID):
    return _getUniversal(tc.VAR_LENGTH, typeID)

def getMaxSpeed(typeID):
    return _getUniversal(tc.VAR_MAXSPEED, typeID)

def getAccel(typeID):
    return _getUniversal(tc.VAR_ACCEL, typeID)

def getDecel(typeID):
    return _getUniversal(tc.VAR_DECEL, typeID)

def getTau(typeID):
    return _getUniversal(tc.VAR_TAU, typeID)

def getVehicleClass(typeID):
    return _getUniversal(tc.VAR_VEHICLECLASS, typeID)

def getEmissionClass(typeID):
    return _getUniversal(tc.VAR_EMISSIONCLASS, typeID)

def getShapeClass(typeID):
    return _getUniversal(tc.VAR_SHAPECLASS, typeID)

def getGUIOffset(typeID):
    return _getUniversal(tc.VAR_GUIOFFSET, typeID)

def getWidth(typeID):
    return _getUniversal(tc.VAR_WIDTH, typeID)

def getColor(typeID):
    return _getUniversal(tc.VAR_COLOR, typeID)


def subscribe(typeID, varIDs=(tc.VAR_MAXSPEED,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE, begin, end, typeID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(typeID, varID, data):
    if typeID not in subscriptionResults:
        subscriptionResults[typeID] = {}
    subscriptionResults[typeID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(typeID=None):
    if typeID == None:
        return subscriptionResults
    return subscriptionResults.get(typeID, None)
