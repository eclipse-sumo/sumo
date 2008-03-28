#!/usr/bin/env python
"""
@file    routecompare.py
@author  Michael.Behrisch@dlr.de
@date    2008-03-25
@version $Id$

This script compares two route sets by calculating
a similarity for any two routes based on the number of common edges
and determining a maximum weighted matching between the route sets.
It needs at least two parameters, which are the route sets to compare.
Optionally a district file may be given, then only routes with
the same origin and destination district are matched
 
Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import sys, optparse
from xml.sax import make_parser, handler

SCALE = 10000
INFINITY = 2**30

class RouteReader(handler.ContentHandler):

    def __init__(self, routeMap):
        self._routes = routeMap
        self._vID = ''
        self._routeID = ''
        self._routeString = ''
        
    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vID = attrs['id']
        elif name == 'route':
            if 'id' in attrs:
                self._routeID = attrs['id']
            else:
                self._routeID = self._vID
                self._vID = ''
            self._routeString = ''

    def endElement(self, name):
        if name == 'route':
            self._routes[self._routeID] = self._routeString.split()

    def characters(self, content):
        if self._routeID != '':
            self._routeString += content

class DistrictReader(handler.ContentHandler):

    def __init__(self, sourceEdges, sinkEdges):
        self._sources = sourceEdges
        self._sinks = sinkEdges
        self._districtID = ''
        
    def startElement(self, name, attrs):
        if name == 'district':
            self._districtID = attrs['id']
        elif name == 'dsource':
            self._sources[attrs['id']] = self._districtID
        elif name == 'dsink':
            self._sinks[attrs['id']] = self._districtID


def compare(first, second):
    commonEdges = 0
    for edge in first:
        if edge in second:
            commonEdges += SCALE
    return commonEdges / max(len(first), len(second))

def matching(routeIDs1, routeIDs2, similarityMatrix, match):
    matchVal = 0
    for id1 in routeIDs1:
        maxMatch = 0
        matchId = ""
        for id2 in routeIDs2:
            if id2 not in match and similarityMatrix[id1][id2] > maxMatch:
                maxMatch = similarityMatrix[id1][id2]
                matchId = id2
        if matchId:
            match[matchId] = id1
            matchVal += maxMatch
        else:
            print "Warning! No match for %s." % id1
    print float(matchVal) / len(routeIDs1) / SCALE
    return matchVal

class Node:
    def __init__(self, routeID, weight):
        self.routeID = routeID
        self.weight = weight
        self.eps = INFINITY
        self.level = INFINITY
        self.match = None 

def augmentSimultan(vTerm):
    dead = set()
    for v in vTerm:
        path = [v]
        l = 0
        while True:
            while len(path[l].pre) > 0 and path[l].pre[0] in dead:
                path[l].pre.pop(0)
            if len(path[l].pre) == 0:
                if l == 0:
                    break
                dead.add(path[l - 1])
                l -= 2
            else:
                if l == len(path) - 1:
                    path.append(None)
                path[l + 1] = path[l].pre.pop(0)
                dead.add(path[l + 1])
                l += 1
                if path[l].level == 0:
                    for j in range(0, l + 1, 2):
                        path[j].match = path[j + 1]
                        path[j + 1].match = path[j]
                    break
                else:
                    if l == len(path) - 1:
                        path.append(None)
                    path[l + 1] = path[l].match
                    l += 1
        
def hungarianDAG(U, V, similarityMatrix):
    while True:
        S = set()
        T = set()
        Q = []
        vTerm = set()
        for u in U:
            u.level = INFINITY
            u.eps = INFINITY
            if not u.match:
                S.add(u)
                u.level = 0
                Q.append(u)
        for v in V:
            v.level = INFINITY
            v.eps = INFINITY
        while len(Q) > 0:
            s = Q.pop(0)
            for t in V:
                if s.weight + t.weight == similarityMatrix[s.routeID][t.routeID]:
                    if t.level > s.level:
                        if t.level == INFINITY:
                            T.add(t)
                            t.level = s.level + 1
                            t.pre = [s]
                            if not t.match:
                                vTerm.add(t)
                            else:
                                S.add(t.match)
                                t.match.level = t.level + 1
                                Q.append(t.match)
                        else:
                            t.pre.append(s)
                else:
                    t.eps = min(t.eps, s.weight + t.weight - similarityMatrix[s.routeID][t.routeID])
        if len(vTerm) > 0:
            break
        epsilon = INFINITY
        for t in V:
            if t.eps < epsilon:
                epsilon = t.eps
        if epsilon == INFINITY:
            break 
        for x in S:
            x.weight -= epsilon
        for x in T:
            x.weight += epsilon
    return vTerm

def maxMatching(routeIDs1, routeIDs2, similarityMatrix, match):
    maxSimilarity = 0
    for id1 in routeIDs1:
        for value in similarityMatrix[id1].itervalues():
            if value > maxSimilarity:
                maxSimilarity = value
    U = []
    for id1 in routeIDs1:
        U.append(Node(id1, maxSimilarity))
    V = []
    for id2 in routeIDs2:
        V.append(Node(id2, 0))
    while True:
        vTerm = hungarianDAG(U, V, similarityMatrix)
        if len(vTerm) == 0:
            break
        augmentSimultan(vTerm)
    matchVal = 0
    for v in V:
        if v.match:
            match[v.routeID] = v.match.routeID
            matchVal += similarityMatrix[v.match.routeID][v.routeID]
    print float(matchVal) / len(routeIDs1) / SCALE, len(match), len(V)
    return matchVal


optParser = optparse.OptionParser()
optParser.add_option("-d", "--districts-file", dest="districts",
                     default="", help="read districts from FILE", metavar="FILE")
optParser.add_option("-s", "--simple-match", action="store_true", dest="simple",
                     default=False, help="use simple matching algorithm")
optParser.add_option("-p", "--print-matching", action="store_true", dest="printmatch",
                     default=False, help="print the resulting matching")
(options, args) = optParser.parse_args()


if len(args) < 2:
    print "Usage: " + sys.argv[0] + " <routes1> <routes2>"
    sys.exit()
routes1 = {}
routes2 = {}
parser = make_parser()
parser.setContentHandler(RouteReader(routes1))
parser.parse(args[0])
parser.setContentHandler(RouteReader(routes2))
parser.parse(args[1])

routeMatrix1 = {}
routeMatrix2 = {}
if options.districts:
    sources = {}
    sinks = {}
    parser.setContentHandler(DistrictReader(sources, sinks))
    parser.parse(options.districts)
    for routes, routeMatrix in [(routes1, routeMatrix1), (routes2, routeMatrix2)]:
        for routeID, route in routes.iteritems():
            source = sources[route[0]]
            sink = sinks[route[-1]]
            if not source in routeMatrix:
                routeMatrix[source] = {}
            if not sink in routeMatrix[source]:
                routeMatrix[source][sink] = []
            routeMatrix[source][sink].append(routeID)        
else:
    for routes, routeMatrix in [(routes1, routeMatrix1), (routes2, routeMatrix2)]:
        routeMatrix["dummySource"] = {}
        routeMatrix["dummySource"]["dummySink"] = list(routes.iterkeys())

match = {}
totalMatch = 0
for source in routeMatrix1.iterkeys():
    if not source in routeMatrix2:
        print "Warning! No routes starting at %s in second route set" % source
        continue
    for sink, routeIDs1 in routeMatrix1[source].iteritems():
        if not sink in routeMatrix2[source]:
            print "Warning! No routes starting at %s and ending at %s in second route set" % (source, sink)
            continue
        routeIDs2 = routeMatrix2[source][sink]
        similarityMatrix = {}
        for id1 in routeIDs1:
            similarityMatrix[id1] = {}
            for id2 in routeIDs2:
                similarityMatrix[id1][id2] = compare(routes1[id1], routes2[id2])
        if options.simple:
            totalMatch += matching(routeIDs1, routeIDs2, similarityMatrix, match)
        else:
            totalMatch += maxMatching(routeIDs1, routeIDs2, similarityMatrix, match)
if options.printmatch:
    for r2, r1 in match.iteritems():
        print r1, r2
print float(totalMatch) / len(routes1) / SCALE
