# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _calibrator.py
# @author  Jakob Erdmann
# @date    2020-03-16

from __future__ import absolute_import
import struct
from . import constants as tc
from .domain import Domain
from .storage import Storage

_RETURN_VALUE_FUNC = {
    tc.VAR_ROAD_ID: Storage.readString,
    tc.VAR_LANE_ID: Storage.readString,
    tc.VAR_VEHSPERHOUR: Storage.readDouble,
    tc.VAR_SPEED: Storage.readDouble,
    tc.VAR_TYPE: Storage.readString,
    tc.VAR_BEGIN: Storage.readDouble,
    tc.VAR_END: Storage.readDouble,
    tc.VAR_ROUTE_ID: Storage.readString,
    tc.VAR_ROUTE_PROBE: Storage.readString,
    tc.VAR_VTYPES: Storage.readStringList,
    tc.VAR_PASSED: Storage.readInt,
    tc.VAR_INSERTED: Storage.readInt,
    tc.VAR_REMOVED: Storage.readInt,
}


class CalibratorDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "calibrator", tc.CMD_GET_CALIBRATOR_VARIABLE, tc.CMD_SET_CALIBRATOR_VARIABLE,
                        tc.CMD_SUBSCRIBE_CALIBRATOR_VARIABLE, tc.RESPONSE_SUBSCRIBE_CALIBRATOR_VARIABLE,
                        tc.CMD_SUBSCRIBE_CALIBRATOR_CONTEXT, tc.RESPONSE_SUBSCRIBE_CALIBRATOR_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getEdgeID(self, calibratorID):
        """getEdgeID(string) -> string
        Returns the edge of this calibrator
        """
        return self._getUniversal(tc.VAR_ROAD_ID, calibratorID)

    def getLaneID(self, calibratorID):
        """getLaneID(string) -> string
        Returns the lane of this calibrator (if it applies to a single lane)
        """
        return self._getUniversal(tc.VAR_LANE_ID, calibratorID)

    def getVehsPerHour(self, calibratorID):
        """getVehsPerHour(string) -> double
        Returns the number of vehicles per hour in the current calibration interval
        """
        return self._getUniversal(tc.VAR_VEHSPERHOUR, calibratorID)

    def getSpeed(self, calibratorID):
        """getSpeed(string) -> double
        Returns the target speed of the current calibration interval
        """
        return self._getUniversal(tc.VAR_SPEED, calibratorID)

    def getTypeID(self, calibratorID):
        """getTypeID(string) -> string
        Returns the type id for the current calibration interval
        """
        return self._getUniversal(tc.VAR_TYPE, calibratorID)

    def getBegin(self, calibratorID):
        """getBegin(string) -> double
        Returns the begin time of the current calibration interval
        """
        return self._getUniversal(tc.VAR_BEGIN, calibratorID)

    def getEnd(self, calibratorID):
        """getEnd(string) -> double
        Returns the end time of the current calibration interval
        """
        return self._getUniversal(tc.VAR_END, calibratorID)

    def getRouteID(self, calibratorID):
        """getRouteID(string) -> string
        Returns the route id for the current calibration interval
        """
        return self._getUniversal(tc.VAR_ROUTE_ID, calibratorID)

    def getRouteProbeID(self, calibratorID):
        """getRouteProbeID(string) -> string
        Returns the routeProbe id for this calibrator
        """
        return self._getUniversal(tc.VAR_ROUTE_PROBE, calibratorID)

    def getVTypes(self, calibratorID):
        """getVTypes(string) -> list(string)

        Returns a list of all types to which the calibrator applies (in a type filter is active)
        """
        return self._getUniversal(tc.VAR_VTYPES, calibratorID)

    def getPassed(self, calibratorID):
        """getPassed(string) -> double
        Returns the number of passed vehicles in the current calibration interval
        """
        return self._getUniversal(tc.VAR_PASSED, calibratorID)

    def getInserted(self, calibratorID):
        """getInserted(string) -> double
        Returns the number of inserted vehicles in the current calibration interval
        """
        return self._getUniversal(tc.VAR_INSERTED, calibratorID)

    def getRemoved(self, calibratorID):
        """getRemoved(string) -> double
        Returns the number of removed vehicles in the current calibration interval
        """
        return self._getUniversal(tc.VAR_REMOVED, calibratorID)

    def setFlow(self, calibratorID, begin, end, vehsPerHour, speed, typeID,
                routeID, departLane="first", departSpeed="max"):
        """setFlow(string, double, double, double, double, string, string, string, string) -> None
        Update or add a calibrator interval
        """
        messageString = struct.pack("!Bi", tc.TYPE_COMPOUND, 8)
        messageString += struct.pack("!Bd", tc.TYPE_DOUBLE, begin)
        messageString += struct.pack("!Bd", tc.TYPE_DOUBLE, end)
        messageString += struct.pack("!Bd", tc.TYPE_DOUBLE, vehsPerHour)
        messageString += struct.pack("!Bd", tc.TYPE_DOUBLE, speed)
        for val in (typeID, routeID, departLane, departSpeed):
            val = str(val)
            messageString += struct.pack("!Bi", tc.TYPE_STRING, len(val)) + val.encode("latin1")
        self._connection._beginMessage(
            tc.CMD_SET_CALIBRATOR_VARIABLE, tc.CMD_SET_FLOW, calibratorID, len(messageString))
        self._connection._string += messageString
        self._connection._sendExact()
