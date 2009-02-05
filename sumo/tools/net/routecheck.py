#!/usr/bin/env python
"""
@file    routecheck.py
@author  Yun-Pang.Wang@dlr.de
@date    2007-03-20
@version $Id: routecheck.py 6171 2008-10-20 12:21:32Z wang $

This script checks if at least one route for a trip (with a given origin and destiantion) exists.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os, string, sys, operator, glob
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
from dijkstra import dijkstraPlain

# This class is used to build the nodes in the investigated network and 
# includes the update-function for searching the k shortest paths.
class Vertex:
    """
    This class is to store node attributes and the respective incoming/outgoing links.
    """
    def __init__(self, num):
        self.inEdges = []
        self.outEdges = []
        self.label = "%s" % num
        self.sourceEdges = []
        self.sinkEdges = []

    def __repr__(self):
        return self.label

# This class is uesed to store link information and estimate 
# as well as flow and capacity for the flow computation and some parameters
# read from the net.
class Edge:
    """
    This class is to record link attributes
    """
    def __init__(self, label, source, target, kind="junction"):
        self.label = label
        self.source = source
        self.target = target
        self.capacity = sys.maxint
        self.kind = kind
        self.maxspeed = 1.0
        self.length = 0.0
        self.freeflowtime = 0.0
        self.numberlane = 0
        self.validateFlows = 0.
        self.helpacttime = self.freeflowtime
        
    def init(self, speed, length, laneNumber):
        self.maxspeed = float(speed)
        self.length = float(length)
        self.numberlane = float(laneNumber)
        if self.source.label == self.target.label:
            self.freeflowtime = 0.0
        else:
            self.freeflowtime = self.length / self.maxspeed
            self.helpacttime = self.freeflowtime
        
    def __repr__(self):
        cap = str(self.capacity)
        if self.capacity == sys.maxint or self.connection != 0:
            cap = "inf"
        return "%s_<%s|%s|%s>" % (self.label, self.kind, self.source, self.target)
                                                      
class Net:
    def __init__(self):
        self._vertices = []
        self._edges = {}
        self._endVertices = []
        self._startVertices = []
        self.ignoreZones = []
        self.sinkEdges = []
        self.sourceEdges = []
        
    def newVertex(self):
        v = Vertex(len(self._vertices))
        self._vertices.append(v)
        return v

    def getEdge(self, edgeLabel):
        return self._edges[edgeLabel]
        
    def addEdge(self, edgeObj):
        edgeObj.source.outEdges.append(edgeObj)
        edgeObj.target.inEdges.append(edgeObj)
        if edgeObj.kind == "real":
            self._edges[edgeObj.label] = edgeObj

    def addIsolatedRealEdge(self, edgeLabel):
        self.addEdge(Edge(edgeLabel, self.newVertex(), self.newVertex(),
                          "real"))                    
    def getTargets(self, ignoreZones):
        target = set()
        for end in self._endVertices:
            if end not in ignoreZones:
                target.add(end)
        return target
            

# The class is for parsing the XML input file (network file). The data parsed is written into the net.
class NetworkReader(handler.ContentHandler):
    def __init__(self, net):
        self._net = net
        self._edge = ''
        self._maxSpeed = 0
        self._laneNumber = 0
        self._length = 0
        self._edgeObj = None
        self._chars = ''
        self._counter = 0
        self._turnlink = None

    def startElement(self, name, attrs):
        self._chars = ''
        if name == 'edge' and (not attrs.has_key('function') or attrs['function'] != 'internal'):
            self._edge = attrs['id']
            self._net.addIsolatedRealEdge(self._edge)
            self._edgeObj = self._net.getEdge(self._edge)
            self._edgeObj.source.label = attrs['from']
            self._edgeObj.target.label = attrs['to']
            self._maxSpeed = 0
            self._laneNumber = 0
            self._length = 0
        elif name == 'succ':
            self._edge = attrs['edge']
            if self._edge[0]!=':':
                self._edgeObj = self._net.getEdge(self._edge)
        elif name == 'succlane' and self._edge!="":       
            l = attrs['lane']
            if l != "SUMO_NO_DESTINATION":
                toEdge = self._net.getEdge(l[:l.rfind('_')])
                newEdge = Edge(self._edge+"_"+l[:l.rfind('_')], self._edgeObj.target, toEdge.source)
                self._net.addEdge(newEdge)
                self._edgeObj.finalizer = l[:l.rfind('_')]
        elif name == 'lane' and self._edge != '':
            self._maxSpeed = max(self._maxSpeed, float(attrs['maxspeed']))
            self._laneNumber = self._laneNumber + 1
            self._length = float(attrs['length'])
      
    def characters(self, content):
        self._chars += content

    def endElement(self, name):
        if name == 'edge':
            self._edgeObj.init(self._maxSpeed, self._length, self._laneNumber)
            self._edge = ''
                
# The class is for parsing the XML input file (districts). The data parsed is written into the net.
class DistrictsReader(handler.ContentHandler):
    def __init__(self, net):
        self._net = net
        self._StartDTIn = None
        self._StartDTOut = None
        self.I = 100

    def startElement(self, name, attrs):
        if name == 'district':
            self._StartDTIn = self._net.newVertex()
            self._StartDTIn.label = attrs['id']
            self._StartDTOut = self._net.newVertex()
            self._StartDTOut.label = self._StartDTIn.label
            self._net._startVertices.append(self._StartDTIn)
            self._net._endVertices.append(self._StartDTOut)
        elif name == 'dsink':
            sinklink = self._net.getEdge(attrs['id'])
            self.I += 1
            conlink = self._StartDTOut.label + str(self.I)
            newEdge = Edge(conlink, sinklink.target, self._StartDTOut, "real")
            self._net.addEdge(newEdge)
            newEdge.weight = attrs['weight']
            newEdge.connection = 1              
        elif name == 'dsource':
            sourcelink = self._net.getEdge(attrs['id'])
            self.I += 1
            conlink = self._StartDTIn.label + str(self.I)
            newEdge = Edge(conlink, self._StartDTIn, sourcelink.source, "real")
            self._net.addEdge(newEdge)
            newEdge.weight = attrs['weight']
            newEdge.connection = 2
            
def getMatrix(net, verbose, matrix, MatrixSum):#, mtxplfile, mtxtfile):
    """
    This method is to read matrix from the given file.
    """
    matrixPshort = []
    startVertices = []
    endVertices = []
    Pshort_EffCells = 0
    periodList = []
    if verbose:
        print 'matrix:', str(matrix)                                 

    ODpairs = 0
    origins = 0
    dest= 0
    CurrentMatrixSum = 0.0
    skipCount = 0
    zones = 0
    smallDemandNum = 0
    for line in open(matrix):
        if line[0] == '$':
            visumCode = line[1:3]
            if visumCode != 'VM':
                skipCount += 1
        elif line[0] != '*' and line[0] != '$':
            skipCount += 1
            if skipCount == 2:
                for elem in line.split():
                    periodList.append(float(elem))
            elif skipCount > 3:
                if zones == 0:
                    for elem in line.split():
                        zones = int(elem)
                elif len(startVertices) < zones:
                    for elem in line.split():
                        if len(elem) > 0:
                            for startVertex in net._startVertices:
                                if startVertex.label == str(elem):
                                    startVertices.append(startVertex)
                            for endVertex in net._endVertices:
                                if endVertex.label == str(elem):
                                    endVertices.append(endVertex)
                    origins = len(startVertices)
                    dest = len(endVertices)        
                elif len(startVertices) == zones:
                    if ODpairs % origins == 0:
                        matrixPshort.append([])
                    for item in line.split():
                        matrixPshort[-1].append(float(item))
                        ODpairs += 1
                        MatrixSum += float(item)
                        CurrentMatrixSum += float(item) 
                        if float(item) > 0.:
                            Pshort_EffCells += 1
                        if float(item) < 1. and float(item) > 0.:
                            smallDemandNum += 1
    begintime = int(periodList[0])
    assignPeriod = int(periodList[1]) - begintime
    smallDemandRatio = float(smallDemandNum)/float(Pshort_EffCells)

    return matrixPshort
    
def main():
    parser = make_parser()
    odPairSet= []
    net = Net()
    count = 0
    tripDir = os.getcwd()
    districts = options.districtfile
    matrix = options.mtxfile
    MatrixSum = 0.
    ignoreZones = []
    print options.ignorelist
    for item in options.ignorelist.split(','):
        for district in net._startVertices:
            if district.label == item:
                ignoreZones.append(district)
        
    parser.setContentHandler(NetworkReader(net))
    parser.parse(options.netfile)    
    print 'done with net reading'

    parser.setContentHandler(DistrictsReader(net))
    parser.parse(districts)
    print 'done with district reading'
    
    matrixPshort = getMatrix(net, options.verbose, matrix, MatrixSum)

    print len(net._edges), "edges read"
    print len(net._startVertices), "start vertices read"
    print len(net._endVertices), "target vertices read"    
    totalCounts = 0
    subCounts = 0
    for start, startVertex in enumerate(net._startVertices):
        if startVertex not in ignoreZones:
            targets = net.getTargets(ignoreZones)
            D, P = dijkstraPlain(startVertex, targets)

            for end, endVertex in enumerate(net._endVertices):
                if startVertex.label != endVertex.label and endVertex not in ignoreZones and matrixPshort[start][end] > 0.:
                    totalCounts += 1
                    helpPath = []
                    vertex = endVertex
    
                    while vertex != startVertex:
                        if vertex in P:
                            if P[vertex].kind == "real":
                                helpPath.append(P[vertex])
                            vertex = P[vertex].source    
                        else:
                            subCounts += 1
                            odPairSet.append((startVertex.label, endVertex.label))
                            break
        else:
            for endVertex in ignoreZones:
                if startVertex.label != endVertex.label:
                    totalCounts += 1
                    helpPath = []
                    vertex = endVertex
    
                    while vertex != startVertex:
                        if vertex in P:
                            if P[vertex].kind == "real":
                                helpPath.append(P[vertex])
                            vertex = P[vertex].source    
                        else:
                            subCounts += 1
                            odPairSet.append((startVertex.label, endVertex.label))
                            break

    foutzones = file('absentConnections.txt', 'w')
    
    for pair in odPairSet:
        foutzones.write('from: %s   to: %s\n' %(pair[0], pair[1]))
        if options.verbose:
            print 'from:', pair[0]
            print 'to:', pari[1]
    foutzones.close()
      
    print 'total OD connnetions:', totalCounts
    if len(odPairSet) > 0:
        print subCounts, 'connections are absent!'
    else:
        print 'all connections exist! '
   
optParser = OptionParser()
optParser.add_option("-t", "--trip-file", dest="tripfile",
                     help="define the trip file")
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="define the net file")
optParser.add_option("-m", "--matrix-file", dest="mtxfile",
                     help="define the matrix file")
optParser.add_option("-d", "--districts-file", dest="districtfile",
                     help="define the district file")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-i", "--ignore-zonelist", dest="ignorelist", type='string',
                     default= "dist_00101,dist_00102,dist_00103,dist_00104,dist_00105", help="define the zones which should be ignored")                     
optParser.add_option("-b", "--debug", action="store_true", dest="debug",
                     default=False, help="debug the program")
                                    
(options, args) = optParser.parse_args()

main()