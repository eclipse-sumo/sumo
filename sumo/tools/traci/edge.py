# -*- coding: utf-8 -*-
"""
@file    edge.py
@author  Michael.Behrisch@dlr.de
@date    2011-03-17
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct, traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:                   traci.Storage.readStringList,
                     tc.VAR_EDGE_TRAVELTIME:       traci.Storage.readFloat,
                     tc.VAR_EDGE_EFFORT:           traci.Storage.readFloat,
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

def _getUniversal(varID, edgeID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_EDGE_VARIABLE, varID, edgeID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getAdaptedTraveltime(edgeID, time):
    traci._beginMessage(tc.CMD_GET_EDGE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, edgeID, 1+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, time)
    return traci._checkResult(tc.CMD_GET_EDGE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, edgeID).readFloat()

def getEffort(edgeID, time):
    traci._beginMessage(tc.CMD_GET_EDGE_VARIABLE, tc.VAR_EDGE_EFFORT, edgeID, 1+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, time)
    return traci._checkResult(tc.CMD_GET_EDGE_VARIABLE, tc.VAR_EDGE_EFFORT, edgeID).readFloat()

def getCO2Emission(edgeID):
    return _getUniversal(tc.VAR_CO2EMISSION, edgeID)

def getCOEmission(edgeID):
    return _getUniversal(tc.VAR_COEMISSION, edgeID)

def getHCEmission(edgeID):
    return _getUniversal(tc.VAR_HCEMISSION, edgeID)

def getPMxEmission(edgeID):
    return _getUniversal(tc.VAR_PMXEMISSION, edgeID)

def getNOxEmission(edgeID):
    return _getUniversal(tc.VAR_NOXEMISSION, edgeID)

def getFuelConsumption(edgeID):
    return _getUniversal(tc.VAR_FUELCONSUMPTION, edgeID)

def getNoiseEmission(edgeID):
    return _getUniversal(tc.VAR_NOISEEMISSION, edgeID)

def getLastStepMeanSpeed(edgeID):
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, edgeID)

def getLastStepOccupancy(edgeID):
    return _getUniversal(tc.LAST_STEP_OCCUPANCY, edgeID)

def getLastStepLength(edgeID):
    return _getUniversal(tc.LAST_STEP_LENGTH, edgeID)

def getTraveltime(edgeID):
    return _getUniversal(tc.VAR_CURRENT_TRAVELTIME, edgeID)

def getLastStepVehicleNumber(edgeID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, edgeID)

def getLastStepHaltingNumber(edgeID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, edgeID)

def getLastStepVehicleIDs(edgeID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, edgeID)


def subscribe(edgeID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_EDGE_VARIABLE, begin, end, edgeID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(edgeID, varID, data):
    if edgeID not in subscriptionResults:
        subscriptionResults[edgeID] = {}
    subscriptionResults[edgeID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(edgeID=None):
    if edgeID == None:
        return subscriptionResults
    return subscriptionResults.get(edgeID, None)


def adaptTraveltime(edgeID, time):
    traci._beginMessage(tc.CMD_SET_EDGE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, edgeID, 1+4)
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, time)
    traci._sendExact()

def setEffort(edgeID, effort):
    traci._beginMessage(tc.CMD_SET_EDGE_VARIABLE, tc.VAR_EDGE_EFFORT, edgeID, 1+4)
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, effort)
    traci._sendExact()

def setMaxSpeed(edgeID, speed):
    traci._beginMessage(tc.CMD_SET_EDGE_VARIABLE, tc.VAR_MAXSPEED, edgeID, 1+4)
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, speed)
    traci._sendExact()
