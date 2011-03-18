# -*- coding: utf-8 -*-
"""
@file    lane.py
@author  Michael.Behrisch@dlr.de
@date    2011-03-17
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
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
        state = result.readString() #not implemented
        result.read("!B")                           # Type String
        direction = result.readString() #not implemented
        result.read("!B")                           # Type Float
        length = result.readFloat()
        links.append((approachedLane, hasPrio, isOpen, hasFoe))
    return links


RETURN_VALUE_FUNC = {tc.ID_LIST:                   traci.Storage.readStringList,
                     tc.VAR_LENGTH:                traci.Storage.readFloat,
                     tc.VAR_MAXSPEED:              traci.Storage.readFloat,
                     tc.LANE_ALLOWED:              traci.Storage.readStringList,
                     tc.LANE_DISALLOWED:           traci.Storage.readStringList,
                     tc.LANE_LINK_NUMBER:          lambda(result): result.read("!B")[0],
                     tc.LANE_LINKS:                _readLinks,
                     tc.VAR_SHAPE:                 traci.polygon.readShape,
                     tc.LANE_EDGE_ID:              traci.Storage.readString,
                     tc.VAR_CO2EMISSION:           traci.Storage.readFloat,
                     tc.VAR_COEMISSION:            traci.Storage.readFloat,
                     tc.VAR_HCEMISSION:            traci.Storage.readFloat,
                     tc.VAR_PMXEMISSION:           traci.Storage.readFloat,
                     tc.VAR_NOXEMISSION:           traci.Storage.readFloat,
                     tc.VAR_FUELCONSUMPTION:       traci.Storage.readFloat,
                     tc.VAR_NOISEEMISSION:         traci.Storage.readFloat,
                     tc.LAST_STEP_MEAN_SPEED:      traci.Storage.readFloat,
                     tc.LAST_STEP_OCCUPANCY:       traci.Storage.readFloat,
                     tc.LAST_STEP_LENGTH:          traci.Storage.readFloat,
                     tc.VAR_CURRENT_TRAVELTIME:    traci.Storage.readFloat,
                     tc.LAST_STEP_VEHICLE_NUMBER:  traci.Storage.readInt,
                     tc.LAST_STEP_VEHICLE_HALTING_NUMBER: traci.Storage.readInt,
                     tc.LAST_STEP_VEHICLE_ID_LIST: traci.Storage.readStringList}
subscriptionResults = {}

def _getUniversal(varID, laneID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_LANE_VARIABLE, varID, laneID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getLength(laneID):
    return _getUniversal(tc.VAR_LENGTH, laneID)

def getMaxSpeed(laneID):
    return _getUniversal(tc.VAR_MAXSPEED, laneID)

def getAllowed(laneID):
    return _getUniversal(tc.LANE_ALLOWED, laneID)

def getDisallowed(laneID):
    return _getUniversal(tc.LANE_DISALLOWED, laneID)

def getLinkNumber(laneID):
    return _getUniversal(tc.LANE_LINK_NUMBER, laneID)

def getLinks(laneID):
    return _getUniversal(tc.LANE_LINKS, laneID)

def getShape(laneID):
    return _getUniversal(tc.VAR_SHAPE, laneID)

def getEdgeID(laneID):
    return _getUniversal(tc.LANE_EDGE_ID, laneID)

def getCO2Emission(laneID):
    return _getUniversal(tc.VAR_CO2EMISSION, laneID)

def getCOEmission(laneID):
    return _getUniversal(tc.VAR_COEMISSION, laneID)

def getHCEmission(laneID):
    return _getUniversal(tc.VAR_HCEMISSION, laneID)

def getPMxEmission(laneID):
    return _getUniversal(tc.VAR_PMXEMISSION, laneID)

def getNOxEmission(laneID):
    return _getUniversal(tc.VAR_NOXEMISSION, laneID)

def getFuelConsumption(laneID):
    return _getUniversal(tc.VAR_FUELCONSUMPTION, laneID)

def getNoiseEmission(laneID):
    return _getUniversal(tc.VAR_NOISEEMISSION, laneID)

def getLastStepMeanSpeed(laneID):
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, laneID)

def getLastStepOccupancy(laneID):
    return _getUniversal(tc.LAST_STEP_OCCUPANCY, laneID)

def getLastStepLength(laneID):
    return _getUniversal(tc.LAST_STEP_LENGTH, laneID)

def getTraveltime(laneID):
    return _getUniversal(tc.VAR_CURRENT_TRAVELTIME, laneID)

def getLastStepVehicleNumber(laneID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, laneID)

def getLastStepHaltingNumber(laneID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, laneID)

def getLastStepVehicleIDs(laneID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, laneID)


def subscribe(laneID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_LANE_VARIABLE, begin, end, laneID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(laneID, varID, data):
    if laneID not in subscriptionResults:
        subscriptionResults[laneID] = {}
    subscriptionResults[laneID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(laneID=None):
    if laneID == None:
        return subscriptionResults
    return subscriptionResults.get(laneID, None)


def setAllowed(laneID, allowedClasses):
    traci._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_ALLOWED, laneID, 1+4+sum(map(len, allowedClasses))+4*len(allowedClasses))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(allowedClasses))
    for c in allowedClasses:
        traci._message.string += struct.pack("!i", len(c)) + c
    traci._sendExact()

def setDisallowed(laneID, disallowedClasses):
    traci._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_DISALLOWED, laneID, 1+4+sum(map(len, disallowedClasses))+4*len(disallowedClasses))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(disallowedClasses))
    for c in disallowedClasses:
        traci._message.string += struct.pack("!i", len(c)) + c
    traci._sendExact()

def setMaxSpeed(laneID, speed):
    traci._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.VAR_MAXSPEED, laneID, 1+4)
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, speed)
    traci._sendExact()

def setLength(laneID, length):
    traci._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.VAR_LENGTH, laneID, 1+4)
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, length)
    traci._sendExact()
