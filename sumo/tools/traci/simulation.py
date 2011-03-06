# -*- coding: utf-8 -*-
"""
@file    simulation.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2008-10-09
@version $Id: traciControl.py 9525 2011-01-04 21:22:52Z behrisch $

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.VAR_TIME_STEP:                      traci.Storage.readInt,
                     tc.VAR_DEPARTED_VEHICLES_IDS:          traci.Storage.readStringList,
                     tc.VAR_ARRIVED_VEHICLES_IDS:           traci.Storage.readStringList,
                     tc.VAR_TELEPORT_STARTING_VEHICLES_IDS: traci.Storage.readStringList,
                     tc.VAR_TELEPORT_ENDING_VEHICLES_IDS:   traci.Storage.readStringList}
subscriptionResults = {}

def _getUniversal(varID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_SIM_VARIABLE, varID, "x")
    return RETURN_VALUE_FUNC[varID](result)

def getCurrentTime():
    return _getUniversal(tc.VAR_TIME_STEP)

def getDepartedIDList():
    return _getUniversal(tc.VAR_DEPARTED_VEHICLES_IDS)

def getArrivedIDList():
    return _getUniversal(tc.VAR_ARRIVED_VEHICLES_IDS)

def getStartingTeleportIDList():
    return _getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_IDS)

def getEndingTeleportIDList():
    return _getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_IDS)


def subscribe(varIDs=(tc.VAR_DEPARTED_VEHICLES_IDS,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_SIM_VARIABLE, begin, end, "x", varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(varID, data):
    subscriptionResults[varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults():
    return subscriptionResults
