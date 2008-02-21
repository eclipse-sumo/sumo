#!/usr/bin/python
# This script reads in the network given as
#  first parameter and extracts nodes and edges 
#  from it which are saved into "nodes.xml" and 
#  "edges.xml" for their reuse in NETCONVERT
# todo:
# - use classes for edges/nodes
# - parse connections
# - parse tls information

import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._id = ''
        self._edge2lanes = {}
        self._edge2speed = {}
        self._edge2shape = {}
        self._edge2from = {}
        self._edge2to = {}
        self._node2x = {}
        self._node2y = {}
        self._currentShapes = []
        self._parseLane = False

    def startElement(self, name, attrs):
        self._parseLane = False
        if name == 'edge':
            if not attrs.has_key('function') or attrs['function'] != 'internal':
                self._id = attrs['id']
                self._edge2from[attrs['id']] = attrs['from']
                self._edge2to[attrs['id']] = attrs['to']
                self._edge2lanes[attrs['id']] = 0
                self._currentShapes = []
            else:
                self._id = ""
        if name == 'lane' and self._id!="":
            self._edge2speed[self._id] = attrs['maxspeed']
            self._edge2lanes[self._id] = self._edge2lanes[self._id] + 1
            self._parseLane = True
            self._currentShapes.append("")
        if name == 'junction':
            self._id = attrs['id']
            if self._id[0]!=':':
                self._node2x[attrs['id']] = attrs['x']
                self._node2y[attrs['id']] = attrs['y']
            else:
                self._id = ""

    def characters(self, content):
        if self._parseLane:
            self._currentShapes[-1] = self._currentShapes[-1] + content

    def endElement(self, name):
        if self._parseLane:
            self._parseLane = False
        if name == 'edge' and self._id!="":
            noShapes = len(self._currentShapes)
            if noShapes%2 == 1 and noShapes>0:
                self._edge2shape[self._id] = self._currentShapes[int(noShapes/2)]
            elif noShapes%2 == 0 and len(self._currentShapes[0])!=2:
                cshapes = []
                minLen = -1
                for i in self._currentShapes:
                    cshape = []
                    es = i.split(" ")
                    for e in es:
                        p = e.split(",")
                        cshape.append((float(p[0]), float(p[1])))
                    cshapes.append(cshape)
                    if minLen==-1 or minLen>len(cshape):
                        minLen = len(cshape)
                self._edge2shape[self._id] = ""
                if minLen>2:
                    for i in range(0, minLen):
                        x = 0.
                        y = 0.
                        for j in range(0, noShapes):
                            x = x + cshapes[j][i][0]
                            y = y + cshapes[j][i][1]
                        x = x / float(noShapes)
                        y = y / float(noShapes)
                        if self._edge2shape[self._id] != "":
                            self._edge2shape[self._id] = self._edge2shape[self._id] + " "
                        self._edge2shape[self._id] = self._edge2shape[self._id] + str(x) + "," + str(y)
    
    def writeNodes(self):
        fd = open("nodes.xml", "w")
        fd.write("<nodes>\n")
        for node in self._node2x:
            fd.write("   <node id=\"" + node + "\" x=\"" + self._node2x[node] + "\" y=\"" + self._node2y[node] + "\"/>\n")
        fd.write("</nodes>\n")

    def writeEdges(self):
        fd = open("edges.xml", "w")
        fd.write("<edges>\n")
        for edge in self._edge2lanes:
            fd.write("   <edge id=\"" + edge + "\" fromnode=\"" + self._edge2from[edge] + "\" tonode=\"" + self._edge2to[edge])
            fd.write("\" maxspeed=\"" + str(self._edge2speed[edge]))
            fd.write("\" nolanes=\"" + str(self._edge2lanes[edge]) + "\"")
            if self._edge2shape[edge]!="":
                fd.write(" shape=\"" + self._edge2shape[edge] + "\"")
            fd.write("/>\n")
        fd.write("</edges>\n")

    
if len(sys.argv) < 1:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
parser = make_parser()
net = NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
net.writeNodes()
net.writeEdges()

