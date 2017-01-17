# -*- coding: utf-8 -*-
"""
@file    vehicletype.py
@author  Michael Behrisch
@author  Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

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

_RETURN_VALUE_FUNC = {tc.VAR_LENGTH:          Storage.readDouble,
                      tc.VAR_MAXSPEED:        Storage.readDouble,
                      tc.VAR_SPEED_FACTOR:    Storage.readDouble,
                      tc.VAR_SPEED_DEVIATION: Storage.readDouble,
                      tc.VAR_ACCEL:           Storage.readDouble,
                      tc.VAR_DECEL:           Storage.readDouble,
                      tc.VAR_IMPERFECTION:    Storage.readDouble,
                      tc.VAR_TAU:             Storage.readDouble,
                      tc.VAR_VEHICLECLASS:    Storage.readString,
                      tc.VAR_EMISSIONCLASS:   Storage.readString,
                      tc.VAR_SHAPECLASS:      Storage.readString,
                      tc.VAR_MINGAP:          Storage.readDouble,
                      tc.VAR_WIDTH:           Storage.readDouble,
                      tc.VAR_HEIGHT:          Storage.readDouble,
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

        Returns the maximum deceleration in m/s^2 of vehicles of this type.
        """
        return self._getUniversal(tc.VAR_DECEL, typeID)

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

        Sets the maximum deceleration in m/s^2 of vehicles of this type.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_DECEL, typeID, decel)

    def setImperfection(self, typeID, imperfection):
        """setImperfection(string, double) -> None

        .
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_VEHICLETYPE_VARIABLE, tc.VAR_IMPERFECTION, typeID, imperfection)

    def setTau(self, typeID, tau):
        """setTau(string, double) -> None

        Sets the driver's reaction time in s for vehicles of this type.
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


VehicleTypeDomain()
