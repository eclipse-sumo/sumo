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
import traci, struct
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:             traci.Storage.readStringList,
                     tc.VAR_LENGTH:          traci.Storage.readFloat,
                     tc.VAR_MAXSPEED:        traci.Storage.readFloat,
                     tc.VAR_SPEED_FACTOR:    traci.Storage.readFloat,
                     tc.VAR_SPEED_DEVIATION: traci.Storage.readFloat,
                     tc.VAR_ACCEL:           traci.Storage.readFloat,
                     tc.VAR_DECEL:           traci.Storage.readFloat,
                     tc.VAR_IMPERFECTION:    traci.Storage.readFloat,
                     tc.VAR_TAU:             traci.Storage.readFloat,
                     tc.VAR_VEHICLECLASS:    traci.Storage.readString,
                     tc.VAR_EMISSIONCLASS:   traci.Storage.readString,
                     tc.VAR_SHAPECLASS:      traci.Storage.readString,
                     tc.VAR_GUIOFFSET:       traci.Storage.readFloat,
                     tc.VAR_WIDTH:           traci.Storage.readFloat,
                     tc.VAR_COLOR:           lambda(result): result.read("!BBBB")}
subscriptionResults = {}

def _getUniversal(varID, typeID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_VEHICLETYPE_VARIABLE, varID, typeID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getLength(typeID):
    return _getUniversal(tc.VAR_LENGTH, typeID)

def getMaxSpeed(typeID):
    return _getUniversal(tc.VAR_MAXSPEED, typeID)

def getSpeedFactor(typeID):
    return _getUniversal(tc.VAR_SPEED_FACTOR, typeID)

def getSpeedDeviation(typeID):
    return _getUniversal(tc.VAR_SPEED_DEVIATION, typeID)

def getAccel(typeID):
    return _getUniversal(tc.VAR_ACCEL, typeID)

def getDecel(typeID):
    return _getUniversal(tc.VAR_DECEL, typeID)

def getImperfection(typeID):
    return _getUniversal(tc.VAR_IMPERFECTION, typeID)

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


def setLength(typeID, length):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_LENGTH, typeID, length)

def setMaxSpeed(typeID, speed):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_MAXSPEED, typeID, speed)

def setVehicleClass(typeID, clazz):
    traci._sendStringCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_VEHICLECLASS, typeID, clazz)

def setSpeedFactor(typeID, factor):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SPEED_FACTOR, typeID, factor)

def setSpeedDeviation(typeID, deviation):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SPEED_DEVIATION, typeID, deviation)

def setEmissionClass(typeID, clazz):
    traci._sendStringCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_EMISSIONCLASS, typeID, clazz)

def setWidth(typeID, width):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_WIDTH, typeID, width)

def setGUIOffset(typeID, offset):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_GUIOFFSET, typeID, offset)

def setShapeClass(typeID, clazz):
    traci._sendStringCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SHAPECLASS, typeID, clazz)

def setAccel(typeID, accel):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_ACCEL, typeID, accel)

def setDecel(typeID, decel):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_DECEL, typeID, decel)

def setImperfection(typeID, imperfection):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_IMPERFECTION, typeID, imperfection)

def setTau(typeID, tau):
    traci._sendFloatCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_TAU, typeID, tau)

def setColor(typeID, color):
    traci._beginMessage(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_COLOR, typeID, 1+1+1+1+1)
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._sendExact()
