# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _vehicletype.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @date    2008-10-09

from __future__ import absolute_import
from .domain import Domain
from . import constants as tc
from . import exceptions


class VTypeDomain(Domain):
    """This class contains all functions which are common to the vehicletype, vehicle and person domain"""

    def __init__(self, name, cmdGetID, cmdSetID,
                 subscribeID, subscribeResponseID,
                 contextID, contextResponseID,
                 retValFunc=None, deprecatedFor=None,
                 subscriptionDefault=(tc.TRACI_ID_LIST,)):
        Domain.__init__(self, name, cmdGetID, cmdSetID,
                        subscribeID, subscribeResponseID, contextID, contextResponseID,
                        retValFunc, deprecatedFor, subscriptionDefault)

    def getLength(self, typeID):
        """getLength(string) -> double

        Returns the length in m of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_LENGTH, typeID)

    def getMaxSpeed(self, typeID):
        """getMaxSpeed(string) -> double

        Returns the maximum speed in m/s of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_MAXSPEED, typeID)

    def getActionStepLength(self, typeID):
        """getActionStepLength(string) -> double

        Returns the action step length for this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_ACTIONSTEPLENGTH, typeID)

    def getSpeedFactor(self, typeID):
        """getSpeedFactor(string) -> double

        Returns the speed factor of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_SPEED_FACTOR, typeID)

    def getSpeedDeviation(self, typeID):
        """getSpeedDeviation(string) -> double

        Returns the maximum speed deviation of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_SPEED_DEVIATION, typeID)

    def getAccel(self, typeID):
        """getAccel(string) -> double

        Returns the maximum acceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_ACCEL, typeID)

    def getDecel(self, typeID):
        """getDecel(string) -> double

        Returns the maximal comfortable deceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_DECEL, typeID)

    def getEmergencyDecel(self, typeID):
        """getEmergencyDecel(string) -> double

        Returns the maximal physically possible deceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_EMERGENCY_DECEL, typeID)

    def getApparentDecel(self, typeID):
        """getApparentDecel(string) -> double

        Returns the apparent deceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_APPARENT_DECEL, typeID)

    def getImperfection(self, typeID):
        """getImperfection(string) -> double

        Returns the driver's imperfection for this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_IMPERFECTION, typeID)

    def getTau(self, typeID):
        """getTau(string) -> double

        Returns the driver's desired headway in s for this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_TAU, typeID)

    def getVehicleClass(self, typeID):
        """getVehicleClass(string) -> string

        Returns the class of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_VEHICLECLASS, typeID)

    def getEmissionClass(self, typeID):
        """getEmissionClass(string) -> string

        Returns the emission class of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_EMISSIONCLASS, typeID)

    def getShapeClass(self, typeID):
        """getShapeClass(string) -> string

        Returns the shape class of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_SHAPECLASS, typeID)

    def getMinGap(self, typeID):
        """getMinGap(string) -> double

        Returns the offset (gap to front vehicle if halting) of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_MINGAP, typeID)

    def getWidth(self, typeID):
        """getWidth(string) -> double

        Returns the width in m of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_WIDTH, typeID)

    def getHeight(self, typeID):
        """getHeight(string) -> double

        Returns the height in m of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_HEIGHT, typeID)

    def getMass(self, typeID):
        """getMass(string) -> double

        Returns the mass in kg of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_MASS, typeID)

    def getColor(self, typeID):
        """getColor(string) -> (integer, integer, integer, integer)

        Returns the color of this type.
        If called in the context of a person or vehicle, it will return their specific color if it has been set.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_COLOR, typeID)

    def getMinGapLat(self, typeID):
        """getMinGapLat(string) -> double

        Returns The desired lateral gap of this type at 50km/h in m
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_MINGAP_LAT, typeID)

    def getMaxSpeedLat(self, typeID):
        """getMaxSpeedLat(string) -> double

        Returns the maximum lateral speed in m/s of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_MAXSPEED_LAT, typeID)

    def getLateralAlignment(self, typeID):
        """getLateralAlignment(string) -> string

        Returns The preferred lateral alignment of the type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_LATALIGNMENT, typeID)

    def getPersonCapacity(self, typeID):
        """getPersonCapacity(string) -> int

        Returns the person capacity of this type.
        If called in the context of a person or vehicle, it will return the value for their current type.
        Use the respective object ID as typeID value in said context.
        """
        return self._getUniversal(tc.VAR_PERSON_CAPACITY, typeID)

    def getBoardingDuration(self, typeID):
        """getBoardingDuration(string) -> double

        Returns the boarding duration of this type
        """
        return self._getUniversal(tc.VAR_BOARDING_DURATION, typeID)

    def getImpatience(self, typeID):
        """getImpatience(string) -> double

        Returns the Impatience of this type
        """
        return self._getUniversal(tc.VAR_IMPATIENCE, typeID)

    def setImpatience(self, typeID, impatience):
        """setImpatience(string, double) -> None

        Sets the impatience of the this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_IMPATIENCE, typeID, "d", impatience)

    def setBoardingDuration(self, typeID, boardingDuration):
        """setBoardingDuration(string, double) -> None

        Sets the boarding duration of the this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_BOARDING_DURATION, typeID, "d", boardingDuration)

    def setLength(self, typeID, length):
        """setLength(string, double) -> None

        Sets the length in m of the this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_LENGTH, typeID, "d", length)

    def setMaxSpeed(self, typeID, speed):
        """setMaxSpeed(string, double) -> None

        Sets the maximum speed in m/s of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_MAXSPEED, typeID, "d", speed)

    def setVehicleClass(self, typeID, clazz):
        """setVehicleClass(string, string) -> None

        Sets the class of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_VEHICLECLASS, typeID, "s", clazz)

    def setSpeedFactor(self, typeID, factor):
        """setSpeedFactor(string, double) -> None

        Sets the speed factor of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_SPEED_FACTOR, typeID, "d", factor)

    def setEmissionClass(self, typeID, clazz):
        """setEmissionClass(string, string) -> None

        Sets the emission class of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_EMISSIONCLASS, typeID, "s", clazz)

    def setShapeClass(self, typeID, shapeClass):
        """setShapeClass(string, string) -> None

        Sets the shape class of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_SHAPECLASS, typeID, "s", shapeClass)

    def setWidth(self, typeID, width):
        """setWidth(string, double) -> None

        Sets the width in m of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_WIDTH, typeID, "d", width)

    def setHeight(self, typeID, height):
        """setHeight(string, double) -> None

        Sets the height in m of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_HEIGHT, typeID, "d", height)

    def setMass(self, typeID, mass):
        """setMass(string, double) -> None

        Sets the mass in kg of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_MASS, typeID, "d", mass)

    def setMinGap(self, typeID, minGap):
        """setMinGap(string, double) -> None

        Sets the offset (gap to front vehicle if halting) of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_MINGAP, typeID, "d", minGap)

    def setAccel(self, typeID, accel):
        """setAccel(string, double) -> None

        Sets the maximum acceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_ACCEL, typeID, "d", accel)

    def setDecel(self, typeID, decel):
        """setDecel(string, double) -> None

        Sets the maximal comfortable deceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_DECEL, typeID, "d", decel)

    def setEmergencyDecel(self, typeID, decel):
        """setEmergencyDecel(string, double) -> None

        Sets the maximal physically possible deceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_EMERGENCY_DECEL, typeID, "d", decel)

    def setApparentDecel(self, typeID, decel):
        """setApparentDecel(string, double) -> None

        Sets the apparent deceleration in m/s^2 of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_APPARENT_DECEL, typeID, "d", decel)

    def setImperfection(self, typeID, imperfection):
        """setImperfection(string, double) -> None

        Sets the driver imperfection of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_IMPERFECTION, typeID, "d", imperfection)

    def setTau(self, typeID, tau):
        """setTau(string, double) -> None

        Sets the driver's tau-parameter (reaction time or anticipation time depending on the car-following model) in s
        for this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_TAU, typeID, "d", tau)

    def setColor(self, typeID, color):
        """setColor(string, (integer, integer, integer, integer)) -> None

        Sets the color of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_COLOR, typeID, "c", color)

    def setMinGapLat(self, typeID, minGapLat):
        """setMinGapLat(string, double) -> None

        Sets the minimum lateral gap at 50km/h of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_MINGAP_LAT, typeID, "d", minGapLat)

    def setMaxSpeedLat(self, typeID, speed):
        """setMaxSpeedLat(string, double) -> None

        Sets the maximum lateral speed of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_MAXSPEED_LAT, typeID, "d", speed)

    def setLateralAlignment(self, typeID, latAlignment):
        """setLateralAlignment(string, string) -> None

        Sets the preferred lateral alignment of this type.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        self._setCmd(tc.VAR_LATALIGNMENT, typeID, "s", latAlignment)

    def setActionStepLength(self, typeID, actionStepLength, resetActionOffset=True):
        """setActionStepLength(string, double, bool) -> None

        Sets the action step length for this type. If resetActionOffset == True (default), the
        next action point is scheduled immediately for all vehicles of the type.
        If resetActionOffset == False, the interval between the last and the next action point is
        updated to match the given value for all vehicles of the type, or if the latter is smaller
        than the time since the last action point, the next action follows immediately.
        If called in the context of a person or vehicle, it will change the value just for the single instance.
        Use the respective object ID as typeID value in said context.
        """
        if actionStepLength < 0:
            raise exceptions.TraCIException("Invalid value for actionStepLength. Given value must be non-negative.")
        # Use negative value to indicate resetActionOffset == False
        if not resetActionOffset:
            actionStepLength *= -1
        self._setCmd(tc.VAR_ACTIONSTEPLENGTH, typeID, "d", actionStepLength)


