# -*- coding: utf-8 -*-
"""
@file    person.py
@author  Jakob Erdmann
@date    2015-02-06
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import struct
import traci
import traci.constants as tc


_RETURN_VALUE_FUNC = {tc.ID_LIST:             traci.Storage.readStringList,
                      tc.ID_COUNT:            traci.Storage.readInt,
                      tc.VAR_SPEED:           traci.Storage.readDouble,
                      tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_ANGLE:           traci.Storage.readDouble,
                      tc.VAR_ROAD_ID:         traci.Storage.readString,
                      tc.VAR_TYPE:            traci.Storage.readString,
                      tc.VAR_ROUTE_ID:        traci.Storage.readString,
                      tc.VAR_COLOR: lambda result: result.read("!BBBB"),
                      tc.VAR_LANEPOSITION:    traci.Storage.readDouble,
                      tc.VAR_LENGTH:          traci.Storage.readDouble,
                      tc.VAR_WAITING_TIME:    traci.Storage.readDouble,
                      tc.VAR_WIDTH:           traci.Storage.readDouble,
                      tc.VAR_MINGAP:          traci.Storage.readDouble,
                      }

subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)


def _getUniversal(varID, personID):
    result = traci._sendReadOneStringCmd(
        tc.CMD_GET_PERSON_VARIABLE, varID, personID)
    return _RETURN_VALUE_FUNC[varID](result)


def getIDList():
    """getIDList() -> list(string)

    Returns a list of ids of all persons currently running within the scenario.
    """
    return _getUniversal(tc.ID_LIST, "")


def getIDCount():
    """getIDCount() -> integer

    Returns the number of person in the network.
    """
    return _getUniversal(tc.ID_COUNT, "")


def getSpeed(personID):
    """getSpeed(string) -> double

    Returns the speed in m/s of the named person within the last step.
    """
    return _getUniversal(tc.VAR_SPEED, personID)


def getPosition(personID):
    """getPosition(string) -> (double, double)

    Returns the position of the named person within the last step [m,m].
    """
    return _getUniversal(tc.VAR_POSITION, personID)


def getPosition3D(personID):
    """getPosition(string) -> (double, double)

    Returns the position of the named person within the last step [m,m,m].
    """
    return _getUniversal(tc.VAR_POSITION3D, personID)


def getAngle(personID):
    """getAngle(string) -> double

    Returns the angle in degrees of the named person within the last step. 
    """
    return _getUniversal(tc.VAR_ANGLE, personID)


def getRoadID(personID):
    """getRoadID(string) -> string

    Returns the id of the edge the named person was at within the last step.
    """
    return _getUniversal(tc.VAR_ROAD_ID, personID)


def getTypeID(personID):
    """getTypeID(string) -> string

    Returns the id of the type of the named person.
    """
    return _getUniversal(tc.VAR_TYPE, personID)


def getLanePosition(personID):
    """getLanePosition(string) -> double

    The position of the person along the lane measured in m.
    """
    return _getUniversal(tc.VAR_LANEPOSITION, personID)


def getColor(personID):
    """getColor(string) -> (integer, integer, integer, integer)

    Returns the person's rgba color.
    """
    return _getUniversal(tc.VAR_COLOR, personID)


def getPersonNumber(personID):
    """getPersonNumber(string) -> integer

    .
    """
    return _getUniversal(tc.VAR_PERSON_NUMBER, personID)


def getLength(personID):
    """getLength(string) -> double

    Returns the length in m of the given person.
    """
    return _getUniversal(tc.VAR_LENGTH, personID)


def getWaitingTime(personID):
    """getWaitingTime() -> double
    The waiting time of a person is defined as the time (in seconds) spent with a
    speed below 0.1m/s since the last time it was faster than 0.1m/s.
    (basically, the waiting time of a person is reset to 0 every time it moves). 
    """
    return _getUniversal(tc.VAR_WAITING_TIME, personID)


def getWidth(personID):
    """getWidth(string) -> double

    Returns the width in m of this person.
    """
    return _getUniversal(tc.VAR_WIDTH, personID)


def getMinGap(personID):
    """getMinGap(string) -> double

    Returns the offset (gap to front person if halting) of this person.
    """
    return _getUniversal(tc.VAR_MINGAP, personID)


def subscribe(personID, varIDs=(tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION), begin=0, end=2**31 - 1):
    """subscribe(string, list(integer), double, double) -> None

    Subscribe to one or more person values for the given interval.
    """
    traci._subscribe(
        tc.CMD_SUBSCRIBE_PERSON_VARIABLE, begin, end, personID, varIDs)


def getSubscriptionResults(personID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)

    Returns the subscription results for the last time step and the given person.
    If no person id is given, all subscription results are returned in a dict.
    If the person id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(personID)


def subscribeContext(personID, domain, dist, varIDs=(tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION), begin=0, end=2**31 - 1):
    traci._subscribeContext(
        tc.CMD_SUBSCRIBE_PERSON_CONTEXT, begin, end, personID, domain, dist, varIDs)


def getContextSubscriptionResults(personID=None):
    return subscriptionResults.getContext(personID)


getParameter, setParameter = traci.getParameterAccessors(
    tc.CMD_GET_PERSON_VARIABLE, tc.CMD_SET_PERSON_VARIABLE)
