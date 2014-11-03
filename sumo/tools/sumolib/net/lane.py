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
Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

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


class Lane:
    """ Lanes from a sumo network """

    def __init__(self, edge, speed, length, allow, disallow):
        self._edge = edge
        self._speed = speed
        self._length = length
        self._shape = []
        self._cachedShapeWithJunctions = None
        self._outgoing = []
        self._params = {}
        self._allowed = get_allowed(allow, disallow)
        edge.addLane(self)

    def getSpeed(self):
        return self._speed

    def getLength(self):
        return self._length 

    def setShape(self, shape):
        self._shape = shape

    def getShape(self, includeJunctions=False):
        if includeJunctions:
            if self._cachedShapeWithJunctions == None:
                if self._edge.getFromNode()._coord != self._shape[0]:
                    self._cachedShapeWithJunctions = [self._edge.getFromNode()._coord] + self._shape
                else:
                    self._cachedShapeWithJunctions = list(self._shape)
                if self._edge.getToNode()._coord != self._shape[-1]:
                    self._cachedShapeWithJunctions += [self._edge.getToNode()._coord]
            return self._cachedShapeWithJunctions
        return self._shape 

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

    def getIndex(self):
        return self._edge._lanes.index(self)

    def getID(self):
        return "%s_%s" % (self._edge._id, self.getIndex())

    def getEdge(self):
        return self._edge

    def addOutgoing(self, conn):
        self._outgoing.append(conn)
