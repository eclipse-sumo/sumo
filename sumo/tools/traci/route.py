# -*- coding: utf-8 -*-
"""
@file    route.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:   traci.Storage.readStringList,
                     tc.VAR_EDGES: traci.Storage.readStringList}
subscriptionResults = {}

def _getUniversal(varID, routeID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_ROUTE_VARIABLE, varID, routeID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getEdges(routeID):
    return _getUniversal(tc.VAR_EDGES, routeID)


def subscribe(vehID, varIDs=(tc.ID_LIST,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_ROUTE_VARIABLE, begin, end, vehID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(routeID, varID, data):
    if vehID not in subscriptionResults:
        subscriptionResults[routeID] = {}
    subscriptionResults[vehID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(routeID=None):
    if routeID == None:
        return subscriptionResults
    return subscriptionResults.get(routeID, None)
