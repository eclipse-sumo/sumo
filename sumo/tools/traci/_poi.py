# -*- coding: utf-8 -*-
"""
@file    poi.py
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
import struct
from .domain import Domain
from .storage import Storage
from . import constants as tc

_RETURN_VALUE_FUNC = {tc.ID_LIST:      Storage.readStringList,
                      tc.ID_COUNT:     Storage.readInt,
                      tc.VAR_TYPE:     Storage.readString,
                      tc.VAR_POSITION: lambda result: result.read("!dd"),
                      tc.VAR_COLOR: lambda result: result.read("!BBBB")}


class PoiDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "poi", tc.CMD_GET_POI_VARIABLE, tc.CMD_SET_POI_VARIABLE,
                        tc.CMD_SUBSCRIBE_POI_VARIABLE, tc.RESPONSE_SUBSCRIBE_POI_VARIABLE,
                        tc.CMD_SUBSCRIBE_POI_CONTEXT, tc.RESPONSE_SUBSCRIBE_POI_CONTEXT,
                        _RETURN_VALUE_FUNC)

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

    def setType(self, poiID, poiType):
        """setType(string, string) -> None

        Sets the (abstract) type of the poi.
        """
        self._connection._beginMessage(
            tc.CMD_SET_POI_VARIABLE, tc.VAR_TYPE, poiID, 1 + 4 + len(poiType))
        self._connection._packString(poiType)
        self._connection._sendExact()

    def setPosition(self, poiID, x, y):
        """setPosition(string, (double, double)) -> None

        Sets the position coordinates of the poi. 
        """
        self._connection._beginMessage(
            tc.CMD_SET_POI_VARIABLE, tc.VAR_POSITION, poiID, 1 + 8 + 8)
        self._connection._string += struct.pack("!Bdd", tc.POSITION_2D, x, y)
        self._connection._sendExact()

    def setColor(self, poiID, color):
        """setColor(string, (integer, integer, integer, integer)) -> None

        Sets the rgba color of the poi.
        """
        self._connection._beginMessage(
            tc.CMD_SET_POI_VARIABLE, tc.VAR_COLOR, poiID, 1 + 1 + 1 + 1 + 1)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(
            color[0]), int(color[1]), int(color[2]), int(color[3]))
        self._connection._sendExact()

    def add(self, poiID, x, y, color, poiType="", layer=0):
        self._connection._beginMessage(tc.CMD_SET_POI_VARIABLE, tc.ADD, poiID, 1 +
                                       4 + 1 + 4 + len(poiType) + 1 + 1 + 1 + 1 + 1 + 1 + 4 + 1 + 8 + 8)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
        self._connection._packString(poiType)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(
            color[0]), int(color[1]), int(color[2]), int(color[3]))
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
        self._connection._string += struct.pack("!Bdd", tc.POSITION_2D, x, y)
        self._connection._sendExact()

    def remove(self, poiID, layer=0):
        self._connection._beginMessage(
            tc.CMD_SET_POI_VARIABLE, tc.REMOVE, poiID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
        self._connection._sendExact()


PoiDomain()
