"""
@file    net.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Karol Stosiek
@author  Michael Behrisch
@date    2008-03-27
@version $Id$

This file contains a content handler for parsing sumo network xml files
and classes that represent the network.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, sys
from xml.sax import saxutils, parse, handler
from copy import copy
from itertools import *

class Lane:
    """ Lanes from a sumo network """

    def __init__(self, edge, speed, length):
        self._edge = edge
        self._speed = speed
        self._length = length
        self._shape = []
        self._outgoing = []
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
         

class Node:
    """ Nodes from a sumo network """
    def __init__(self, id, coord, incLanes):
        self._id = id
        self._coord = coord
        self._incoming = []
        self._outgoing = []
        self._foes = {}
        self._prohibits = {}
        self._incLanes = incLanes

    def addOutgoing(self, edge):
        self._outgoing.append(edge)
        
    def getOutgoing(self):
        return self._outgoing

    def addIncoming(self, edge):
        self._incoming.append(edge)

    def getIncoming(self):
        return self._incoming

    def setFoes(self, index, foes, prohibits):
        self._foes[index] = foes
        self._prohibits[index] = prohibits

    def areFoes(self, link1, link2):
        return self._foes[link1][len(self._foes[link1]) - link2 - 1] == '1'

    def getLinkIndex(self, link):
        ret = 0
        for lid in self._incLanes:
            (e, l) = lid.split("_")
            lane = None
            for et in self._incoming:
                for l in et._lanes:
                    if l==link[0]:
                        lane = l
            
            if l[0]==link[0] and l[1]==link[1]:
                return ret
            ret += 1
        return -1

    def forbids(self, possProhibitor, possProhibited):
        possProhibitorIndex = self.getLinkIndex(possProhibitor)
        possProhibitedIndex = self.getLinkIndex(possProhibited)
        if possProhibitorIndex < 0 or possProhibitedIndex < 0:
            return False
        ps = self._prohibits[possProhibitedIndex]
        return ps[-(possProhibitorIndex-1)]=='1'


class Connection:
    """edge connection for a sumo network"""
    def __init__(self, fromEdge, toEdge, fromLane, toLane, direction, tls, tllink):
        self._from = fromEdge
        self._to = toEdge
        self._fromLane = fromLane
        self._toLane = toLane
        self._tls = tls
        self._tlLink = tllink


class TLS:
    """Traffic Light Signal for a sumo network"""
    def __init__(self, id):
        self._id = id
        self._connections = []
        self._maxConnectionNo = -1
        self._programs = {}

    def addConnection(self, inLane, outLane, linkNo):
        self._connections.append( [inLane, outLane, linkNo] )
        if linkNo>self._maxConnectionNo:
            self._maxConnectionNo = linkNo

    def getConnections(self):
        return self._connections

    def getID(self):
        return self._id

    def getLinks(self):
        links = {}
        for connection in self._connections:
            if connection[2] not in links:
                links[connection[2]] = []
            links[connection[2]].append(connection)
        return links

    def getEdges(self):
        edges = set()
        for c in self._connections:
            edges.add(c[0].getEdge())
        return edges

    def addProgram(self, program):
        self._programs[program._id] = program


class TLSProgram:
    def __init__(self, id, offset, type):
        self._id = id
        self._type = type
        self._offset = offset
        self._phases = []

    def addPhase(self, state, duration):
        self._phases.append( (state, duration) )


class Roundabout:
    def __init__(self, nodes):
        self._nodes = nodes

    def getNodes(self):
        return self._nodes


class Net:
    """The whole sumo network."""
    def __init__(self):
        self._id2node = {}
        self._id2edge = {}
        self._id2tls = {}
        self._nodes = []
        self._edges = []
        self._tlss = []
        self._ranges = [ [10000, -10000], [10000, -10000] ]
        self._roundabouts = []

    def addNode(self, id, coord=None, incLanes=None):
        if id not in self._id2node:
            node = Node(id, coord, incLanes)
            self._nodes.append(node)
            self._id2node[id] = node
        self.setAdditionalNodeInfo(self._id2node[id], coord, incLanes)
        return self._id2node[id]
    
    def setAdditionalNodeInfo(self, node, coord, incLanes):
        if coord!=None and node._coord==None:
            node._coord = coord
            self._ranges[0][0] = min(self._ranges[0][0], coord[0])
            self._ranges[0][1] = max(self._ranges[0][1], coord[0])
            self._ranges[1][0] = min(self._ranges[1][0], coord[1])
            self._ranges[1][1] = max(self._ranges[1][1], coord[1])
        if incLanes!=None and node._incLanes==None:
            node._incLanes = incLanes

    def addEdge(self, id, fromID, toID, prio, function, name):
        if id not in self._id2edge:
            fromN = self.addNode(fromID)
            toN = self.addNode(toID)
            edge = Edge(id, fromN, toN, prio, function, name)
            self._edges.append(edge)
            self._id2edge[id] = edge
        return self._id2edge[id]

    def addLane(self, edge, speed, length):
        return Lane(edge, speed, length)

    def addRoundabout(self, nodes):
        roundabout = Roundabout(nodes)
        self._roundabouts.append(roundabout)
        return roundabout

    def addConnection(self, fromEdge, toEdge, fromlane, tolane, direction, tls, tllink):
        conn = Connection(fromEdge, toEdge, fromlane, tolane, direction, tls, tllink)
        fromEdge.addOutgoing(conn)
        fromlane.addOutgoing(conn)
        toEdge._addIncoming(conn)

    def getEdges(self):
        return self._edges

    def getRoundabouts(self):
        return self._roundabouts
        
    def getEdge(self, id):
        return self._id2edge[id]

    def getNode(self, id):
        return self._id2node[id]

    def getNodes(self):
        return self._nodes

    def getTLSSecure(self, tlid):
        if tlid in self._id2tls:
            tls = self._id2tls[tlid]
        else:
            tls = TLS(tlid)
            self._id2tls[tlid] = tls
            self._tlss.append(tls)
        return tls

    def addTLS(self, tlid, inLane, outLane, linkNo):
        tls = self.getTLSSecure(tlid)
        tls.addConnection(inLane, outLane, linkNo)
        return tls

    def addTLSProgram(self, tlid, programID, offset, type):
        tls = self.getTLSSecure(tlid)
        program = TLSProgram(programID, offset, type)
        tls.addProgram(program)
        return program


    def setFoes(self, junctionID, index, foes, prohibits):
        self._id2node[junctionID].setFoes(index, foes, prohibits)

    def forbids(self, possProhibitor, possProhibited):
        return possProhibitor[0].getEdge()._to.forbids(possProhibitor, possProhibited)

    def getDownstreamEdges(self, edge, distance, stopOnTLS):
        ret = []
        seen = set()
        toProc = []
        toProc.append( [edge, 0, [] ] )
        while not len(toProc)==0:
            ie = toProc.pop()
            if ie[0] in seen:
                continue
            seen.add(ie[0])
            if ie[1] + ie[0].getLength() >= distance:
                ret.append( [ie[0], ie[0].getLength()+ie[1]-distance, ie[2], False] )
                continue
            if len(ie[0]._incoming)==0:
                ret.append( [ie[0], ie[0].getLength()+ie[1], ie[2], True] )
                continue
            mn = []
            hadTLS = False
            for ci in ie[0]._incoming:
                if ci not in seen:
                    prev = copy(ie[2])
                    if stopOnTLS and ci._tls and ci!=edge and not hadTLS:
                        ret.append( [ie[0], ie[1], prev, True ] )
                        hadTLS = True
                    else:
                        prev.append(ie[0])
                        mn.append( [ci, ie[0].getLength()+ie[1], prev ] )
            if not hadTLS:
                toProc.extend(mn)
        return ret

    
class NetReader(handler.ContentHandler):
    """Reads a network, storing the edge geometries, lane numbers and max. speeds"""

    def __init__(self, **others):
        self._net = others.get('net', Net())
        self._currentEdge = None
        self._currentNode = None
        self._currentLane = None
        self._currentShape = ""
        self._withPhases = others.get('withPrograms', False)
        self._withConnections = others.get('withConnections', True)
        self._withFoes = others.get('withFoes', True)

    def startElement(self, name, attrs):
        if name == 'edge':
            if not attrs.has_key('function') or attrs['function'] != 'internal':
                prio = -1
                if attrs.has_key('priority'):
                    prio = int(attrs['priority'])
                function = ""
                if attrs.has_key('function'):
                    function = attrs['function']
                name = ""
                if attrs.has_key('name'):
                    name = attrs['name']
                self._currentEdge = self._net.addEdge(attrs['id'],
                    attrs['from'], attrs['to'], prio, function, name)
            else:
                self._currentEdge = None
        if name == 'lane' and self._currentEdge!=None:
            self._currentLane = self._net.addLane(self._currentEdge, float(attrs['speed']), float(attrs['length']))
            if attrs.has_key('shape'):
                self._currentShape = attrs['shape'] # deprecated: at some time, this is mandatory
            else:
                self._currentShape = ""
        if name == 'junction':
            if attrs['id'][0]!=':':
                self._currentNode = self._net.addNode(attrs['id'], [ float(attrs['x']), float(attrs['y']) ], attrs['incLanes'].split(" ") )
        if name == 'succ' and self._withConnections: # deprecated
            if attrs['edge'][0]!=':':
                self._currentEdge = self._net.getEdge(attrs['edge'])
                self._currentLane = attrs['lane']
                self._currentLane = int(self._currentLane[self._currentLane.rfind('_')+1:])
            else:
                self._currentEdge = None
        if name == 'succlane' and self._withConnections: # deprecated
            lid = attrs['lane']
            if lid[0]!=':' and lid!="SUMO_NO_DESTINATION" and self._currentEdge:
                connected = self._net.getEdge(lid[:lid.rfind('_')])
                tolane = int(lid[lid.rfind('_')+1:])
                if attrs.has_key('tl') and attrs['tl']!="":
                    tl = attrs['tl']
                    tllink = int(attrs['linkIdx'])
                    tlid = attrs['tl']
                    toEdge = self._net.getEdge(lid[:lid.rfind('_')])
                    tolane2 = toEdge._lanes[tolane]
                    tls = self._net.addTLS(tlid, self._currentEdge._lanes[self._currentLane], tolane2, tllink)
                    self._currentEdge.setTLS(tls)
                else:
                    tl = ""
                    tllink = -1
                toEdge = self._net.getEdge(lid[:lid.rfind('_')])
                tolane = toEdge._lanes[tolane]
                self._net.addConnection(self._currentEdge, connected, self._currentEdge._lanes[self._currentLane], tolane, attrs['dir'], tl, tllink)
        if name == 'connection' and self._withConnections and attrs['from'][0] != ":":
            fromEdge = self._net.getEdge(attrs['from'])
            toEdge = self._net.getEdge(attrs['to'])
            fromLane = fromEdge.getLane(int(attrs['fromLane']))
            toLane = toEdge.getLane(int(attrs['toLane']))
            if attrs.has_key('tl') and attrs['tl']!="":
                tl = attrs['tl']
                tllink = int(attrs['linkIndex'])
                tls = self._net.addTLS(tl, fromLane, toLane, tllink)
                fromEdge.setTLS(tls)
            else:
                tl = ""
                tllink = -1
            self._net.addConnection(fromEdge, toEdge, fromLane, toLane, attrs['dir'], tl, tllink)
        if self._withFoes and (name=='ROWLogic' or name=='row-logic'): # 'row-logic' is deprecated!!!
            self._currentNode = attrs['id']
        if name == 'logicitem' and self._withFoes: # deprecated
            self._net.setFoes(self._currentNode, int(attrs['request']), attrs["foes"], attrs["response"])
        if name == 'request' and self._withFoes:
            self._currentNode.setFoes(int(attrs['index']), attrs["foes"], attrs["response"])
        if self._withPhases and (name=='tlLogic' or name=='tl-logic'): # tl-logic is deprecated!!!
            self._currentProgram = self._net.addTLSProgram(attrs['id'], attrs['programID'], int(attrs['offset']), attrs['type'])
        if self._withPhases and name=='phase':
            self._currentProgram.addPhase(attrs['state'], int(attrs['duration']))
        if name == 'roundabout':
            self._net.addRoundabout(attrs['nodes'].split())

    def characters(self, content):
        if self._currentLane!=None:
            self._currentShape = self._currentShape + content


    def endElement(self, name):
        if name == 'lane' and self._currentLane:
            cshape = []
            es = self._currentShape.split(" ")
            for e in es:
                p = e.split(",")
                cshape.append((float(p[0]), float(p[1])))
            self._currentLane.setShape(cshape)
            self._currentLane = None
            self._currentShape = ""
        if name == 'edge' and self._currentEdge:
            self._currentEdge.rebuildShape();
        if name == 'edge':
            self._currentEdge = None
        if name=='ROWLogic' or name=='row-logic': # 'row-logic' is deprecated!!!
            self._haveROWLogic = False
        if self._withPhases and (name=='tlLogic' or name=='tl-logic'): # tl-logic is deprecated!!!
            self._currentProgram = None

    def getNet(self):
        return self._net


def readNet(filename, **others):
    netreader = NetReader(**others)
    try:
        parse(filename, netreader)
    except KeyError:
        print >> sys.stderr, "Please mind that the network format has changed in 0.13.0, you may need to update your network!"
        raise
    return netreader.getNet()
