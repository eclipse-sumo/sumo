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

RETURN_VALUE_FUNC = {tc.ID_LIST:                        traci.Storage.readStringList,
                     tc.LAST_STEP_VEHICLE_NUMBER:       traci.Storage.readInt,
                     tc.LAST_STEP_MEAN_SPEED:           traci.Storage.readFloat,
                     tc.LAST_STEP_VEHICLE_ID_LIST:      traci.Storage.readStringList,
                     tc.LAST_STEP_OCCUPANCY:            traci.Storage.readFloat,
                     tc.LAST_STEP_LENGTH:               traci.Storage.readFloat,
                     tc.LAST_STEP_TIME_SINCE_DETECTION: traci.Storage.readFloat,
                     tc.LAST_STEP_VEHICLE_DATA:         readVehicleData}
subscriptionResults = {}

def _getUniversal(varID, loopID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, varID, loopID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getLastStepVehicleNumber(loopID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, loopID)

def getLastStepMeanSpeed(loopID):
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, loopID)

def getLastStepVehicleIDs(loopID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, loopID)

def getLastStepOccupancy(loopID):
    return _getUniversal(tc.LAST_STEP_OCCUPANCY, loopID)

def getLastStepMeanLength(loopID):
    return _getUniversal(tc.LAST_STEP_LENGTH, loopID)

def getTimeSinceDetection(loopID):
    return _getUniversal(tc.LAST_STEP_TIME_SINCE_DETECTION, loopID)

def getVehicleData(loopID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_DATA, loopID)


def subscribe(loopID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE, begin, end, loopID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(loopID, varID, data):
    if loopID not in subscriptionResults:
        subscriptionResults[loopID] = {}
    subscriptionResults[loopID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(loopID=None):
    if loopID == None:
        return subscriptionResults
    return subscriptionResults.get(loopID, None)
