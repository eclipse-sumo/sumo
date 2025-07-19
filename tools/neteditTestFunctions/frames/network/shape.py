# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    shape.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...general.functions import focusOnFrame
from ...input.mouse import leftClick, leftClickOffset
from ...input.keyboard import typeKey


def createSquaredShape(referencePosition, position, size, close):
    """
    @brief Create squared Polygon in position with a certain size
    """
    # call create rectangled shape
    createRectangledShape(referencePosition, position, size, size, close)


def createRectangledShape(referencePosition, position, sizex, sizey, close):
    """
    @brief Create rectangle Polygon in position with a certain size
    """
    # focus current frame
    focusOnFrame()
    # start draw
    typeKey('enter')
    # create polygon
    leftClick(referencePosition, position)
    leftClickOffset(referencePosition, position, 0, int(float(sizey) / -2))
    leftClickOffset(referencePosition, position, int(sizex / float(-2)), int(float(sizey) / -2))
    leftClickOffset(referencePosition, position, int(sizex / float(-2)), 0)
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, position)
    # finish draw
    typeKey('enter')


def createLineShape(referencePosition, position, sizex, sizey, close):
    """
    @brief Create line Polygon in position with a certain size
    """
    # focus current frame
    focusOnFrame()
    # start draw
    typeKey('enter')
    # create polygon
    leftClick(referencePosition, position)
    leftClickOffset(referencePosition, position, (sizex / -2), (sizey / -2))
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, position)
    # finish draw
    typeKey('enter')


def createGEOPOI():
    """
    @brief create GEO POI
    """
    # focus current frame
    focusOnFrame()
    # place cursor in create GEO POI
    for _ in range(20):
        typeKey('tab')
    # create geoPOI
    typeKey('space')


def GEOPOILonLat():
    """
    @brief change GEO POI format as Lon Lat
    """
    # focus current frame
    focusOnFrame()
    # place cursor in lon-lat
    for _ in range(16):
        typeKey('tab')
    # Change current value
    typeKey('space')


def GEOPOILatLon():
    """
    @brief change GEO POI format as Lat Lon
    """
    # focus current frame
    focusOnFrame()
    # place cursor in lat-lon
    for _ in range(17):
        typeKey('tab')
    # Change current value
    typeKey('space')
