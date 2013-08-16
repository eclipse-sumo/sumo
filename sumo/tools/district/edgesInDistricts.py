#!/usr/bin/env python
"""
@file    edgesInDistricts.py
@author  Daniel Krajzewicz
@date    2007-07-26
@version $Id$

<documentation missing>

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2007-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
import math


def parseShape(shape):
    poses = shape.split()
    ret = []
    for pos in poses:
        xy = pos.split(",")
        ret.append((float(xy[0]), float(xy[1])))
    return ret


def angle2D(p1, p2):
    theta1 = math.atan2(p1[1], p1[0]);
    theta2 = math.atan2(p2[1], p2[0]);
    dtheta = theta2 - theta1;
    while dtheta > 3.1415926535897932384626433832795:
        dtheta -= 2.0*3.1415926535897932384626433832795
    while dtheta < -3.1415926535897932384626433832795:
        dtheta += 2.0*3.1415926535897932384626433832795
    return dtheta
    

def isWithin(pos, shape):
    angle = 0.
    for i in range(0, len(shape)-1):
        p1 = ( (shape[i][0] - pos[0]), (shape[i][1] - pos[1]) )
        p2 = ( (shape[i+1][0] - pos[0]), (shape[i+1][1] - pos[1]) )
        angle = angle + angle2D(p1, p2)
    i = len(shape)-1
    p1 = ( (shape[i][0] - pos[0]), (shape[i][1] - pos[1]) )
    p2 = ( (shape[0][0] - pos[0]), (shape[0][1] - pos[1]) )
    angle = angle + angle2D(p1, p2)
    return math.fabs(angle)>=3.1415926535897932384626433832795


# written into the net. All members are "private".
class NetDistrictEdgeHandler(handler.ContentHandler):
    def __init__(self):
        self._haveDistrict = False
        self._parsingDistrictShape = False
        self._parsingLaneShape = False
        self._currentID = ""
        self._shape = ""
        self._districtShapes = {}
        self._laneShapes = {}
        self._districtLanes = {}
        self._laneDisttricts = {}
        self._invalidatedLanes = []
        self._laneSpeeds = {}
        self._laneLengths = {}
        self._noLanes = {}

    def startElement(self, name, attrs):
        if name == 'taz':    
            self._haveDistrict = True
            self._currentID = attrs['id']
            self._districtLanes[self._currentID] = []
        elif name == 'shape' and self._haveDistrict:
            self._parsingDistrictShape = True
        elif name == 'lane':
            self._currentID = attrs['id']
            self._parsingLaneShape = True
            self._laneDisttricts[self._currentID] = []
            self._laneSpeeds[self._currentID] = float(attrs['speed'])
            self._laneLengths[self._currentID] = float(attrs['length'])
            edgeid = self._currentID
            edgeid = edgeid[:edgeid.rfind("_")]
            if edgeid in self._noLanes:
                self._noLanes[edgeid] = self._noLanes[edgeid] + 1
            else:
                self._noLanes[edgeid] = 1

    def characters(self, content):
        if self._parsingDistrictShape:
            self._shape += content
        if self._parsingLaneShape:
            self._shape += content

    def endElement(self, name):
        if name == 'district':    
            self._haveDistrict = False
        elif name == 'shape' and self._haveDistrict:
            self._parsingDistrictShape = False
            if self._shape!='':
                self._districtShapes[self._currentID] = parseShape(self._shape)
                self._shape = ""
        elif name == 'lane':
            self._parsingLaneShape = False
            if self._shape!='':
                self._laneShapes[self._currentID] = parseShape(self._shape)
                self._shape = ""

    def computeWithin(self, complete):
        for lane in self._laneShapes:
            lshape = self._laneShapes[lane]
            for district in self._districtShapes:
                dshape = self._districtShapes[district]
                found = False
                for pos in lshape:
                    if not found and isWithin(pos, dshape):
                        found = True
                        self._districtLanes[district].append(lane)
                        self._laneDisttricts[lane].append(district)
        if complete:
            for lane in self._laneDisttricts:
                if len(self._laneDisttricts[lane])>1:
                    self._invalidatedLanes.append(lane)

    def writeResults(self, output, maxspeed, weighted):
        fd = open(output, "w")
        fd.write("<tazs>\n")
        for district in self._districtLanes:
            lanes = 0
            for lane in self._districtLanes[district]:
                if lane not in self._invalidatedLanes and self._laneSpeeds[lane]<maxspeed:
                    lanes = lanes + 1
            if lanes==0:
                print "District '" + district + "' has no edges!"
            else:
                edges = {}
                fd.write("   <taz id=\"" + district + "\">\n")
                for lane in self._districtLanes[district]:
                    edgeid = lane 
                    edgeid = edgeid[:edgeid.rfind("_")]
                    if edgeid not in edges and lane not in self._invalidatedLanes and self._laneSpeeds[lane]<maxspeed:
                        weight = 1.
                        if weighted:
                            weight = float(self._laneSpeeds[lane]) * float(self._laneLengths[lane])
                        fd.write("      <tazSource id=\"" + edgeid + "\" weight=\"" + str(weight) + "\"/>\n")
                        fd.write("      <tazSink id=\"" + edgeid + "\" weight=\"" + str(weight) + "\"/>\n")
                        edges[edgeid] = 1
                fd.write("   </taz>\n")
        fd.write("</tazs>\n")
        fd.close()
                
        

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-c", "--complete", action="store_true", dest="complete",
                     default=False, help="Assigns only distinct lanes")
optParser.add_option("-n", "--net-files", dest="netfiles",
                     help="read SUMO network(s) from FILE(s) (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output", dest="output",
                     help="write results to FILE (mandatory)", metavar="FILE")
optParser.add_option("-m", "--max-speed", type="float", dest="maxspeed",
                     default=1000.0, help="used lanes which speed is not greater than this (m/s)")
optParser.add_option("-w", "--weighted", action="store_true", dest="weighted",
                     default=False, help="Weights sources/sinks by lane number and length")
(options, args) = optParser.parse_args()

parser = make_parser()
reader = NetDistrictEdgeHandler()
parser.setContentHandler(reader)
for netfile in options.netfiles.split(","):
    if options.verbose:
        print "Reading net '" + netfile + "'"
    parser.parse(netfile)
if options.verbose:
    print "Calculating"
reader.computeWithin(options.complete)
if options.verbose:
    print "Writing results"
reader.writeResults(options.output, options.maxspeed, options.weighted)
