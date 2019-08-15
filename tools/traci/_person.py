# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    _person.py
# @author  Jakob Erdmann
# @date    2015-02-06
# @version $Id$

from __future__ import absolute_import
import struct
from .domain import Domain
from .storage import Storage
from . import constants as tc
from . import _simulation as simulation


_RETURN_VALUE_FUNC = {tc.TRACI_ID_LIST: Storage.readStringList,
                      tc.ID_COUNT: Storage.readInt,
                      tc.VAR_SPEED: Storage.readDouble,
                      tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_POSITION3D: lambda result: result.read("!ddd"),
                      tc.VAR_ANGLE: Storage.readDouble,
                      tc.VAR_SLOPE: Storage.readDouble,
                      tc.VAR_ROAD_ID: Storage.readString,
                      tc.VAR_TYPE: Storage.readString,
                      tc.VAR_ROUTE_ID: Storage.readString,
                      tc.VAR_COLOR: lambda result: result.read("!BBBB"),
                      tc.VAR_LANEPOSITION: Storage.readDouble,
                      tc.VAR_LENGTH: Storage.readDouble,
                      tc.VAR_WAITING_TIME: Storage.readDouble,
                      tc.VAR_WIDTH: Storage.readDouble,
                      tc.VAR_MINGAP: Storage.readDouble,
                      tc.VAR_NEXT_EDGE: Storage.readString,
                      tc.VAR_STAGE: simulation._readStage,
                      tc.VAR_STAGES_REMAINING: Storage.readInt,
                      tc.VAR_VEHICLE: Storage.readString,
                      tc.VAR_EDGES: Storage.readStringList,
                      }


