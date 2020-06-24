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

# @file    _poi.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @date    2008-10-09

from __future__ import absolute_import
import struct
from .domain import Domain
from .storage import Storage
from . import constants as tc
from .exceptions import TraCIException


class PoiDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "poi", tc.CMD_GET_POI_VARIABLE, tc.CMD_SET_POI_VARIABLE,
                        tc.CMD_SUBSCRIBE_POI_VARIABLE, tc.RESPONSE_SUBSCRIBE_POI_VARIABLE,
                        tc.CMD_SUBSCRIBE_POI_CONTEXT, tc.RESPONSE_SUBSCRIBE_POI_CONTEXT)

    def getType(self, poiID):
        """getType(string) -> string

        Returns the (abstract) type of the poi.
        """
        return self._getUniversal(tc.VAR_TYPE, poiID)

    def getPosition(self, poiID):
        """getPosition(string) -> (double, double)

        Returns the position coordinates of the given poi.
        """
        return self._getUniversal(tc.VAR_POSITION, poiID)

    def getColor(self, poiID):
        """getColor(string) -> (integer, integer, integer, integer)

        Returns the rgba color of the given poi.
        """
        return self._getUniversal(tc.VAR_COLOR, poiID)

    def getWidth(self, poiID):
        """getWidth(string) -> double

        Returns the width of the given poi.
        """
        return self._getUniversal(tc.VAR_WIDTH, poiID)

    def getHeight(self, poiID):
        """getHeight(string) -> double

        Returns the height of the given poi.
        """
        return self._getUniversal(tc.VAR_HEIGHT, poiID)

    def getAngle(self, poiID):
        """getAngle(string) -> double

        Returns the angle of the given poi.
        """
        return self._getUniversal(tc.VAR_ANGLE, poiID)

    def getImageFile(self, poiID):
        """getImageFile(string) -> string

        Returns the image file of the given poi.
        """
        return self._getUniversal(tc.VAR_IMAGEFILE, poiID)

    def setType(self, poiID, poiType):
        """setType(string, string) -> None

        Sets the (abstract) type of the poi.
        """
        self._setCmd(tc.VAR_TYPE, poiID, "s", poiType)

    def setPosition(self, poiID, x, y):
        """setPosition(string, (double, double)) -> None

        Sets the position coordinates of the poi.
        """       
        self._setCmd(tc.VAR_POSITION, poiID, "o", (x, y))

    def setColor(self, poiID, color):
        """setColor(string, (integer, integer, integer, integer)) -> None

        Sets the rgba color of the poi, i.e. (255,0,0) for the color red.
        The fourth component (alpha) is optional.
        """
        self._setCmd(tc.VAR_COLOR, poiID, "c", color)

    def setWidth(self, poiID, width):
        """setWidth(string, double) -> None

        Sets the width of the poi.
        """
        self._setCmd(tc.VAR_WIDTH, poiID, "d", width)

    def setHeight(self, poiID, height):
        """setHeight(string, double) -> None

        Sets the height of the poi.
        """
        self._setCmd(tc.VAR_HEIGHT, poiID, "d", height)

    def setAngle(self, poiID, angle):
        """setAngle(string, double) -> None

        Sets the angle of the poi.
        """      
        self._setCmd(tc.VAR_ANGLE, poiID, "d", angle)

    def setImageFile(self, poiID, imageFile):
        """setImageFile(string, string) -> None

        Sets the image file of the poi.
        """      
        self._setCmd(tc.VAR_IMAGEFILE, poiID, "s", imageFile)

    def add(self, poiID, x, y, color, poiType="", layer=0, imgFile="", width=1, height=1, angle=0):
        """add(string, double, double, (integer, integer, integer, integer), string, integer, string,double, double, double, double)-> None
        
        Adds a poi with the given values
        """       
        self._setCmd(tc.ADD, poiID, "tsciosddd", 8, poiType, color, layer, (x,y), imgFile, width, height, angle)

    def remove(self, poiID, layer=0):
        """
        remove(string, integer) -> None
        Removes the poi with the given poiID
        """  
        self._setCmd(tc.REMOVE, poiID, "i", layer)

    def highlight(self, poiID, color=(255, 0, 0, 255), size=-1, alphaMax=-1, duration=-1, type=0):
        """ highlight(string, color, float, ubyte) -> void
            Adds a circle of the given color highlighting the poi.
            If a positive size [in m] is given the size of the highlight is chosen accordingly,
            otherwise the image size of the poi is used as reference.
            If alphaMax and duration are positive, the circle fades in and out within the given duration,
            otherwise it is permanently added on top of the poi.
        """
        if type > 255:
            raise TraCIException("poi.highlight(): maximal value for type is 255")
        if alphaMax > 255:
            raise TraCIException("poi.highlight(): maximal value for alphaMax is 255")
        if alphaMax <= 0 and duration > 0:
            raise TraCIException("poi.highlight(): duration>0 requires alphaMax>0")
        if alphaMax > 0 and duration <= 0:
            raise TraCIException("poi.highlight(): alphaMax>0 requires duration>0")

        if type > 0:
            compoundLength = 5
        elif alphaMax > 0:
            compoundLength = 4
        elif size > 0:
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

        self._connection._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.VAR_HIGHLIGHT, poiID, msg_length)
        self._connection._string += struct.pack("!BB", tc.TYPE_COMPOUND, compoundLength)
        if compoundLength >= 1:
            self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]),
                                                    int(color[2]), int(color[3]) if len(color) > 3 else 255)
        if compoundLength >= 2:
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, size)
        if compoundLength >= 3:
            self._connection._string += struct.pack("!BB", tc.TYPE_UBYTE, alphaMax)
            self._connection._string += struct.pack("!Bd", tc.TYPE_DOUBLE, duration)
        if compoundLength >= 5:
            self._connection._string += struct.pack("!BB", tc.TYPE_UBYTE, type)
        self._connection._sendExact()

