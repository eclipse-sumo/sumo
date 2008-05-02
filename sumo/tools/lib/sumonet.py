#!/usr/bin/env python
"""
@file    netmatching.py
@author  Daniel.Krajzewicz@dlr.de
@date    2008-03-27
@version $Id: $


Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, string, sys
import math
from optparse import OptionParser
from xml.sax import saxutils, make_parser, handler
from numpy import append, array, dot, shape, transpose, zeros
from itertools import *


class NetLane:
    def __init__(self, edge, speed, length):
        self._edge = edge
        self._speed = speed
        self._length = length
        edge.addLane(self)

    def getSpeed(self):
        return self._speed

    def getLength(self):
        return self._length 



class NetEdge:
    def __init__(self, id, fromN, toN, prio):
        self._id = id
        self._from = fromN
        self._to = toN
        self._priority = prio
        fromN.addOutgoing(self)
        toN.addIncoming(self)
        self._lanes = []
        self._speed = None
        self._length = None
        self._incoming = []
        self._outgoing = []

    def addLane(self, lane):
        self._lanes.append(lane)
        self._speed = lane.getSpeed()
        self._length = lane.getLength()

    def addOutgoing(self, edge):
        if edge not in self._outgoing:
            self._outgoing.append(edge)

    def addIncoming(self, edge):
        if edge not in self._incoming:
            self._incoming.append(edge)

    def getIncoming(self):
        return self._incoming

    def getOutgoing(self):
        return self._outgoing



class NetNode:
    def __init__(self, id, coord):
        self._id = id
        self._coord = coord
        self._incoming = []
        self._outgoing = []

    def addOutgoing(self, edge):
        self._outgoing.append(edge)

    def addIncoming(self, edge):
        self._incoming.append(edge)

class Net:
    def __init__(self):
        self._id2node = {}
        self._id2edge = {}
        self._nodes = []
        self._edges = []
        self._ranges = [ [10000, -10000], [10000, -10000] ]

    def addNode(self, id, coord=None):
        if id not in self._id2node:
            node = NetNode(id, coord)
            self._nodes.append(node)
            self._id2node[id] = node
        if coord!=None and self._id2node[id]._coord==None:
            self._id2node[id]._coord = coord
            self._ranges[0][0] = min(self._ranges[0][0], coord[0])
            self._ranges[0][1] = max(self._ranges[0][1], coord[0])
            self._ranges[1][0] = min(self._ranges[1][0], coord[1])
            self._ranges[1][1] = max(self._ranges[1][1], coord[1])
        return self._id2node[id]

    def addEdge(self, id, fromID, toID, prio):
        if id not in self._id2edge:
            fromN = self.addNode(fromID)
            toN = self.addNode(toID)
            edge = NetEdge(id, fromN, toN, prio)
            self._edges.append(edge)
            self._id2edge[id] = edge
        return self._id2edge[id]

    def addLane(self, edge, speed, length):
        lane = NetLane(edge, speed, length)

    def getEdge(self, id):
        return self._id2edge[id]


class NetReader(handler.ContentHandler):
    """Reads a network, storing the edge geometries, lane numbers and max. speeds"""

    def __init__(self):
        self._net = Net()
        self._currentEdge = None
        self._currentNode = None
        self._currentLane = None
        self._currentShape = ""

    def startElement(self, name, attrs):
        if name == 'edge':
            if not attrs.has_key('function') or attrs['function'] != 'internal':
                prio = -1
                if attrs.has_key('priority'):
                    prio = int(attrs['priority'])
                self._currentEdge = self._net.addEdge(attrs['id'], attrs['from'], attrs['to'], prio)
            else:
                self._currentEdge = None
        if name == 'lane' and self._currentEdge!=None:
            self._currentLane = self._net.addLane(self._currentEdge, float(attrs['maxspeed']), float(attrs['length']))
            self._currentShape = ""
        if name == 'junction':
            if attrs['id'][0]!=':':
                self._currentNode = self._net.addNode(attrs['id'], [ float(attrs['x']), float(attrs['y']) ] )
        if name == 'succ':
            if attrs['edge'][0]!=':':
                self._currentEdge = self._net.getEdge(attrs['edge'])
        if name == 'succlane':
            lid = attrs['lane']
            if lid[0]!=':' and lid!="SUMO_NO_DESTINATION":
                connected = self._net.getEdge(lid[:lid.rfind('_')])
                self._currentEdge.addOutgoing(connected)
                connected.addIncoming(self._currentEdge)

    def characters(self, content):
        if self._currentLane!=None:
            self._currentShape =self._currentShape + content

    def endElement(self, name):
        if name == 'lane':
            self._currentLane = None
        if name == 'edge':
            self._currentEdge = None

    def getNet(self):
        return self._net



