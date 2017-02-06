# -*- coding: utf-8 -*-
"""
@file    polygon.py
@author  Michael Behrisch
@date    2011-03-16
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

_RETURN_VALUE_FUNC = {tc.VAR_TYPE:  Storage.readString,
                      tc.VAR_SHAPE: Storage.readShape,
                      tc.VAR_FILL: lambda result: bool(result.read("!B")[0]),
                      tc.VAR_COLOR: lambda result: result.read("!BBBB")}


class PolygonDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "polygon", tc.CMD_GET_POLYGON_VARIABLE, tc.CMD_SET_POLYGON_VARIABLE,
                        tc.CMD_SUBSCRIBE_POLYGON_VARIABLE, tc.RESPONSE_SUBSCRIBE_POLYGON_VARIABLE,
                        tc.CMD_SUBSCRIBE_POLYGON_CONTEXT, tc.RESPONSE_SUBSCRIBE_POLYGON_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getType(self, polygonID):
        """getType(string) -> string

        Returns the (abstract) type of the polygon.
        """
        return self._getUniversal(tc.VAR_TYPE, polygonID)

    def getShape(self, polygonID):
        """getShape(string) -> list((double, double))

        Returns the shape (list of 2D-positions) of this polygon.
        """
        return self._getUniversal(tc.VAR_SHAPE, polygonID)

    def getColor(self, polygonID):
        """getColor(string) -> (integer, integer, integer, integer)

        Returns the rgba color of this polygon.
        """
        return self._getUniversal(tc.VAR_COLOR, polygonID)

    def getFilled(self, polygonID):
        """getFilled(string) -> bool
        Returns whether the polygon is filled
        """
        return self._getUniversal(tc.VAR_FILL, polygonID)

    def setType(self, polygonID, polygonType):
        """setType(string, string) -> None

        Sets the (abstract) type of the polygon.
        """
        self._connection._beginMessage(
            tc.CMD_SET_POLYGON_VARIABLE, tc.VAR_TYPE, polygonID, 1 + 4 + len(polygonType))
        self._connection._packString(polygonType)
        self._connection._sendExact()

    def setShape(self, polygonID, shape):
        """setShape(string, list((double, double))) -> None

        Sets the shape (list of 2D-positions) of this polygon.
        """
        self._connection._beginMessage(tc.CMD_SET_POLYGON_VARIABLE,
                                       tc.VAR_SHAPE, polygonID, 1 + 1 + len(shape) * (8 + 8))
        self._connection._string += struct.pack("!BB",
                                                tc.TYPE_POLYGON, len(shape))
        for p in shape:
            self._connection._string += struct.pack("!dd", p)
        self._connection._sendExact()

    def setColor(self, polygonID, color):
        """setColor(string, (integer, integer, integer, integer)) -> None

        Sets the rgba color of this polygon.
        """
        self._connection._beginMessage(
            tc.CMD_SET_POLYGON_VARIABLE, tc.VAR_COLOR, polygonID, 1 + 1 + 1 + 1 + 1)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(
            color[0]), int(color[1]), int(color[2]), int(color[3]))
        self._connection._sendExact()

    def setFilled(self, polygonID, filled):
        """setFilled(string) -> bool
        Returns whether the polygon is filled
        """
        self._connection._sendUByteCmd(
            tc.CMD_SET_POLYGON_VARIABLE, tc.VAR_FILL, polygonID, (1 if filled else 0))

    def add(self, polygonID, shape, color, fill=False, polygonType="", layer=0):
        self._connection._beginMessage(tc.CMD_SET_POLYGON_VARIABLE, tc.ADD, polygonID, 1 + 4 + 1 + 4 +
                                       len(polygonType) + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 4 + 1 + 1 + len(shape) * (8 + 8))
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 5)
        self._connection._packString(polygonType)
        self._connection._string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(
            color[0]), int(color[1]), int(color[2]), int(color[3]))
        self._connection._string += struct.pack("!BB",
                                                tc.TYPE_UBYTE, int(fill))
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
        self._connection._string += struct.pack("!BB",
                                                tc.TYPE_POLYGON, len(shape))
        for p in shape:
            self._connection._string += struct.pack("!dd", *p)
        self._connection._sendExact()

    def remove(self, polygonID, layer=0):
        self._connection._beginMessage(
            tc.CMD_SET_POLYGON_VARIABLE, tc.REMOVE, polygonID, 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_INTEGER, layer)
        self._connection._sendExact()


PolygonDomain()
