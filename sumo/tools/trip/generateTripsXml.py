#!/usr/bin/env python
"""
@file    generateTripsXml.py
@author  Yun-Pang.Wang@dlr.de
@date    2009-02-09
@version $Id$

This script generate a trip file as input data in sumo

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os, string, sys, operator, math, datetime, random, bz2, StringIO
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "assign"))
from dijkstra import dijkstraPlain
from inputs import getMatrix

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
        self.sourceConnNodes = []
        self.sinkConnNodes = []
    def __repr__(self):
        return self.label
        
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
                        if P[vertex].kind == "real":
                            length += P[vertex].length
                        vertex = P[vertex].source
                odConnTable[startVertex.label][endVertex.label].append([source.outEdges[0].label, link.label, length])
             
        if options.limitlength and len(odConnTable[startVertex.label][endVertex.label]) > 0:
            for count, item in enumerate(odConnTable[startVertex.label][endVertex.label]):
                if count == 0:
                    minLength = item[2]
                else:
                    if item[2] < minLength:
                        minLength = item[2]
            minLength *= 1.6
            for item in odConnTable[startVertex.label][endVertex.label]:
                if item[2] > minLength:
                    odConnTable[startVertex.label][endVertex.label].remove(item)
                    
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
        if name == 'edge' and self._edge != '':
            self._edgeObj.init(self._maxSpeed, self._length, self._laneNumber)
            self._edge = ''
                
# The class is for parsing the XML input file (districts). The data parsed is written into the net.
class DistrictsReader(handler.ContentHandler):
    def __init__(self, net):
        self._net = net
        self._district = None
        self.I = 100

    def startElement(self, name, attrs):
        if name == 'district':
            self._districtSource = self._net.newVertex()
            self._districtSource.label = attrs['id']
            self._net._startVertices.append(self._districtSource)
            self._districtSink = self._net.newVertex()
            self._districtSink.label = attrs['id']
            self._net._endVertices.append(self._districtSink)
        elif name == 'dsink':
            sinklink = self._net.getEdge(attrs['id'])
            self.I += 1
            conlink = self._districtSink.label + str(self.I)
            newEdge = Edge(conlink, sinklink.target, self._districtSink, "real")
            self._net.addEdge(newEdge)
            newEdge.weight = attrs['weight']
            self._districtSink.sinkConnNodes.append(sinklink.target)
            newEdge.connection = 1
        elif name == 'dsource':
            sourcelink = self._net.getEdge(attrs['id'])
            self.I += 1
            conlink = self._districtSource.label + str(self.I)
            newEdge = Edge(conlink, self._districtSource, sourcelink.source, "real")
            self._net.addEdge(newEdge)
            newEdge.weight = attrs['weight']
            self._districtSource.sourceConnNodes.append(sourcelink.source)
            newEdge.connection = 2
            
    def endElement(self, name):
        if name == 'district':
            self._district = ''
    
def addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList):
    counts += 1.
    vehID += 1
    depart = random.randint(begin*3600, (begin + period)*3600)
    if len(odConnTable[startVertex.label][endVertex.label]) > 0:
        connIndex = random.randint(0, len(odConnTable[startVertex.label][endVertex.label])-1)
        connPair = odConnTable[startVertex.label][endVertex.label][connIndex]
        veh = Trip(vehID, depart, connPair[0], connPair[1], startVertex.label, endVertex.label)
        tripList.append(veh)
    
    return counts, vehID, tripList    
       
def main(options):
    parser = make_parser()
    isBZ2= False
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
    
    parser.setContentHandler(NetworkReader(net))
    if isBZ2:
        parser.parse(StringIO.StringIO(netfile.read()))
        netfile.close()
    else:
        parser.parse(netfile)    

    parser.setContentHandler(DistrictsReader(net))
    parser.parse(districts)
    
    matrixPshort, startVertices, endVertices, currentMatrixSum, begin, period = getMatrix(net, options.debug, matrix, matrixSum)[:6]

    if options.debug:
        print len(net._edges), "edges read"
        print len(net._startVertices), "start vertices read"
        print len(net._endVertices), "target vertices read"
        print 'currentMatrixSum:', currentMatrixSum
        
    if options.getconns:
        if options.debug:
            print 'generate odConnTable'
        for start, startVertex in enumerate(startVertices):
            if startVertex.label not in odConnTable:
                odConnTable[startVertex.label] = {}
            for source in startVertex.sourceConnNodes:
                targets = net.getTargets()
                D, P = dijkstraPlain(source, targets)
                for end, endVertex in enumerate(endVertices):
                    if startVertex.label != endVertex.label and matrixPshort[start][end] > 0.:
                        if endVertex.label not in odConnTable[startVertex.label]:
                            odConnTable[startVertex.label][endVertex.label]= []
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
    
    if hasattr(options, "scale"):
        for start in range(len(startVertices)):
            for end in range(len(endVertices)):
                matrixPshort[start][end] *= options.scale
                print 'demand:', matrixPshort[start][end]

    for start, startVertex in enumerate(startVertices):
        for end, endVertex in enumerate(endVertices):
            if startVertex.label != endVertex.label and matrixPshort[start][end] > 0.:
                counts = 0.
                if options.odestimation:
                    if matrixPshort[start][end] < 1.:
                        counts, vehID, tripList = addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList)
                    else:
                        matrixSum += matrixPshort[start][end]
                        while (counts < float(math.ceil(matrixPshort[start][end])) and (matrixPshort[start][end] - counts) > 0.5 and float(subVehID) < matrixSum)or float(subVehID) < matrixSum:
                            counts, vehID, tripList = addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList)
                            subVehID += 1
                else:
                    matrixSum += matrixPshort[start][end]
                    while (counts < float(math.ceil(matrixPshort[start][end])) and (matrixPshort[start][end] - counts) > 0.5 and float(vehID) < matrixSum) or float(vehID) < matrixSum:
                        counts, vehID, tripList = addVeh(counts, vehID, begin, period, odConnTable, startVertex, endVertex, tripList)
    if options.debug:
        print 'total demand:', matrixSum           
        print vehID, 'trips generated' 
    tripList.sort(key=operator.attrgetter('depart'))
    
    departpos = "free"
    if __name__ == "__main__":
        departpos = options.departpos
    for trip in tripList:
        fouttrips.write('   <tripdef id="%s" depart="%s" from="%s" to="%s" fromtaz="%s" totaz="%s" departlane="free" departpos="%s" departspeed="max"/>\n' \
                            %(trip.label, trip.depart, trip.sourceEdge, trip.sinkEdge, trip.sourceDistrict, trip.sinkDistrict, departpos))
    fouttrips.write("</tripdefs>")
    fouttrips.close()
    
    return odConnTable

if __name__ == "__main__":    
    optParser = OptionParser()
    optParser.add_option("-r", "--data-dir", dest="datadir",
                         default= os.getcwd(), help="give the data directory path")
    optParser.add_option("-n", "--net-file", dest="netfile",
                         help="define the net file")
    optParser.add_option("-m", "--matrix-file", dest="mtxfile",
                         help="define the matrix file")
    optParser.add_option("-d", "--districts-file", dest="districtfile",
                         help="define the district file")
    optParser.add_option("-l", "--limitlength", action="store_true", dest="limitlength",
                        default=False, help="the route length of possible connections of a given OD pair shall be less than 1.6 * min.length")   
    optParser.add_option("-t", "--trip-file", dest="tripfile",
                         default= "trips.trips.xml", help="define the output trip filename")
    optParser.add_option("-x", "--odestimation", action="store_true", dest="odestimation",
                         default=False, help="generate trips for OD estimation")
    optParser.add_option("-b", "--debug", action="store_true",
                         default=False, help="debug the program")
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-s", "--scale", type="float", default=1., help="scale demand by ")
    optParser.add_option("-D", "--depart-pos", dest="departpos", type="choice",
                     choices=('random', 'free', 'random_free'),
                     default = 'free', help="choose departure position: random, free, random_free")
    optParser.add_option("-C", "--get-connections", action="store_true", dest="getconns",   
                     default= True, help="generate the OD connection directory, if set as False, a odConnTables.py should be available in the defined data directory")
    (options, args) = optParser.parse_args()
    
    if not options.netfile or not options.mtxfile or not options.districtfile:
        optParser.print_help()
        sys.exit()
    
    main(options)