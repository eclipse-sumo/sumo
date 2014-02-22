# -*- coding: utf-8 -*-
"""
@file    vehicletype.py
@author  Michael Behrisch
@author  Lena Kalleske
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
import traci, struct
import traci.constants as tc

_RETURN_VALUE_FUNC = {tc.ID_LIST:             traci.Storage.readStringList,
                      tc.ID_COUNT:            traci.Storage.readInt,
                      tc.VAR_LENGTH:          traci.Storage.readDouble,
                      tc.VAR_MAXSPEED:        traci.Storage.readDouble,
                      tc.VAR_SPEED_FACTOR:    traci.Storage.readDouble,
                      tc.VAR_SPEED_DEVIATION: traci.Storage.readDouble,
                      tc.VAR_ACCEL:           traci.Storage.readDouble,
                      tc.VAR_DECEL:           traci.Storage.readDouble,
                      tc.VAR_IMPERFECTION:    traci.Storage.readDouble,
                      tc.VAR_TAU:             traci.Storage.readDouble,
                      tc.VAR_VEHICLECLASS:    traci.Storage.readString,
                      tc.VAR_EMISSIONCLASS:   traci.Storage.readString,
                      tc.VAR_SHAPECLASS:      traci.Storage.readString,
                      tc.VAR_MINGAP:          traci.Storage.readDouble,
                      tc.VAR_WIDTH:           traci.Storage.readDouble,
                      tc.VAR_COLOR:           lambda result: result.read("!BBBB")}
subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, typeID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_VEHICLETYPE_VARIABLE, varID, typeID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of ids of currently loaded vehicle types.
    """
    return _getUniversal(tc.ID_LIST, "")

def getIDCount():
    """getIDCount() -> integer
    
    Returns the number of currently loaded vehicle types.
    """
    return _getUniversal(tc.ID_COUNT, "")

def getLength(typeID):
    """getLength(string) -> double
    
    Returns the length in m of the vehicles of this type.
    """
    return _getUniversal(tc.VAR_LENGTH, typeID)

def getMaxSpeed(typeID):
    """getMaxSpeed(string) -> double
    
    Returns the maximum speed in m/s of vehicles of this type.
    """
    return _getUniversal(tc.VAR_MAXSPEED, typeID)

def getSpeedFactor(typeID):
    """getSpeedFactor(string) -> double
    
    .
    """
    return _getUniversal(tc.VAR_SPEED_FACTOR, typeID)

def getSpeedDeviation(typeID):
    """getSpeedDeviation(string) -> double
    
    Returns the maximum speed deviation of vehicles of this type.
    """
    return _getUniversal(tc.VAR_SPEED_DEVIATION, typeID)

def getAccel(typeID):
    """getAccel(string) -> double
    
    Returns the maximum acceleration in m/s^2 of vehicles of this type.
    """
    return _getUniversal(tc.VAR_ACCEL, typeID)

def getDecel(typeID):
    """getDecel(string) -> double
    
    Returns the maximum deceleration in m/s^2 of vehicles of this type.
    """
    return _getUniversal(tc.VAR_DECEL, typeID)

def getImperfection(typeID):
    """getImperfection(string) -> double
    
    .
    """
    return _getUniversal(tc.VAR_IMPERFECTION, typeID)

def getTau(typeID):
    """getTau(string) -> double
    
    Returns the driver's reaction time in s for vehicles of this type.
    """
    return _getUniversal(tc.VAR_TAU, typeID)

def getVehicleClass(typeID):
    """getVehicleClass(string) -> string
    
    Returns the class of vehicles of this type.
    """
    return _getUniversal(tc.VAR_VEHICLECLASS, typeID)

def getEmissionClass(typeID):
    """getEmissionClass(string) -> string
    
    Returns the emission class of vehicles of this type.
    """
    return _getUniversal(tc.VAR_EMISSIONCLASS, typeID)

def getShapeClass(typeID):
    """getShapeClass(string) -> string
    
    Returns the shape class of vehicles of this type. 
    """
    return _getUniversal(tc.VAR_SHAPECLASS, typeID)

