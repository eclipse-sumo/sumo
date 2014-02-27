"""
@file    lane.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Karol Stosiek
@author  Michael Behrisch
@date    2011-11-28
@version $Id$

This file contains a Python-representation of a single lane.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

# taken from sumo/src/utils/common/SUMOVehicleClass.cpp
SUMO_VEHICLE_CLASSES = (
        "private",           
        "public_transport",  
        "public_emergency",  
        "public_authority",  
        "public_army",       
        "vip",               
        "ignoring",          
        "passenger",         
        "hov",               
        "taxi",              
        "bus",               
        "delivery",          
        "transport",         
        "lightrail",         
        "cityrail",          
        "rail_slow",         
        "rail_fast",         
        "motorcycle",        
        "bicycle",           
        "pedestrian",        
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

    def getShape(self):
        return self._shape 

    def getIndex(self):
        return self._edge._lanes.index(self)

    def getID(self):
        return "%s_%s" % (self._edge._id, self.getIndex())

    def getEdge(self):
        return self._edge

    def addOutgoing(self, conn):
        self._outgoing.append(conn)
