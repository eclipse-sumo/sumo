# -*- coding: utf-8 -*-
"""
@file    vehicle.py
@author  Michael Behrisch
@author  Lena Kalleske
@author  Mario Krumnow
@author  Lena Kalleske
@author  Jakob Erdmann
@author  Laura Bieker
@author  Daniel Krajzewicz
@date    2011-03-09
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


def _readBestLanes(result):
    result.read("!iB")
    nbLanes = result.read("!i")[0]  # Length
    lanes = []
    for i in range(nbLanes):
        result.read("!B")
        laneID = result.readString()
        length, occupation, offset = result.read("!BdBdBb")[1::2]
        allowsContinuation = result.read("!BB")[1]
        nextLanesNo = result.read("!Bi")[1]
        nextLanes = []
        for j in range(nextLanesNo):
            nextLanes.append(result.readString())
        lanes.append(
            [laneID, length, occupation, offset, allowsContinuation, nextLanes])
    return lanes


def _readLeader(result):
    result.read("!iB")
    vehicleID = result.readString()
    result.read("!B")
    dist = result.readDouble()
    if vehicleID:
        return vehicleID, dist
    return None


def _readNextTLS(result):
    result.read("!iB")  # numCompounds, TYPE_INT
    numTLS = result.read("!i")[0]
    nextTLS = []
    for i in range(numTLS):
        result.read("!B")
        tlsID = result.readString()
        tlsIndex, dist, state = result.read("!BiBdBB")[1::2]
        nextTLS.append((tlsID, tlsIndex, dist, chr(state)))
    return nextTLS


_RETURN_VALUE_FUNC = {tc.VAR_SPEED:           Storage.readDouble,
                      tc.VAR_SPEED_WITHOUT_TRACI: Storage.readDouble,
                      tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_POSITION3D: lambda result: result.read("!ddd"),
                      tc.VAR_ANGLE:           Storage.readDouble,
                      tc.VAR_ROAD_ID:         Storage.readString,
                      tc.VAR_LANE_ID:         Storage.readString,
                      tc.VAR_LANE_INDEX:      Storage.readInt,
                      tc.VAR_TYPE:            Storage.readString,
                      tc.VAR_ROUTE_ID:        Storage.readString,
                      tc.VAR_ROUTE_INDEX:     Storage.readInt,
                      tc.VAR_COLOR: lambda result: result.read("!BBBB"),
                      tc.VAR_LANEPOSITION:    Storage.readDouble,
                      tc.VAR_CO2EMISSION:     Storage.readDouble,
                      tc.VAR_COEMISSION:      Storage.readDouble,
                      tc.VAR_HCEMISSION:      Storage.readDouble,
                      tc.VAR_PMXEMISSION:     Storage.readDouble,
                      tc.VAR_NOXEMISSION:     Storage.readDouble,
                      tc.VAR_FUELCONSUMPTION: Storage.readDouble,
                      tc.VAR_NOISEEMISSION:   Storage.readDouble,
                      tc.VAR_ELECTRICITYCONSUMPTION: Storage.readDouble,
                      tc.VAR_PERSON_NUMBER:   Storage.readInt,
                      tc.VAR_EDGE_TRAVELTIME: Storage.readDouble,
                      tc.VAR_EDGE_EFFORT:     Storage.readDouble,
                      tc.VAR_ROUTE_VALID: lambda result: bool(result.read("!B")[0]),
                      tc.VAR_EDGES:           Storage.readStringList,
                      tc.VAR_SIGNALS:         Storage.readInt,
                      tc.VAR_LENGTH:          Storage.readDouble,
                      tc.VAR_MAXSPEED:        Storage.readDouble,
                      tc.VAR_ALLOWED_SPEED:   Storage.readDouble,
                      tc.VAR_VEHICLECLASS:    Storage.readString,
                      tc.VAR_SPEED_FACTOR:    Storage.readDouble,
                      tc.VAR_SPEED_DEVIATION: Storage.readDouble,
                      tc.VAR_EMISSIONCLASS:   Storage.readString,
                      tc.VAR_WAITING_TIME:    Storage.readDouble,
                      tc.VAR_SPEEDSETMODE:    Storage.readInt,
                      tc.VAR_SLOPE:           Storage.readDouble,
                      tc.VAR_WIDTH:           Storage.readDouble,
                      tc.VAR_HEIGHT:          Storage.readDouble,
                      tc.VAR_LINE:            Storage.readString,
                      tc.VAR_VIA:             Storage.readStringList,
                      tc.VAR_MINGAP:          Storage.readDouble,
                      tc.VAR_SHAPECLASS:      Storage.readString,
                      tc.VAR_ACCEL:           Storage.readDouble,
                      tc.VAR_DECEL:           Storage.readDouble,
                      tc.VAR_IMPERFECTION:    Storage.readDouble,
                      tc.VAR_TAU:             Storage.readDouble,
                      tc.VAR_BEST_LANES:      _readBestLanes,
                      tc.VAR_LEADER:          _readLeader,
                      tc.VAR_NEXT_TLS:        _readNextTLS,
                      tc.DISTANCE_REQUEST:    Storage.readDouble,
                      tc.VAR_STOPSTATE: lambda result: result.read("!B")[0],
                      tc.VAR_DISTANCE:        Storage.readDouble}


class VehicleDomain(Domain):

    DEPART_TRIGGERED = -1
    DEPART_CONTAINER_TRIGGERED = -2
    DEPART_NOW = -3

    DEPART_SPEED_RANDOM = -2
    DEPART_SPEED_MAX = -3

    STOP_DEFAULT = 0
    STOP_PARKING = 1
    STOP_TRIGGERED = 2
    STOP_CONTAINER_TRIGGERED = 4
    STOP_BUS_STOP = 8
    STOP_CONTAINER_STOP = 16

    DEPART_LANE_RANDOM = -2
    DEPART_LANE_FREE = -3
    DEPART_LANE_ALLOWED_FREE = -4
    DEPART_LANE_BEST_FREE = -5
    DEPART_LANE_FIRST_ALLOWED = -6

    def __init__(self):
        Domain.__init__(self, "vehicle", tc.CMD_GET_VEHICLE_VARIABLE, tc.CMD_SET_VEHICLE_VARIABLE,
                        tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE, tc.RESPONSE_SUBSCRIBE_VEHICLE_VARIABLE,
                        tc.CMD_SUBSCRIBE_VEHICLE_CONTEXT, tc.RESPONSE_SUBSCRIBE_VEHICLE_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getSpeed(self, vehID):
        """getSpeed(string) -> double

        Returns the speed in m/s of the named vehicle within the last step.
        """
        return self._getUniversal(tc.VAR_SPEED, vehID)

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

        Returns the CO2 emission in mg for the last time step.
        """
        return self._getUniversal(tc.VAR_CO2EMISSION, vehID)

    def getCOEmission(self, vehID):
        """getCOEmission(string) -> double

        Returns the CO emission in mg for the last time step.
        """
        return self._getUniversal(tc.VAR_COEMISSION, vehID)

    def getHCEmission(self, vehID):
        """getHCEmission(string) -> double

        Returns the HC emission in mg for the last time step.
        """
        return self._getUniversal(tc.VAR_HCEMISSION, vehID)

    def getPMxEmission(self, vehID):
        """getPMxEmission(string) -> double

        Returns the particular matter emission in mg for the last time step.
        """
        return self._getUniversal(tc.VAR_PMXEMISSION, vehID)

    def getNOxEmission(self, vehID):
        """getNOxEmission(string) -> double

        Returns the NOx emission in mg for the last time step.
        """
        return self._getUniversal(tc.VAR_NOXEMISSION, vehID)

    def getFuelConsumption(self, vehID):
        """getFuelConsumption(string) -> double

        Returns the fuel consumption in ml for the last time step.
        """
        return self._getUniversal(tc.VAR_FUELCONSUMPTION, vehID)

    def getNoiseEmission(self, vehID):
        """getNoiseEmission(string) -> double

        Returns the noise emission in db for the last time step.
        """
        return self._getUniversal(tc.VAR_NOISEEMISSION, vehID)

    def getElectricityConsumption(self, vehID):
        """getElectricityConsumption(string) -> double

        Returns the electricity consumption in ml for the last time step.
        """
        return self._getUniversal(tc.VAR_ELECTRICITYCONSUMPTION, vehID)

    def getPersonNumber(self, vehID):
        """getPersonNumber(string) -> integer
        Returns the total number of persons which includes those defined
        using attribute 'personNumber' as well as <person>-objects which are riding in
        this vehicle.
        """
        return self._getUniversal(tc.VAR_PERSON_NUMBER, vehID)

    def getAdaptedTraveltime(self, vehID, time, edgeID):
        """getAdaptedTraveltime(string, double, string) -> double

        .
        """
        self._connection._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE,
                                       tc.VAR_EDGE_TRAVELTIME, vehID, 1 + 4 + 1 + 4 + 1 + 4 + len(edgeID))
        self._connection._string += struct.pack(
            "!BiBi", tc.TYPE_COMPOUND, 2, tc.TYPE_INTEGER, time)
        self._connection._packString(edgeID)
        return self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, vehID).readDouble()

    def getEffort(self, vehID, time, edgeID):
        """getEffort(string, double, string) -> double

        .
        """
        self._connection._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE,
                                       tc.VAR_EDGE_EFFORT, vehID, 1 + 4 + 1 + 4 + 1 + 4 + len(edgeID))
        self._connection._string += struct.pack(
            "!BiBi", tc.TYPE_COMPOUND, 2, tc.TYPE_INTEGER, time)
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

        Returns the maximum speed deviation of the vehicle type.
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
        """
        return self._getUniversal(tc.VAR_WAITING_TIME, vehID)

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

        Returns the maximum deceleration possibility in m/s^2 of this vehicle.
        """
        return self._getUniversal(tc.VAR_DECEL, vehID)

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

        Return the leading vehicle id together with the distance.
        The dist parameter defines the maximum lookahead, 0 calculates a lookahead from the brake gap.
        """
        self._connection._beginMessage(
            tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_LEADER, vehID, 1 + 8)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, dist)
        return _readLeader(self._connection._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_LEADER, vehID))

    def getNextTLS(self, vehID):
        """getNextTLS(string) -> 

        Return list of upcoming traffic lights [(tlsID, tlsIndex, distance, state), ...]
        """
        return self._getUniversal(tc.VAR_NEXT_TLS, vehID)

    def subscribeLeader(self, vehID, dist=0., begin=0, end=2**31 - 1):
        """subscribeLeader(string, double) -> None

        Subscribe for the leading vehicle id together with the distance.
        The dist parameter defines the maximum lookahead, 0 calculates a lookahead from the brake gap.
        """
        self._connection._subscribe(tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE, begin, end, vehID,
                                    (tc.VAR_LEADER,), {tc.VAR_LEADER: struct.pack("!Bd", tc.TYPE_DOUBLE, dist)})

    def getDrivingDistance(self, vehID, edgeID, pos, laneID=0):
        """getDrivingDistance(string, string, double, integer) -> double

        Return the distance to the given edge and position along the vehicles route.
        """
        self._connection._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST,
                                       vehID, 1 + 4 + 1 + 4 + len(edgeID) + 8 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._packString(edgeID, tc.POSITION_ROADMAP)
        self._connection._string += struct.pack("!dBB",
                                                pos, laneID, tc.REQUEST_DRIVINGDIST)
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

    def setMaxSpeed(self, vehID, speed):
        """setMaxSpeed(string, double) -> None

        Sets the maximum speed in m/s for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_MAXSPEED, vehID, speed)

    def setStop(self, vehID, edgeID, pos=1., laneIndex=0, duration=2**31 - 1, flags=STOP_DEFAULT, startPos=tc.INVALID_DOUBLE_VALUE, until=-1):
        """setStop(string, string, double, integer, integer, integer, double, integer) -> None

        Adds or modifies a stop with the given parameters. The duration and the until attribute are
        in milliseconds.
        """
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_STOP,
                                       vehID, 1 + 4 + 1 + 4 + len(edgeID) + 1 + 8 + 1 + 1 + 1 + 4 + 1 + 1 + 1 + 8 + 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 7)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!BdBBBiBB", tc.TYPE_DOUBLE, pos,
                                                tc.TYPE_BYTE, laneIndex, tc.TYPE_INTEGER, duration, tc.TYPE_BYTE, flags)
        self._connection._string += struct.pack("!BdBi",
                                                tc.TYPE_DOUBLE, startPos, tc.TYPE_INTEGER, until)
        self._connection._sendExact()

    def setBusStop(self, vehID, stopID, duration=2**31 - 1, until=-1, flags=STOP_DEFAULT):
        """setBusStop(string, string, integer, integer, integer) -> None

        Adds or modifies a bus stop with the given parameters. The duration and the until attribute are
        in milliseconds.
        """
        self.setStop(vehID, stopID, duration=duration,
                     until=until, flags=flags | self.STOP_BUS_STOP)

    def setContainerStop(self, vehID, stopID, duration=2**31 - 1, until=-1, flags=STOP_DEFAULT):
        """setContainerStop(string, string, integer, integer, integer) -> None

        Adds or modifies a container stop with the given parameters. The duration and the until attribute are
        in milliseconds.
        """
        self.setStop(vehID, stopID, duration=duration, until=until,
                     flags=flags | self.STOP_CONTAINER_STOP)

    def resume(self, vehID):
        """resume(string) -> None

        Resumes the vehicle from the current stop (throws an error if the vehicle is not stopped).
        """
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_RESUME, vehID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
        self._connection._sendExact()

    def changeLane(self, vehID, laneIndex, duration):
        """changeLane(string, int, int) -> None

        Forces a lane change to the lane with the given index; if successful,
        the lane will be chosen for the given amount of time (in ms). 
        """
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGELANE, vehID, 1 + 4 + 1 + 1 + 1 + 4)
        self._connection._string += struct.pack(
            "!BiBBBi", tc.TYPE_COMPOUND, 2, tc.TYPE_BYTE, laneIndex, tc.TYPE_INTEGER, duration)
        self._connection._sendExact()

    def slowDown(self, vehID, speed, duration):
        """slowDown(string, double, int) -> None

        Changes the speed smoothly to the given value over the given amount
        of time in ms (can also be used to increase speed). 
        """
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_SLOWDOWN, vehID, 1 + 4 + 1 + 8 + 1 + 4)
        self._connection._string += struct.pack(
            "!BiBdBi", tc.TYPE_COMPOUND, 2, tc.TYPE_DOUBLE, speed, tc.TYPE_INTEGER, duration)
        self._connection._sendExact()

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

    def setAdaptedTraveltime(self, vehID, begTime, endTime, edgeID, time):
        """setAdaptedTraveltime(string, double, string, double) -> None

        Inserts the information about the travel time of edge "edgeID" valid
        from begin time to end time into the vehicle's internal edge weights
        container. .
        """
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME,
                                       vehID, 1 + 4 + 1 + 4 + 1 + 4 + 1 + 4 + len(edgeID) + 1 + 8)
        self._connection._string += struct.pack("!BiBiBi", tc.TYPE_COMPOUND, 4, tc.TYPE_INTEGER, begTime,
                                                tc.TYPE_INTEGER, endTime)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, time)
        self._connection._sendExact()

    def setEffort(self, vehID, begTime, endTime, edgeID, effort):
        """setEffort(string, double, string, double) -> None

        Inserts the information about the effort of edge "edgeID" valid from
        begin time to end time into the vehicle's internal edge weights
        container.
        """
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT,
                                       vehID, 1 + 4 + 1 + 4 + 1 + 4 + 1 + 4 + len(edgeID) + 1 + 4)
        self._connection._string += struct.pack("!BiBiBi", tc.TYPE_COMPOUND, 4, tc.TYPE_INTEGER, begTime,
                                                tc.TYPE_INTEGER, endTime)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, effort)
        self._connection._sendExact()

    LAST_TRAVEL_TIME_UPDATE = -1

    def rerouteTraveltime(self, vehID, currentTravelTimes=True):
        """rerouteTraveltime(string, bool) -> None Reroutes a vehicle. If
        currentTravelTimes is True (default) then the current traveltime of the
        edges is loaded and used for rerouting. If currentTravelTimes is False
        custom travel times are used. The various functions and options for
        customizing travel times are described at http://sumo.dlr.de/wiki/Simulation/Routing

        When rerouteTravelTime has been called once with option
        currentTravelTimes=True, all edge weights are set to the current travel
        times at the time of that call (even for subsequent simulation steps). 
        """
        if currentTravelTimes:
            time = self._connection.simulation.getCurrentTime()
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
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SPEED, vehID, speed)

    def setColor(self, vehID, color):
        """setColor(string, (integer, integer, integer, integer))
        sets color for vehicle with the given ID.
        i.e. (255,0,0,0) for the color red. 
        The fourth integer (alpha) is only used when drawing vehicles with raster images
        """
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_COLOR, vehID, 1 + 1 + 1 + 1 + 1)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(
            color[0]), int(color[1]), int(color[2]), int(color[3]))
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

        Sets the maximum deceleration in m/s^2 for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_DECEL, vehID, decel)

    def setImperfection(self, vehID, imperfection):
        """setImperfection(string, double) -> None

        Sets the driver imperfection sigma.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_IMPERFECTION, vehID, imperfection)

    def setTau(self, vehID, tau):
        """setTau(string, double) -> None

        Sets the driver's reaction time in s for this vehicle.
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

    def add(self, vehID, routeID, depart=DEPART_NOW, pos=0, speed=0,
            lane=DEPART_LANE_FIRST_ALLOWED, typeID="DEFAULT_VEHTYPE"):
        """
        Add a new vehicle (old style)
        """
        self._connection._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.ADD, vehID,
                                       1 + 4 + 1 + 4 + len(typeID) + 1 + 4 + len(routeID) + 1 + 4 + 1 + 8 + 1 + 8 + 1 + 1)
        if depart > 0:
            depart *= 1000
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 6)
        self._connection._packString(typeID)
        self._connection._packString(routeID)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, depart)
        self._connection._string += struct.pack("!BdBd",
                                                tc.TYPE_DOUBLE, pos, tc.TYPE_DOUBLE, speed)
        self._connection._string += struct.pack("!Bb", tc.TYPE_BYTE, lane)
        self._connection._sendExact()

    def addFull(self, vehID, routeID, typeID="DEFAULT_VEHTYPE", depart=None,
                departLane="first", departPos="base", departSpeed="0",
                arrivalLane="current", arrivalPos="max", arrivalSpeed="current",
                fromTaz="", toTaz="", line="", personCapacity=0, personNumber=0):
        """
        Add a new vehicle (new style with all possible parameters)
        """
        messageString = struct.pack("!Bi", tc.TYPE_COMPOUND, 14)
        if depart is None:
            depart = str(self._connection.simulation.getCurrentTime() / 1000.)
        for val in (routeID, typeID, depart, departLane, departPos, departSpeed,
                    arrivalLane, arrivalPos, arrivalSpeed, fromTaz, toTaz, line):
            messageString += struct.pack("!Bi",
                                         tc.TYPE_STRING, len(val)) + str(val).encode("latin1")
        messageString += struct.pack("!Bi", tc.TYPE_INTEGER, personCapacity)
        messageString += struct.pack("!Bi", tc.TYPE_INTEGER, personNumber)

        self._connection._beginMessage(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.ADD_FULL, vehID, len(messageString))
        self._connection._string += messageString
        self._connection._sendExact()

    def remove(self, vehID, reason=tc.REMOVE_VAPORIZED):
        '''Remove vehicle with the given ID for the give reason. 
           Reasons are defined in module constants and start with REMOVE_'''
        self._connection._sendByteCmd(
            tc.CMD_SET_VEHICLE_VARIABLE, tc.REMOVE, vehID, reason)

    def moveToXY(self, vehID, edgeID, lane, x, y, angle, keepRoute=1):
        '''Place vehicle at the given x,y coordinates and force it's angle to
        the given value (for drawing). If keepRoute is set to 1, the closest position
        within the existing route is taken. If keepRoute is set to 0, the vehicle may move to
        any edge in the network but it's route then only consists of that edge.
        If keepRoute is set to 2 the vehicle has all the freedom of keepRoute=0
        but in addition to that may even move outside the road network.
        edgeID and lane are optional placement hints to resovle ambiguities'''
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

    moveToVTD = moveToXY  # deprecated method name for backwards compatibility

    def subscribe(self, objectID, varIDs=(tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION), begin=0, end=2**31 - 1):
        """subscribe(string, list(integer), int, int) -> None

        Subscribe to one or more object values for the given interval.
        """
        Domain.subscribe(self, objectID, varIDs, begin, end)

    def subscribeContext(self, objectID, domain, dist, varIDs=(tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION), begin=0, end=2**31 - 1):
        """subscribe(string, int, double, list(integer), int, int) -> None

        Subscribe to one or more object values of the given domain around the
        given objectID in a given radius
        """
        Domain.subscribeContext(
            self, objectID, domain, dist, varIDs, begin, end)

VehicleDomain()
