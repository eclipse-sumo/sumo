# -*- coding: utf-8 -*-
"""
@file    _lanearea.py
@author  Mario Krumnow
@author  Laura Bieker
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
import struct
from . import constants as tc
from .domain import Domain
from .storage import Storage

_RETURN_VALUE_FUNC = {tc.JAM_LENGTH_METERS:         Storage.readDouble,
                      tc.JAM_LENGTH_VEHICLE:        Storage.readInt,
                      tc.LAST_STEP_MEAN_SPEED:      Storage.readDouble,
                      tc.VAR_POSITION:              Storage.readDouble,
                      tc.VAR_LENGTH:                Storage.readDouble,
                      tc.VAR_LANE_ID:               Storage.readString,
                      tc.LAST_STEP_VEHICLE_ID_LIST: Storage.readStringList,
                      tc.LAST_STEP_VEHICLE_NUMBER:  Storage.readInt,
                      tc.LAST_STEP_OCCUPANCY:       Storage.readDouble}


class LaneAreaDomain(Domain):

    def __init__(self, name="lanearea", deprecatedFor=None):
        Domain.__init__(self, name, tc.CMD_GET_AREAL_DETECTOR_VARIABLE, None,
                        tc.CMD_SUBSCRIBE_AREAL_DETECTOR_VARIABLE, tc.RESPONSE_SUBSCRIBE_AREAL_DETECTOR_VARIABLE,
                        tc.CMD_SUBSCRIBE_AREAL_DETECTOR_CONTEXT, tc.RESPONSE_SUBSCRIBE_AREAL_DETECTOR_CONTEXT,
                        _RETURN_VALUE_FUNC, deprecatedFor)

    def getJamLengthVehicle(self, detID):
        """getJamLengthVehicle(string) -> integer

        Returns the jam length in vehicles within the last simulation step.
        """
        return self._getUniversal(tc.JAM_LENGTH_VEHICLE, detID)

    def getJamLengthMeters(self, detID):
        """getJamLengthMeters(string) -> double

        Returns the jam length in meters within the last simulation step. 
        """
        return self._getUniversal(tc.JAM_LENGTH_METERS, detID)

    def getLastStepMeanSpeed(self, detID):
        """getLastStepMeanSpeed(string) -> double

        Returns the current mean speed in m/s of vehicles that were on the named e2.
        """
        return self._getUniversal(tc.LAST_STEP_MEAN_SPEED, detID)

    def getLastStepVehicleIDs(self, detID):
        """getLastStepVehicleIDs(string) -> list(string)

        Returns the list of ids of vehicles that were on the named detector in the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, detID)

    def getLastStepOccupancy(self, detID):
        """getLastStepMeanSpeed(string) -> double

        Returns the percentage of space the detector was occupied by a vehicle [%]
        """
        return self._getUniversal(tc.LAST_STEP_OCCUPANCY, detID)

    def getPosition(self, detID):
        """getPosition(string) -> double

        Returns the starting position of the detector measured from the beginning of the lane in meters.
        """
        return self._getUniversal(tc.VAR_POSITION, detID)

    def getLaneID(self, detID):
        """getLaneID(string) -> string

        Returns the id of the lane the detector is on.
        """
        return self._getUniversal(tc.VAR_LANE_ID, detID)

    def getLength(self, detID):
        """getLength(string) -> double

        Returns the length of the detector
        """
        return self._getUniversal(tc.VAR_LENGTH, detID)

    def getLastStepVehicleNumber(self, detID):
        """getLastStepVehicleNumber(string) -> integer

        Returns the number of vehicles that were on the named detector within the last simulation step.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, detID)


LaneAreaDomain()
LaneAreaDomain("areal", "lanearea")
