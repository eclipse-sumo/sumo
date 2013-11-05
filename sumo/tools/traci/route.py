# -*- coding: utf-8 -*-
"""
@file    route.py
@author  Michael Behrisch
@author  Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import traci, struct
import traci.constants as tc

_RETURN_VALUE_FUNC = {tc.ID_LIST:   traci.Storage.readStringList,
                      tc.ID_COUNT:  traci.Storage.readInt,
                      tc.VAR_EDGES: traci.Storage.readStringList}
subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, routeID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_ROUTE_VARIABLE, varID, routeID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of all routes in the network.
    """
    return _getUniversal(tc.ID_LIST, "")

def getIDCount():
    """getIDCount() -> integer
    
    Returns the number of currently loaded routes.
    """
    return _getUniversal(tc.ID_COUNT, "")

def getEdges(routeID):
    """getEdges(string) -> list(string)
    
    Returns a list of all edges in the route.
    """
    return _getUniversal(tc.VAR_EDGES, routeID)


def subscribe(routeID, varIDs=(tc.ID_LIST,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more route values for the given interval.
    """
    traci._subscribe(tc.CMD_SUBSCRIBE_ROUTE_VARIABLE, begin, end, routeID, varIDs)

def getSubscriptionResults(routeID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step and the given route.
    If no route id is given, all subscription results are returned in a dict.
    If the route id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(routeID)

def subscribeContext(routeID, domain, dist, varIDs=(tc.ID_LIST,), begin=0, end=2**31-1):
    traci._subscribeContext(tc.CMD_SUBSCRIBE_ROUTE_CONTEXT, begin, end, routeID, domain, dist, varIDs)

def getContextSubscriptionResults(routeID=None):
    return subscriptionResults.getContext(routeID)


def add(routeID, edges):
    traci._beginMessage(tc.CMD_SET_ROUTE_VARIABLE, tc.ADD, routeID,
                        1+4+sum(map(len, edges))+4*len(edges))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(edges))
    for e in edges:
        traci._message.string += struct.pack("!i", len(e)) + e
    traci._sendExact()
