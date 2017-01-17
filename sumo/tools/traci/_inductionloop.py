# -*- coding: utf-8 -*-
"""
@file    inductionloop.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-16
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from .domain import Domain
from .storage import Storage
from . import constants as tc


def readVehicleData(result):
    result.readLength()
    nbData = result.readInt()
    data = []
    for i in range(nbData):
        result.read("!B")
        vehID = result.readString()
        result.read("!B")
        length = result.readDouble()
        result.read("!B")
        entryTime = result.readDouble()
        result.read("!B")
        leaveTime = result.readDouble()
        result.read("!B")
        typeID = result.readString()
        data.append([vehID, length, entryTime, leaveTime, typeID])
    return data

_RETURN_VALUE_FUNC = {tc.VAR_POSITION:                   Storage.readDouble,
                      tc.VAR_LANE_ID:                    Storage.readString,
                      tc.LAST_STEP_VEHICLE_NUMBER:       Storage.readInt,
                      tc.LAST_STEP_MEAN_SPEED:           Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_ID_LIST:      Storage.readStringList,
                      tc.LAST_STEP_OCCUPANCY:            Storage.readDouble,
                      tc.LAST_STEP_LENGTH:               Storage.readDouble,
                      tc.LAST_STEP_TIME_SINCE_DETECTION: Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_DATA:         readVehicleData}


class InductionLoopDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "inductionloop", tc.CMD_GET_INDUCTIONLOOP_VARIABLE, None,
                        tc.CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE, tc.RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE,
                        tc.CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT, tc.RESPONSE_SUBSCRIBE_INDUCTIONLOOP_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getPosition(self, loopID):
        """getPosition(string) -> double

        Returns the position measured from the beginning of the lane in meters.
        """
        return self._getUniversal(tc.VAR_POSITION, loopID)

    def getLaneID(self, loopID):
        """getLaneID(string) -> string

        Returns the id of the lane the loop is on.
        """
        return self._getUniversal(tc.VAR_LANE_ID, loopID)

    def getLastStepVehicleNumber(self, loopID):
        """getLastStepVehicleNumber(string) -> integer

        Returns the number of vehicles that were on the named induction loop within the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, loopID)

    def getLastStepMeanSpeed(self, loopID):
        """getLastStepMeanSpeed(string) -> double

        Returns the mean speed in m/s of vehicles that were on the named induction loop within the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_MEAN_SPEED, loopID)

    def getLastStepVehicleIDs(self, loopID):
        """getLastStepVehicleIDs(string) -> list(string)

        Returns the list of ids of vehicles that were on the named induction loop in the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, loopID)

    def getLastStepOccupancy(self, loopID):
        """getLastStepOccupancy(string) -> double

        Returns the percentage of time the detector was occupied by a vehicle.
        """
        return self._getUniversal(tc.LAST_STEP_OCCUPANCY, loopID)

    def getLastStepMeanLength(self, loopID):
        """getLastStepMeanLength(string) -> double

        Returns the mean length in m of vehicles which were on the detector in the last step.
        """
        return self._getUniversal(tc.LAST_STEP_LENGTH, loopID)

    def getTimeSinceDetection(self, loopID):
        """getTimeSinceDetection(string) -> double

        Returns the time in s since last detection.
        """
        return self._getUniversal(tc.LAST_STEP_TIME_SINCE_DETECTION, loopID)

    def getVehicleData(self, loopID):
        """getVehicleData(string) -> [(veh_id, veh_length, entry_time, exit_time, vType), ...]

        Returns a complex structure containing several information about vehicles which passed the detector.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_DATA, loopID)


InductionLoopDomain()
