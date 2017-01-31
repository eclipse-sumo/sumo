# -*- coding: utf-8 -*-
"""
@file    edge.py
@author  Michael Behrisch
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
from . import constants as tc
from .domain import Domain
from .storage import Storage
from . import exceptions


def _TIME2STEPS(time):
    """Conversion from (float) time in seconds to milliseconds as int"""
    return int(time * 1000)


_RETURN_VALUE_FUNC = {tc.VAR_EDGE_TRAVELTIME:       Storage.readDouble,
                      tc.VAR_WAITING_TIME:          Storage.readDouble,
                      tc.VAR_EDGE_EFFORT:           Storage.readDouble,
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
                      tc.VAR_CURRENT_TRAVELTIME:    Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_NUMBER:  Storage.readInt,
                      tc.LAST_STEP_VEHICLE_HALTING_NUMBER: Storage.readInt,
                      tc.LAST_STEP_VEHICLE_ID_LIST: Storage.readStringList,
                      tc.LAST_STEP_PERSON_ID_LIST: Storage.readStringList,
                      }


class EdgeDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "edge", tc.CMD_GET_EDGE_VARIABLE, tc.CMD_SET_EDGE_VARIABLE,
                        tc.CMD_SUBSCRIBE_EDGE_VARIABLE, tc.RESPONSE_SUBSCRIBE_EDGE_VARIABLE,
                        tc.CMD_SUBSCRIBE_EDGE_CONTEXT, tc.RESPONSE_SUBSCRIBE_EDGE_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getAdaptedTraveltime(self, edgeID, time):
        """getAdaptedTraveltime(string, double) -> double

        Returns the travel time value (in s) used for (re-)routing 
        which is valid on the edge at the given time.
        """
        self._connection._beginMessage(tc.CMD_GET_EDGE_VARIABLE, tc.VAR_EDGE_TRAVELTIME,
                                       edgeID, 1 + 4)
        self._connection._string += struct.pack(
            "!Bi", tc.TYPE_INTEGER, time)
        return self._connection._checkResult(tc.CMD_GET_EDGE_VARIABLE,
                                             tc.VAR_EDGE_TRAVELTIME, edgeID).readDouble()

    def getWaitingTime(self, edgeID):
        """getWaitingTime() -> double 
        Returns the sum of the waiting time of all vehicles currently on
        that edge (see traci.vehicle.getWaitingTime).
        """
        return self._getUniversal(tc.VAR_WAITING_TIME, edgeID)

    def getEffort(self, edgeID, time):
        """getEffort(string, double) -> double

        Returns the effort value used for (re-)routing 
        which is valid on the edge at the given time.
        """
        self._connection._beginMessage(tc.CMD_GET_EDGE_VARIABLE, tc.VAR_EDGE_EFFORT,
                                       edgeID, 1 + 4)
        self._connection._string += struct.pack(
            "!Bi", tc.TYPE_INTEGER, time)
        return self._connection._checkResult(tc.CMD_GET_EDGE_VARIABLE,
                                             tc.VAR_EDGE_EFFORT, edgeID).readDouble()

    def getCO2Emission(self, edgeID):
        """getCO2Emission(string) -> double

        Returns the CO2 emission in mg for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_CO2EMISSION, edgeID)

    def getCOEmission(self, edgeID):
        """getCOEmission(string) -> double

        Returns the CO emission in mg for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_COEMISSION, edgeID)

    def getHCEmission(self, edgeID):
        """getHCEmission(string) -> double

        Returns the HC emission in mg for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_HCEMISSION, edgeID)

    def getPMxEmission(self, edgeID):
        """getPMxEmission(string) -> double

        Returns the particular matter emission in mg for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_PMXEMISSION, edgeID)

    def getNOxEmission(self, edgeID):
        """getNOxEmission(string) -> double

        Returns the NOx emission in mg for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_NOXEMISSION, edgeID)

    def getFuelConsumption(self, edgeID):
        """getFuelConsumption(string) -> double

        Returns the fuel consumption in ml for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_FUELCONSUMPTION, edgeID)

    def getNoiseEmission(self, edgeID):
        """getNoiseEmission(string) -> double

        Returns the noise emission in db for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_NOISEEMISSION, edgeID)

    def getElectricityConsumption(self, edgeID):
        """getElectricityConsumption(string) -> double

        Returns the electricity consumption in ml for the last time step.
        """
        return self._getUniversal(tc.VAR_ELECTRICITYCONSUMPTION, edgeID)

    def getLastStepMeanSpeed(self, edgeID):
        """getLastStepMeanSpeed(string) -> double

        Returns the average speed in m/s for the last time step on the given edge.
        """
        return self._getUniversal(tc.LAST_STEP_MEAN_SPEED, edgeID)

    def getLastStepOccupancy(self, edgeID):
        """getLastStepOccupancy(string) -> double

        Returns the occupancy in % for the last time step on the given edge.
        """
        return self._getUniversal(tc.LAST_STEP_OCCUPANCY, edgeID)

    def getLastStepLength(self, edgeID):
        """getLastStepLength(string) -> double

        Returns the mean vehicle length in m for the last time step on the given edge.
        """
        return self._getUniversal(tc.LAST_STEP_LENGTH, edgeID)

    def getTraveltime(self, edgeID):
        """getTraveltime(string) -> double

        Returns the estimated travel time in s for the last time step on the given edge.
        """
        return self._getUniversal(tc.VAR_CURRENT_TRAVELTIME, edgeID)

    def getLastStepVehicleNumber(self, edgeID):
        """getLastStepVehicleNumber(string) -> integer

        Returns the total number of vehicles for the last time step on the given edge.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, edgeID)

    def getLastStepHaltingNumber(self, edgeID):
        """getLastStepHaltingNumber(string) -> integer

        Returns the total number of halting vehicles for the last time step on the given edge.
        A speed of less than 0.1 m/s is considered a halt.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, edgeID)

    def getLastStepVehicleIDs(self, edgeID):
        """getLastStepVehicleIDs(string) -> list(string)

        Returns the ids of the vehicles for the last time step on the given edge.
        """
        return self._getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, edgeID)

    def getLastStepPersonIDs(self, edgeID):
        """getLastStepPersonIDs(string) -> list(string)

        Returns the ids of the persons on the given edge during the last time step.
        """
        return self._getUniversal(tc.LAST_STEP_PERSON_ID_LIST, edgeID)

    def adaptTraveltime(self, edgeID, time, begin=None, end=None):
        """adaptTraveltime(string, double) -> None

        Adapt the travel time value (in s) used for (re-)routing for the given edge.

        When setting begin time and end time (in seconds), the changes only
        apply to that time range
        """
        if begin is None and end is None:
            self._connection._beginMessage(
                tc.CMD_SET_EDGE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, edgeID, 1 + 4 + 1 + 8)
            self._connection._string += struct.pack("!BiBd",
                                                    tc.TYPE_COMPOUND, 1, tc.TYPE_DOUBLE, time)
            self._connection._sendExact()
        elif begin is not None and end is not None:
            self._connection._beginMessage(
                tc.CMD_SET_EDGE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, edgeID, 1 + 4 + 1 + 4 + 1 + 4 + 1 + 8)
            self._connection._string += struct.pack("!BiBiBiBd",
                                                    tc.TYPE_COMPOUND, 3,
                                                    tc.TYPE_INTEGER, begin,
                                                    tc.TYPE_INTEGER, end,
                                                    tc.TYPE_DOUBLE, time)
            self._connection._sendExact()
        else:
            raise TraCIException(
                "Both, begin time and end time must be specified")

    def setEffort(self, edgeID, effort, begin=None, end=None):
        """setEffort(string, double) -> None

        Adapt the effort value used for (re-)routing for the given edge.

        When setting begin time and end time (in seconds), the changes only
        apply to that time range
        """
        if begin is None and end is None:
            self._connection._beginMessage(
                tc.CMD_SET_EDGE_VARIABLE, tc.VAR_EDGE_EFFORT, edgeID, 1 + 4 + 1 + 8)
            self._connection._string += struct.pack("!BiBd",
                                                    tc.TYPE_COMPOUND, 1, tc.TYPE_DOUBLE, effort)
            self._connection._sendExact()
        elif begin is not None and end is not None:
            self._connection._beginMessage(
                tc.CMD_SET_EDGE_VARIABLE, tc.VAR_EDGE_EFFORT, edgeID, 1 + 4 + 1 + 4 + 1 + 4 + 1 + 8)
            self._connection._string += struct.pack("!BiBiBiBd",
                                                    tc.TYPE_COMPOUND, 3,
                                                    tc.TYPE_INTEGER, begin,
                                                    tc.TYPE_INTEGER, end,
                                                    tc.TYPE_DOUBLE, effort)
            self._connection._sendExact()
        else:
            raise TraCIException(
                "Both, begin time and end time must be specified")

    def setMaxSpeed(self, edgeID, speed):
        """setMaxSpeed(string, double) -> None

        Set a new maximum speed (in m/s) for all lanes of the edge.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_EDGE_VARIABLE, tc.VAR_MAXSPEED, edgeID, speed)


EdgeDomain()
