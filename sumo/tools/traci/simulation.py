# -*- coding: utf-8 -*-
"""
@file    simulation.py
@author  Michael.Behrisch@dlr.de
@date    2011-03-15
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.VAR_TIME_STEP:                         traci.Storage.readInt,
                     tc.VAR_LOADED_VEHICLES_NUMBER:            traci.Storage.readInt,
                     tc.VAR_LOADED_VEHICLES_IDS:               traci.Storage.readStringList,
                     tc.VAR_DEPARTED_VEHICLES_NUMBER:          traci.Storage.readInt,
                     tc.VAR_DEPARTED_VEHICLES_IDS:             traci.Storage.readStringList,
                     tc.VAR_ARRIVED_VEHICLES_NUMBER:           traci.Storage.readInt,
                     tc.VAR_ARRIVED_VEHICLES_IDS:              traci.Storage.readStringList,
                     tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER: traci.Storage.readInt,
                     tc.VAR_TELEPORT_STARTING_VEHICLES_IDS:    traci.Storage.readStringList,
                     tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER:   traci.Storage.readInt,
                     tc.VAR_TELEPORT_ENDING_VEHICLES_IDS:      traci.Storage.readStringList,
                     tc.VAR_DELTA_T:                           traci.Storage.readInt,
                     tc.VAR_NET_BOUNDING_BOX:                  lambda(result): (result.read("!ff"), result.read("!ff"))}
subscriptionResults = {}

def _getUniversal(varID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_SIM_VARIABLE, varID, "x")
    return RETURN_VALUE_FUNC[varID](result)

def getCurrentTime():
    return _getUniversal(tc.VAR_TIME_STEP)

def getLoadedNumber():
    return _getUniversal(tc.VAR_LOADED_VEHICLES_NUMBER)

def getLoadedIDList():
    return _getUniversal(tc.VAR_LOADED_VEHICLES_IDS)

def getDepartedNumber():
    return _getUniversal(tc.VAR_DEPARTED_VEHICLES_NUMBER)

def getDepartedIDList():
    return _getUniversal(tc.VAR_DEPARTED_VEHICLES_IDS)

def getArrivedNumber():
    return _getUniversal(tc.VAR_ARRIVED_VEHICLES_NUMBER)

def getArrivedIDList():
    return _getUniversal(tc.VAR_ARRIVED_VEHICLES_IDS)

def getStartingTeleportNumber():
    return _getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER)

def getStartingTeleportIDList():
    return _getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_IDS)

def getEndingTeleportNumber():
    return _getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER)

def getEndingTeleportIDList():
    return _getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_IDS)

def getDeltaT():
    return _getUniversal(tc.VAR_DELTA_T)

def getNetBoundary():
    return _getUniversal(tc.VAR_NET_BOUNDING_BOX)


def subscribe(varIDs=(tc.VAR_DEPARTED_VEHICLES_IDS,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_SIM_VARIABLE, begin, end, "x", varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(objectID, varID, data):
    subscriptionResults[varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults():
    return subscriptionResults
