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
                      tc.VAR_VEHICLE:         Storage.readString,
                      tc.VAR_EDGES:           Storage.readStringList,
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

    def getEdges(self, personID):
        """getEdges(string) -> list(string)

        Returns a list of all edges in the current stage.
        For waiting stages this is a single edge
        For walking stages this is the complete route
        For driving stages this is [origin, destination]
        """
        return self._getUniversal(tc.VAR_EDGES, personID)

    def getStage(self, personID):
        """getStage(string) -> int
        Returns 
          0 for driving
          1 for waiting
          2 for walking
          3 for not-yet-departed
        """
        return self._getUniversal(tc.VAR_STAGE, personID)

    def getVehicle(self, personID):
        """getVehicle(string) -> string
        Returns the id of the current vehicle if the person is in stage driving
        and has entered a vehicle.
        Return the empty string otherwise
        """
        return self._getUniversal(tc.VAR_VEHICLE, personID)

PersonDomain()
