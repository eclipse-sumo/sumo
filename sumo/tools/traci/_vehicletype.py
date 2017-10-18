# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _vehicletype.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @date    2008-10-09
# @version $Id$

from __future__ import absolute_import
from .domain import Domain
from .storage import Storage
import struct
from . import constants as tc
from . import exceptions

_RETURN_VALUE_FUNC = {tc.VAR_LENGTH: Storage.readDouble,
                      tc.VAR_MAXSPEED: Storage.readDouble,
                      tc.VAR_SPEED_FACTOR: Storage.readDouble,
                      tc.VAR_SPEED_DEVIATION: Storage.readDouble,
                      tc.VAR_ACCEL: Storage.readDouble,
                      tc.VAR_DECEL: Storage.readDouble,
                      tc.VAR_EMERGENCY_DECEL: Storage.readDouble,
                      tc.VAR_APPARENT_DECEL: Storage.readDouble,
                      tc.VAR_ACTIONSTEPLENGTH: Storage.readDouble,
                      tc.VAR_IMPERFECTION: Storage.readDouble,
                      tc.VAR_TAU: Storage.readDouble,
                      tc.VAR_VEHICLECLASS: Storage.readString,
                      tc.VAR_EMISSIONCLASS: Storage.readString,
                      tc.VAR_SHAPECLASS: Storage.readString,
                      tc.VAR_MINGAP: Storage.readDouble,
                      tc.VAR_WIDTH: Storage.readDouble,
                      tc.VAR_HEIGHT: Storage.readDouble,
                      tc.VAR_MAXSPEED_LAT: Storage.readDouble,
                      tc.VAR_MINGAP_LAT: Storage.readDouble,
                      tc.VAR_LATALIGNMENT: Storage.readString,
                      tc.VAR_COLOR: lambda result: result.read("!BBBB")}


class VehicleTypeDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "vehicletype", tc.CMD_GET_VEHICLETYPE_VARIABLE, tc.CMD_SET_VEHICLETYPE_VARIABLE,
                        tc.CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE, tc.RESPONSE_SUBSCRIBE_VEHICLETYPE_VARIABLE,
                        tc.CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT, tc.RESPONSE_SUBSCRIBE_VEHICLETYPE_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getLength(self, typeID):
        """getLength(string) -> double

        Returns the length in m of the vehicles of this type.
        """
        return self._getUniversal(tc.VAR_LENGTH, typeID)

    def getMaxSpeed(self, typeID):
        """getMaxSpeed(string) -> double

        Returns the maximum speed in m/s of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_MAXSPEED, typeID)

    def getSpeedFactor(self, typeID):
        """getSpeedFactor(string) -> double

        .
        """
        return self._getUniversal(tc.VAR_SPEED_FACTOR, typeID)

    def getSpeedDeviation(self, typeID):
        """getSpeedDeviation(string) -> double

        Returns the maximum speed deviation of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_SPEED_DEVIATION, typeID)

    def getAccel(self, typeID):
        """getAccel(string) -> double

        Returns the maximum acceleration in m/s^2 of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_ACCEL, typeID)

    def getDecel(self, typeID):
        """getDecel(string) -> double

        Returns the maximal comfortable deceleration in m/s^2 of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_DECEL, typeID)

    def getEmergencyDecel(self, typeID):
        """getEmergencyDecel(string) -> double

        Returns the maximal physically possible deceleration in m/s^2 of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_EMERGENCY_DECEL, typeID)

    def getApparentDecel(self, typeID):
        """getApparentDecel(string) -> double

        Returns the apparent deceleration in m/s^2 of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_APPARENT_DECEL, typeID)
    
    def getActionStepLength(self, typeID):
        """getActionStepLength(string) -> double

        Returns the action step length for vehicles of this type.
        """
        return self._getUniversal(tc.VAR_ACTIONSTEPLENGTH, typeID)

    def getImperfection(self, typeID):
        """getImperfection(string) -> double

        .
        """
        return self._getUniversal(tc.VAR_IMPERFECTION, typeID)

    def getTau(self, typeID):
        """getTau(string) -> double

        Returns the driver's reaction time in s for vehicles of this type.
        """
        return self._getUniversal(tc.VAR_TAU, typeID)

    def getVehicleClass(self, typeID):
        """getVehicleClass(string) -> string

        Returns the class of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_VEHICLECLASS, typeID)

    def getEmissionClass(self, typeID):
        """getEmissionClass(string) -> string

        Returns the emission class of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_EMISSIONCLASS, typeID)

    def getShapeClass(self, typeID):
        """getShapeClass(string) -> string

        Returns the shape class of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_SHAPECLASS, typeID)

    def getMinGap(self, typeID):
        """getMinGap(string) -> double

        Returns the offset (gap to front vehicle if halting) of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_MINGAP, typeID)

    def getWidth(self, typeID):
        """getWidth(string) -> double

        Returns the width in m of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_WIDTH, typeID)

    def getHeight(self, typeID):
        """getHeight(string) -> double

        Returns the height in m of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_HEIGHT, typeID)

    def getColor(self, typeID):
        """getColor(string) -> (integer, integer, integer, integer)

        Returns the color of this type.
        """
        return self._getUniversal(tc.VAR_COLOR, typeID)

    def getMaxSpeedLat(self, vehID):
        """getMaxSpeedLat(string) -> double

        Returns the maximum lateral speed in m/s of this type.
        """
        return self._getUniversal(tc.VAR_MAXSPEED_LAT, vehID)

    def getLateralAlignment(self, vehID):
        """getLateralAlignment(string) -> string

        Returns The preferred lateral alignment of the type
        """
        return self._getUniversal(tc.VAR_LATALIGNMENT, vehID)

    def getMinGapLat(self, vehID):
        """getMinGapLat(string) -> double

        Returns The desired lateral gap of this type at 50km/h in m
        """
        return self._getUniversal(tc.VAR_MINGAP_LAT, vehID)

    def setLength(self, typeID, length):
        """setLength(string, double) -> None

        Sets the length in m of the vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_LENGTH, typeID, length)

    def setMaxSpeed(self, typeID, speed):
        """setMaxSpeed(string, double) -> None

        Sets the maximum speed in m/s of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_MAXSPEED, typeID, speed)

    def setVehicleClass(self, typeID, clazz):
        """setVehicleClass(string, string) -> None

        Sets the class of vehicles of this type.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_VEHICLECLASS, typeID, clazz)

    def setSpeedFactor(self, typeID, factor):
        """setSpeedFactor(string, double) -> None

        .
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SPEED_FACTOR, typeID, factor)

    def setSpeedDeviation(self, typeID, deviation):
        """setSpeedDeviation(string, double) -> None

        Sets the maximum speed deviation of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SPEED_DEVIATION, typeID, deviation)

    def setEmissionClass(self, typeID, clazz):
        """setEmissionClass(string, string) -> None

        Sets the emission class of vehicles of this type.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_EMISSIONCLASS, typeID, clazz)

    def setWidth(self, typeID, width):
        """setWidth(string, double) -> None

        Sets the width in m of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_WIDTH, typeID, width)

    def setHeight(self, typeID, height):
        """setHeight(string, double) -> None

        Sets the height in m of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_HEIGHT, typeID, height)

    def setMinGap(self, typeID, minGap):
        """setMinGap(string, double) -> None

        Sets the offset (gap to front vehicle if halting) of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_MINGAP, typeID, minGap)

    def setMinGapLat(self, typeID, minGapLat):
        """setMinGapLat(string, double) -> None

        Sets the minimum lateral gap at 50km/h of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_MINGAP_LAT, typeID, minGapLat)

    def setMaxSpeedLat(self, typeID, speed):
        """setMaxSpeedLat(string, double) -> None

        Sets the maximum lateral speed of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_MAXSPEED_LAT, typeID, speed)

    def setLateralAlignment(self, typeID, latAlignment):
        """setLateralAlignment(string, string) -> None

        Sets the preferred lateral alignment of this type.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_LATALIGNMENT, typeID, latAlignment)

    def setShapeClass(self, typeID, clazz):
        """setShapeClass(string, string) -> None

        Sets the shape class of vehicles of this type.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_SHAPECLASS, typeID, clazz)

    def setAccel(self, typeID, accel):
        """setAccel(string, double) -> None

        Sets the maximum acceleration in m/s^2 of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_ACCEL, typeID, accel)

    def setDecel(self, typeID, decel):
        """setDecel(string, double) -> None

        Sets the maximal comfortable deceleration in m/s^2 of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_DECEL, typeID, decel)

    def setEmergencyDecel(self, typeID, decel):
        """setDecel(string, double) -> None

        Sets the maximal physically possible deceleration in m/s^2 of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_EMERGENCY_DECEL, typeID, decel)

    def setActionStepLength(self, typeID, actionStepLength, resetActionOffset=True):
        """setActionStepLength(string, double, bool) -> None

        Sets the action step length for vehicles of this type. If resetActionOffset == True (default), the 
        next action point is scheduled immediately for all vehicles of the type. 
        If resetActionOffset == False, the interval between the last and the next action point is 
        updated to match the given value for all vehicles of the type, or if the latter is smaller 
        than the time since the last action point, the next action follows immediately.
        """
        if actionStepLength < 0:
            raise exceptions.TraCIException("Invalid value for actionStepLength. Given value must be non-negative.")
        # Use negative value to indicate resetActionOffset == False
        if not resetActionOffset:
            actionStepLength*=-1
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_ACTIONSTEPLENGTH, typeID, actionStepLength)

    def setApparentDecel(self, typeID, decel):
        """setDecel(string, double) -> None

        Sets the apparent deceleration in m/s^2 of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_APPARENT_DECEL, typeID, decel)

    def setImperfection(self, typeID, imperfection):
        """setImperfection(string, double) -> None

        .
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_IMPERFECTION, typeID, imperfection)

    def setTau(self, typeID, tau):
        """setTau(string, double) -> None

        Sets the driver's tau-parameter (reaction time or anticipation time depending on the car-following model) in s for vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_TAU, typeID, tau)

    def setColor(self, typeID, color):
        """setColor(string, (integer, integer, integer, integer)) -> None

        Sets the color of this type.
        """
        self._connection._beginMessage(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_COLOR, typeID, 1 + 1 + 1 + 1 + 1)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(
            color[0]), int(color[1]), int(color[2]), int(color[3]))
        self._connection._sendExact()

    def copy(self, origTypeID, newTypeID):
        """copy(string, string) -> None

        Duplicates the vType with ID origTypeID. The newly created vType is assigned the ID newTypeID
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.COPY, origTypeID, newTypeID)

VehicleTypeDomain()
