# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    _vehicle.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Lena Kalleske
# @author  Jakob Erdmann
# @author  Laura Bieker
# @author  Daniel Krajzewicz
# @author  Leonhard Luecken
# @date    2011-03-09
# @version $Id$

from __future__ import absolute_import
import struct
import warnings
from .domain import Domain
from .storage import Storage
from . import constants as tc
from .exceptions import TraCIException


def _readBestLanes(result):
    result.read("!iB")
    nbLanes = result.read("!i")[0]  # Length
    lanes = []
    for i in range(nbLanes):
        result.read("!B")
        laneID = result.readString()
        length, occupation, offset = result.read("!BdBdBb")[1::2]
        allowsContinuation = bool(result.read("!BB")[1])
        nextLanesNo = result.read("!Bi")[1]
        nextLanes = []
        for j in range(nextLanesNo):
            nextLanes.append(result.readString())
        lanes.append((laneID, length, occupation, offset, allowsContinuation, tuple(nextLanes)))
    return tuple(lanes)


def _readLeader(result):
    result.read("!iB")
    vehicleID = result.readString()
    result.read("!B")
    dist = result.readDouble()
    if vehicleID:
        return vehicleID, dist
    return None


def _readNeighbors(result):
    """ result has structure:
    byte(TYPE_COMPOUND) | length(neighList) | Per list entry: string(vehID) | double(dist)
    """
    N = result.readInt()  # length of the vehicle list
    neighs = []
    for i in range(N):
        vehID = result.readString()
        dist = result.readDouble()
        neighs.append((vehID, dist))
    return neighs


def _readNextTLS(result):
    result.read("!iB")  # numCompounds, TYPE_INT
    numTLS = result.read("!i")[0]
    nextTLS = []
    for i in range(numTLS):
        result.read("!B")
        tlsID = result.readString()
        tlsIndex, dist, state = result.read("!BiBdBB")[1::2]
        nextTLS.append((tlsID, tlsIndex, dist, chr(state)))
    return tuple(nextTLS)


def _readNextStops(result):
    result.read("!iB")  # numCompounds, TYPE_INT
    numStops = result.read("!i")[0]
    nextStop = []
    for i in range(numStops):
        result.read("!B")
        lane = result.readString()
        result.read("!B")
        endPos = result.readDouble()
        result.read("!B")
        stoppingPlaceID = result.readString()
        result.read("!B")
        stopFlags = result.readInt()
        result.read("!B")
        duration = result.readDouble()
        result.read("!B")
        until = result.readDouble()
        nextStop.append((lane, endPos, stoppingPlaceID, stopFlags, duration, until))
    return tuple(nextStop)


_RETURN_VALUE_FUNC = {tc.VAR_SPEED: Storage.readDouble,
                      tc.VAR_SPEED_LAT: Storage.readDouble,
                      tc.VAR_SPEED_WITHOUT_TRACI: Storage.readDouble,
                      tc.VAR_ACCELERATION: Storage.readDouble,
                      tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_POSITION3D: lambda result: result.read("!ddd"),
                      tc.VAR_ANGLE: Storage.readDouble,
                      tc.VAR_ROAD_ID: Storage.readString,
                      tc.VAR_LANE_ID: Storage.readString,
                      tc.VAR_LANE_INDEX: Storage.readInt,
                      tc.VAR_TYPE: Storage.readString,
                      tc.VAR_ROUTE_ID: Storage.readString,
                      tc.VAR_ROUTE_INDEX: Storage.readInt,
                      tc.VAR_COLOR: lambda result: result.read("!BBBB"),
                      tc.VAR_LANEPOSITION: Storage.readDouble,
                      tc.VAR_CO2EMISSION: Storage.readDouble,
                      tc.VAR_COEMISSION: Storage.readDouble,
                      tc.VAR_HCEMISSION: Storage.readDouble,
                      tc.VAR_PMXEMISSION: Storage.readDouble,
                      tc.VAR_NOXEMISSION: Storage.readDouble,
                      tc.VAR_FUELCONSUMPTION: Storage.readDouble,
                      tc.VAR_NOISEEMISSION: Storage.readDouble,
                      tc.VAR_ELECTRICITYCONSUMPTION: Storage.readDouble,
                      tc.VAR_PERSON_CAPACITY: Storage.readInt,
                      tc.VAR_PERSON_NUMBER: Storage.readInt,
                      tc.LAST_STEP_PERSON_ID_LIST: Storage.readStringList,
                      tc.VAR_EDGE_TRAVELTIME: Storage.readDouble,
                      tc.VAR_EDGE_EFFORT: Storage.readDouble,
                      tc.VAR_ROUTE_VALID: lambda result: bool(result.read("!i")[0]),
                      tc.VAR_EDGES: Storage.readStringList,
                      tc.VAR_SIGNALS: Storage.readInt,
                      tc.VAR_LENGTH: Storage.readDouble,
                      tc.VAR_MAXSPEED: Storage.readDouble,
                      tc.VAR_ALLOWED_SPEED: Storage.readDouble,
                      tc.VAR_VEHICLECLASS: Storage.readString,
                      tc.VAR_SPEED_FACTOR: Storage.readDouble,
                      tc.VAR_SPEED_DEVIATION: Storage.readDouble,
                      tc.VAR_EMISSIONCLASS: Storage.readString,
                      tc.VAR_WAITING_TIME: Storage.readDouble,
                      tc.VAR_ACCUMULATED_WAITING_TIME: Storage.readDouble,
                      tc.VAR_LANECHANGE_MODE: Storage.readInt,
                      tc.VAR_SPEEDSETMODE: Storage.readInt,
                      tc.VAR_SLOPE: Storage.readDouble,
                      tc.VAR_WIDTH: Storage.readDouble,
                      tc.VAR_HEIGHT: Storage.readDouble,
                      tc.VAR_LINE: Storage.readString,
                      tc.VAR_VIA: Storage.readStringList,
                      tc.VAR_MINGAP: Storage.readDouble,
                      tc.VAR_SHAPECLASS: Storage.readString,
                      tc.VAR_ACCEL: Storage.readDouble,
                      tc.VAR_DECEL: Storage.readDouble,
                      tc.VAR_EMERGENCY_DECEL: Storage.readDouble,
                      tc.VAR_APPARENT_DECEL: Storage.readDouble,
                      tc.VAR_ACTIONSTEPLENGTH: Storage.readDouble,
                      tc.VAR_LASTACTIONTIME: Storage.readDouble,
                      tc.VAR_IMPERFECTION: Storage.readDouble,
                      tc.VAR_TAU: Storage.readDouble,
                      tc.VAR_BEST_LANES: _readBestLanes,
                      tc.VAR_LEADER: _readLeader,
                      tc.VAR_NEIGHBORS: _readNeighbors,
                      tc.VAR_NEXT_TLS: _readNextTLS,
                      tc.VAR_NEXT_STOPS: _readNextStops,
                      tc.VAR_LANEPOSITION_LAT: Storage.readDouble,
                      tc.VAR_MAXSPEED_LAT: Storage.readDouble,
                      tc.VAR_MINGAP_LAT: Storage.readDouble,
                      tc.VAR_LATALIGNMENT: Storage.readString,
                      tc.DISTANCE_REQUEST: Storage.readDouble,
                      tc.VAR_ROUTING_MODE: Storage.readInt,
                      tc.VAR_STOPSTATE: Storage.readInt,
                      tc.VAR_DISTANCE: Storage.readDouble}


