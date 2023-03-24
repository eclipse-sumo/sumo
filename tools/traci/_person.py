# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _person.py
# @author  Jakob Erdmann
# @date    2015-02-06

from __future__ import absolute_import
from ._vehicletype import VTypeDomain
from . import constants as tc
from . import _simulation as simulation


class Reservation(object):

    def __init__(self, id, persons, group, fromEdge, toEdge, departPos, arrivalPos,
                 depart, reservationTime, state):
        self.id = id
        self.persons = persons
        self.group = group
        self.fromEdge = fromEdge
        self.toEdge = toEdge
        self.arrivalPos = arrivalPos
        self.departPos = departPos
        self.depart = depart
        self.reservationTime = reservationTime
        self.state = state

    def __attr_repr__(self, attrname, default=""):
        if getattr(self, attrname) == default:
            return ""
        else:
            val = getattr(self, attrname)
            if val == tc.INVALID_DOUBLE_VALUE:
                val = "INVALID"
            return "%s=%s" % (attrname, val)

    def __repr__(self):
        return "Reservation(%s)" % ', '.join([v for v in [
            self.__attr_repr__("id"),
            self.__attr_repr__("persons"),
            self.__attr_repr__("group"),
            self.__attr_repr__("fromEdge"),
            self.__attr_repr__("toEdge"),
            self.__attr_repr__("departPos"),
            self.__attr_repr__("arrivalPos"),
            self.__attr_repr__("depart"),
            self.__attr_repr__("reservationTime"),
            self.__attr_repr__("state"),
        ] if v != ""])


def _readReservation(result):
    # compound size and type
    assert(result.read("!i")[0] == 10)
    id = result.readTypedString()
    persons = result.readTypedStringList()
    group = result.readTypedString()
    fromEdge = result.readTypedString()
    toEdge = result.readTypedString()
    departPos = result.readTypedDouble()
    arrivalPos = result.readTypedDouble()
    depart = result.readTypedDouble()
    reservationTime = result.readTypedDouble()
    state = result.readTypedInt()
    return Reservation(id, persons, group, fromEdge, toEdge, departPos,
                       arrivalPos, depart, reservationTime, state)


_RETURN_VALUE_FUNC = {tc.VAR_STAGE: simulation._readStage,
                      }