def getMinGap(typeID):
    """getMinGap(string) -> double
    
    Returns the offset (gap to front vehicle if halting) of vehicles of this type.
    """
    return _getUniversal(tc.VAR_MINGAP, typeID)

def getWidth(typeID):
    """getWidth(string) -> double
    
    Returns the width in m of vehicles of this type.
    """
    return _getUniversal(tc.VAR_WIDTH, typeID)

def getColor(typeID):
    """getColor(string) -> (integer, integer, integer, integer)
    
    Returns the color of this type.
    """
    return _getUniversal(tc.VAR_COLOR, typeID)


def subscribe(typeID, varIDs=(tc.VAR_MAXSPEED,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more vehicle type values for the given interval.
    """
    traci._subscribe(tc.CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE, begin, end, typeID, varIDs)

def getSubscriptionResults(typeID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step and the given vehicle type.
    If no vehicle type id is given, all subscription results are returned in a dict.
    If the vehicle type id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(typeID)

def subscribeContext(typeID, domain, dist, varIDs=(tc.VAR_MAXSPEED,), begin=0, end=2**31-1):
    traci._subscribeContext(tc.CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT, begin, end, typeID, domain, dist, varIDs)

def getContextSubscriptionResults(typeID=None):
    return subscriptionResults.getContext(typeID)


def setLength(typeID, length):
    """setLength(string, double) -> None
    
    Sets the length in m of the vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_LENGTH, typeID, length)

def setMaxSpeed(typeID, speed):
    """setMaxSpeed(string, double) -> None
    
    Sets the maximum speed in m/s of vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_MAXSPEED, typeID, speed)

def setVehicleClass(typeID, clazz):
    """setVehicleClass(string, string) -> None
    
    Sets the class of vehicles of this type.
    """
    traci._sendStringCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_VEHICLECLASS, typeID, clazz)

def setSpeedFactor(typeID, factor):
    """setSpeedFactor(string, double) -> None
    
    .
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SPEED_FACTOR, typeID, factor)

def setSpeedDeviation(typeID, deviation):
    """setSpeedDeviation(string, double) -> None
    
    Sets the maximum speed deviation of vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SPEED_DEVIATION, typeID, deviation)

def setEmissionClass(typeID, clazz):
    """setEmissionClass(string, string) -> None
    
    Sets the emission class of vehicles of this type.
    """
    traci._sendStringCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_EMISSIONCLASS, typeID, clazz)

def setWidth(typeID, width):
    """setWidth(string, double) -> None
    
    Sets the width in m of vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_WIDTH, typeID, width)

def setMinGap(typeID, minGap):
    """setMinGap(string, double) -> None
    
    Sets the offset (gap to front vehicle if halting) of vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_MINGAP, typeID, minGap)

def setShapeClass(typeID, clazz):
    """setShapeClass(string, string) -> None
    
    Sets the shape class of vehicles of this type. 
    """
    traci._sendStringCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SHAPECLASS, typeID, clazz)

def setAccel(typeID, accel):
    """setAccel(string, double) -> None
    
    Sets the maximum acceleration in m/s^2 of vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_ACCEL, typeID, accel)

def setDecel(typeID, decel):
    """setDecel(string, double) -> None

    Sets the maximum deceleration in m/s^2 of vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_DECEL, typeID, decel)

def setImperfection(typeID, imperfection):
    """setImperfection(string, double) -> None
    
    .
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_IMPERFECTION, typeID, imperfection)

def setTau(typeID, tau):
    """setTau(string, double) -> None
    
    Sets the driver's reaction time in s for vehicles of this type.
    """
    traci._sendDoubleCmd(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_TAU, typeID, tau)

def setColor(typeID, color):
    """setColor(string, (integer, integer, integer, integer)) -> None
    
    Sets the color of this type.
    """
    traci._beginMessage(tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_COLOR, typeID, 1+1+1+1+1)
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._sendExact()