class VehicleDomain(Domain):
    # imported for backwards compatibility
    STOP_DEFAULT = tc.STOP_DEFAULT
    STOP_PARKING = tc.STOP_PARKING
    STOP_TRIGGERED = tc.STOP_TRIGGERED
    STOP_CONTAINER_TRIGGERED = tc.STOP_CONTAINER_TRIGGERED
    STOP_BUS_STOP = tc.STOP_BUS_STOP
    STOP_CONTAINER_STOP = tc.STOP_CONTAINER_STOP
    STOP_CHARGING_STATION = tc.STOP_CHARGING_STATION
    STOP_PARKING_AREA = tc.STOP_PARKING_AREA
    DEPART_TRIGGERED = tc.DEPARTFLAG_TRIGGERED
    DEPART_CONTAINER_TRIGGERED = tc.DEPARTFLAG_CONTAINER_TRIGGERED
    DEPART_NOW = tc.DEPARTFLAG_NOW
    DEPART_SPEED_RANDOM = tc.DEPARTFLAG_SPEED_RANDOM
    DEPART_SPEED_MAX = tc.DEPARTFLAG_SPEED_MAX
    DEPART_LANE_RANDOM = tc.DEPARTFLAG_LANE_RANDOM
    DEPART_LANE_FREE = tc.DEPARTFLAG_LANE_FREE
    DEPART_LANE_ALLOWED_FREE = tc.DEPARTFLAG_LANE_ALLOWED_FREE
    DEPART_LANE_BEST_FREE = tc.DEPARTFLAG_LANE_BEST_FREE
    DEPART_LANE_FIRST_ALLOWED = tc.DEPARTFLAG_LANE_FIRST_ALLOWED

    def __init__(self):
        Domain.__init__(self, "vehicle", tc.CMD_GET_VEHICLE_VARIABLE, tc.CMD_SET_VEHICLE_VARIABLE,
                        tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE, tc.RESPONSE_SUBSCRIBE_VEHICLE_VARIABLE,
                        tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT, tc.RESPONSE_SUBSCRIBE_VEHICLE_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getSpeed(self, vehID):
        """getSpeed(string) -> double

        Returns the (longitudinal) speed in m/s of the named vehicle within the last step.
        """
        return self._getUniversal(tc.VAR_SPEED, vehID)

    def getLateralSpeed(self, vehID):
        """getLateralSpeed(string) -> double

        Returns the lateral speed in m/s of the named vehicle within the last step.
        """
        return self._getUniversal(tc.VAR_SPEED_LAT, vehID)

    def getAcceleration(self, vehID):
        """getAcceleration(string) -> double

        Returns the acceleration in m/s^2 of the named vehicle within the last step.
        """
        return self._getUniversal(tc.VAR_ACCELERATION, vehID)

    def getSpeedWithoutTraCI(self, vehID):
        """getSpeedWithoutTraCI(string) -> double
        Returns the speed that the vehicle would drive if not speed-influencing
        command such as setSpeed or slowDown was given.
        """
        return self._getUniversal(tc.VAR_SPEED_WITHOUT_TRACI, vehID)

    def getPosition(self, vehID):
        """getPosition(string) -> (double, double)

        Returns the position of the named vehicle within the last step [m,m].
        """
        return self._getUniversal(tc.VAR_POSITION, vehID)

    def getPosition3D(self, vehID):
        """getPosition3D(string) -> (double, double, double)

        Returns the position of the named vehicle within the last step [m,m,m].
        """
        return self._getUniversal(tc.VAR_POSITION3D, vehID)

    def getAngle(self, vehID):
        """getAngle(string) -> double

        Returns the angle in degrees of the named vehicle within the last step.
        """
        return self._getUniversal(tc.VAR_ANGLE, vehID)

    def getRoadID(self, vehID):
        """getRoadID(string) -> string

        Returns the id of the edge the named vehicle was at within the last step.
        """
        return self._getUniversal(tc.VAR_ROAD_ID, vehID)

    def getLaneID(self, vehID):
        """getLaneID(string) -> string

        Returns the id of the lane the named vehicle was at within the last step.
        """
        return self._getUniversal(tc.VAR_LANE_ID, vehID)

    def getLaneIndex(self, vehID):
        """getLaneIndex(string) -> integer

        Returns the index of the lane the named vehicle was at within the last step.
        """
        return self._getUniversal(tc.VAR_LANE_INDEX, vehID)

    def getTypeID(self, vehID):
        """getTypeID(string) -> string

        Returns the id of the type of the named vehicle.
        """
        return self._getUniversal(tc.VAR_TYPE, vehID)

    def getRouteID(self, vehID):
        """getRouteID(string) -> string

        Returns the id of the route of the named vehicle.
        """
        return self._getUniversal(tc.VAR_ROUTE_ID, vehID)

    def getRouteIndex(self, vehID):
        """getRouteIndex(string) -> int

        Returns the index of the current edge within the vehicles route or -1 if the
        vehicle has not yet departed
        """
        return self._getUniversal(tc.VAR_ROUTE_INDEX, vehID)

    def getRoute(self, vehID):
        """getRoute(string) -> list(string)

        Returns the ids of the edges the vehicle's route is made of.
        """
        return self._getUniversal(tc.VAR_EDGES, vehID)

    def getLanePosition(self, vehID):
        """getLanePosition(string) -> double

        The position of the vehicle along the lane measured in m.
        """
        return self._getUniversal(tc.VAR_LANEPOSITION, vehID)

    def getColor(self, vehID):
        """getColor(string) -> (integer, integer, integer, integer)

        Returns the vehicle's rgba color.
        """
        return self._getUniversal(tc.VAR_COLOR, vehID)

    def getCO2Emission(self, vehID):
        """getCO2Emission(string) -> double

        Returns the CO2 emission in mg/s for the last time step.
        Multiply by the step length to get the value for one step.
        """
        return self._getUniversal(tc.VAR_CO2EMISSION, vehID)

    def getCOEmission(self, vehID):
        """getCOEmission(string) -> double

        Returns the CO emission in mg/s for the last time step.
        Multiply by the step length to get the value for one step.
        """
        return self._getUniversal(tc.VAR_COEMISSION, vehID)

    def getHCEmission(self, vehID):
        """getHCEmission(string) -> double

        Returns the HC emission in mg/s for the last time step.
        Multiply by the step length to get the value for one step.
        """
        return self._getUniversal(tc.VAR_HCEMISSION, vehID)

    def getPMxEmission(self, vehID):
        """getPMxEmission(string) -> double

        Returns the particular matter emission in mg/s for the last time step.
        Multiply by the step length to get the value for one step.
        """
        return self._getUniversal(tc.VAR_PMXEMISSION, vehID)

    def getNOxEmission(self, vehID):
        """getNOxEmission(string) -> double

        Returns the NOx emission in mg/s for the last time step.
        Multiply by the step length to get the value for one step.
        """
        return self._getUniversal(tc.VAR_NOXEMISSION, vehID)

    def getFuelConsumption(self, vehID):
        """getFuelConsumption(string) -> double

        Returns the fuel consumption in ml/s for the last time step.
        Multiply by the step length to get the value for one step.
        """
        return self._getUniversal(tc.VAR_FUELCONSUMPTION, vehID)

    def getNoiseEmission(self, vehID):
        """getNoiseEmission(string) -> double

        Returns the noise emission in db for the last time step.
        """
        return self._getUniversal(tc.VAR_NOISEEMISSION, vehID)

    def getElectricityConsumption(self, vehID):
        """getElectricityConsumption(string) -> double

        Returns the electricity consumption in Wh/s for the last time step.
        Multiply by the step length to get the value for one step.
        """
        return self._getUniversal(tc.VAR_ELECTRICITYCONSUMPTION, vehID)

    def getPersonCapacity(self, vehID):
        """getPersonCapacity(string) -> int

        Returns the person capacity of the vehicle
        """
        return self._getUniversal(tc.VAR_PERSON_CAPACITY, vehID)

    def getPersonNumber(self, vehID):
        """getPersonNumber(string) -> integer
        Returns the total number of persons which includes those defined
        using attribute 'personNumber' as well as <person>-objects which are riding in
        this vehicle.
        """
        return self._getUniversal(tc.VAR_PERSON_NUMBER, vehID)

    def getPersonIDList(self, vehID):
        """getPersonIDList(string) -> integer
        Returns the list of persons which includes those defined using attribute 'personNumber'
        as well as <person>-objects which are riding in this vehicle.
        """
        return self._getUniversal(tc.LAST_STEP_PERSON_ID_LIST, vehID)

    def getAdaptedTraveltime(self, vehID, time, edgeID):
        """getAdaptedTraveltime(string, double, string) -> double

        .
        """
        self._connection._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE,
                                       tc.VAR_EDGE_TRAVELTIME, vehID, 1 + 4 + 1 + 8 + 1 + 4 + len(edgeID))
        self._connection._string += struct.pack(
            "!BiBd", tc.TYPE_COMPOUND, 2, tc.TYPE_DOUBLE, time)
        self._connection._packString(edgeID)
        return self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, vehID).readDouble()

    def getEffort(self, vehID, time, edgeID):
        """getEffort(string, double, string) -> double

        .
        """
        self._connection._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE,
                                       tc.VAR_EDGE_EFFORT, vehID, 1 + 4 + 1 + 8 + 1 + 4 + len(edgeID))
        self._connection._string += struct.pack(
            "!BiBd", tc.TYPE_COMPOUND, 2, tc.TYPE_DOUBLE, time)
        self._connection._packString(edgeID)
        return self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT, vehID).readDouble()

    def isRouteValid(self, vehID):
        """isRouteValid(string) -> bool
        Returns whether the current vehicle route is connected for the vehicle
        class of the given vehicle.
        """
        return self._getUniversal(tc.VAR_ROUTE_VALID, vehID)

    def getSignals(self, vehID):
        """getSignals(string) -> integer

        Returns an integer encoding the state of a vehicle's signals.
        """
        return self._getUniversal(tc.VAR_SIGNALS, vehID)

    def getLength(self, vehID):
        """getLength(string) -> double

        Returns the length in m of the given vehicle.
        """
        return self._getUniversal(tc.VAR_LENGTH, vehID)

    def getMaxSpeed(self, vehID):
        """getMaxSpeed(string) -> double

        Returns the maximum speed in m/s of this vehicle.
        """
        return self._getUniversal(tc.VAR_MAXSPEED, vehID)

    def getLateralLanePosition(self, vehID):
        """getLateralLanePosition(string) -> double

        Returns The lateral position of the vehicle on its current lane measured in m.
        """
        return self._getUniversal(tc.VAR_LANEPOSITION_LAT, vehID)

    def getMaxSpeedLat(self, vehID):
        """getMaxSpeedLat(string) -> double

        Returns the maximum lateral speed in m/s of this vehicle.
        """
        return self._getUniversal(tc.VAR_MAXSPEED_LAT, vehID)

    def getLateralAlignment(self, vehID):
        """getLateralAlignment(string) -> string

        Returns The preferred lateral alignment of the vehicle
        """
        return self._getUniversal(tc.VAR_LATALIGNMENT, vehID)

    def getMinGapLat(self, vehID):
        """getMinGapLat(string) -> double

        Returns The desired lateral gap of this vehicle at 50km/h in m
        """
        return self._getUniversal(tc.VAR_MINGAP_LAT, vehID)

    def getAllowedSpeed(self, vehID):
        """getAllowedSpeed(string) -> double

        Returns the maximum allowed speed on the current lane regarding speed factor in m/s for this vehicle.
        """
        return self._getUniversal(tc.VAR_ALLOWED_SPEED, vehID)

    def getVehicleClass(self, vehID):
        """getVehicleClass(string) -> string

        Returns the vehicle class of this vehicle.
        """
        return self._getUniversal(tc.VAR_VEHICLECLASS, vehID)

    def getSpeedFactor(self, vehID):
        """getSpeedFactor(string) -> double

        Returns the chosen speed factor for this vehicle.
        """
        return self._getUniversal(tc.VAR_SPEED_FACTOR, vehID)

    def getSpeedDeviation(self, vehID):
        """getSpeedDeviation(string) -> double

        Returns the standard deviation for the speed factor of the vehicle type.
        """
        return self._getUniversal(tc.VAR_SPEED_DEVIATION, vehID)

    def getEmissionClass(self, vehID):
        """getEmissionClass(string) -> string

        Returns the emission class of this vehicle.
        """
        return self._getUniversal(tc.VAR_EMISSIONCLASS, vehID)

    def getWaitingTime(self, vehID):
        """getWaitingTime() -> double
        The waiting time of a vehicle is defined as the time (in seconds) spent with a
        speed below 0.1m/s since the last time it was faster than 0.1m/s.
        (basically, the waiting time of a vehicle is reset to 0 every time it moves).
        A vehicle that is stopping intentionally with a <stop> does not accumulate waiting time.
        """
        return self._getUniversal(tc.VAR_WAITING_TIME, vehID)

    def getAccumulatedWaitingTime(self, vehID):
        """getAccumulatedWaitingTime() -> double
        The accumulated waiting time of a vehicle collects the vehicle's waiting time
        over a certain time interval (interval length is set per option '--waiting-time-memory')
        """
        return self._getUniversal(tc.VAR_ACCUMULATED_WAITING_TIME, vehID)

    def getLaneChangeMode(self, vehID):
        """getLaneChangeMode(string) -> integer

        Gets the vehicle's lane change mode as a bitset.
        """
        return self._getUniversal(tc.VAR_LANECHANGE_MODE, vehID)

    def getSpeedMode(self, vehID):
        """getSpeedMode -> int
        The speed mode of a vehicle
        """
        return self._getUniversal(tc.VAR_SPEEDSETMODE, vehID)

    def getSlope(self, vehID):
        """getSlope -> double
        The slope at the current position of the vehicle in degrees
        """
        return self._getUniversal(tc.VAR_SLOPE, vehID)

    def getWidth(self, vehID):
        """getWidth(string) -> double

        Returns the width in m of this vehicle.
        """
        return self._getUniversal(tc.VAR_WIDTH, vehID)

    def getHeight(self, vehID):
        """getHeight(string) -> double

        Returns the height in m of this vehicle.
        """
        return self._getUniversal(tc.VAR_HEIGHT, vehID)

    def getLine(self, vehID):
        """getLine(string) -> string

        Returns the line information of this vehicle.
        """
        return self._getUniversal(tc.VAR_LINE, vehID)

    def getVia(self, vehID):
        """getVia(string) -> list(string)

        Returns the ids of via edges for this vehicle
        """
        return self._getUniversal(tc.VAR_VIA, vehID)

    def getMinGap(self, vehID):
        """getMinGap(string) -> double

        Returns the offset (gap to front vehicle if halting) of this vehicle.
        """
        return self._getUniversal(tc.VAR_MINGAP, vehID)

    def getShapeClass(self, vehID):
        """getShapeClass(string) -> string

        Returns the shape class of this vehicle.
        """
        return self._getUniversal(tc.VAR_SHAPECLASS, vehID)

    def getAccel(self, vehID):
        """getAccel(string) -> double

        Returns the maximum acceleration possibility in m/s^2 of this vehicle.
        """
        return self._getUniversal(tc.VAR_ACCEL, vehID)

    def getDecel(self, vehID):
        """getDecel(string) -> double

        Returns the preferred maximal deceleration possibility in m/s^2 of this vehicle.
        """
        return self._getUniversal(tc.VAR_DECEL, vehID)

    def getEmergencyDecel(self, vehID):
        """getEmergencyDecel(string) -> double

        Returns the maximal physically possible deceleration in m/s^2 of this vehicle.
        """
        return self._getUniversal(tc.VAR_EMERGENCY_DECEL, vehID)

    def getApparentDecel(self, vehID):
        """getApparentDecel(string) -> double

        Returns the apparent deceleration in m/s^2 of this vehicle.
        """
        return self._getUniversal(tc.VAR_APPARENT_DECEL, vehID)

    def getActionStepLength(self, vehID):
        """getActionStepLength(string) -> double

        Returns the action step length for this vehicle.
        """
        return self._getUniversal(tc.VAR_ACTIONSTEPLENGTH, vehID)

    def getLastActionTime(self, vehID):
        """getLastActionTime(string) -> double

        Returns the time of last action point for this vehicle.
        """
        return self._getUniversal(tc.VAR_LASTACTIONTIME, vehID)

    def getImperfection(self, vehID):
        """getImperfection(string) -> double

        .
        """
        return self._getUniversal(tc.VAR_IMPERFECTION, vehID)

    def getTau(self, vehID):
        """getTau(string) -> double

        Returns the driver's reaction time in s for this vehicle.
        """
        return self._getUniversal(tc.VAR_TAU, vehID)

    def getBestLanes(self, vehID):
        """getBestLanes(string) ->

        Information about the wish to use subsequent edges' lanes.
        """
        return self._getUniversal(tc.VAR_BEST_LANES, vehID)

    def getLeader(self, vehID, dist=0.):
        """getLeader(string, double) -> (string, double)

        Return the leading vehicle id together with the distance. The distance
        is measured from the front + minGap to the back of the leader, so it does not include the
        minGap of the vehicle.
        The dist parameter defines the maximum lookahead, 0 calculates a lookahead from the brake gap.
        Note that the returned leader may be farther away than the given dist.
        """
        self._connection._beginMessage(
            tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_LEADER, vehID, 1 + 8)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, dist)
        return _readLeader(self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_LEADER, vehID))

    def getRightFollowers(self, vehID, blockingOnly=False):
        """ bool -> list(pair(string, double))
        Convenience method, see getNeighbors()
        """
        if blockingOnly:
            mode = 5
        else:
            mode = 1
        return self.getNeighbors(vehID, mode)

    def getRightLeaders(self, vehID, blockingOnly=False):
        """ bool -> list(pair(string, double))
        Convenience method, see getNeighbors()
        """
        if blockingOnly:
            mode = 7
        else:
            mode = 3
        return self.getNeighbors(vehID, mode)

    def getLeftFollowers(self, vehID, blockingOnly=False):
        """ bool -> list(pair(string, double))
        Convenience method, see getNeighbors()
        """
        if blockingOnly:
            mode = 4
        else:
            mode = 0
        return self.getNeighbors(vehID, mode)

    def getLeftLeaders(self, vehID, blockingOnly=False):
        """ bool -> list(pair(string, double))
        Convenience method, see getNeighbors()
        """
        if blockingOnly:
            mode = 6
        else:
            mode = 2
        return self.getNeighbors(vehID, mode)

    def getNeighbors(self, vehID, mode):
        """ byte -> list(pair(string, double))

        The parameter mode is a bitset (UBYTE), specifying the following:
        bit 1: query lateral direction (left:0, right:1)
        bit 2: query longitudinal direction (followers:0, leaders:1)
        bit 3: blocking (return all:0, return only blockers:1)

        The returned list contains pairs (ID, dist) for all lane change relevant neighboring leaders, resp. followers,
        along with their longitudinal distance to the ego vehicle (egoFront - egoMinGap to leaderBack, resp.
        followerFront - followerMinGap to egoBack. The value can be negative for overlapping neighs).
        For the non-sublane case, the lists will contain at most one entry.

        Note: The exact set of blockers in case blocking==1 is not determined for the sublane model,
        but either all neighboring vehicles are returned (in case LCA_BLOCKED) or
        none is returned (in case !LCA_BLOCKED).
        """
        self._connection._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_NEIGHBORS, vehID, 2)
        self._connection._string += struct.pack("!BB", tc.TYPE_UBYTE, mode)
        return _readNeighbors(self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_NEIGHBORS, vehID))

    def getNextTLS(self, vehID):
        """getNextTLS(string) ->

        Return list of upcoming traffic lights [(tlsID, tlsIndex, distance, state), ...]
        """
        return self._getUniversal(tc.VAR_NEXT_TLS, vehID)

    def getNextStops(self, vehID):
        """getNextStop(string) -> [(string, double, string, int, int, int)], ...

        Return list of upcoming stops [(lane, endPos, stoppingPlaceID, stopFlags, duration, until), ...]
        where integer stopFlag is defined as:
               1 * stopped +
               2 * parking +
               4 * personTriggered +
               8 * containerTriggered +
              16 * isBusStop +
              32 * isContainerStop +
              64 * chargingStation +
             128 * parkingarea
        with each of these flags defined as 0 or 1.
        """
        return self._getUniversal(tc.VAR_NEXT_STOPS, vehID)

    def subscribeLeader(self, vehID, dist=0., begin=0, end=2**31 - 1):
        """subscribeLeader(string, double) -> None

        Subscribe for the leading vehicle id together with the distance.
        The dist parameter defines the maximum lookahead, 0 calculates a lookahead from the brake gap.
        """
        self._connection._subscribe(tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE, begin, end, vehID,
                                    (tc.VAR_LEADER,), {tc.VAR_LEADER: struct.pack("!Bd", tc.TYPE_DOUBLE, dist)})

    def getDrivingDistance(self, vehID, edgeID, pos, laneIndex=0):
        """getDrivingDistance(string, string, double, integer) -> double

        Return the distance to the given edge and position along the vehicles route.
        """
        self._connection._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST,
                                       vehID, 1 + 4 + 1 + 4 + len(edgeID) + 8 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._packString(edgeID, tc.POSITION_ROADMAP)
        self._connection._string += struct.pack("!dBB",
                                                pos, laneIndex, tc.REQUEST_DRIVINGDIST)
        return self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST, vehID).readDouble()

    def getDrivingDistance2D(self, vehID, x, y):
        """getDrivingDistance2D(string, double, double) -> integer

        Return the distance to the given network position along the vehicles route.
        """
        self._connection._beginMessage(
            tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST, vehID, 1 + 4 + 1 + 8 + 8 + 1)
        self._connection._string += struct.pack("!BiBddB", tc.TYPE_COMPOUND, 2,
                                                tc.POSITION_2D, x, y, tc.REQUEST_DRIVINGDIST)
        return self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST, vehID).readDouble()

    def getDistance(self, vehID):
        """getDistance(string) -> double

        Returns the distance to the starting point like an odometer
        """
        return self._getUniversal(tc.VAR_DISTANCE, vehID)

    def getStopState(self, vehID):
        """getStopState(string) -> integer

        Returns information in regard to stopping:
        The returned integer is defined as 1 * stopped + 2 * parking
        + 4 * personTriggered + 8 * containerTriggered + 16 * isBusStop
        + 32 * isContainerStop
        with each of these flags defined as 0 or 1
        """
        return self._getUniversal(tc.VAR_STOPSTATE, vehID)

    def isStopped(self, vehID):
        """isStopped(string) -> bool
        Return whether the vehicle is stopped
        """
        return (self.getStopState(vehID) & 1) == 1

    def isStoppedParking(self, vehID):
        """isStoppedParking(string) -> bool
        Return whether the vehicle is parking (implies stopped)
        """
        return (self.getStopState(vehID) & 2) == 2

    def isStoppedTriggered(self, vehID):
        """isStoppedTriggered(string) -> bool
        Return whether the vehicle is stopped and waiting for a person or container
        """
        return (self.getStopState(vehID) & 12) > 0

    def isAtBusStop(self, vehID):
        """isAtBusStop(string) -> bool
        Return whether the vehicle is stopped at a bus stop
        """
        return (self.getStopState(vehID) & 16) == 16

    def isAtContainerStop(self, vehID):
        """isAtContainerStop(string) -> bool
        Return whether the vehicle is stopped at a container stop
        """
        return (self.getStopState(vehID) & 32) == 32

    def getLaneChangeState(self, vehID, direction):
        """getLaneChangeState(string, int) -> (int, int)
        Return the lane change state for the vehicle
        """
        self._connection._beginMessage(
            tc.CMD_GET_VEHICLE_VARIABLE, tc.CMD_CHANGELANE, vehID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, direction)
        result = self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.CMD_CHANGELANE, vehID)
        return result.read("!iBiBi")[2::2]  # ignore num compounds and type int

    def getLaneChangeStatePretty(self, vehID, direction):
        """getLaneChangeState(string, int) -> ([string, ...], [string, ...])
        Return the lane change state for the vehicle as a list of string constants
        """
        constants = {
            0: 'stay',
            1: 'left',
            2: 'right',
            3: 'strategic',
            4: 'cooperative',
            5: 'speedGain',
            6: 'keepRight',
            7: 'TraCI',
            8: 'urgent',
            9: 'blocked by left leader',
            10: 'blocked by left follower',
            11: 'blocked by right leader',
            12: 'bloecked by right follower',
            13: 'overlapping',
            14: 'insufficient space',
            15: 'sublane',
        }

        def prettifyBitstring(intval):
            return [v for k, v in constants.items() if (intval & 2**k)]

        state, stateTraCI = self.getLaneChangeState(vehID, direction)
        return prettifyBitstring(state), prettifyBitstring(stateTraCI)

    def couldChangeLane(self, vehID, direction, state=None):
        """couldChangeLane(string, int) -> bool
        Return whether the vehicle could change lanes in the specified direction
        """
        if state is None:
            state, stateTraCI = self.getLaneChangeState(vehID, direction)
            if self.wantsAndCouldChangeLane(vehID, direction, stateTraCI):
                # vehicle changed in the last step. state is no longer applicable
                return False
        return state != tc.LCA_UNKNOWN and (state & tc.LCA_BLOCKED == 0)

    def wantsAndCouldChangeLane(self, vehID, direction, state=None):
        """wantsAndCouldChangeLane(string, int) -> bool
        Return whether the vehicle wants to and could change lanes in the specified direction
        """
        if state is None:
            state, stateTraCI = self.getLaneChangeState(vehID, direction)
            if self.wantsAndCouldChangeLane(vehID, direction, stateTraCI):
                # vehicle changed in the last step. state is no longer applicable
                return False
        if state & tc.LCA_BLOCKED == 0:
            if direction == -1:
                return state & tc.LCA_RIGHT != 0
            if direction == 1:
                return state & tc.LCA_LEFT != 0
        return False

    def getRoutingMode(self, vehID):
        """returns the current routing mode:
        tc.ROUTING_MODE_DEFAULT    : use weight storages and fall-back to edge speeds (default)
        tc.ROUTING_MODE_AGGREGATED : use global smoothed travel times from device.rerouting
        """
        return self._getUniversal(tc.VAR_ROUTING_MODE, vehID)

    def setMaxSpeed(self, vehID, speed):
        """setMaxSpeed(string, double) -> None

        Sets the maximum speed in m/s for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_MAXSPEED, vehID, speed)

    def setMaxSpeedLat(self, vehID, speed):
        """setMaxSpeedLat(string, double) -> None

        Sets the maximum lateral speed in m/s for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_MAXSPEED_LAT, vehID, speed)

    def rerouteParkingArea(self, vehID, parkingAreaID):
        """rerouteParkingArea(string, string)

        Changes the next parking area in parkingAreaID, updates the vehicle route,
        and preserve consistency in case of passengers/containers on board.
        """
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_REROUTE_TO_PARKING, vehID,
                                       1 + 4 +  # compound
                                       1 + 4 + len(parkingAreaID))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 1)
        self._connection._packString(parkingAreaID)
        self._connection._sendExact()

    def setStop(self, vehID, edgeID, pos=1., laneIndex=0, duration=tc.INVALID_DOUBLE_VALUE,
                flags=tc.STOP_DEFAULT, startPos=tc.INVALID_DOUBLE_VALUE, until=tc.INVALID_DOUBLE_VALUE):
        """setStop(string, string, double, integer, double, integer, double, double) -> None

        Adds or modifies a stop with the given parameters. The duration and the until attribute are
        in seconds.
        """
        if type(duration) is int and duration >= 1000 and duration % 1000 == 0:
            warnings.warn("API change now handles duration as floating point seconds", stacklevel=2)
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_STOP,
                                       vehID, (1 + 4 + 1 + 4 + len(edgeID) + 1 + 8 + 1 + 1 +
                                               1 + 8 + 1 + 1 + 1 + 8 + 1 + 8))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 7)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!BdBBBdBB", tc.TYPE_DOUBLE, pos,
                                                tc.TYPE_BYTE, laneIndex, tc.TYPE_DOUBLE, duration, tc.TYPE_BYTE, flags)
        self._connection._string += struct.pack("!BdBd",
                                                tc.TYPE_DOUBLE, startPos, tc.TYPE_DOUBLE, until)
        self._connection._sendExact()

    def setBusStop(self, vehID, stopID, duration=tc.INVALID_DOUBLE_VALUE,
                   until=tc.INVALID_DOUBLE_VALUE, flags=tc.STOP_DEFAULT):
        """setBusStop(string, string, double, double, integer) -> None

        Adds or modifies a bus stop with the given parameters. The duration and the until attribute are
        in seconds.
        """
        self.setStop(vehID, stopID, duration=duration,
                     until=until, flags=flags | tc.STOP_BUS_STOP)

    def setContainerStop(self, vehID, stopID, duration=tc.INVALID_DOUBLE_VALUE,
                         until=tc.INVALID_DOUBLE_VALUE, flags=tc.STOP_DEFAULT):
        """setContainerStop(string, string, double, double, integer) -> None

        Adds or modifies a container stop with the given parameters. The duration and the until attribute are
        in seconds.
        """
        self.setStop(vehID, stopID, duration=duration, until=until,
                     flags=flags | tc.STOP_CONTAINER_STOP)

    def setChargingStationStop(self, vehID, stopID, duration=tc.INVALID_DOUBLE_VALUE,
                               until=tc.INVALID_DOUBLE_VALUE, flags=tc.STOP_DEFAULT):
        """setChargingStationStop(string, string, double, double, integer) -> None

        Adds or modifies a stop at a chargingStation with the given parameters. The duration and the until attribute are
        in seconds.
        """
        self.setStop(vehID, stopID, duration=duration, until=until,
                     flags=flags | tc.STOP_CHARGING_STATION)

    def setParkingAreaStop(self, vehID, stopID, duration=tc.INVALID_DOUBLE_VALUE,
                           until=tc.INVALID_DOUBLE_VALUE, flags=tc.STOP_PARKING):
        """setParkingAreaStop(string, string, double, double, integer) -> None

        Adds or modifies a stop at a parkingArea with the given parameters. The duration and the until attribute are
        in seconds.
        """
        self.setStop(vehID, stopID, duration=duration, until=until,
                     flags=flags | tc.STOP_PARKING_AREA)

    def resume(self, vehID):
        """resume(string) -> None

        Resumes the vehicle from the current stop (throws an error if the vehicle is not stopped).
        """
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_RESUME, vehID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
        self._connection._sendExact()

    def changeLane(self, vehID, laneIndex, duration):
        """changeLane(string, int, double) -> None

        Forces a lane change to the lane with the given index; if successful,
        the lane will be chosen for the given amount of time (in s).
        """
        if type(duration) is int and duration >= 1000:
            warnings.warn("API change now handles duration as floating point seconds", stacklevel=2)
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGELANE, vehID, 1 + 4 + 1 + 1 + 1 + 8)
        self._connection._string += struct.pack(
            "!BiBBBd", tc.TYPE_COMPOUND, 2, tc.TYPE_BYTE, laneIndex, tc.TYPE_DOUBLE, duration)
        self._connection._sendExact()

    def changeLaneRelative(self, vehID, indexOffset, duration):
        """changeLaneRelative(string, int, double) -> None

        Forces a relative lane change; if successful,
        the lane will be chosen for the given amount of time (in s).
        The indexOffset specifies the target lane relative to the vehicles current lane
        """
        if type(duration) is int and duration >= 1000:
            warnings.warn("API change now handles duration as floating point seconds", stacklevel=2)
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGELANE, vehID, 1 + 4 + 1 + 1 + 1 + 8 + 1 + 1)
        self._connection._string += struct.pack(
            "!BiBbBdBB", tc.TYPE_COMPOUND, 3, tc.TYPE_BYTE, indexOffset, tc.TYPE_DOUBLE, duration, tc.TYPE_BYTE, 1)
        self._connection._sendExact()

    def changeSublane(self, vehID, latDist):
        """changeLane(string, double) -> None
        Forces a lateral change by the given amount (negative values indicate changing to the right, positive
        to the left). This will override any other lane change motivations but conform to
        safety-constraints as configured by laneChangeMode.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGESUBLANE, vehID, latDist)

    def slowDown(self, vehID, speed, duration):
        """slowDown(string, double, double) -> None

        Changes the speed smoothly to the given value over the given amount
        of time in seconds (can also be used to increase speed).
        """
        if type(duration) is int and duration >= 1000:
            warnings.warn("API change now handles duration as floating point seconds", stacklevel=2)
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_SLOWDOWN, vehID, 1 + 4 + 1 + 8 + 1 + 8)
        self._connection._string += struct.pack(
            "!BiBdBd", tc.TYPE_COMPOUND, 2, tc.TYPE_DOUBLE, speed, tc.TYPE_DOUBLE, duration)
        self._connection._sendExact()

    def openGap(self, vehID, newTimeHeadway, newSpaceHeadway, duration, changeRate, maxDecel=-1, referenceVehID=None):
        """openGap(string, double, double, double, double, double, string) -> None

        Changes the vehicle's desired time headway (cf-parameter tau) smoothly to the given new value
        using the given change rate. Similarly, the given space headway is applied gradually
        to achieve a minimal spatial gap.
        The vehicle is commanded to keep the increased headway for
        the given duration once its target value is attained. The maximal value for the
        deceleration can be given to prevent harsh braking due to the change of tau. If maxDecel=-1,
        the limit determined by the CF model is used.
        A vehicle ID for a reference vehicle can optionally be given, otherwise, the gap is created with
        respect to the current leader on the ego vehicle's current lane.
        Note that this does only affect the following behavior regarding the current leader and does
        not influence the gap acceptance during lane change, etc.
        """
        if type(duration) is int and duration >= 1000:
            warnings.warn("API change now handles duration as floating point seconds", stacklevel=2)
        nParams = 5
        # compoundType, nParams, float params (2 newHeadways, duration, changeRate, maxDecel)
        msgLength = 1 + 4 + (1 + 8) * nParams
        if referenceVehID is not None:
            nParams = 6
            msgLength += 1 + 4 + len(referenceVehID)  # TYPE_STRING, len, referenceVehID
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_OPENGAP, vehID, msgLength)
        self._connection._string += struct.pack("!BiBdBdBdBdBd", tc.TYPE_COMPOUND, nParams,
                                                tc.TYPE_DOUBLE, newTimeHeadway, tc.TYPE_DOUBLE, newSpaceHeadway,
                                                tc.TYPE_DOUBLE, duration, tc.TYPE_DOUBLE, changeRate,
                                                tc.TYPE_DOUBLE, maxDecel)
        if nParams == 6:
            self._connection._packString(referenceVehID)
        self._connection._sendExact()

    def deactivateGapControl(self, vehID):
        """deactivateGapControl(string) -> None

        Deactivate the vehicle's gap control
        """
        self.openGap(vehID, -1, -1, -1, -1)

    def requestToC(self, vehID, leadTime):
        """ requestToC(string, double) -> None

        Interface for triggering a transition of control for a vehicle equipped with a ToC device.
        """
        self.setParameter(vehID, "device.toc.requestToC", str(leadTime))

    def changeTarget(self, vehID, edgeID):
        """changeTarget(string, string) -> None

        The vehicle's destination edge is set to the given edge id. The route is rebuilt.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGETARGET, vehID, edgeID)

    def setType(self, vehID, typeID):
        """setType(string, string) -> None

        Sets the id of the type for the named vehicle.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_TYPE, vehID, typeID)

    def setRouteID(self, vehID, routeID):
        """setRouteID(string, string) -> None

        Changes the vehicles route to the route with the given id.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_ROUTE_ID, vehID, routeID)

    def setRoute(self, vehID, edgeList):
        """
        setRoute(string, list) ->  None

        changes the vehicle route to given edges list.
        The first edge in the list has to be the one that the vehicle is at at the moment.

        example usage:
        setRoute('1', ['1', '2', '4', '6', '7'])

        this changes route for vehicle id 1 to edges 1-2-4-6-7
        """
        if isinstance(edgeList, str):
            edgeList = [edgeList]
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_ROUTE, vehID,
                                       1 + 4 + sum(map(len, edgeList)) + 4 * len(edgeList))
        self._connection._packStringList(edgeList)
        self._connection._sendExact()

    def updateBestLanes(self, vehID):
        """ updateBestLanes(string) -> None
        Triggers an update of the vehicle's bestLanes (structure determining the lane preferences used by LC models)
        It may be called after modifying the vClass for instance.
        """
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_UPDATE_BESTLANES, vehID)

    def setAdaptedTraveltime(self, vehID, edgeID, time=None, begTime=None, endTime=None):
        """setAdaptedTraveltime(string, string, double, double, double) -> None
        Inserts the information about the travel time of edge "edgeID" valid
        from begin time to end time into the vehicle's internal edge weights
        container.
        If the time is not specified, any previously set values for that edge
        are removed.
        If begTime or endTime are not specified the value is set for the whole
        simulation duration.
        """
        if type(edgeID) != str and type(begTime) == str:
            # legacy handling
            warnings.warn(
                "Parameter order has changed for setAdaptedTraveltime(). Attempting legacy ordering. " +
                "Please update your code.", stacklevel=2)
            return self.setAdaptedTraveltime(vehID, begTime, endTime, edgeID, time)
        if time is None:
            # reset
            self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME,
                                           vehID, 1 + 4 + 1 + 4 + len(edgeID))
            self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 1)
            self._connection._packString(edgeID)
            self._connection._sendExact()
        elif begTime is None:
            # set value for the whole simulation
            self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME,
                                           vehID, 1 + 4 + 1 + 4 + len(edgeID) + 1 + 8)
            self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
            self._connection._packString(edgeID)
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, time)
            self._connection._sendExact()
        else:
            self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME,
                                           vehID, 1 + 4 + 1 + 8 + 1 + 8 + 1 + 4 + len(edgeID) + 1 + 8)
            self._connection._string += struct.pack("!BiBdBd", tc.TYPE_COMPOUND, 4, tc.TYPE_DOUBLE, begTime,
                                                    tc.TYPE_DOUBLE, endTime)
            self._connection._packString(edgeID)
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, time)
            self._connection._sendExact()

    def setEffort(self, vehID, edgeID, effort=None, begTime=None, endTime=None):
        """setEffort(string, string, double, double, double) -> None
        Inserts the information about the effort of edge "edgeID" valid from
        begin time to end time into the vehicle's internal edge weights
        container.
        If the time is not specified, any previously set values for that edge
        are removed.
        If begTime or endTime are not specified the value is set for the whole
        simulation duration.
        """
        if type(edgeID) != str and type(begTime) == str:
            # legacy handling
            warnings.warn(
                "Parameter order has changed for setEffort(). Attempting legacy ordering. Please update your code.",
                stacklevel=2)
            return self.setEffort(vehID, begTime, endTime, edgeID, effort)
        if effort is None:
            # reset
            self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT,
                                           vehID, 1 + 4 + 1 + 4 + len(edgeID))
            self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 1)
            self._connection._packString(edgeID)
            self._connection._sendExact()
        elif begTime is None:
            # set value for the whole simulation
            self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT,
                                           vehID, 1 + 4 + 1 + 4 + len(edgeID) + 1 + 8)
            self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
            self._connection._packString(edgeID)
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, effort)
            self._connection._sendExact()
        else:
            self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT,
                                           vehID, 1 + 4 + 1 + 8 + 1 + 8 + 1 + 4 + len(edgeID) + 1 + 8)
            self._connection._string += struct.pack("!BiBdBd", tc.TYPE_COMPOUND, 4, tc.TYPE_DOUBLE, begTime,
                                                    tc.TYPE_DOUBLE, endTime)
            self._connection._packString(edgeID)
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, effort)
            self._connection._sendExact()

    LAST_TRAVEL_TIME_UPDATE = -1

    def setRoutingMode(self, vehID, routingMode):
        """sets the current routing mode:
        tc.ROUTING_MODE_DEFAULT    : use weight storages and fall-back to edge speeds (default)
        tc.ROUTING_MODE_AGGREGATED : use global smoothed travel times from device.rerouting
        """
        self._connection._sendIntCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_ROUTING_MODE, vehID, routingMode)

    def rerouteTraveltime(self, vehID, currentTravelTimes=True):
        """rerouteTraveltime(string, bool) -> None Reroutes a vehicle. If
        currentTravelTimes is True (default) then the current traveltime of the
        edges is loaded and used for rerouting. If currentTravelTimes is False
        custom travel times are used. The various functions and options for
        customizing travel times are described at https://sumo.dlr.de/wiki/Simulation/Routing

        When rerouteTraveltime has been called once with option
        currentTravelTimes=True, all edge weights are set to the current travel
        times at the time of that call (even for subsequent simulation steps).
        """
        if currentTravelTimes:
            time = self._connection.simulation.getTime()
            if time != self.LAST_TRAVEL_TIME_UPDATE:
                self.LAST_TRAVEL_TIME_UPDATE = time
                for edge in self._connection.edge.getIDList():
                    self._connection.edge.adaptTraveltime(
                        edge, self._connection.edge.getTraveltime(edge))
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_REROUTE_TRAVELTIME, vehID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
        self._connection._sendExact()

    def rerouteEffort(self, vehID):
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_REROUTE_EFFORT, vehID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
        self._connection._sendExact()

    def setSignals(self, vehID, signals):
        """setSignals(string, integer) -> None

        Sets an integer encoding the state of the vehicle's signals.
        """
        self._connection._sendIntCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SIGNALS, vehID, signals)

    def moveTo(self, vehID, laneID, pos):
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE,
                                       tc.VAR_MOVE_TO, vehID, 1 + 4 + 1 + 4 + len(laneID) + 1 + 8)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._packString(laneID)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, pos)
        self._connection._sendExact()

    def setSpeed(self, vehID, speed):
        """setSpeed(string, double) -> None

        Sets the speed in m/s for the named vehicle within the last step.
        Calling with speed=-1 hands the vehicle control back to SUMO.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SPEED, vehID, speed)

    def setColor(self, vehID, color):
        """setColor(string, (integer, integer, integer, integer))

        Sets the color for the vehicle with the given ID, i.e. (255,0,0) for the color red.
        The fourth component (alpha) is optional.
        """
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_COLOR, vehID, 1 + 1 + 1 + 1 + 1)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]),
                                                int(color[3]) if len(color) > 3 else 255)
        self._connection._sendExact()

    def setLength(self, vehID, length):
        """setLength(string, double) -> None

        Sets the length in m for the given vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_LENGTH, vehID, length)

    def setVehicleClass(self, vehID, clazz):
        """setVehicleClass(string, string) -> None

        Sets the vehicle class for this vehicle.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_VEHICLECLASS, vehID, clazz)

    def setSpeedFactor(self, vehID, factor):
        """setSpeedFactor(string, double) -> None

        .
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SPEED_FACTOR, vehID, factor)

    def setEmissionClass(self, vehID, clazz):
        """setEmissionClass(string, string) -> None

        Sets the emission class for this vehicle.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EMISSIONCLASS, vehID, clazz)

    def setWidth(self, vehID, width):
        """setWidth(string, double) -> None

        Sets the width in m for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_WIDTH, vehID, width)

    def setHeight(self, vehID, height):
        """setHeight(string, double) -> None

        Sets the height in m for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_HEIGHT, vehID, height)

    def setLine(self, vehID, line):
        """setHeight(string, string) -> None

        Sets the line information for this vehicle.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_LINE, vehID, line)

    def setVia(self, vehID, edgeList):
        """
        setVia(string, list) ->  None

        changes the via edges to the given edges list (to be used during
        subsequent rerouting calls).

        Note: a single edgeId as argument is allowed as shorthand for a list of length 1
        """
        if isinstance(edgeList, str):
            edgeList = [edgeList]
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_VIA, vehID,
                                       1 + 4 + sum(map(len, edgeList)) + 4 * len(edgeList))
        self._connection._packStringList(edgeList)
        self._connection._sendExact()

    def setMinGap(self, vehID, minGap):
        """setMinGap(string, double) -> None

        Sets the offset (gap to front vehicle if halting) for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_MINGAP, vehID, minGap)

    def setMinGapLat(self, vehID, minGapLat):
        """setMinGapLat(string, double) -> None

        Sets the minimum lateral gap of the vehicle at 50km/h in m
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_MINGAP_LAT, vehID, minGapLat)

    def setLateralAlignment(self, vehID, align):
        """setLateralAlignment(string, string) -> None

        Sets the preferred lateral alignment for this vehicle.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_LATALIGNMENT, vehID, align)

    def setShapeClass(self, vehID, clazz):
        """setShapeClass(string, string) -> None

        Sets the shape class for this vehicle.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SHAPECLASS, vehID, clazz)

    def setAccel(self, vehID, accel):
        """setAccel(string, double) -> None

        Sets the maximum acceleration in m/s^2 for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_ACCEL, vehID, accel)

    def setDecel(self, vehID, decel):
        """setDecel(string, double) -> None

        Sets the preferred maximal deceleration in m/s^2 for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_DECEL, vehID, decel)

    def setEmergencyDecel(self, vehID, decel):
        """setEmergencyDecel(string, double) -> None

        Sets the maximal physically possible deceleration in m/s^2 for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EMERGENCY_DECEL, vehID, decel)

    def setApparentDecel(self, vehID, decel):
        """setApparentDecel(string, double) -> None

        Sets the apparent deceleration in m/s^2 for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_APPARENT_DECEL, vehID, decel)

    def setActionStepLength(self, vehID, actionStepLength, resetActionOffset=True):
        """setActionStepLength(string, double, bool) -> None

        Sets the action step length for this vehicle. If resetActionOffset == True (default), the
        next action point is scheduled immediately. if If resetActionOffset == False, the interval
        between the last and the next action point is updated to match the given value, or if the latter
        is smaller than the time since the last action point, the next action follows immediately.
        """
        if actionStepLength < 0:
            raise TraCIException("Invalid value for actionStepLength. Given value must be non-negative.")
        # Use negative value to indicate resetActionOffset == False
        if not resetActionOffset:
            actionStepLength *= -1
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_ACTIONSTEPLENGTH, vehID, actionStepLength)

    def highlight(self, vehID, color=(255, 0, 0, 255), size=-1, alphaMax=-1, duration=-1, type=0):
        """ highlight(string, color, float, ubyte) -> void
            Adds a circle of the given color tracking the vehicle.
            If a positive size [in m] is given the size of the highlight is chosen accordingly,
            otherwise the length of the vehicle is used as reference.
            If alphaMax and duration are positive, the circle fades in and out within the given duration,
            otherwise it permanently follows the vehicle.
        """
        if (type > 255):
            raise TraCIException("poi.highlight(): maximal value for type is 255")
        if (alphaMax > 255):
            raise TraCIException("vehicle.highlight(): maximal value for alphaMax is 255")
        if (alphaMax <= 0 and duration > 0):
            raise TraCIException("vehicle.highlight(): duration>0 requires alphaMax>0")
        if (alphaMax > 0 and duration <= 0):
            raise TraCIException("vehicle.highlight(): alphaMax>0 requires duration>0")

        if (type > 0):
            compoundLength = 5
        elif (alphaMax > 0):
            compoundLength = 4
        elif (size > 0):
            compoundLength = 2
        elif color:
            compoundLength = 1
        else:
            compoundLength = 0

        msg_length = 1 + 1
        if compoundLength >= 1:
            msg_length += 1 + 4
        if compoundLength >= 2:
            msg_length += 1 + 8
        if compoundLength >= 3:
            msg_length += 1 + 8 + 1 + 1
        if compoundLength >= 5:
            msg_length += 1 + 1
        if not color:
            # Send red as highlight standard
            color = (255, 0, 0, 255)

        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_HIGHLIGHT, vehID, msg_length)
        self._connection._string += struct.pack("!BB", tc.TYPE_COMPOUND, compoundLength)
        if (compoundLength >= 1):
            self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]),
                                                    int(color[2]), int(color[3]) if len(color) > 3 else 255)
        if (compoundLength >= 2):
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, size)
        if (compoundLength >= 3):
            self._connection._string += struct.pack("!BB", tc.TYPE_UBYTE, alphaMax)
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, duration)
        if (compoundLength >= 5):
            self._connection._string += struct.pack("!BB", tc.TYPE_UBYTE, type)
        self._connection._sendExact()

    def setImperfection(self, vehID, imperfection):
        """setImperfection(string, double) -> None

        Sets the driver imperfection sigma.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_IMPERFECTION, vehID, imperfection)

    def setTau(self, vehID, tau):
        """setTau(string, double) -> None

        Sets the driver's tau-parameter (reaction time or anticipation time depending on the car-following model) in s
        for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_TAU, vehID, tau)

    def setLaneChangeMode(self, vehID, lcm):
        """setLaneChangeMode(string, integer) -> None

        Sets the vehicle's lane change mode as a bitset.
        """
        self._connection._sendIntCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_LANECHANGE_MODE, vehID, lcm)

    def setSpeedMode(self, vehID, sm):
        """setSpeedMode(string, integer) -> None

        Sets the vehicle's speed mode as a bitset.
        """
        self._connection._sendIntCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SPEEDSETMODE, vehID, sm)

    def addLegacy(self, vehID, routeID, depart=tc.DEPARTFLAG_NOW, pos=0, speed=0,
                  lane=tc.DEPARTFLAG_LANE_FIRST_ALLOWED, typeID="DEFAULT_VEHTYPE"):
        """
        Add a new vehicle (old style)
        """
        if depart == tc.DEPARTFLAG_NOW:
            depart = "now"
        elif depart == tc.DEPARTFLAG_TRIGGERED:
            depart = "triggered"
        else:
            depart = str(depart)
        if pos < 0:
            print("Invalid departure position.")
            return
        if lane == tc.DEPARTFLAG_LANE_FIRST_ALLOWED:
            lane = "first"
        elif lane == tc.DEPARTFLAG_LANE_FREE:
            lane = "free"
        else:
            lane = str(lane)
        self.addFull(vehID, routeID, typeID, depart, lane, str(pos), str(speed))

    def add(self, vehID, routeID, typeID="DEFAULT_VEHTYPE", depart=None,
            departLane="first", departPos="base", departSpeed="0",
            arrivalLane="current", arrivalPos="max", arrivalSpeed="current",
            fromTaz="", toTaz="", line="", personCapacity=0, personNumber=0):
        """
        Add a new vehicle (new style with all possible parameters)
        """
        messageString = struct.pack("!Bi", tc.TYPE_COMPOUND, 14)
        if depart is None:
            depart = str(self._connection.simulation.getTime())
        for val in (routeID, typeID, depart, departLane, departPos, departSpeed,
                    arrivalLane, arrivalPos, arrivalSpeed, fromTaz, toTaz, line):
            val = str(val)
            messageString += struct.pack("!Bi", tc.TYPE_STRING, len(val)) + val.encode("latin1")
        messageString += struct.pack("!Bi", tc.TYPE_INTEGER, personCapacity)
        messageString += struct.pack("!Bi", tc.TYPE_INTEGER, personNumber)

        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.ADD_FULL, vehID, len(messageString))
        self._connection._string += messageString
        self._connection._sendExact()

    addFull = add

    def remove(self, vehID, reason=tc.REMOVE_VAPORIZED):
        '''Remove vehicle with the given ID for the give reason.
           Reasons are defined in module constants and start with REMOVE_'''
        self._connection._sendByteCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.REMOVE, vehID, reason)

    def moveToXY(self, vehID, edgeID, lane, x, y, angle=tc.INVALID_DOUBLE_VALUE, keepRoute=1):
        '''Place vehicle at the given x,y coordinates and force it's angle to
        the given value (for drawing).
        If the angle is set to INVALID_DOUBLE_VALUE, the vehicle assumes the
        natural angle of the edge on which it is driving.
        If keepRoute is set to 1, the closest position
        within the existing route is taken. If keepRoute is set to 0, the vehicle may move to
        any edge in the network but it's route then only consists of that edge.
        If keepRoute is set to 2 the vehicle has all the freedom of keepRoute=0
        but in addition to that may even move outside the road network.
        edgeID and lane are optional placement hints to resolve ambiguities'''
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.MOVE_TO_XY,
                                       vehID, 1 + 4 + 1 + 4 + len(edgeID) + 1 + 4 + 1 + 8 + 1 + 8 + 1 + 8 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 6)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, lane)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, x)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, y)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, angle)
        self._connection._string += struct.pack("!BB", tc.TYPE_BYTE, keepRoute)
        self._connection._sendExact()

    def subscribe(self, objectID, varIDs=(tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION), begin=0, end=2**31 - 1):
        """subscribe(string, list(integer), int, int) -> None

        Subscribe to one or more object values for the given interval.
        """
        Domain.subscribe(self, objectID, varIDs, begin, end)

    def subscribeContext(self, objectID, domain, dist, varIDs=(
            tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION), begin=0, end=2**31 - 1):
        """subscribe(string, int, double, list(integer), int, int) -> None

        Subscribe to one or more object values of the given domain around the
        given objectID in a given radius
        """
        Domain.subscribeContext(
            self, objectID, domain, dist, varIDs, begin, end)

    def addSubscriptionFilterLanes(self, lanes, noOpposite=False, downstreamDist=None, upstreamDist=None):
        """addSubscriptionFilterLanes(list(integer), bool, double, double) -> None

        Adds a lane-filter to the last modified vehicle context subscription (call it just after subscribing).
        lanes is a list of relative lane indices (-1 -> right neighboring lane of the ego, 0 -> ego lane, etc.)
        noOpposite specifies whether vehicles on opposite direction lanes shall be returned
        downstreamDist and upstreamDist specify the range of the search for surrounding vehicles along the road net.
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_LANES, lanes)
        if noOpposite:
            self.addSubscriptionFilterNoOpposite()
        if downstreamDist is not None:
            self.addSubscriptionFilterDownstreamDistance(downstreamDist)
        if upstreamDist is not None:
            self.addSubscriptionFilterUpstreamDistance(upstreamDist)

    def addSubscriptionFilterNoOpposite(self):
        """addSubscriptionFilterNoOpposite() -> None

        Omits vehicles on other edges than the ego's for the last modified vehicle context subscription
        (call it just after subscribing).
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_NOOPPOSITE)

    def addSubscriptionFilterDownstreamDistance(self, dist):
        """addSubscriptionFilterDownstreamDist(float) -> None

        Sets the downstream distance along the network for vehicles to be returned by the last modified
        vehicle context subscription (call it just after subscribing).
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_DOWNSTREAM_DIST, dist)

    def addSubscriptionFilterUpstreamDistance(self, dist):
        """addSubscriptionFilterUpstreamDist(float) -> None

        Sets the upstream distance along the network for vehicles to be returned by the last modified
        vehicle context subscription (call it just after subscribing).
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_UPSTREAM_DIST, dist)

    def addSubscriptionFilterCFManeuver(self, downstreamDist=None, upstreamDist=None):
        """addSubscriptionFilterCFManeuver() -> None

        Restricts vehicles returned by the last modified vehicle context subscription to leader and follower of the ego.
        downstreamDist and upstreamDist specify the range of the search for leader and follower along the road net.
        """
        self.addSubscriptionFilterLeadFollow([0])
        if downstreamDist is not None:
            self.addSubscriptionFilterDownstreamDistance(downstreamDist)
        if upstreamDist is not None:
            self.addSubscriptionFilterUpstreamDistance(upstreamDist)

    def addSubscriptionFilterLCManeuver(self, direction, noOpposite=False, downstreamDist=None, upstreamDist=None):
        """addSubscriptionFilterLCManeuver(int) -> None

        Restricts vehicles returned by the last modified vehicle context subscription to neighbor and ego-lane leader
        and follower of the ego.
        direction - lane change direction (in {-1=right, 1=left})
        noOpposite specifies whether vehicles on opposite direction lanes shall be returned
        downstreamDist and upstreamDist specify the range of the search for leader and follower along the road net.
        Combine with: distance filters; vClass/vType filter.
        """
        if direction is None:
            # Using default: both directions
            lanes = [-1, 0, 1]
        elif not (direction == -1 or direction == 1):
            warnings.warn("Ignoring lane change subscription filter " +
                          "with non-neighboring lane offset direction=%s." % direction)
            return
        else:
            lanes = [0, direction]
        self.addSubscriptionFilterLeadFollow(lanes)
        if noOpposite:
            self.addSubscriptionFilterNoOpposite()
        if downstreamDist is not None:
            self.addSubscriptionFilterDownstreamDistance(downstreamDist)
        if upstreamDist is not None:
            self.addSubscriptionFilterUpstreamDistance(upstreamDist)

    def addSubscriptionFilterLeadFollow(self, lanes):
        """addSubscriptionFilterLCManeuver() -> None

        Restricts vehicles returned by the last modified vehicle context subscription to neighbor and ego-lane leader
        and follower of the ego.
        Combine with: lanes-filter to restrict to one direction; distance filters; vClass/vType filter.
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_LEAD_FOLLOW)
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_LANES, lanes)

    def addSubscriptionFilterTurn(self, downstreamDist=None, upstreamDist=None):
        """addSubscriptionFilterTurn() -> None

        Restricts vehicles returned by the last modified vehicle context subscription to foes on an upcoming junction
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_TURN)
        if downstreamDist is not None:
            self.addSubscriptionFilterDownstreamDistance(downstreamDist)
        if upstreamDist is not None:
            self.addSubscriptionFilterUpstreamDistance(upstreamDist)

    def addSubscriptionFilterVClass(self, vClasses):
        """addSubscriptionFilterVClass(list(String)) -> None

        Restricts vehicles returned by the last modified vehicle context subscription to vehicles of the given classes
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_VCLASS, vClasses)

    def addSubscriptionFilterVType(self, vTypes):
        """addSubscriptionFilterVType(list(String)) -> None

        Restricts vehicles returned by the last modified vehicle context subscription to vehicles of the given types
        """
        self._connection._addSubscriptionFilter(tc.FILTER_TYPE_VTYPE, vTypes)
