# -*- coding: utf-8 -*-
"""
@file    junction.py
@author  Michael.Behrisch@dlr.de
@date    2011-03-17
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:      traci.Storage.readStringList,
                     tc.VAR_POSITION: lambda(result): result.read("!ff")}
subscriptionResults = {}

def _getUniversal(varID, junctionID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_JUNCTION_VARIABLE, varID, junctionID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getPosition(junctionID):
    return _getUniversal(tc.VAR_POSITION, junctionID)


def subscribe(junctionID, varIDs=(tc.VAR_POSITION,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_JUNCTION_VARIABLE, begin, end, junctionID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(junctionID, varID, data):
    if junctionID not in subscriptionResults:
        subscriptionResults[junctionID] = {}
    subscriptionResults[junctionID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(junctionID=None):
    if junctionID == None:
        return subscriptionResults
    return subscriptionResults.get(junctionID, None)
