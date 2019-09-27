# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    _simulation.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2011-03-15
# @version $Id$

from __future__ import absolute_import
import struct
import warnings
from . import constants as tc
from .domain import Domain
from .storage import Storage
from .exceptions import FatalTraCIError


class Stage(object):

    def __init__(self, type, vType, line, destStop, edges, travelTime, cost, length, intended,
                 depart, departPos, arrivalPos, description):
        self.type = type
        self.vType = vType
        self.line = line
        self.destStop = destStop
        self.edges = edges
        self.travelTime = travelTime
        self.cost = cost
        self.length = length
        self.intended = intended
        self.depart = depart
        self.departPos = departPos
        self.arrivalPos = arrivalPos
        self.description = description

    def __attr_repr__(self, attrname, default=""):
        if getattr(self, attrname) == default:
            return ""
        else:
            return "%s=%s" % (attrname, getattr(self, attrname))

    def __repr__(self):
        return "Stage(%s)" % ', '.join([v for v in [
            self.__attr_repr__("type"),
            self.__attr_repr__("vType"),
            self.__attr_repr__("line"),
            self.__attr_repr__("destStop"),
            self.__attr_repr__("edges"),
            self.__attr_repr__("travelTime"),
            self.__attr_repr__("cost"),
            self.__attr_repr__("length"),
            self.__attr_repr__("intended"),
            self.__attr_repr__("depart"),
            self.__attr_repr__("departPos"),
            self.__attr_repr__("arrivalPos"),
            self.__attr_repr__("description"),
        ] if v != ""])


def _readStage(result):
    # compound size and type
    assert(result.read("!i")[0] == 13)
    stageType = result.readTypedInt()
    vType = result.readTypedString()
    line = result.readTypedString()
    destStop = result.readTypedString()
    edges = result.readTypedStringList()
    travelTime = result.readTypedDouble()
    cost = result.readTypedDouble()
    length = result.readTypedDouble()
    intended = result.readTypedString()
    depart = result.readTypedDouble()
    departPos = result.readTypedDouble()
    arrivalPos = result.readTypedDouble()
    description = result.readTypedString()
    return Stage(stageType, vType, line, destStop, edges, travelTime, cost,
                 length, intended, depart, departPos, arrivalPos, description)


def _stageSize(stage):
    size = 1 + 4  # compound
    size += 1 + 4  # stage type
    size += 1 + 4 + len(stage.vType)  # vType
    size += 1 + 4 + len(stage.line)  # line
    size += 1 + 4 + len(stage.destStop)  # destStop
    size += 1 + 4 + sum(map(len, stage.edges)) + 4 * len(stage.edges)  # edges
    size += 1 + 8  # travelTime
    size += 1 + 8  # cost
    size += 1 + 8  # length
    size += 1 + 4 + len(stage.intended)  # intended
    size += 1 + 8  # depart
    size += 1 + 8  # departPos
    size += 1 + 8  # arrivalPos
    size += 1 + 4 + len(stage.description)
    return size


def _writeStage(stage, connection):
    connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 13)
    connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, stage.type)
    connection._packString(stage.vType)
    connection._packString(stage.line)
    connection._packString(stage.destStop)
    connection._packStringList(stage.edges)
    connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, stage.travelTime)
    connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, stage.cost)
    connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, stage.length)
    connection._packString(stage.intended)
    connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, stage.depart)
    connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, stage.departPos)
    connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, stage.arrivalPos)
    connection._packString(stage.description)


