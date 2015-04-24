# -*- coding: utf-8 -*-
"""
@file    areal.py
@author  Mario Krumnow
@author  Laura Bieker
@date    2011-03-16
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

_RETURN_VALUE_FUNC = {tc.ID_LIST:                          traci.Storage.readStringList,
                      tc.ID_COUNT:                         traci.Storage.readInt,
                      tc.JAM_LENGTH_METERS:                traci.Storage.readDouble,
                      tc.JAM_LENGTH_VEHICLE:               traci.Storage.readInt,
                      tc.LAST_STEP_MEAN_SPEED:             traci.Storage.readDouble,
                      tc.VAR_POSITION:                     traci.Storage.readDouble,
                      tc.VAR_LENGTH:                       traci.Storage.readDouble,
                      tc.VAR_LANE_ID:                      traci.Storage.readString,
                      tc.LAST_STEP_VEHICLE_ID_LIST:        traci.Storage.readStringList,
                      tc.LAST_STEP_VEHICLE_NUMBER:         traci.Storage.readInt,
                      tc.LAST_STEP_OCCUPANCY:              traci.Storage.readDouble}
subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)


def _getUniversal(varID, detID):
    result = traci._sendReadOneStringCmd(
        tc.CMD_GET_AREAL_DETECTOR_VARIABLE, varID, detID)
    return _RETURN_VALUE_FUNC[varID](result)


def getIDList():
    """getIDList() -> list(string)

    Returns a list of ids of all areal detectors within the scenario.
    """
    return _getUniversal(tc.ID_LIST, "")


def getIDCount():
    """getIDCount() -> integer

    Returns the number of areal detectors in the network.
    """
    return _getUniversal(tc.ID_COUNT, "")


def getJamLengthVehicle(detID):
    """getJamLengthVehicle(string) -> integer

    Returns the jam length in vehicles within the last simulation step.
    """
    return _getUniversal(tc.JAM_LENGTH_VEHICLE, detID)


def getJamLengthMeters(detID):
    """getJamLengthMeters(string) -> double

    Returns the jam length in meters within the last simulation step. 
    """
    return _getUniversal(tc.JAM_LENGTH_METERS, detID)


def getLastStepMeanSpeed(detID):
    """getLastStepMeanSpeed(string) -> double

    Returns the current mean speed in m/s of vehicles that were on the named e2.
    """
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, detID)


def getLastStepVehicleIDs(detID):
    """getLastStepVehicleIDs(string) -> list(string)

    Returns the list of ids of vehicles that were on the named induction loop in the last simulation step.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, detID)


def getLastStepOccupancy(detID):
    """getLastStepMeanSpeed(string) -> double

    Returns the percentage of space the detector was occupied by a vehicle [%]
    """
    return _getUniversal(tc.LAST_STEP_OCCUPANCY, detID)


def getPosition(detID):
    """getPosition(string) -> double

    Returns the starting position of the detector measured from the beginning of the lane in meters.
    """
    return _getUniversal(tc.VAR_POSITION, detID)


def getLaneID(detID):
    """getLaneID(string) -> string

    Returns the id of the lane the detector is on.
    """
    return _getUniversal(tc.VAR_LANE_ID, detID)


def getLength(detID):
    """getLength(string) -> double

    Returns the length of the detector
    """
    return _getUniversal(tc.VAR_LENGTH, detID)


def getLastStepVehicleNumber(detID):
    """getLastStepVehicleNumber(string) -> integer

    Returns the number of vehicles that were on the named induction loop within the last simulation step.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, detID)



def subscribe(detID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31 - 1):
    """subscribe(string, list(integer), double, double) -> None

    Subscribe to one or more detector values for the given interval.
    """
    traci._subscribe(
        tc.CMD_SUBSCRIBE_AREAL_DETECTOR_VARIABLE, begin, end, detID, varIDs)


def getSubscriptionResults(detID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)

    Returns the subscription results for the last time step and the given detector.
    If no detector id is given, all subscription results are returned in a dict.
    If the detector id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(detID)


def subscribeContext(detID, domain, dist, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31 - 1):
    traci._subscribeContext(
        tc.CMD_SUBSCRIBE_AREAL_DETECTOR_CONTEXT, begin, end, detID, domain, dist, varIDs)


def getContextSubscriptionResults(detID=None):
    return subscriptionResults.getContext(detID)
