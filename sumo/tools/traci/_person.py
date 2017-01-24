# -*- coding: utf-8 -*-
"""
@file    person.py
@author  Jakob Erdmann
@date    2015-02-06
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


_RETURN_VALUE_FUNC = {tc.ID_LIST:             Storage.readStringList,
                      tc.ID_COUNT:            Storage.readInt,
                      tc.VAR_SPEED:           Storage.readDouble,
                      tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_ANGLE:           Storage.readDouble,
                      tc.VAR_ROAD_ID:         Storage.readString,
                      tc.VAR_TYPE:            Storage.readString,
                      tc.VAR_ROUTE_ID:        Storage.readString,
                      tc.VAR_COLOR: lambda result: result.read("!BBBB"),
                      tc.VAR_LANEPOSITION:    Storage.readDouble,
                      tc.VAR_LENGTH:          Storage.readDouble,
                      tc.VAR_WAITING_TIME:    Storage.readDouble,
                      tc.VAR_WIDTH:           Storage.readDouble,
                      tc.VAR_MINGAP:          Storage.readDouble,
                      tc.VAR_NEXT_EDGE:       Storage.readString,
                      tc.VAR_STAGE:           Storage.readInt,
                      tc.VAR_STAGES_REMAINING: Storage.readInt,
                      tc.VAR_VEHICLE:         Storage.readString,
                      tc.VAR_EDGES:           Storage.readStringList,
                      }


class PersonDomain(Domain):
    DEPART_NOW = -3

    STAGE_DRIVING = 0
    STAGE_WAITING = 1
    STAGE_WALKING = 2
    STAGE_AWAITING_DEPARTURE = 3

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
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, nextStageIndex)
        return self._connection._checkResult(tc.CMD_GET_PERSON_VARIABLE,
                                             tc.VAR_EDGES, personID).readStringList()


    def getStage(self, personID, nextStageIndex=0):
        """getStage(string, int) -> int
        Returns the type of the nth next stage
          0 for driving
          1 for waiting
          2 for walking
          3 for not-yet-departed
        nextStageIndex 0 retrieves value for the current stage.
        nextStageIndex must be lower then value of getRemainingStages(personID)
        """
        self._connection._beginMessage(
            tc.CMD_GET_PERSON_VARIABLE, tc.VAR_STAGE, personID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, nextStageIndex)
        return self._connection._checkResult(tc.CMD_GET_PERSON_VARIABLE,
                                             tc.VAR_STAGE, personID).readInt()

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


    def remove(self, personID):
        """remove(string)
        Removes the person from the simulation
        """
        # remove all stages after the current and then abort the current stage
        while getRemainingStages(personID) > 1:
            removeStage(1);
        removeStage(0);


    def add(self, personID, edgeID, pos, depart=DEPART_NOW, typeID="DEFAULT_PEDTYPE"):
        """add(string, string, double, int, string)
        Inserts a new person to the simulation at the given edge, position and
        time (in s). This function should be followed by appending Stages or the person
        will immediatly vanish on departure.
        """
        if depart > 0:
            depart *= 1000
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.ADD, personID,
                                       1 + 4 + 1 + 4 + len(typeID) + 1 + 4 + len(edgeID) + 1 + 4 + 1 + 8)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
        self._connection._packString(typeID)
        self._connection._packString(edgeID)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, depart)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, pos)
        self._connection._sendExact()

    def appendWaitingStage(self, personID, duration, description="waiting", stopID=""):
        """appendWaitingStage(string, int, string, string)
        Appends a waiting stage with duration in s to the plan of the given person
        """
        duration *= 1000
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.APPEND_STAGE, personID,
                                       1 + 4  # compound
                                       + 1 + 4 # stage type
                                       + 1 + 4 # duration
                                       + 1 + 4 + len(description) 
                                       + 1 + 4 + len(stopID))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, self.STAGE_WAITING)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, duration)
        self._connection._packString(description)
        self._connection._packString(stopID)
        self._connection._sendExact()

    def appendWalkingStage(self, personID, edges, arrivalPos, duration=-1, speed=-1, stopID=""):
        """appendWalkingStage(string, stringList, double, int, double, string)
        Appends a walking stage to the plan of the given person
        The walking speed can either be specified, computed from the duration parameter (in s) or taken from the type of the person
        """
        if duration is not None:
            duration *= 1000

        if isinstance(edges, str):
            edges = [edgeList]
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.APPEND_STAGE, personID,
                                       1 + 4  # compound
                                       + 1 + 4  # stageType
                                       + 1 + 4 + sum(map(len, edges)) + 4 * len(edges) 
                                       + 1 + 8 # arrivalPos
                                       + 1 + 4 # duration
                                       + 1 + 8 # speed
                                       + 1 + 4 + len(stopID) 
                                       )
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 6)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, self.STAGE_WALKING)
        self._connection._packStringList(edges)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, arrivalPos)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, duration)
        self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, speed)
        self._connection._packString(stopID)
        self._connection._sendExact()

    def appendDrivingStage(self, personID, toEdge, lines, stopID=""):
        """appendDrivingStage(string, string, string, string)
        Appends a driving stage to the plan of the given person
        The lines parameter should be a space-separated list of line ids
        """
        self._connection._beginMessage(tc.CMD_SET_PERSON_VARIABLE, tc.APPEND_STAGE, personID,
                                       1 + 4 # compound
                                       + 1 + 4 # stage type
                                       + 1 + 4 + len(toEdge) 
                                       + 1 + 4 + len(lines) 
                                       + 1 + 4 + len(stopID))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, self.STAGE_DRIVING)
        self._connection._packString(toEdge)
        self._connection._packString(lines)
        self._connection._packString(stopID)
        self._connection._sendExact()

    def removeStage(self, personID, nextStageIndex):
        """removeStage(string, int)
        Removes the the nth next stage
        nextStageIndex must be lower then value of getRemainingStages(personID)
        nextStageIndex 0 immediately aborts the current stage and proceeds to the next stage
        """
        pass

PersonDomain()
