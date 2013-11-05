# -*- coding: utf-8 -*-
"""
@file    lane.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-17
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import struct, traci
import traci.constants as tc

def _readLinks(result):
    result.read("!Bi") # Type Compound, Length
    nbLinks = result.readInt()
    links = []
    for i in range(nbLinks):
        result.read("!B")                           # Type String
        approachedLane = result.readString()
        result.read("!B")                           # Type String
        approachedInternal = result.readString()
        result.read("!B")                           # Type Byte
        hasPrio = bool(result.read("!B"))
        result.read("!B")                           # Type Byte
        isOpen = bool(result.read("!B"))
        result.read("!B")                           # Type Byte
        hasFoe = bool(result.read("!B"))
        result.read("!B")                           # Type String
        state = result.readString() 
        result.read("!B")                           # Type String
        direction = result.readString() 
        result.read("!B")                           # Type Float
        length = result.readDouble()
        links.append((approachedLane, hasPrio, isOpen, hasFoe))
    return links


_RETURN_VALUE_FUNC = {tc.ID_LIST:                   traci.Storage.readStringList,
                      tc.ID_COUNT:                  traci.Storage.readInt,
                      tc.VAR_LENGTH:                traci.Storage.readDouble,
                      tc.VAR_MAXSPEED:              traci.Storage.readDouble,
                      tc.VAR_WIDTH:                 traci.Storage.readDouble,
                      tc.LANE_ALLOWED:              traci.Storage.readStringList,
                      tc.LANE_DISALLOWED:           traci.Storage.readStringList,
                      tc.LANE_LINK_NUMBER:          lambda result: result.read("!B")[0],
                      tc.LANE_LINKS:                _readLinks,
                      tc.VAR_SHAPE:                 traci.Storage.readShape,
                      tc.LANE_EDGE_ID:              traci.Storage.readString,
                      tc.VAR_CO2EMISSION:           traci.Storage.readDouble,
                      tc.VAR_COEMISSION:            traci.Storage.readDouble,
                      tc.VAR_HCEMISSION:            traci.Storage.readDouble,
                      tc.VAR_PMXEMISSION:           traci.Storage.readDouble,
                      tc.VAR_NOXEMISSION:           traci.Storage.readDouble,
                      tc.VAR_FUELCONSUMPTION:       traci.Storage.readDouble,
                      tc.VAR_NOISEEMISSION:         traci.Storage.readDouble,
                      tc.LAST_STEP_MEAN_SPEED:      traci.Storage.readDouble,
                      tc.LAST_STEP_OCCUPANCY:       traci.Storage.readDouble,
                      tc.LAST_STEP_LENGTH:          traci.Storage.readDouble,
                      tc.VAR_WAITING_TIME:          traci.Storage.readDouble,
                      tc.VAR_CURRENT_TRAVELTIME:    traci.Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_NUMBER:  traci.Storage.readInt,
                      tc.LAST_STEP_VEHICLE_HALTING_NUMBER: traci.Storage.readInt,
                      tc.LAST_STEP_VEHICLE_ID_LIST: traci.Storage.readStringList}
subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, laneID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_LANE_VARIABLE, varID, laneID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of all lanes in the network.
    """
    return _getUniversal(tc.ID_LIST, "")

def getIDCount():
    """getIDCount() -> integer
    
    Returns the number of lanes in the network.
    """
    return _getUniversal(tc.ID_COUNT, "")
    
def getLength(laneID):
    """getLength(string) -> double
    
    Returns the length in m.
    """
    return _getUniversal(tc.VAR_LENGTH, laneID)

def getMaxSpeed(laneID):
    """getMaxSpeed(string) -> double
    
    Returns the maximum allowed speed on the lane in m/s.
    """
    return _getUniversal(tc.VAR_MAXSPEED, laneID)

def getWidth(laneID):
    """getWidth(string) -> double
    
    Returns the width of the lane in m.
    """
    return _getUniversal(tc.VAR_WIDTH, laneID)

def getAllowed(laneID):
    """getAllowed(string) -> list(string)
    
    Returns a list of allowed vehicle classes. An empty list means all vehicles are allowed.
    """
    return _getUniversal(tc.LANE_ALLOWED, laneID)

def getDisallowed(laneID):
    """getDisallowed(string) -> list(string)
    
    Returns a list of disallowed vehicle classes.
    """
    return _getUniversal(tc.LANE_DISALLOWED, laneID)

def getLinkNumber(laneID):
    """getLinkNumber(string) -> integer
    
    Returns the number of connections to successive lanes.
    """
    return _getUniversal(tc.LANE_LINK_NUMBER, laneID)

def getLinks(laneID):
    """getLinks(string) -> list((string, bool, bool, bool))
    
    A list containing ids of successor lanes together with priority, open and foe.
    """
    return _getUniversal(tc.LANE_LINKS, laneID)

