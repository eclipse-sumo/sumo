# -*- coding: utf-8 -*-
"""
@file    inductionloop.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-16
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import traci
import traci.constants as tc

def readVehicleData(result):
    result.readLength()
    nbData = result.readInt()
    data = []
    for i in range(nbData):
        result.read("!B")
        vehID = result.readString()
        result.read("!B")
        length = result.readDouble()
        result.read("!B")
        entryTime = result.readDouble()
        result.read("!B")
        leaveTime = result.readDouble()
        result.read("!B")
        typeID = result.readString()
        data.append( [ vehID, length, entryTime, leaveTime, typeID ] ) 
    return data

_RETURN_VALUE_FUNC = {tc.ID_LIST:                       traci.Storage.readStringList,
                     tc.ID_COUNT:                       traci.Storage.readInt,
                     tc.VAR_POSITION:                   traci.Storage.readDouble,
                     tc.VAR_LANE_ID:                    traci.Storage.readString,
                     tc.LAST_STEP_VEHICLE_NUMBER:       traci.Storage.readInt,
                     tc.LAST_STEP_MEAN_SPEED:           traci.Storage.readDouble,
                     tc.LAST_STEP_VEHICLE_ID_LIST:      traci.Storage.readStringList,
                     tc.LAST_STEP_OCCUPANCY:            traci.Storage.readDouble,
                     tc.LAST_STEP_LENGTH:               traci.Storage.readDouble,
                     tc.LAST_STEP_TIME_SINCE_DETECTION: traci.Storage.readDouble,
                     tc.LAST_STEP_VEHICLE_DATA:         readVehicleData}
subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, loopID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, varID, loopID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of all induction loops in the network.
    """
    return _getUniversal(tc.ID_LIST, "")

def getIDCount():
    """getIDCount() -> integer
    
    Returns the number of induction loops in the network.
    """
    return _getUniversal(tc.ID_COUNT, "")

def getPosition(loopID):
    """getPosition(string) -> double
    
    Returns the position measured from the beginning of the lane in meters.
    """
    return _getUniversal(tc.VAR_POSITION, loopID)

def getLaneID(loopID):
    """getLaneID(string) -> string
    
    Returns the id of the lane the loop is on.
    """
    return _getUniversal(tc.VAR_LANE_ID, loopID)

def getLastStepVehicleNumber(loopID):
    """getLastStepVehicleNumber(string) -> integer
    
    Returns the number of vehicles that were on the named induction loop within the last simulation step.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, loopID)

def getLastStepMeanSpeed(loopID):
    """getLastStepMeanSpeed(string) -> double
    
    Returns the mean speed in m/s of vehicles that were on the named induction loop within the last simulation step.
    """
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, loopID)

def getLastStepVehicleIDs(loopID):
    """getLastStepVehicleIDs(string) -> list(string)
    
    Returns the list of ids of vehicles that were on the named induction loop in the last simulation step.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, loopID)

def getLastStepOccupancy(loopID):
    """getLastStepOccupancy(string) -> double
    
    Returns the percentage of time the detector was occupied by a vehicle.
    """
    return _getUniversal(tc.LAST_STEP_OCCUPANCY, loopID)

def getLastStepMeanLength(loopID):
    """getLastStepMeanLength(string) -> double
    
    Returns the mean length in m of vehicles which were on the detector in the last step.
    """
    return _getUniversal(tc.LAST_STEP_LENGTH, loopID)

def getTimeSinceDetection(loopID):
    """getTimeSinceDetection(string) -> double
    
    Returns the time in s since last detection.
    """
    return _getUniversal(tc.LAST_STEP_TIME_SINCE_DETECTION, loopID)

def getVehicleData(loopID):
    """getVehicleData(string) -> integer
    
    Returns a complex structure containing several information about vehicles which passed the detector.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_DATA, loopID)


def subscribe(loopID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more induction loop values for the given interval.
    """
    traci._subscribe(tc.CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE, begin, end, loopID, varIDs)

def getSubscriptionResults(loopID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step and the given loop.
    If no loop id is given, all subscription results are returned in a dict.
    If the loop id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(loopID)

def subscribeContext(loopID, domain, dist, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    traci._subscribeContext(tc.CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT, begin, end, loopID, domain, dist, varIDs)

def getContextSubscriptionResults(loopID=None):
    return subscriptionResults.getContext(loopID)