_RETURN_VALUE_FUNC = {tc.VAR_TIME: Storage.readDouble,
                      tc.VAR_TIME_STEP: Storage.readInt,
                      tc.VAR_LOADED_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_LOADED_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_DEPARTED_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_DEPARTED_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_ARRIVED_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_ARRIVED_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_PARKING_STARTING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_PARKING_STARTING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_PARKING_ENDING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_PARKING_ENDING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_STOP_STARTING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_STOP_STARTING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_STOP_ENDING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_STOP_ENDING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_COLLIDING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_COLLIDING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_EMERGENCYSTOPPING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_EMERGENCYSTOPPING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_MIN_EXPECTED_VEHICLES: Storage.readInt,
                      tc.VAR_BUS_STOP_ID_LIST: Storage.readStringList,
                      tc.VAR_BUS_STOP_WAITING: Storage.readInt,
                      tc.VAR_BUS_STOP_WAITING_IDS: Storage.readStringList,
                      tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_TELEPORT_STARTING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER: Storage.readInt,
                      tc.VAR_TELEPORT_ENDING_VEHICLES_IDS: Storage.readStringList,
                      tc.VAR_DELTA_T: Storage.readDouble,
                      tc.VAR_NET_BOUNDING_BOX: Storage.readShape}


class SimulationDomain(Domain):

    Stage = Stage

    def __init__(self):
        Domain.__init__(self, "simulation", tc.CMD_GET_SIM_VARIABLE, tc.CMD_SET_SIM_VARIABLE,
                        tc.CMD_SUBSCRIBE_SIM_VARIABLE, tc.RESPONSE_SUBSCRIBE_SIM_VARIABLE,
                        tc.CMD_SUBSCRIBE_SIM_CONTEXT, tc.RESPONSE_SUBSCRIBE_SIM_CONTEXT,
                        _RETURN_VALUE_FUNC)

    @staticmethod
    def walkingStage(edges, arrivalPos, destStop="", description=""):
        return Stage(2, "", "", destStop, edges, 0, 0, 0, "", 0, 0, arrivalPos, description)

    def getTime(self):
        """getTime() -> double

        Returns the current simulation time in s.
        """
        return self._getUniversal(tc.VAR_TIME)

    def step(self, time=0.):
        """step(double) -> None
        Make a simulation step and simulate up to the given sim time (in seconds).
        If the given value is 0 or absent, exactly one step is performed.
        Values smaller than or equal to the current sim time result in no action.
        """
        if self._connection is None:
            raise FatalTraCIError("Not connected.")
        return self._connection.simulationStep(time)

    def getCurrentTime(self):
        """getCurrentTime() -> integer

        Returns the current simulation time in ms.
        """
        warnings.warn("getCurrentTime is deprecated, please use getTime which returns floating point seconds",
                      stacklevel=2)
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

        Returns the number of vehicles which arrived (have reached their destination and are removed from the road
        network) in this time step.
        """
        return self._getUniversal(tc.VAR_ARRIVED_VEHICLES_NUMBER)

    def getArrivedIDList(self):
        """getArrivedIDList() -> list(string)

        Returns a list of ids of vehicles which arrived (have reached their destination and are removed from the road
        network) in this time step.
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

    def getCollidingVehiclesNumber(self):
        """getCollidingVehiclesNumber() -> integer
        Return number of vehicles involved in a collision (typically 2 per
        collision).
        """
        return self._getUniversal(tc.VAR_COLLIDING_VEHICLES_NUMBER)

    def getCollidingVehiclesIDList(self):
        """getCollidingVehiclesIDList() -> list(string)
        Return Ids of vehicles involved in a collision (typically 2 per
        collision).
        """
        return self._getUniversal(tc.VAR_COLLIDING_VEHICLES_IDS)

    def getEmergencyStoppingVehiclesNumber(self):
        """getEmergencyStoppingVehiclesNumber() -> integer
        Return number of vehicles that performed an emergency stop in the last step
        """
        return self._getUniversal(tc.VAR_EMERGENCYSTOPPING_VEHICLES_NUMBER)

    def getEmergencyStoppingVehiclesIDList(self):
        """getEmergencyStoppingVehiclesIDList() -> list(string)
        Return Ids of vehicles that peformed an emergency stop in the last step
        """
        return self._getUniversal(tc.VAR_EMERGENCYSTOPPING_VEHICLES_IDS)

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

    def getBusStopIDList(self):
        return self._getUniversal(tc.VAR_BUS_STOP_ID_LIST)

    def getBusStopWaiting(self, stopID):
        """getBusStopWaiting() -> integer
        Get the total number of waiting persons at the named bus stop.
        """
        return self._getUniversal(tc.VAR_BUS_STOP_WAITING, stopID)

    def getBusStopWaitingIDList(self, stopID):
        """getBusStopWaiting() -> integer
        Get the IDs of waiting persons at the named bus stop.
        """
        return self._getUniversal(tc.VAR_BUS_STOP_WAITING_IDS, stopID)

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
        """getDeltaT() -> double
        Returns the length of one simulation step in seconds
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

    def convertRoad(self, x, y, isGeo=False, vClass="ignoring"):
        posType = tc.POSITION_2D
        if isGeo:
            posType = tc.POSITION_LON_LAT
        self._connection._beginMessage(
            tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1 + 4 + 1 + 8 + 8 + 1 + 1 + 1 + 4 + len(vClass))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
        self._connection._string += struct.pack("!Bdd", posType, x, y)
        self._connection._string += struct.pack("!BB", tc.TYPE_UBYTE, tc.POSITION_ROADMAP)
        self._connection._packString(vClass)
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

        Reads two positions on the road network and an indicator whether the air or the driving distance shall be
        computed. Returns the according distance.
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

    def findRoute(self, fromEdge, toEdge, vType="", depart=-1., routingMode=0):
        self._connection._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.FIND_ROUTE, "",
                                       (1 + 4 + 1 + 4 + len(fromEdge) + 1 + 4 + len(toEdge) + 1 + 4 + len(vType) +
                                        1 + 8 + 1 + 4))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 5)
        self._connection._packString(fromEdge)
        self._connection._packString(toEdge)
        self._connection._packString(vType)
        self._connection._string += struct.pack("!BdBi", tc.TYPE_DOUBLE, depart, tc.TYPE_INTEGER, routingMode)
        return _readStage(self._connection._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.FIND_ROUTE, ""))

    def findIntermodalRoute(self, fromEdge, toEdge, modes="", depart=-1., routingMode=0, speed=-1.,
                            walkFactor=-1., departPos=0., arrivalPos=tc.INVALID_DOUBLE_VALUE, departPosLat=0.,
                            pType="", vType="", destStop=""):
        self._connection._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.FIND_INTERMODAL_ROUTE, "",
                                       1 + 4 + 1 + 4 + len(fromEdge) + 1 + 4 + len(toEdge) + 1 + 4 + len(modes) +
                                       1 + 8 + 1 + 4 + 1 + 8 + 1 + 8 + 1 + 8 + 1 + 8 + 1 + 8 + 1 + 4 + len(pType) +
                                       1 + 4 + len(vType) + 1 + 4 + len(destStop))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 13)
        self._connection._packString(fromEdge)
        self._connection._packString(toEdge)
        self._connection._packString(modes)
        self._connection._string += struct.pack("!BdBi", tc.TYPE_DOUBLE, depart, tc.TYPE_INTEGER, routingMode)
        self._connection._string += struct.pack("!BdBd", tc.TYPE_DOUBLE, speed, tc.TYPE_DOUBLE, walkFactor)
        self._connection._string += struct.pack("!BdBd", tc.TYPE_DOUBLE, departPos, tc.TYPE_DOUBLE, arrivalPos)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, departPosLat)
        self._connection._packString(pType)
        self._connection._packString(vType)
        self._connection._packString(destStop)
        answer = self._connection._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.FIND_INTERMODAL_ROUTE, "")
        result = []
        for _ in range(answer.readInt()):
            answer.read("!B")                   # Type
            result.append(_readStage(answer))
        return result

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
