#!/usr/bin/env python
"""
@file    generateTripsXml.py
@author  Daniel Krajzewicz
@author  Yun-Pang Floetteroed
@author  Eric Melde
@author  Michael Behrisch
@date    2009-02-09
@version $Id$

This script generate a trip file as input data in sumo

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, string, sys, operator, math, datetime, random, bz2, StringIO
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "assign"))
from dijkstra import dijkstraPlain
from inputs import getMatrix
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net

# This class is used to build the nodes in the investigated network and 
# includes the update-function for searching the k shortest paths.

class Net(sumolib.net.Net):
    def __init__(self):
        sumolib.net.Net.__init__(self)
        self._endVertices = []
        self._startVertices = []
        
    def addNode(self, id, type=None, coord=None, incLanes=None):
        if id not in self._id2node:
            node = Vertex(id, coord, incLanes)
            self._nodes.append(node)
            self._id2node[id] = node
        self.setAdditionalNodeInfo(self._id2node[id], type, coord, incLanes)
        return self._id2node[id]

    def getEdge(self, edgeLabel):
        return self._id2edge[edgeLabel]

    def addSourceTargetEdge(self, edgeObj):
        edgeObj.source.addOutgoing(edgeObj)
        edgeObj.target.addIncoming(edgeObj)
        self._edges[edgeObj._id] = edgeObj
                          
    def getstartVertices(self):
        return self._startVertices
        
    def getendVertices(self):
        return self._endVertices
        
    def getstartCounts(self):
        return len(self._startVertices)
        
    def getendCounts(self):
        return len(self._endVertices)
        
    def getTargets(self):
        target = set()
        for end in self._endVertices:
            for sink in end.sinkConnNodes:
                if sink not in target:
                   target.add(end)
        return target
        
    def checkRoute(self, startVertex, endVertex, start, end, P, odConnTable, source, options):
        for node in endVertex.sinkConnNodes:
            length = 0.
            vertex = node
            if node in P: 
                link = P[node]
                if options.limitlength:
                    while vertex != source:
                        #if P[vertex].kind == "real":
                        length += P[vertex].length
                        vertex = P[vertex].source
                odConnTable[startVertex._id][endVertex._id].append([source.getOutgoing()[0]._id, link._id, length])
             
        if options.limitlength and len(odConnTable[startVertex._id][endVertex._id]) > 0:
            for count, item in enumerate(odConnTable[startVertex._id][endVertex._id]):
                if count == 0:
                    minLength = item[2]
                else:
                    if item[2] < minLength:
                        minLength = item[2]
            minLength *= 1.6
            for item in odConnTable[startVertex._id][endVertex._id]:
                if item[2] > minLength:
                    odConnTable[startVertex._id][endVertex._id].remove(item)

class Vertex(sumolib.net.Node):
    """
    This class is to store node attributes and the respective incoming/outgoing links.
    """
    def __init__(self, id, type=None, coord=None, incLanes=None):
        sumolib.net.Node.__init__(self, id, type, coord, incLanes)
        self.sourceConnNodes = []
        self.sinkConnNodes = []
    
    def __repr__(self):
        return self._id

# This class is uesed to store link information and estimate 
# as well as flow and capacity for the flow computation and some parameters
# read from the net.
class Edge(sumolib.net.Edge):
    """
    This class is to record link attributes
    """
    def __init__(self, id, source, target, prio, function, name):
        sumolib.net.Edge.__init__(self, id, source, target, prio, function, name)
        self.capacity = sys.maxint
        self.freeflowtime = 0.0
        self.helpacttime = 0.0
        self.weight = 0.
        self.connection = 0.
            
    def addLane(self, lane):
        sumolib.net.Edge.addLane(self, lane)
        if self._from._id == self._to._id:
            self.freeflowtime = 0.0
        else:
            self.freeflowtime = self._length / self._speed
            self.actualtime = self.freeflowtime
            self.helpacttime = self.freeflowtime

    def __repr__(self):
        cap = str(self.capacity)
        if self.capacity == sys.maxint or self.connection != 0:
            cap = "inf"
        return "%s_%s_%s_%s<%s|%s|%s|%s|%s|%s|%s|%s|%s>" % (self._function, self._id, self._from, self._to, self._speed)

class Trip:
    """
    This class is to store trip attributes.
    """
    def __init__(self, num, depart, source, sink, sourceD, sinkD):
        self.label = "%s" % num
        self.depart = depart
        self.sourceEdge = source
        self.sinkEdge = sink
        self.sourceDistrict = sourceD
        self.sinkDistrict = sinkD
        
    def __repr__(self):
        return self.label
              
# The class is for parsing the XML input file (districts). The data parsed is written into the net.
class DistrictsReader(handler.ContentHandler):
    def __init__(self, net):
        self._net = net
        self._district = None
        self.I = 100

    def startElement(self, name, attrs):
        if name == 'taz':
            self._districtSource = self._net.addNode(attrs['id'])
            self._net._startVertices.append(self._districtSource)
            self._districtSink = self._net.addNode(attrs['id'])
            self._net._endVertices.append(self._districtSink)
        elif name == 'tazSink':
            sinklink = self._net.getEdge(attrs['id'])
            self.I += 1
            conlink = self._districtSink._id + str(self.I)
            newEdge = self._net.addEdge(conlink, sinklink._to._id, self._districtSink._id, "-1", "virtual", "")
            speed = sinklink.getSpeed()
            length = sinklink.getLength()

            for i in range(0,sinklink.getLaneNumber()):
                newLane = self._net.addLane(newEdge, speed, length)
                newEdge.addLane(newLane)
                fromlane = sinklink.getLane(i)
                self._net.addConnection(sinklink, newEdge, fromlane, newLane, "s", "", -1)
            newEdge.weight = attrs['weight']
            self._districtSink.sinkConnNodes.append(sinklink._to)
            newEdge.connection = 1
        elif name == 'tazSource':
            sourcelink = self._net.getEdge(attrs['id'])
            self.I += 1
            conlink = self._districtSource._id + str(self.I)
            newEdge = self._net.addEdge(conlink, self._districtSource._id, sourcelink._from._id, "-1", "virtual", "")
            speed = sourcelink.getSpeed()
            length = sourcelink.getLength()

            for i in range(0,sourcelink.getLaneNumber()):
                newLane = self._net.addLane(newEdge, speed, length)
                newEdge.addLane(newLane)
                tolane = sourcelink.getLane(i)
                self._net.addConnection(newEdge, sourcelink, newLane, tolane, "s", "", -1)

            newEdge.weight = attrs['weight']
            self._districtSource.sourceConnNodes.append(sourcelink._from)
            newEdge.connection = 2
            
    def endElement(self, name):
        if name == 'taz':
            self._district = ''
    
def addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList, vehIDtoODMap):
    counts += 1.
    vehID += 1
    endtime = int((float(begin + period) - 0.5) * 3600)    # The last half hour will not release any vehicles
    depart = random.randint(begin * 3600, endtime)
    if len(odConnTable[startVertex._id][endVertex._id]) > 0:
        connIndex = random.randint(0, len(odConnTable[startVertex._id][endVertex._id]) - 1)
        connPair = odConnTable[startVertex._id][endVertex._id][connIndex]
        veh = Trip(vehID, depart, connPair[0], connPair[1], startVertex._id, endVertex._id)
        vehIDtoODMap[str(vehID)] = [startVertex._id, endVertex._id]
        tripList.append(veh)
    
    return counts, vehID, tripList, vehIDtoODMap
       
def main(options):
    parser = make_parser()
    isBZ2 = False
    dataDir = options.datadir
    districts = os.path.join(dataDir, options.districtfile)
    matrix = os.path.join(dataDir, options.mtxfile)
    netfile = os.path.join(dataDir, options.netfile)
    print 'generate Trip file for:', netfile
    
    if "bz2" in netfile:
        netfile = bz2.BZ2File(netfile)
        isBZ2 = True
     
    matrixSum = 0.
    tripList = []
    net = Net()
    odConnTable = {}
    vehIDtoODMap = {}

    sumolib.net.readNet(options.netfile, net=net)

    if isBZ2:
        parser.parse(StringIO.StringIO(netfile.read()))
        netfile.close()
    else:
        parser.parse(netfile)

    parser.setContentHandler(DistrictsReader(net))
    parser.parse(districts)

    matrixPshort, startVertices, endVertices, currentMatrixSum, begin, period = getMatrix(net, options.debug, matrix, matrixSum)[:6]

    for edge in net.getEdges():
        edge.helpacttime = 0.
        
    if options.debug:
        print len(net._edges), "edges read"
        print len(net._startVertices), "start vertices read"
        print len(net._endVertices), "target vertices read"
        print 'currentMatrixSum:', currentMatrixSum
        
    if options.getconns:
        if options.debug:
            print 'generate odConnTable'
        for start, startVertex in enumerate(startVertices):
            if startVertex._id not in odConnTable:
                odConnTable[startVertex._id] = {}
                
            for source in startVertex.sourceConnNodes:
                targets = net.getTargets()
                D, P = dijkstraPlain(source, targets)
                for end, endVertex in enumerate(endVertices):
                    if startVertex._id != endVertex._id and matrixPshort[start][end] > 0.:
                        if endVertex._id not in odConnTable[startVertex._id]:
                            odConnTable[startVertex._id][endVertex._id] = []
                        net.checkRoute(startVertex, endVertex, start, end, P, odConnTable, source, options)
    else:
        if options.debug:
            print 'import and use the given odConnTable'
        sys.path.append(options.datadir)
        from odConnTables import odConnTable
        
    # output trips
    if options.verbose:
        print 'output the trip file'
    vehID = 0
    subVehID = 0
    random.seed(42)
    matrixSum = 0.
    fouttrips = file(options.tripfile, 'w')
    fouttrips.write('<?xml version="1.0"?>\n')
    print >> fouttrips, """<!-- generated on %s by $Id$ -->
    """ % datetime.datetime.now()
    fouttrips.write("<tripdefs>\n")

    if options.demandscale != 1.:
        print 'demand scale %s is used.' % options.demandscale
        for start in range(len(startVertices)):
            for end in range(len(endVertices)):
                matrixPshort[start][end] *= options.demandscale

    for start, startVertex in enumerate(startVertices):
        for end, endVertex in enumerate(endVertices):
            if startVertex._id != endVertex._id and matrixPshort[start][end] > 0.:
                counts = 0.
                if options.odestimation:
                    if matrixPshort[start][end] < 1.:
                        counts, vehID, tripList, vehIDtoODMap = addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList, vehIDtoODMap)
                    else:
                        matrixSum += matrixPshort[start][end]
                        while (counts < float(math.ceil(matrixPshort[start][end])) and (matrixPshort[start][end] - counts) > 0.5 and float(subVehID) < matrixSum)or float(subVehID) < matrixSum:
                            counts, vehID, tripList, vehIDtoODMap = addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList, vehIDtoODMap)
                            subVehID += 1
                else:
                    matrixSum += matrixPshort[start][end]
                    while (counts < float(math.ceil(matrixPshort[start][end])) and (matrixPshort[start][end] - counts) > 0.5 and float(vehID) < matrixSum) or float(vehID) < matrixSum:
                        counts, vehID, tripList, vehIDtoODMap = addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList, vehIDtoODMap)
    if options.debug:
        print 'total demand:', matrixSum           
        print vehID, 'trips generated' 
    tripList.sort(key=operator.attrgetter('depart'))
    
    departpos = "free"
    if __name__ == "__main__":
        departpos = options.departpos
    for trip in tripList:
        fouttrips.write('   <trip id="%s" depart="%s" from="%s" to="%s" fromtaz="%s" totaz="%s" departlane="free" departpos="%s" departspeed="max"/>\n' \
                            % (trip.label, trip.depart, trip.sourceEdge, trip.sinkEdge, trip.sourceDistrict, trip.sinkDistrict, departpos))
    fouttrips.write("</tripdefs>")
    fouttrips.close()
    
    return odConnTable, vehIDtoODMap

if __name__ == "__main__":    
    optParser = OptionParser()
    optParser.add_option("-r", "--data-dir", dest="datadir",
                         default=os.getcwd(), help="give the data directory path")
    optParser.add_option("-n", "--net-file", dest="netfile",
                         help="define the net file (mandatory)")
    optParser.add_option("-m", "--matrix-file", dest="mtxfile",
                         help="define the matrix file (mandatory)")
    optParser.add_option("-d", "--districts-file", dest="districtfile",
                         help="define the district file (mandatory)")
    optParser.add_option("-l", "--limitlength", action="store_true", dest="limitlength",
                        default=False, help="the route length of possible connections of a given OD pair shall be less than 1.6 * min.length")   
    optParser.add_option("-t", "--trip-file", dest="tripfile",
                         default="trips.trips.xml", help="define the output trip filename")
    optParser.add_option("-x", "--odestimation", action="store_true", dest="odestimation",
                         default=False, help="generate trips for OD estimation")
    optParser.add_option("-b", "--debug", action="store_true",
                         default=False, help="debug the program")
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-f", "--scale-factor", dest="demandscale", type="float", default=1., help="scale demand by ")
    optParser.add_option("-D", "--depart-pos", dest="departpos", type="choice",
                     choices=('random', 'free', 'random_free'),
                     default='free', help="choose departure position: random, free, random_free")
    optParser.add_option("-C", "--get-connections", action="store_true", dest="getconns",
                     default=True, help="generate the OD connection directory, if set as False, a odConnTables.py should be available in the defined data directory")
    (options, args) = optParser.parse_args()
    
    if not options.netfile or not options.mtxfile or not options.districtfile:
        optParser.print_help()
        sys.exit()
    
    main(options)
