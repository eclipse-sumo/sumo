# -*- coding: utf-8 -*-
"""
@file    poi.py
@author  Michael Behrisch
@author  Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct, traci
import traci.constants as tc

_RETURN_VALUE_FUNC = {tc.ID_LIST:      traci.Storage.readStringList,
                      tc.VAR_TYPE:     traci.Storage.readString,
                      tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_COLOR:    lambda result: result.read("!BBBB")}
subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, poiID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_POI_VARIABLE, varID, poiID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of all pois in the network.
    """
    return _getUniversal(tc.ID_LIST, "")

def getType(poiID):
    """getType(string) -> string
    
    .
    """
    return _getUniversal(tc.VAR_TYPE, poiID)

def getPosition(poiID):
    """getPosition(string) -> (double, double)
    
    .
    """
    return _getUniversal(tc.VAR_POSITION, poiID)

def getColor(poiID):
    """getColor(string) -> (integer, integer, integer, integer)
    
    .
    """
    return _getUniversal(tc.VAR_COLOR, poiID)


def subscribe(poiID, varIDs=(tc.VAR_POSITION,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more poi values for the given interval.
    A call to this method clears all previous subscription results.
    """
    subscriptionResults.reset()
    traci._subscribe(tc.CMD_SUBSCRIBE_POI_VARIABLE, begin, end, poiID, varIDs)

def getSubscriptionResults(poiID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step and the given poi.
    If no poi id is given, all subscription results are returned in a dict.
    If the poi id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(poiID)

def subscribeContext(poiID, domain, dist, varIDs=(tc.VAR_POSITION,), begin=0, end=2**31-1):
    subscriptionResults.reset()
    traci._subscribeContext(tc.CMD_SUBSCRIBE_POI_CONTEXT, begin, end, poiID, domain, dist, varIDs)

def getContextSubscriptionResults(poiID=None):
    return subscriptionResults.getContext(poiID)


def setType(poiID, poiType):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.VAR_TYPE, poiID, 1+4+len(poiType))
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(poiType)) + poiType
    traci._sendExact()

def setPosition(poiID, x, y):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.VAR_POSITION, poiID, 1+8+8)
    traci._message.string += struct.pack("!Bdd", tc.POSITION_2D, x, y)
    traci._sendExact()

def setColor(poiID, color):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.VAR_COLOR, poiID, 1+1+1+1+1)
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._sendExact()

def add(poiID, x, y, color, poiType="", layer=0):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.ADD, poiID, 1+4 + 1+4+len(poiType) + 1+1+1+1+1 + 1+4 + 1+8+8)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(poiType)) + poiType
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
    traci._message.string += struct.pack("!Bdd", tc.POSITION_2D, x, y)
    traci._sendExact()

def remove(poiID, layer=0):
    traci._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.REMOVE, poiID, 1+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
    traci._sendExact()
