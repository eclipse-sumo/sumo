"""
@file    node.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Karol Stosiek
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2011-11-28
@version $Id$

This file contains a Python-representation of a single node.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2016 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


class Node:

    """ Nodes from a sumo network """

    def __init__(self, id, type, coord, incLanes, intLanes=None):
        self._id = id
        self._type = type
        self._coord = coord
        self._incoming = []
        self._outgoing = []
        self._foes = {}
        self._prohibits = {}
        self._incLanes = incLanes
        self._intLanes = intLanes

    def getID(self):
        return self._id

    def addOutgoing(self, edge):
        self._outgoing.append(edge)

    def getOutgoing(self):
        return self._outgoing

    def addIncoming(self, edge):
        self._incoming.append(edge)

    def getIncoming(self):
        return self._incoming

    def getInternal(self):
        return self._intLanes

    def setFoes(self, index, foes, prohibits):
        self._foes[index] = foes
        self._prohibits[index] = prohibits

    def areFoes(self, link1, link2):
        return self._foes[link1][len(self._foes[link1]) - link2 - 1] == '1'

    def getLinkIndex(self, conn):
        ret = 0
        for lane_id in self._incLanes:
            (edge_id, index) = lane_id.split("_")
            edge = [e for e in self._incoming if e.getID() == edge_id][0]
            for candidate_conn in edge.getLane(int(index)).getOutgoing():
                if candidate_conn == conn:
                    return ret
                ret += 1
        return -1

    def forbids(self, possProhibitor, possProhibited):
        possProhibitorIndex = self.getLinkIndex(possProhibitor)
        possProhibitedIndex = self.getLinkIndex(possProhibited)
        if possProhibitorIndex < 0 or possProhibitedIndex < 0:
            return False
        ps = self._prohibits[possProhibitedIndex]
        return ps[-(possProhibitorIndex - 1)] == '1'

    def getCoord(self):
        return self._coord

    def getType(self):
        return self._type

    def getConnections(self, source=None, target=None):
        incoming = list(self.getIncoming())
        if source:
            incoming = [source]
        conns = []
        for e in incoming:
            for l in e.getLanes():
                all_outgoing = l.getOutgoing()
                outgoing = []
                if target:
                    for o in all_outgoing:
                        if o.getTo() == target:
                            outgoing.append(o)
                else:
                    outgoing = all_outgoing
                conns.extend(outgoing)
        return conns