class VehicleTypeDomain(VTypeDomain):
    """This class contains all functions which are specific to the vehicletype domain"""

    def __init__(self):
        VTypeDomain.__init__(self, "vehicletype", tc.CMD_GET_VEHICLETYPE_VARIABLE, tc.CMD_SET_VEHICLETYPE_VARIABLE,
                             tc.CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE, tc.RESPONSE_SUBSCRIBE_VEHICLETYPE_VARIABLE,
                             tc.CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT, tc.RESPONSE_SUBSCRIBE_VEHICLETYPE_CONTEXT)

    def getScale(self, typeID):
        """getScale(string) -> double
        Returns the traffic scaling factor
        """
        return self._getUniversal(tc.VAR_SCALE, typeID)

    def setSpeedDeviation(self, typeID, deviation):
        """setSpeedDeviation(string, double) -> None

        Sets the maximum speed deviation of this type.
        """
        self._setCmd(tc.VAR_SPEED_DEVIATION, typeID, "d", deviation)

    def setScale(self, typeID, value):
        """setScale(string, double) -> None
        Sets the traffic scaling factor
        """
        self._setCmd(tc.VAR_SCALE, typeID, "d", value)

    def copy(self, origTypeID, newTypeID):
        """copy(string, string) -> None

        Duplicates the vType with ID origTypeID. The newly created vType is assigned the ID newTypeID
        """
        self._setCmd(tc.COPY, origTypeID, "s", newTypeID)
