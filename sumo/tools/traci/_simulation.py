# -*- coding: utf-8 -*-
"""
@file    simulation.py
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2011-03-15
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2016 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from .domain import Domain
from .storage import Storage
import struct
from . import constants as tc

_RETURN_VALUE_FUNC = {tc.VAR_TIME_STEP:                         Storage.readInt,
                      tc.VAR_LOADED_VEHICLES_NUMBER:            Storage.readInt,
                      tc.VAR_LOADED_VEHICLES_IDS:               Storage.readStringList,
                      tc.VAR_DEPARTED_VEHICLES_NUMBER:          Storage.readInt,
                      tc.VAR_DEPARTED_VEHICLES_IDS:             Storage.readStringList,
                      tc.VAR_ARRIVED_VEHICLES_NUMBER:           Storage.readInt,
                      tc.VAR_ARRIVED_VEHICLES_IDS:              Storage.readStringList,
                      tc.VAR_PARKING_STARTING_VEHICLES_NUMBER:  Storage.readInt,
                      tc.VAR_PARKING_STARTING_VEHICLES_IDS:     Storage.readStringList,
                      tc.VAR_PARKING_ENDING_VEHICLES_NUMBER:    Storage.readInt,
                      tc.VAR_PARKING_ENDING_VEHICLES_IDS:       Storage.readStringList,
                      tc.VAR_STOP_STARTING_VEHICLES_NUMBER:     Storage.readInt,
                      tc.VAR_STOP_STARTING_VEHICLES_IDS:        Storage.readStringList,
                      tc.VAR_STOP_ENDING_VEHICLES_NUMBER:       Storage.readInt,
                      tc.VAR_STOP_ENDING_VEHICLES_IDS:          Storage.readStringList,
                      tc.VAR_MIN_EXPECTED_VEHICLES:             Storage.readInt,
                      tc.VAR_BUS_STOP_WAITING:                  Storage.readInt,
                      tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_TELEPORT_STARTING_VEHICLES_IDS:    Storage.readStringList,
                      tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER:   Storage.readInt,
                      tc.VAR_TELEPORT_ENDING_VEHICLES_IDS:      Storage.readStringList,
                      tc.VAR_DELTA_T:                           Storage.readInt,
                      tc.VAR_NET_BOUNDING_BOX: lambda result: (result.read("!dd"), result.read("!dd"))}


class SimulationDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "simulation", tc.CMD_GET_SIM_VARIABLE, tc.CMD_SET_SIM_VARIABLE,
                        tc.CMD_SUBSCRIBE_SIM_VARIABLE, tc.RESPONSE_SUBSCRIBE_SIM_VARIABLE,
                        tc.CMD_SUBSCRIBE_SIM_CONTEXT, tc.RESPONSE_SUBSCRIBE_SIM_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getCurrentTime(self):
        """getCurrentTime() -> integer

        Returns the current simulation time in ms.
        """
        return self._getUniversal(tc.VAR_TIME_STEP)

    def getLoadedNumber(self):
        """getLoadedNumber() -> integer

        Returns the number of vehicles which were loaded in this time step.
        """
        return self._getUniversal(tc.VAR_LOADED_VEHICLES_NUMBER)

    def getLoadedIDList(self):
        """getLoadedIDList() -> list(string)

        Returns a list of ids of vehicles which were loaded in this time step. 
        """
        return self._getUniversal(tc.VAR_LOADED_VEHICLES_IDS)

    def getDepartedNumber(self):
        """getDepartedNumber() -> integer

        Returns the number of vehicles which departed (were inserted into the road network) in this time step.
        """
        return self._getUniversal(tc.VAR_DEPARTED_VEHICLES_NUMBER)

    def getDepartedIDList(self):
        """getDepartedIDList() -> list(string)

        Returns a list of ids of vehicles which departed (were inserted into the road network) in this time step. 
        """
        return self._getUniversal(tc.VAR_DEPARTED_VEHICLES_IDS)

    def getArrivedNumber(self):
        """getArrivedNumber() -> integer

        Returns the number of vehicles which arrived (have reached their destination and are removed from the road network) in this time step. 
        """
        return self._getUniversal(tc.VAR_ARRIVED_VEHICLES_NUMBER)

    def getArrivedIDList(self):
        """getArrivedIDList() -> list(string)

        Returns a list of ids of vehicles which arrived (have reached their destination and are removed from the road network) in this time step. 
        """
        return self._getUniversal(tc.VAR_ARRIVED_VEHICLES_IDS)

    def getParkingStartingVehiclesNumber(self):
        """getParkingStartingVehiclesNumber() -> integer

        . 
        """
        return self._getUniversal(tc.VAR_PARKING_STARTING_VEHICLES_NUMBER)

    def getParkingStartingVehiclesIDList(self):
        """getParkingStartingVehiclesIDList() -> list(string)

        . 
        """
        return self._getUniversal(tc.VAR_PARKING_STARTING_VEHICLES_IDS)

    def getParkingEndingVehiclesNumber(self):
        """getParkingEndingVehiclesNumber() -> integer

        . 
        """
        return self._getUniversal(tc.VAR_PARKING_ENDING_VEHICLES_NUMBER)

    def getParkingEndingVehiclesIDList(self):
        """getParkingEndingVehiclesIDList() -> list(string)

        . 
        """
        return self._getUniversal(tc.VAR_PARKING_ENDING_VEHICLES_IDS)

    def getStopStartingVehiclesNumber(self):
        """getStopStartingVehiclesNumber() -> integer

        . 
        """
        return self._getUniversal(tc.VAR_STOP_STARTING_VEHICLES_NUMBER)

    def getStopStartingVehiclesIDList(self):
        """getStopStartingVehiclesIDList() -> list(string)

        . 
        """
        return self._getUniversal(tc.VAR_STOP_STARTING_VEHICLES_IDS)

    def getStopEndingVehiclesNumber(self):
        """getStopEndingVehiclesNumber() -> integer

        . 
        """
        return self._getUniversal(tc.VAR_STOP_ENDING_VEHICLES_NUMBER)

    def getStopEndingVehiclesIDList(self):
        """getStopEndingVehiclesIDList() -> list(string)

        . 
        """
        return self._getUniversal(tc.VAR_STOP_ENDING_VEHICLES_IDS)

    def getMinExpectedNumber(self):
        """getMinExpectedNumber() -> integer

        Returns the number of vehicles which are in the net plus the
        ones still waiting to start. This number may be smaller than
        the actual number of vehicles still to come because of delayed
        route file parsing. If the number is 0 however, it is
        guaranteed that all route files have been parsed completely
        and all vehicles have left the network.
        """
        return self._getUniversal(tc.VAR_MIN_EXPECTED_VEHICLES)

    def getBusStopWaiting(self):
        """getBusStopWaiting() -> integer
        Get the total number of waiting persons at the named bus stop.
        """
        return self._getUniversal(tc.VAR_BUS_STOP_WAITING)

    def getStartingTeleportNumber(self):
        """getStartingTeleportNumber() -> integer

        Returns the number of vehicles which started to teleport in this time step. 
        """
        return self._getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER)

    def getStartingTeleportIDList(self):
        """getStartingTeleportIDList() -> list(string)

        Returns a list of ids of vehicles which started to teleport in this time step. 
        """
        return self._getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_IDS)

    def getEndingTeleportNumber(self):
        """getEndingTeleportNumber() -> integer

        Returns the number of vehicles which ended to be teleported in this time step. 
        """
        return self._getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER)

    def getEndingTeleportIDList(self):
        """getEndingTeleportIDList() -> list(string)

        Returns a list of ids of vehicles which ended to be teleported in this time step. 
        """
        return self._getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_IDS)

    def getDeltaT(self):
        """getDeltaT() -> integer
        Returns the length of one simulation step in milliseconds
        """
        return self._getUniversal(tc.VAR_DELTA_T)

    def getNetBoundary(self):
        """getNetBoundary() -> ((double, double), (double, double))

        The boundary box of the simulation network.
        """
        return self._getUniversal(tc.VAR_NET_BOUNDING_BOX)

    def convert2D(self, edgeID, pos, laneIndex=0, toGeo=False):
        posType = tc.POSITION_2D
        if toGeo:
            posType = tc.POSITION_LON_LAT
        self._connection._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION,
                                       "", 1 + 4 + 1 + 4 + len(edgeID) + 8 + 1 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._packString(edgeID, tc.POSITION_ROADMAP)
        self._connection._string += struct.pack("!dBBB",
                                                pos, laneIndex, tc.TYPE_UBYTE, posType)
        return self._connection._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "").read("!dd")

    def convert3D(self, edgeID, pos, laneIndex=0, toGeo=False):
        posType = tc.POSITION_3D
        if toGeo:
            posType = tc.POSITION_LON_LAT_ALT
        self._connection._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION,
                                       "", 1 + 4 + 1 + 4 + len(edgeID) + 8 + 1 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._packString(edgeID, tc.POSITION_ROADMAP)
        self._connection._string += struct.pack("!dBBB",
                                                pos, laneIndex, tc.TYPE_UBYTE, posType)
        return self._connection._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "").read("!ddd")

    def convertRoad(self, x, y, isGeo=False):
        posType = tc.POSITION_2D
        if isGeo:
            posType = tc.POSITION_LON_LAT
        self._connection._beginMessage(
            tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1 + 4 + 1 + 8 + 8 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._string += struct.pack("!Bdd", posType, x, y)
        self._connection._string += struct.pack("!BB",
                                                tc.TYPE_UBYTE, tc.POSITION_ROADMAP)
        result = self._connection._checkResult(
            tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "")
        return result.readString(), result.readDouble(), result.read("!B")[0]

    def convertGeo(self, x, y, fromGeo=False):
        fromType = tc.POSITION_2D
        toType = tc.POSITION_LON_LAT
        if fromGeo:
            fromType = tc.POSITION_LON_LAT
            toType = tc.POSITION_2D
        self._connection._beginMessage(
            tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1 + 4 + 1 + 8 + 8 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._string += struct.pack("!Bdd", fromType, x, y)
        self._connection._string += struct.pack("!BB", tc.TYPE_UBYTE, toType)
        return self._connection._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "").read("!dd")

    def getDistance2D(self, x1, y1, x2, y2, isGeo=False, isDriving=False):
        """getDistance2D(double, double, double, double, boolean, boolean) -> double

        Returns the distance between the two coordinate pairs (x1,y1) and (x2,y2)

        If isGeo=True, coordinates are interpreted as longitude and latitude rather
        than cartesian coordinates in meters.

        If isDriving=True, the coordinates are mapped onto the road network and the
        length of the shortest route in the network is returned. Otherwise, the
        straight-line distance is returned.
        """
        posType = tc.POSITION_2D
        if isGeo:
            posType = tc.POSITION_LON_LAT
        distType = tc.REQUEST_AIRDIST
        if isDriving:
            distType = tc.REQUEST_DRIVINGDIST
        self._connection._beginMessage(
            tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "", 1 + 4 + 1 + 8 + 8 + 1 + 8 + 8 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
        self._connection._string += struct.pack("!Bdd", posType, x1, y1)
        self._connection._string += struct.pack(
            "!BddB", posType, x2, y2, distType)
        return self._connection._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "").readDouble()

    def getDistanceRoad(self, edgeID1, pos1, edgeID2, pos2, isDriving=False):
        """getDistanceRoad(string, double, string, double, boolean) -> double

        Reads two positions on the road network and an indicator whether the air or the driving distance shall be computed. Returns the according distance.
        """
        distType = tc.REQUEST_AIRDIST
        if isDriving:
            distType = tc.REQUEST_DRIVINGDIST
        self._connection._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "",
                                       1 + 4 + 1 + 4 + len(edgeID1) + 8 + 1 + 1 + 4 + len(edgeID2) + 8 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
        self._connection._packString(edgeID1, tc.POSITION_ROADMAP)
        self._connection._string += struct.pack("!dB", pos1, 0)
        self._connection._packString(edgeID2, tc.POSITION_ROADMAP)
        self._connection._string += struct.pack("!dBB", pos2, 0, distType)
        return self._connection._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "").readDouble()

    def clearPending(self, routeID=""):
        self._connection._beginMessage(tc.CMD_SET_SIM_VARIABLE, tc.CMD_CLEAR_PENDING_VEHICLES, "",
                                       1 + 4 + len(routeID))
        self._connection._packString(routeID)
        self._connection._sendExact()

    def saveState(self, fileName):
        self._connection._beginMessage(tc.CMD_SET_SIM_VARIABLE, tc.CMD_SAVE_SIMSTATE, "",
                                       1 + 4 + len(fileName))
        self._connection._packString(fileName)
        self._connection._sendExact()

    def subscribe(self, varIDs=(tc.VAR_DEPARTED_VEHICLES_IDS,), begin=0, end=2**31 - 1):
        """subscribe(list(integer), double, double) -> None

        Subscribe to one or more simulation values for the given interval.
        """
        Domain.subscribe(self, "", varIDs, begin, end)

    def getSubscriptionResults(self):
        """getSubscriptionResults() -> dict(integer: <value_type>)

        Returns the subscription results for the last time step.
        It is not possible to retrieve older subscription results than the ones
        from the last time step.
        """
        return Domain.getSubscriptionResults(self, "")


SimulationDomain()
