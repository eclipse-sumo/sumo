# -*- coding: utf-8 -*-
"""
@file    multientryexit.py
@author  Michael Behrisch
@date    2011-03-16
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import traci
import traci.constants as tc

_RETURN_VALUE_FUNC = {tc.ID_LIST:                          traci.Storage.readStringList,
                      tc.LAST_STEP_VEHICLE_NUMBER:         traci.Storage.readInt,
                      tc.LAST_STEP_MEAN_SPEED:             traci.Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_ID_LIST:        traci.Storage.readStringList,
                      tc.LAST_STEP_VEHICLE_HALTING_NUMBER: traci.Storage.readInt}
subscriptionResults = traci.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, detID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, varID, detID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of all e3 detectors in the network.
    """
    return _getUniversal(tc.ID_LIST, "")

def getLastStepVehicleNumber(detID):
    """getLastStepVehicleNumber(string) -> integer
    
    .
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, detID)

def getLastStepMeanSpeed(detID):
    """getLastStepMeanSpeed(string) -> double
    
    .
    """
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, detID)

def getLastStepVehicleIDs(detID):
    """getLastStepVehicleIDs(string) -> list(string)
    
    .
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, detID)

def getLastStepHaltingNumber(detID):
    """getLastStepHaltingNumber(string) -> integer
    
    .
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, detID)


def subscribe(detID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more detector values for the given interval.
    A call to this method clears all previous subscription results.
    """
    subscriptionResults.reset()
    traci._subscribe(tc.CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, begin, end, detID, varIDs)

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

def subscribeContext(detID, domain, dist, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    subscriptionResults.reset()
    traci._subscribeContext(tc.CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_CONTEXT, begin, end, detID, domain, dist, varIDs)

def getContextSubscriptionResults(detID=None):
    return subscriptionResults.getContext(detID)
