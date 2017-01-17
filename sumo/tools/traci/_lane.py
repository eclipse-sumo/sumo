# -*- coding: utf-8 -*-
"""
@file    lane.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Jakob Erdmann
@date    2011-03-17
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
from .domain import Domain
from .storage import Storage
from . import constants as tc


def _readLinks(result):
    result.read("!Bi")  # Type Compound, Length
    nbLinks = result.readInt()
    links = []
    for i in range(nbLinks):
        result.read("!B")                           # Type String
        approachedLane = result.readString()
        result.read("!B")                           # Type String
        approachedInternal = result.readString()
        result.read("!B")                           # Type Byte
        hasPrio = bool(result.read("!B"))
        result.read("!B")                           # Type Byte
        isOpen = bool(result.read("!B"))
        result.read("!B")                           # Type Byte
        hasFoe = bool(result.read("!B"))
        result.read("!B")                           # Type String
        state = result.readString()
        result.read("!B")                           # Type String
        direction = result.readString()
        result.read("!B")                           # Type Float
        length = result.readDouble()
        links.append((approachedLane, hasPrio, isOpen, hasFoe,
                      approachedInternal, state, direction, length))
    return links


_RETURN_VALUE_FUNC = {tc.VAR_LENGTH:                Storage.readDouble,
                      tc.VAR_MAXSPEED:              Storage.readDouble,
                      tc.VAR_WIDTH:                 Storage.readDouble,
                      tc.LANE_ALLOWED:              Storage.readStringList,
                      tc.LANE_DISALLOWED:           Storage.readStringList,
                      tc.LANE_LINK_NUMBER: lambda result: result.read("!B")[0],
                      tc.LANE_LINKS:                _readLinks,
                      tc.VAR_SHAPE:                 Storage.readShape,
                      tc.LANE_EDGE_ID:              Storage.readString,
                      tc.VAR_CO2EMISSION:           Storage.readDouble,
                      tc.VAR_COEMISSION:            Storage.readDouble,
                      tc.VAR_HCEMISSION:            Storage.readDouble,
                      tc.VAR_PMXEMISSION:           Storage.readDouble,
                      tc.VAR_NOXEMISSION:           Storage.readDouble,
                      tc.VAR_FUELCONSUMPTION:       Storage.readDouble,
                      tc.VAR_NOISEEMISSION:         Storage.readDouble,
                      tc.VAR_ELECTRICITYCONSUMPTION: Storage.readDouble,
                      tc.LAST_STEP_MEAN_SPEED:      Storage.readDouble,
                      tc.LAST_STEP_OCCUPANCY:       Storage.readDouble,
                      tc.LAST_STEP_LENGTH:          Storage.readDouble,
                      tc.VAR_WAITING_TIME:          Storage.readDouble,
                      tc.VAR_CURRENT_TRAVELTIME:    Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_NUMBER:  Storage.readInt,
                      tc.LAST_STEP_VEHICLE_HALTING_NUMBER: Storage.readInt,
                      tc.LAST_STEP_VEHICLE_ID_LIST: Storage.readStringList}


class LaneDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "lane", tc.CMD_GET_LANE_VARIABLE, tc.CMD_SET_LANE_VARIABLE,
                        tc.CMD_SUBSCRIBE_LANE_VARIABLE, tc.RESPONSE_SUBSCRIBE_LANE_VARIABLE,
                        tc.CMD_SUBSCRIBE_LANE_CONTEXT, tc.RESPONSE_SUBSCRIBE_LANE_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getLength(self, laneID):
        """getLength(string) -> double

        Returns the length in m.
        """
        return self._getUniversal(tc.VAR_LENGTH, laneID)

    def getMaxSpeed(self, laneID):
        """getMaxSpeed(string) -> double

        Returns the maximum allowed speed on the lane in m/s.
        """
        return self._getUniversal(tc.VAR_MAXSPEED, laneID)

    def getWidth(self, laneID):
        """getWidth(string) -> double

        Returns the width of the lane in m.
        """
        return self._getUniversal(tc.VAR_WIDTH, laneID)

    def getAllowed(self, laneID):
        """getAllowed(string) -> list(string)

        Returns a list of allowed vehicle classes. An empty list means all vehicles are allowed.
        """
        return self._getUniversal(tc.LANE_ALLOWED, laneID)

    def getDisallowed(self, laneID):
        """getDisallowed(string) -> list(string)

        Returns a list of disallowed vehicle classes.
        """
        return self._getUniversal(tc.LANE_DISALLOWED, laneID)

    def getLinkNumber(self, laneID):
        """getLinkNumber(string) -> integer

        Returns the number of connections to successive lanes.
        """
        return self._getUniversal(tc.LANE_LINK_NUMBER, laneID)

    def getLinks(self, laneID, extended=False):
        """getLinks(string) -> list((string, bool, bool, bool))
        A list containing id of successor lane together with priority, open and foe
        for each link.
        if extended=True, each result tuple contains
        (string approachedLane, bool hasPrio, bool isOpen, bool hasFoe, 
        string approachedInternal, string state, string direction, float length)
        """
        complete_data = self._getUniversal(tc.LANE_LINKS, laneID)
        if extended:
            return complete_data
        else:
            # for downward compatibility
            return [tuple(d[:4]) for d in complete_data]

    def getShape(self, laneID):
        """getShape(string) -> list((double, double))

        List of 2D positions (cartesian) describing the geometry.
        """
        return self._getUniversal(tc.VAR_SHAPE, laneID)

    def getEdgeID(self, laneID):
        """getEdgeID(string) -> string

        Returns the id of the edge the lane belongs to.
        """
        return self._getUniversal(tc.LANE_EDGE_ID, laneID)

    def getCO2Emission(self, laneID):
        """getCO2Emission(string) -> double

        Returns the CO2 emission in mg for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_CO2EMISSION, laneID)

    def getCOEmission(self, laneID):
        """getCOEmission(string) -> double

        Returns the CO emission in mg for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_COEMISSION, laneID)

    def getHCEmission(self, laneID):
        """getHCEmission(string) -> double

        Returns the HC emission in mg for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_HCEMISSION, laneID)

    def getPMxEmission(self, laneID):
        """getPMxEmission(string) -> double

        Returns the particular matter emission in mg for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_PMXEMISSION, laneID)

    def getNOxEmission(self, laneID):
        """getNOxEmission(string) -> double

        Returns the NOx emission in mg for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_NOXEMISSION, laneID)

    def getFuelConsumption(self, laneID):
        """getFuelConsumption(string) -> double

        Returns the fuel consumption in ml for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_FUELCONSUMPTION, laneID)

    def getNoiseEmission(self, laneID):
        """getNoiseEmission(string) -> double

        Returns the noise emission in db for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_NOISEEMISSION, laneID)

    def getElectricityConsumption(self, laneID):
        """getElectricityConsumption(string) -> double

        Returns the electricity consumption in ml for the last time step.
        """
        return self._getUniversal(tc.VAR_ELECTRICITYCONSUMPTION, laneID)

    def getLastStepMeanSpeed(self, laneID):
        """getLastStepMeanSpeed(string) -> double

        Returns the average speed in m/s for the last time step on the given lane.
        """
        return self._getUniversal(tc.LAST_STEP_MEAN_SPEED, laneID)

    def getLastStepOccupancy(self, laneID):
        """getLastStepOccupancy(string) -> double

        Returns the occupancy in % for the last time step on the given lane.
        """
        return self._getUniversal(tc.LAST_STEP_OCCUPANCY, laneID)

    def getLastStepLength(self, laneID):
        """getLastStepLength(string) -> double

        Returns the mean vehicle length in m for the last time step on the given lane.
        """
        return self._getUniversal(tc.LAST_STEP_LENGTH, laneID)

    def getWaitingTime(self, laneID):
        """getWaitingTime() -> double

        .
        """
        return self._getUniversal(tc.VAR_WAITING_TIME, laneID)

    def getTraveltime(self, laneID):
        """getTraveltime(string) -> double

        Returns the estimated travel time in s for the last time step on the given lane.
        """
        return self._getUniversal(tc.VAR_CURRENT_TRAVELTIME, laneID)

    def getLastStepVehicleNumber(self, laneID):
        """getLastStepVehicleNumber(string) -> integer

        Returns the total number of vehicles for the last time step on the given lane.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, laneID)

    def getLastStepHaltingNumber(self, laneID):
        """getLastStepHaltingNumber(string) -> integer

        Returns the total number of halting vehicles for the last time step on the given lane.
        A speed of less than 0.1 m/s is considered a halt.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, laneID)

    def getLastStepVehicleIDs(self, laneID):
        """getLastStepVehicleIDs(string) -> list(string)

        Returns the ids of the vehicles for the last time step on the given lane.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, laneID)

    def setAllowed(self, laneID, allowedClasses):
        """setAllowed(string, list) -> None

        Sets a list of allowed vehicle classes. Setting an empty list means all vehicles are allowed.
        """
        if isinstance(allowedClasses, str):
            allowedClasses = [allowedClasses]
        self._connection._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_ALLOWED, laneID,
                                       1 + 4 + sum(map(len, allowedClasses)) + 4 * len(allowedClasses))
        self._connection._packStringList(allowedClasses)
        self._connection._sendExact()

    def setDisallowed(self, laneID, disallowedClasses):
        """setDisallowed(string, list) -> None

        Sets a list of disallowed vehicle classes.
        """
        if isinstance(disallowedClasses, str):
            disallowedClasses = [disallowedClasses]
        self._connection._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_DISALLOWED, laneID,
                                       1 + 4 + sum(map(len, disallowedClasses)) + 4 * len(disallowedClasses))
        self._connection._packStringList(disallowedClasses)
        self._connection._sendExact()

    def setMaxSpeed(self, laneID, speed):
        """setMaxSpeed(string, double) -> None

        Sets a new maximum allowed speed on the lane in m/s.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_LANE_VARIABLE, tc.VAR_MAXSPEED, laneID, speed)

    def setLength(self, laneID, length):
        """setLength(string, double) -> None

        Sets the length of the lane in m.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_LANE_VARIABLE, tc.VAR_LENGTH, laneID, length)


LaneDomain()