def getShape(laneID):
    """getShape(string) -> list((double, double))
    
    List of 2D positions (cartesian) describing the geometry.
    """
    return _getUniversal(tc.VAR_SHAPE, laneID)

def getEdgeID(laneID):
    """getEdgeID(string) -> string
    
    Returns the id of the edge the lane belongs to.
    """
    return _getUniversal(tc.LANE_EDGE_ID, laneID)

def getCO2Emission(laneID):
    """getCO2Emission(string) -> double
    
    Returns the CO2 emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_CO2EMISSION, laneID)

def getCOEmission(laneID):
    """getCOEmission(string) -> double
    
    Returns the CO emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_COEMISSION, laneID)

def getHCEmission(laneID):
    """getHCEmission(string) -> double
    
    Returns the HC emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_HCEMISSION, laneID)

def getPMxEmission(laneID):
    """getPMxEmission(string) -> double
    
    Returns the particular matter emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_PMXEMISSION, laneID)

def getNOxEmission(laneID):
    """getNOxEmission(string) -> double
    
    Returns the NOx emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_NOXEMISSION, laneID)

def getFuelConsumption(laneID):
    """getFuelConsumption(string) -> double
    
    Returns the fuel consumption in ml for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_FUELCONSUMPTION, laneID)

def getNoiseEmission(laneID):
    """getNoiseEmission(string) -> double
    
    Returns the noise emission in db for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_NOISEEMISSION, laneID)

def getLastStepMeanSpeed(laneID):
    """getLastStepMeanSpeed(string) -> double
    
    Returns the average speed in m/s for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, laneID)

def getLastStepOccupancy(laneID):
    """getLastStepOccupancy(string) -> double
    
    Returns the occupancy in % for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_OCCUPANCY, laneID)

def getLastStepLength(laneID):
    """getLastStepLength(string) -> double
    
    Returns the mean vehicle length in m for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_LENGTH, laneID)

def getWaitingTime(laneID):
    """getWaitingTime() -> double
    
    .
    """
    return _getUniversal(tc.VAR_WAITING_TIME, laneID)     
    
def getTraveltime(laneID):
    """getTraveltime(string) -> double
    
    Returns the estimated travel time in s for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_CURRENT_TRAVELTIME, laneID)

def getLastStepVehicleNumber(laneID):
    """getLastStepVehicleNumber(string) -> integer
    
    Returns the total number of vehicles for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, laneID)

def getLastStepHaltingNumber(laneID):
    """getLastStepHaltingNumber(string) -> integer
    
    Returns the total number of halting vehicles for the last time step on the given lane.
    A speed of less than 0.1 m/s is considered a halt.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, laneID)

def getLastStepVehicleIDs(laneID):
    """getLastStepVehicleIDs(string) -> list(string)
    
    Returns the ids of the vehicles for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, laneID)


def subscribe(laneID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more lane values for the given interval.
    """
    traci._subscribe(tc.CMD_SUBSCRIBE_LANE_VARIABLE, begin, end, laneID, varIDs)

def getSubscriptionResults(laneID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step and the given lane.
    If no lane id is given, all subscription results are returned in a dict.
    If the lane id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(laneID)

def subscribeContext(laneID, domain, dist, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    traci._subscribeContext(tc.CMD_SUBSCRIBE_LANE_CONTEXT, begin, end, laneID, domain, dist, varIDs)

def getContextSubscriptionResults(laneID=None):
    return subscriptionResults.getContext(laneID)


def setAllowed(laneID, allowedClasses):
    """setAllowed(string, list) -> None
    
    Sets a list of allowed vehicle classes. Setting an empty list means all vehicles are allowed.
    """
    traci._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_ALLOWED, laneID, 1+4+sum(map(len, allowedClasses))+4*len(allowedClasses))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(allowedClasses))
    for c in allowedClasses:
        traci._message.string += struct.pack("!i", len(c)) + c
    traci._sendExact()

def setDisallowed(laneID, disallowedClasses):
    """setDisallowed(string, list) -> None
    
    Sets a list of disallowed vehicle classes.
    """
    traci._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_DISALLOWED, laneID, 1+4+sum(map(len, disallowedClasses))+4*len(disallowedClasses))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(disallowedClasses))
    for c in disallowedClasses:
        traci._message.string += struct.pack("!i", len(c)) + c
    traci._sendExact()

def setMaxSpeed(laneID, speed):
    """setMaxSpeed(string, double) -> None
    
    Sets a new maximum allowed speed on the lane in m/s.
    """
    traci._sendDoubleCmd(tc.CMD_SET_LANE_VARIABLE, tc.VAR_MAXSPEED, laneID, speed)

def setLength(laneID, length):
    """setLength(string, double) -> None
    
    Sets the length of the lane in m.
    """
    traci._sendDoubleCmd(tc.CMD_SET_LANE_VARIABLE, tc.VAR_LENGTH, laneID, length)
