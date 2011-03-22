# -*- coding: utf-8 -*-
"""
@file    multientryexit.py
@author  Michael.Behrisch@dlr.de
@date    2011-03-16
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:                          traci.Storage.readStringList,
                     tc.LAST_STEP_VEHICLE_NUMBER:         traci.Storage.readInt,
                     tc.LAST_STEP_MEAN_SPEED:             traci.Storage.readFloat,
                     tc.LAST_STEP_VEHICLE_ID_LIST:        traci.Storage.readStringList,
                     tc.LAST_STEP_VEHICLE_HALTING_NUMBER: traci.Storage.readInt}
subscriptionResults = {}

def _getUniversal(varID, detID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, varID, detID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getLastStepVehicleNumber(detID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, detID)

def getLastStepMeanSpeed(detID):
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, detID)

def getLastStepVehicleIDs(detID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, detID)

def getLastStepHaltingNumber(detID):
    return _getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, detID)


def subscribe(detID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, begin, end, detID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(detID, varID, data):
    if detID not in subscriptionResults:
        subscriptionResults[detID] = {}
    subscriptionResults[detID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(detID=None):
    if detID == None:
        return subscriptionResults
    return subscriptionResults.get(detID, None)
