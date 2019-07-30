# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    _gui.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-09
# @version $Id$

from __future__ import absolute_import
import struct
from .domain import Domain
from .storage import Storage
from . import constants as tc

_RETURN_VALUE_FUNC = {tc.VAR_VIEW_ZOOM: Storage.readDouble,
                      tc.VAR_VIEW_OFFSET: lambda result: result.read("!dd"),
                      tc.VAR_VIEW_SCHEMA: Storage.readString,
                      tc.VAR_VIEW_BOUNDARY: Storage.readShape,
                      tc.VAR_HAS_VIEW: lambda result: bool(result.read("!i")[0]),
                      tc.VAR_TRACK_VEHICLE: Storage.readString}


class GuiDomain(Domain):
    DEFAULT_VIEW = 'View #0'

    def __init__(self):
        Domain.__init__(self, "gui", tc.CMD_GET_GUI_VARIABLE, tc.CMD_SET_GUI_VARIABLE,
                        tc.CMD_SUBSCRIBE_GUI_VARIABLE, tc.RESPONSE_SUBSCRIBE_GUI_VARIABLE,
                        tc.CMD_SUBSCRIBE_GUI_CONTEXT, tc.RESPONSE_SUBSCRIBE_GUI_CONTEXT,
                        _RETURN_VALUE_FUNC)

    def getZoom(self, viewID=DEFAULT_VIEW):
        """getZoom(string): -> double

        Returns the current zoom factor.
        """
        return self._getUniversal(tc.VAR_VIEW_ZOOM, viewID)

    def getOffset(self, viewID=DEFAULT_VIEW):
        """getOffset(string): -> (double, double)

        Returns the x and y offset of the center of the current view.
        """
        return self._getUniversal(tc.VAR_VIEW_OFFSET, viewID)

    def getSchema(self, viewID=DEFAULT_VIEW):
        """getSchema(string): -> string

        Returns the name of the current coloring scheme.
        """
        return self._getUniversal(tc.VAR_VIEW_SCHEMA, viewID)

    def getBoundary(self, viewID=DEFAULT_VIEW):
        """getBoundary(string): -> ((double, double), (double, double))

        Returns the coordinates of the lower left and the upper right corner of the currently visible view.
        """
        return self._getUniversal(tc.VAR_VIEW_BOUNDARY, viewID)

    def setZoom(self, viewID, zoom):
        """setZoom(string, double) -> None

        Set the current zoom factor for the given view.
        """
        self._connection._sendDoubleCmd(
            tc.CMD_SET_GUI_VARIABLE, tc.VAR_VIEW_ZOOM, viewID, zoom)

    def setOffset(self, viewID, x, y):
        """setOffset(string, double, double) -> None

        Set the current offset for the given view.
        """
        self._connection._beginMessage(
            tc.CMD_SET_GUI_VARIABLE, tc.VAR_VIEW_OFFSET, viewID, 1 + 8 + 8)
        self._connection._string += struct.pack("!Bdd", tc.POSITION_2D, x, y)
        self._connection._sendExact()

    def setSchema(self, viewID, schemeName):
        """setSchema(string, string) -> None

        Set the current coloring scheme for the given view.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_GUI_VARIABLE, tc.VAR_VIEW_SCHEMA, viewID, schemeName)

    def setBoundary(self, viewID, xmin, ymin, xmax, ymax):
        """setBoundary(string, double, double, double, double) -> None
        Sets the boundary of the visible network. If the window has a different
        aspect ratio than the given boundary, the view is expanded along one
        axis to meet the window aspect ratio and contain the given boundary.
        """
        self._connection._beginMessage(
            tc.CMD_SET_GUI_VARIABLE, tc.VAR_VIEW_BOUNDARY, viewID, 1 + 1 + 8 + 8 + 8 + 8)
        self._connection._string += struct.pack("!BBdddd", tc.TYPE_POLYGON, 2, xmin, ymin, xmax, ymax)
        self._connection._sendExact()

    def screenshot(self, viewID, filename, width=-1, height=-1):
        """screenshot(string, string, int, int) -> None

        Save a screenshot for the given view to the given filename.
        The fileformat is guessed from the extension, the available
        formats differ from platform to platform but should at least
        include ps, svg and pdf, on linux probably gif, png and jpg as well.
        Width and height of the image can be given as optional parameters.
        """
        self._connection._beginMessage(
            tc.CMD_SET_GUI_VARIABLE, tc.VAR_SCREENSHOT, viewID, 1 + 4 + 1 + 4 + len(filename) + 1 + 4 + 1 + 4)
        self._connection._string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
        self._connection._packString(filename)
        self._connection._string += struct.pack("!BiBi", tc.TYPE_INTEGER, width, tc.TYPE_INTEGER, height)
        self._connection._sendExact()

    def trackVehicle(self, viewID, vehID):
        """trackVehicle(string, string) -> None

        Start visually tracking the given vehicle on the given view.
        """
        self._connection._sendStringCmd(
            tc.CMD_SET_GUI_VARIABLE, tc.VAR_TRACK_VEHICLE, viewID, vehID)

    def hasView(self, viewID=DEFAULT_VIEW):
        """hasView(string): -> bool

        Check whether the given view exists.
        """
        return self._getUniversal(tc.VAR_HAS_VIEW, viewID)

    def getTrackedVehicle(self, viewID=DEFAULT_VIEW):
        """getTrackedVehicle(string): -> string

        Returns the id of the currently tracked vehicle
        """
        return self._getUniversal(tc.VAR_TRACK_VEHICLE, viewID)
