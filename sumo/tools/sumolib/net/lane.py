"""
@file    lane.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Karol Stosiek
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011-11-28
@version $Id$

This file contains a Python-representation of a single lane.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sumolib.geomhelper

# taken from sumo/src/utils/common/SUMOVehicleClass.cpp
SUMO_VEHICLE_CLASSES = (
    "public_emergency",  # deprecated
    "public_authority",  # deprecated
    "public_army",       # deprecated
    "public_transport",  # deprecated
    "transport",         # deprecated
    "lightrail",         # deprecated
    "cityrail",          # deprecated
    "rail_slow",         # deprecated
    "rail_fast",         # deprecated

    "private",
    "emergency",
    "authority",
    "army",
    "vip",
    "passenger",
    "hov",
    "taxi",
    "bus",
    "coach",
    "delivery",
    "truck",
    "trailer",
    "tram",
    "rail_urban",
    "rail",
    "rail_electric",
    "motorcycle",
    "moped",
    "bicycle",
    "pedestrian",
    "evehicle",
    "ship",
    "custom1",
    "custom2")


def get_allowed(allow, disallow):
    """ Normalize the given string attributes as a list of all allowed vClasses."""
    if allow is None and disallow is None:
        return SUMO_VEHICLE_CLASSES
    elif disallow is None:
        return allow.split()
    else:
        disallow = disallow.split()
        return tuple([c for c in SUMO_VEHICLE_CLASSES if not c in disallow])


def addJunctionPos(shape, fromPos, toPos):
    """Extends shape with the given positions in case they differ from the
    existing endpoints. assumes that shape and positions have the same dimensionality"""
    result = list(shape)
    if fromPos != shape[0]:
        result = [fromPos] + result
    if toPos != shape[-1]:
        result.append(toPos)
    return result


class Lane:

    """ Lanes from a sumo network """

    def __init__(self, edge, speed, length, allow, disallow):
        self._edge = edge
        self._speed = speed
        self._length = length
        self._shape = None
        self._shape3D = None
        self._shapeWithJunctions = None
        self._shapeWithJunctions3D = None
        self._outgoing = []
        self._params = {}
        self._allowed = get_allowed(allow, disallow)
        edge.addLane(self)

    def isInternal(self):
        """Returns True, if the lane is an internal lane. 

        Raises a ValueError if the lane does not yet have an edge yet."""

        if self._edge is None:
            raise ValueError, 'edge for this lane not yet defined'
        else:
            return self._edge.isInternal()

    def getSpeed(self):
        return self._speed

    def getLength(self):
        return self._length

    def setShape(self, shape):
        """Set the shape of the lane

        shape must be a list containing x,y,z coords as numbers
        to represent the shape of the lane
        """
        for pp in shape:
            if len(pp) != 3:
                raise ValueError('shape point must consist of x,y,z')

        self._shape3D = shape
        self._shape = [(x, y) for x, y, z in shape]

    def getShape(self, includeJunctions=False):
        """Returns the shape of the lane in 2d.

        This function returns the shape of the lane, as defined in the net.xml 
        file. The returned shape is a list containing numerical
        2-tuples representing the x,y coordinates of the shape points.

        For includeJunctions=True the returned list will contain 
        additionally the coords (x,y) of the fromNode of the 
        corresponding edge as first element and the coords (x,y) 
        of the toNode as last element. 

        For internal lanes, includeJunctions is ignored and the unaltered 
        shape of the lane is returned.
        """

        if includeJunctions and not self._edge.isInternal():
            if self._shapeWithJunctions is None:
                self._shapeWithJunctions = addJunctionPos(self._shape,
                                                          self._edge.getFromNode().getCoord(),
                                                          self._edge.getToNode().getCoord())
            return self._shapeWithJunctions
        return self._shape

    def getShape3D(self, includeJunctions=False):
        """Returns the shape of the lane in 3d.

        This function returns the shape of the lane, as defined in the net.xml 
        file. The returned shape is a list containing numerical 
        3-tuples representing the x,y,z coordinates of the shape points 
        where z defaults to zero. 

        For includeJunction=True the returned list will contain 
        additionally the coords (x,y,z) of the fromNode of the 
        corresponding edge as first element and the coords (x,y,z) 
        of the toNode as last element. 

        For internal lanes, includeJunctions is ignored and the unaltered 
        shape of the lane is returned.
        """

        if includeJunctions and not self._edge.isSpecial():
            if self._shapeWithJunctions3D is None:
                self._shapeWithJunctions3D = addJunctionPos(self._shape3D,
                                                            self._edge.getFromNode(
                                                            ).getCoord3D(),
                                                            self._edge.getToNode().getCoord3D())
            return self._shapeWithJunctions3D
        return self._shape3D

    def getBoundingBox(self, includeJunctions=True):
        s = self.getShape(includeJunctions)
        xmin = s[0][0]
        xmax = s[0][0]
        ymin = s[0][1]
        ymax = s[0][1]
        for p in s[1:]:
            xmin = min(xmin, p[0])
            xmax = max(xmax, p[0])
            ymin = min(ymin, p[1])
            ymax = max(ymax, p[1])
        assert(xmin != xmax or ymin != ymax)
        return (xmin, ymin, xmax, ymax)

    def getClosestLanePosAndDist(self, point, perpendicular=False):
        return sumolib.geomhelper.polygonOffsetAndDistanceToPoint(point, self.getShape(), perpendicular)

    def getIndex(self):
        return self._edge._lanes.index(self)

    def getID(self):
        return "%s_%s" % (self._edge._id, self.getIndex())

    def getEdge(self):
        return self._edge

    def addOutgoing(self, conn):
        self._outgoing.append(conn)

    def getOutgoing(self):
        return self._outgoing

    def setParam(self, key, value):
        self._params[key] = value

    def getParam(self, key, default=None):
        return self._params.get(key, default)
