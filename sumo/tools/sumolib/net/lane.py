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
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
class Lane:
    """ Lanes from a sumo network """

    def __init__(self, edge, speed, length):
        self._edge = edge
        self._speed = speed
        self._length = length
        self._shape = []
        self._outgoing = []
        self._params = {}
        edge.addLane(self)

    def getSpeed(self):
        return self._speed

    def getLength(self):
        return self._length 

    def setShape(self, shape):
        self._shape = shape

    def getShape(self):
        return self._shape 

    def getID(self):
        return self._edge._id + "_" + str(self._edge._lanes.index(self))

    def getEdge(self):
        return self._edge

    def addOutgoing(self, conn):
        self._outgoing.append(conn)
