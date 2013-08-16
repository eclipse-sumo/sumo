"""
@file    edge.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Karol Stosiek
@author  Michael Behrisch
@date    2011-11-28
@version $Id$

This file contains a Python-representation of a single edge.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
class Edge:
    """ Edges from a sumo network """

    def __init__(self, id, fromN, toN, prio, function, name):
        self._id = id
        self._from = fromN
        self._to = toN
        self._priority = prio
        fromN.addOutgoing(self)
        toN.addIncoming(self)
        self._lanes = []
        self._speed = None                                                          
        self._length = None
        self._incoming = {}
        self._outgoing = {}
        self._shape = None
        self._function = function
        self._tls = None
        self._name = name

    def getName(self):
        return self._name

    def getTLS(self):
        return self._tls

    def addLane(self, lane):
        self._lanes.append(lane)
        self._speed = lane.getSpeed()
        self._length = lane.getLength()

    def addOutgoing(self, conn):
        if conn._to not in self._outgoing:
            self._outgoing[conn._to] = []
        self._outgoing[conn._to].append(conn)

    def _addIncoming(self, conn):
        if conn._from not in self._incoming:
            self._incoming[conn._from] = []
        self._incoming[conn._from].append(conn)

    def setShape(self, shape):
        self._shape = shape

    def getID(self):
        return self._id

    def getIncoming(self):
        return self._incoming

    def getOutgoing(self):
        return self._outgoing

    def getShape(self):
        if not self._shape:
            shape = []
            shape.append(self._from._coord)
            shape.append(self._to._coord)
            return shape
        return self._shape

    def getSpeed(self):
        return self._speed

    def getLaneNumber(self):
        return len(self._lanes)

    def getLane(self, idx):
        return self._lanes[idx]

    def rebuildShape(self):
        noShapes = len(self._lanes)
        if noShapes%2 == 1:
            self.setShape(self._lanes[int(noShapes/2)]._shape)
        else:
            shape = []
            minLen = -1
            for l in self._lanes:
                if minLen==-1 or minLen>len(l.getShape()):
                    minLen = len(l._shape)
            for i in range(0, minLen):
                x = 0.
                y = 0.
                for j in range(0, len(self._lanes)):
                    x = x + self._lanes[j]._shape[i][0]
                    y = y + self._lanes[j]._shape[i][1]
                x = x / float(len(self._lanes))
                y = y / float(len(self._lanes))
                shape.append( [ x, y ] )
            self.setShape(shape)

    def getLength(self):
         return self._lanes[0].getLength()

    def setTLS(self, tls):
         self._tls = tls

    def getFromNode(self):
        return self._from

    def getToNode(self):
        return self._to
         
    def is_fringe(self):
        return len(self.getIncoming()) == 0 or len(self.getOutgoing()) == 0