class PersonDomain(Domain):
    def __init__(self):
        Domain.__init__(self, "person", tc.CMD_GET_PERSON_VARIABLE, tc.CMD_SET_PERSON_VARIABLE,
                        tc.CMD_SUBSCRIBE_PERSON_VARIABLE, tc.RESPONSE_SUBSCRIBE_PERSON_VARIABLE,
                        tc.CMD_SUBSCRIBE_PERSON_CONTEXT, tc.RESPONSE_SUBSCRIBE_PERSON_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getSpeed(self, personID):
        """getSpeed(string) -> double

        Returns the speed in m/s of the named person within the last step.
        """
        return self._getUniversal(tc.VAR_SPEED, personID)

    def getPosition(self, personID):
        """getPosition(string) -> (double, double)

        Returns the position of the named person within the last step [m,m].
        """
        return self._getUniversal(tc.VAR_POSITION, personID)

    def getPosition3D(self, personID):
        """getPosition(string) -> (double, double, double)

        Returns the position of the named person within the last step [m,m,m].
        """
        return self._getUniversal(tc.VAR_POSITION3D, personID)

    def getAngle(self, personID):
        """getAngle(string) -> double

        Returns the angle in degrees of the named person within the last step.
        """
        return self._getUniversal(tc.VAR_ANGLE, personID)

    def getSlope(self, personID):
        """getSlope(string) -> double

        Returns the slope at the current position of the person in degrees
        """
        return self._getUniversal(tc.VAR_SLOPE, personID)

    def getRoadID(self, personID):
        """getRoadID(string) -> string

        Returns the id of the edge the named person was at within the last step.
        """
        return self._getUniversal(tc.VAR_ROAD_ID, personID)

    def getTypeID(self, personID):
        """getTypeID(string) -> string

        Returns the id of the type of the named person.
        """
        return self._getUniversal(tc.VAR_TYPE, personID)

    def getLanePosition(self, personID):
        """getLanePosition(string) -> double

        The position of the person along the lane measured in m.
        """
        return self._getUniversal(tc.VAR_LANEPOSITION, personID)

    def getColor(self, personID):
        """getColor(string) -> (integer, integer, integer, integer)

        Returns the person's rgba color.
        """
        return self._getUniversal(tc.VAR_COLOR, personID)

    def getLength(self, personID):
        """getLength(string) -> double

        Returns the length in m of the given person.
        """
        return self._getUniversal(tc.VAR_LENGTH, personID)

    def getWaitingTime(self, personID):
        """getWaitingTime() -> double
        The waiting time of a person is defined as the time (in seconds) spent with a
        speed below 0.1m/s since the last time it was faster than 0.1m/s.
        (basically, the waiting time of a person is reset to 0 every time it moves).
        """
        return self._getUniversal(tc.VAR_WAITING_TIME, personID)

    def getWidth(self, personID):
        """getWidth(string) -> double

        Returns the width in m of this person.
        """
        return self._getUniversal(tc.VAR_WIDTH, personID)

    def getMinGap(self, personID):
        """getMinGap(string) -> double

        Returns the offset (gap to front person if halting) of this person.
        """
        return self._getUniversal(tc.VAR_MINGAP, personID)

    def getNextEdge(self, personID):
        """getNextEdge() -> string
        If the person is walking, returns the next edge on the persons route
        (including crossing and walkingareas). If there is no further edge or the
        person is in another stage, returns the empty string.
        """
        return self._getUniversal(tc.VAR_NEXT_EDGE, personID)

    def getEdges(self, personID, nextStageIndex=0):
        """getEdges(string, int) -> list(string)

        Returns a list of all edges in the nth next stage.
        For waiting stages this is a single edge
        For walking stages this is the complete route
        For driving stages this is [origin, destination]

        nextStageIndex 0 retrieves value for the current stage.
        nextStageIndex must be lower then value of getRemainingStages(personID)
        """
        self._connection._beginMessage(
            tc.CMD_GET_PERSON_VARIABLE, tc.VAR_EDGES, personID, 1 + 4)
        self._connection._string += struct.pack("!Bi",
                                                tc.TYPE_INTEGER, nextStageIndex)
        return self._connection._checkResult(tc.CMD_GET_PERSON_VARIABLE,
                                             tc.VAR_EDGES, personID).readStringList()

    def getStage(self, personID, nextStageIndex=0):
        """getStage(string, int) -> int
        Returns the type of the nth next stage
          0 for not-yet-departed
          1 for waiting
          2 for walking
          3 for driving
          4 for access to busStop or trainStop
          5 for personTrip
        nextStageIndex 0 retrieves value for the current stage.
        nextStageIndex must be lower then value of getRemainingStages(personID)
        """
        self._connection._beginMessage(
            tc.CMD_GET_PERSON_VARIABLE, tc.VAR_STAGE, personID, 1 + 4)
        self._connection._string += struct.pack("!Bi",
                                                tc.TYPE_INTEGER, nextStageIndex)
        return simulation._readStage(self._connection._checkResult(tc.CMD_GET_PERSON_VARIABLE,
                                                                   tc.VAR_STAGE, personID))

    def getRemainingStages(self, personID):
        """getStage(string) -> int
        Returns the number of remaining stages (at least 1)
        """
        return self._getUniversal(tc.VAR_STAGES_REMAINING, personID)

    def getVehicle(self, personID):
        """getVehicle(string) -> string
        Returns the id of the current vehicle if the person is in stage driving
        and has entered a vehicle.
        Return the empty string otherwise
        """
        return self._getUniversal(tc.VAR_VEHICLE, personID)

    def removeStages(self, personID):
        """remove(string)
        Removes all stages of the person. If no new phases are appended,
        the person will be removed from the simulation in the next simulationStep().
        """
        # remove all stages after the current and then abort the current stage
        while self.getRemainingStages(personID) > 1:
            self.removeStage(personID, 1)
        self.removeStage(personID, 0)

    def add(self, personID, edgeID, pos, depart=tc.DEPARTFLAG_NOW, typeID="DEFAULT_PEDTYPE"):
        """add(string, string, double, double, string)
        Inserts a new person to the simulation at the given edge, position and
        time (in s). This function should be followed by appending Stages or the person
        will immediately vanish on departure.
        """
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.ADD, personID,
                                       1 + 4 + 1 + 4 + len(typeID) + 1 + 4 + len(edgeID) + 1 + 8 + 1 + 8)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
        self._connection._packString(typeID)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, depart)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, pos)
        self._connection._sendExact()

    def appendWaitingStage(self, personID, duration, description="waiting", stopID=""):
        """appendWaitingStage(string, float, string, string)
        Appends a waiting stage with duration in s to the plan of the given person
        """
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.APPEND_STAGE, personID,
                                       1 + 4 +  # compound
                                       1 + 4 +  # stage type
                                       1 + 8 +  # duration
                                       1 + 4 + len(description) +
                                       1 + 4 + len(stopID))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
        self._connection._string += struct.pack(
            "!Bi", tc.TYPE_INTEGER, tc.STAGE_WAITING)
        self._connection._string += struct.pack("!Bd",
                                                tc.TYPE_DOUBLE, duration)
        self._connection._packString(description)
        self._connection._packString(stopID)
        self._connection._sendExact()

    def appendWalkingStage(self, personID, edges, arrivalPos, duration=-1, speed=-1, stopID=""):
        """appendWalkingStage(string, stringList, double, double, double, string)
        Appends a walking stage to the plan of the given person
        The walking speed can either be specified, computed from the duration parameter (in s) or taken from the
        type of the person
        """
        if isinstance(edges, str):
            edges = [edges]
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.APPEND_STAGE, personID,
                                       1 + 4 +  # compound
                                       1 + 4 +  # stageType
                                       1 + 4 + \
                                       sum(map(len, edges)) + 4 * len(edges) +
                                       1 + 8 +  # arrivalPos
                                       1 + 8 +  # duration
                                       1 + 8 +  # speed
                                       1 + 4 + len(stopID)
                                       )
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 6)
        self._connection._string += struct.pack(
            "!Bi", tc.TYPE_INTEGER, tc.STAGE_WALKING)
        self._connection._packStringList(edges)
        self._connection._string += struct.pack("!Bd",
                                                tc.TYPE_DOUBLE, arrivalPos)
        self._connection._string += struct.pack("!Bd",
                                                tc.TYPE_DOUBLE, duration)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, speed)
        self._connection._packString(stopID)
        self._connection._sendExact()

    def appendStage(self, personID, stage):
        """appendStage(string, stage)
        Appends a stage object to the plan of the given person
        Such an object is obtainable using getStage
        """
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.APPEND_STAGE,
                                       personID, simulation._stageSize(stage))
        simulation._writeStage(stage, self._connection)
        self._connection._sendExact()

    def replaceStage(self, personID, stageIndex, stage):
        """replaceStage(string, int, stage)
        Replaces the nth subsequent stage with the given stage object
        Such an object is obtainable using getStage
        """
        msgSize = (1 + 4  # compound
                   + 1 + 4  # stageIndex
                   + simulation._stageSize(stage))

        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.REPLACE_STAGE, personID, msgSize)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, stageIndex)
        simulation._writeStage(stage, self._connection)
        self._connection._sendExact()

    def appendDrivingStage(self, personID, toEdge, lines, stopID=""):
        """appendDrivingStage(string, string, string, string)
        Appends a driving stage to the plan of the given person
        The lines parameter should be a space-separated list of line ids
        """
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.APPEND_STAGE, personID,
                                       1 + 4 +  # compound
                                       1 + 4 +  # stage type
                                       1 + 4 + len(toEdge) +
                                       1 + 4 + len(lines) +
                                       1 + 4 + len(stopID))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
        self._connection._string += struct.pack(
            "!Bi", tc.TYPE_INTEGER, tc.STAGE_DRIVING)
        self._connection._packString(toEdge)
        self._connection._packString(lines)
        self._connection._packString(stopID)
        self._connection._sendExact()

    def removeStage(self, personID, nextStageIndex):
        """removeStage(string, int)
        Removes the nth next stage
        nextStageIndex must be lower then value of getRemainingStages(personID)
        nextStageIndex 0 immediately aborts the current stage and proceeds to the next stage
        """
        self._connection._beginMessage(
            tc.CMD_SET_PERSON_VARIABLE, tc.REMOVE_STAGE, personID, 1 + 4)
        self._connection._string += struct.pack("!Bi",
                                                tc.TYPE_INTEGER, nextStageIndex)
        self._connection._sendExact()

    def rerouteTraveltime(self, personID):
        """rerouteTraveltime(string) -> None Reroutes a pedestrian (walking person).
        """
        self._connection._beginMessage(
            tc.CMD_SET_PERSON_VARIABLE, tc.CMD_REROUTE_TRAVELTIME, personID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
        self._connection._sendExact()

    def moveToXY(self, personID, edgeID, x, y, angle=tc.INVALID_DOUBLE_VALUE, keepRoute=1):
        '''Place person at the given x,y coordinates and force it's angle to
        the given value (for drawing).
        If the angle is set to INVALID_DOUBLE_VALUE, the vehicle assumes the
        natural angle of the edge on which it is driving.
        If keepRoute is set to 1, the closest position
        within the existing route is taken. If keepRoute is set to 0, the vehicle may move to
        any edge in the network but it's route then only consists of that edge.
        If keepRoute is set to 2 the person has all the freedom of keepRoute=0
        but in addition to that may even move outside the road network.
        edgeID is an optional placement hint to resolve ambiguities'''
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.MOVE_TO_XY,
                                       personID, 1 + 4 + 1 + 4 + len(edgeID) + 1 + 8 + 1 + 8 + 1 + 8 + 1 + 1)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 5)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, x)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, y)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, angle)
        self._connection._string += struct.pack("!BB", tc.TYPE_BYTE, keepRoute)
        self._connection._sendExact()

    def setSpeed(self, personID, speed):
        """setSpeed(string, double) -> None

        Sets the maximum speed in m/s for the named person for subsequent step.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_PERSON_VARIABLE, tc.VAR_SPEED, personID, speed)

    def setType(self, personID, typeID):
        """setType(string, string) -> None

        Sets the id of the type for the named person.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_PERSON_VARIABLE, tc.VAR_TYPE, personID, typeID)

    def setWidth(self, personID, width):
        """setWidth(string, double) -> None

        Sets the width in m for this person.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_PERSON_VARIABLE, tc.VAR_WIDTH, personID, width)

    def setHeight(self, personID, height):
        """setHeight(string, double) -> None

        Sets the height in m for this person.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_PERSON_VARIABLE, tc.VAR_HEIGHT, personID, height)

    def setLength(self, personID, length):
        """setLength(string, double) -> None

        Sets the length in m for the given person.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_PERSON_VARIABLE, tc.VAR_LENGTH, personID, length)

    def setMinGap(self, personID, minGap):
        """setMinGap(string, double) -> None

        Sets the offset (gap to front person if halting) for this vehicle.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_PERSON_VARIABLE, tc.VAR_MINGAP, personID, minGap)

    def setColor(self, personID, color):
        """setColor(string, (integer, integer, integer, integer))

        Sets the color for the vehicle with the given ID, i.e. (255,0,0) for the color red.
        The fourth component (alpha) is optional.
        """
        self._connection._beginMessage(
            tc.CMD_SET_PERSON_VARIABLE, tc.VAR_COLOR, personID, 1 + 1 + 1 + 1 + 1)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]),
                                                int(color[3]) if len(color) > 3 else 255)
        self._connection._sendExact()