class PersonDomain(VTypeDomain):
    def __init__(self):
        VTypeDomain.__init__(self, "person", tc.CMD_GET_PERSON_VARIABLE, tc.CMD_SET_PERSON_VARIABLE,
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
        """getPosition3D(string) -> (double, double, double)

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

    def getLaneID(self, personID):
        """getLaneID(string) -> string

        Returns the id of the lane the named person was at within the last step.
        If the current person stage does not provide a lane, "" is returned.
        """
        return self._getUniversal(tc.VAR_LANE_ID, personID)

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

    def getSpeedFactor(self, personID):
        """getSpeedFactor(string) -> double

        Returns the quotient of this persons maximum speed and the maximum speed of its type
        """
        return self._getUniversal(tc.VAR_SPEED_FACTOR, personID)

    def getWaitingTime(self, personID):
        """getWaitingTime(string) -> double
        The waiting time of a person is defined as the time (in seconds) spent with a
        speed below 0.1m/s since the last time it was faster than 0.1m/s.
        (basically, the waiting time of a person is reset to 0 every time it moves).
        """
        return self._getUniversal(tc.VAR_WAITING_TIME, personID)

    def getNextEdge(self, personID):
        """getNextEdge(string) -> string
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
        return self._getUniversal(tc.VAR_EDGES, personID, "i", nextStageIndex)

    def getStage(self, personID, nextStageIndex=0):
        """getStage(string, int) -> int
        Returns the the nth stage object (type simulation.Stage)
        Attribute type of this object has the following meaning:
          0 for not-yet-departed
          1 for waiting
          2 for walking
          3 for driving
          4 for access to busStop or trainStop
          5 for personTrip
        nextStageIndex 0 retrieves value for the current stage.
        nextStageIndex must be lower then value of getRemainingStages(personID)
        """
        return self._getUniversal(tc.VAR_STAGE, personID, "i", nextStageIndex)

    def getRemainingStages(self, personID):
        """getRemainingStages(string) -> int
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

    def getTaxiReservations(self, onlyNew=0):
        """getTaxiReservations(int) -> list(Stage)
        Returns all reservations. If onlyNew is 1, each reservation is returned
        only once
        """
        answer = self._getCmd(tc.VAR_TAXI_RESERVATIONS, "", "i", onlyNew)
        answer.read("!B")                   # Type
        result = []
        for _ in range(answer.readInt()):
            answer.read("!B")                   # Type
            result.append(_readReservation(answer))
        return tuple(result)

    def splitTaxiReservation(self, reservationID, personIDs):
        """splitTaxiReservation(string, list(string)) -> string
        Splits given list of person ids from the reservation with the given id
        and creates a new reservation for these persons. Returns the new
        reservation id
        """
        return self._getUniversal(tc.SPLIT_TAXI_RESERVATIONS, reservationID, "l", personIDs)

    def removeStages(self, personID):
        """removeStages(string)
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
        format = "tssdd"
        values = [4, typeID, edgeID, depart, pos]
        self._setCmd(tc.ADD, personID, format, *values)

    def appendWaitingStage(self, personID, duration, description="waiting", stopID=""):
        """appendWaitingStage(string, float, string, string)
        Appends a waiting stage with duration in s to the plan of the given person
        """
        format = "tidss"
        values = [4, tc.STAGE_WAITING, duration, description, stopID]
        self._setCmd(tc.APPEND_STAGE, personID, format, *values)

    def appendWalkingStage(self, personID, edges, arrivalPos, duration=-1, speed=-1, stopID=""):
        """appendWalkingStage(string, stringList, double, double, double, string)
        Appends a walking stage to the plan of the given person
        The walking speed can either be specified, computed from the duration parameter (in s) or taken from the
        type of the person
        """
        if isinstance(edges, str):
            edges = [edges]
        format = "tilddds"
        values = [6, tc.STAGE_WALKING, edges, arrivalPos, duration, speed, stopID]
        self._setCmd(tc.APPEND_STAGE, personID, format, *values)

    def appendDrivingStage(self, personID, toEdge, lines, stopID=""):
        """appendDrivingStage(string, string, string, string)
        Appends a driving stage to the plan of the given person
        The lines parameter should be a space-separated list of line ids
        """
        format = "tisss"
        values = [4, tc.STAGE_DRIVING, toEdge, lines, stopID]
        self._setCmd(tc.APPEND_STAGE, personID, format, *values)

    def appendStage(self, personID, stage):
        """appendStage(string, stage)
        Appends a stage object to the plan of the given person
        Such an object is obtainable using getStage
        """
        format, values = simulation._writeStage(stage)
        self._setCmd(tc.APPEND_STAGE, personID, format, *values)

    def replaceStage(self, personID, stageIndex, stage):
        """replaceStage(string, int, stage)
        Replaces the nth subsequent stage with the given stage object
        Such an object is obtainable using getStage
        """
        format, values = simulation._writeStage(stage)
        format = "ti" + format
        values = [2, stageIndex] + values
        self._setCmd(tc.REPLACE_STAGE, personID, format, *values)

    def removeStage(self, personID, nextStageIndex):
        """removeStage(string, int)
        Removes the nth next stage
        nextStageIndex must be lower then value of getRemainingStages(personID)
        nextStageIndex 0 immediately aborts the current stage and proceeds to the next stage
        """
        self._setCmd(tc.REMOVE_STAGE, personID, "i", nextStageIndex)

    def rerouteTraveltime(self, personID):
        """rerouteTraveltime(string) -> None Reroutes a pedestrian (walking person).
        """
        self._setCmd(tc.CMD_REROUTE_TRAVELTIME, personID, "t", 0)

    def remove(self, personID, reason=tc.REMOVE_VAPORIZED):
        '''Remove person with the given ID for the give reason.
           Reasons are defined in module constants and start with REMOVE_'''
        self._setCmd(tc.REMOVE, personID, "b", reason)

    def moveTo(self, personID, laneID, pos, posLat=tc.INVALID_DOUBLE_VALUE):
        """moveTo(string, string, double, double) -> None

        Move a person to a new position along it's current route.
        """
        self._setCmd(tc.VAR_MOVE_TO, personID, "tsdd", 3, laneID, pos, posLat)

    def moveToXY(self, personID, edgeID, x, y, angle=tc.INVALID_DOUBLE_VALUE, keepRoute=1, matchThreshold=100):
        '''Place person at the given x,y coordinates and force it's angle to
        the given value (for drawing).
        If the angle is set to INVALID_DOUBLE_VALUE, the vehicle assumes the
        natural angle of the edge on which it is driving.
        If keepRoute is set to 1, the closest position
        within the existing route is taken. If keepRoute is set to 0, the vehicle may move to
        any edge in the network but it's route then only consists of that edge.
        If keepRoute is set to 2 the person has all the freedom of keepRoute=0
        but in addition to that may even move outside the road network.
        edgeID is an optional placement hint to resolve ambiguities.
        The command fails if no suitable target position is found within the
        distance given by matchThreshold.
        '''
        format = "tsdddbd"
        values = [6, edgeID, x, y, angle, keepRoute, matchThreshold]
        self._setCmd(tc.MOVE_TO_XY, personID, format, *values)

    def setSpeed(self, personID, speed):
        """setSpeed(string, double) -> None

        Sets the maximum speed in m/s for the named person for subsequent step.
        """
        self._setCmd(tc.VAR_SPEED, personID, "d", speed)

    def setType(self, personID, typeID):
        """setType(string, string) -> None

        Sets the id of the type for the named person.
        """
        self._setCmd(tc.VAR_TYPE, personID, "s", typeID)
