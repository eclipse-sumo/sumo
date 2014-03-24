#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    kShortest.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-02-14
@version $Id$

Calculating k shortest paths in a dijkstra like fashion
storing k predecessors

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, random, string, sys

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net

class Predecessor:

    def __init__(self, edge, pred, distance):
        self.edge = edge
        self.pred = pred
        self.distance = distance


def _addNewPredecessor(edge, updatePred, newPreds):
    for pred in newPreds:
        if pred.pred == updatePred:
            return
    pred = updatePred
    if options.noloops:
        while pred != None:
            if pred.edge == edge:
                return
            pred = pred.pred
    newPreds.append(Predecessor(edge, updatePred,
                                updatePred.distance + edge.weight))

def update(vertex, edge):
    updatePreds = edge._from.preds
    if len(vertex.preds) == options.k\
       and updatePreds[0].distance + edge.weight >= vertex.preds[options.k-1].distance:
        return False
    newPreds = []
    updateIndex = 0
    predIndex = 0
    while len(newPreds) < options.k\
          and (updateIndex < len(updatePreds)\
               or predIndex < len(vertex.preds)):
        if predIndex == len(vertex.preds):
            _addNewPredecessor(edge, updatePreds[updateIndex], newPreds)
            updateIndex += 1
        elif updateIndex == len(updatePreds):
            newPreds.append(vertex.preds[predIndex])
            predIndex += 1
        elif updatePreds[updateIndex].distance + edge.weight < vertex.preds[predIndex].distance:
            _addNewPredecessor(edge, updatePreds[updateIndex], newPreds)
            updateIndex += 1
        else:
            newPreds.append(vertex.preds[predIndex])
            predIndex += 1
    if predIndex == len(newPreds): # no new added
        return False
    vertex.preds = newPreds
    returnVal = not vertex.wasUpdated
    vertex.wasUpdated = True
    return returnVal

def calcPaths(net, startEdgeLabel):
    for n in net.getNodes():
        n.preds = []
        n.wasUpdated = False
    for e in net.getEdges():
        e.weight = e.getLane(0).getLength()
        if options.traveltime:
            e.weight /= e.getLane(0).getSpeed()
    if startEdgeLabel:
        startVertex = net.getEdge[startEdgeLabel]._from
    else:
        startVertex = net.getNodes()[0]
    startVertex.preds.append(Predecessor(None, None, 0))
    updatedVertices = [startVertex]
    while len(updatedVertices) > 0:
        vertex = updatedVertices.pop(0)
        vertex.wasUpdated = False
        for edge in vertex.getOutgoing():
            if edge._to != startVertex and update(edge._to, edge):
                updatedVertices.append(edge._to)
    printRoutes(net, startVertex)

def printRoutes(net, startVertex):
    if options.traveltime:
        weight="duration"
    else:
        weight="length"
    print "<routes>"
    for lastVertex in net.getNodes():
        routes = []
        for num, startPred in enumerate(lastVertex.preds):
            vertex = lastVertex  
            pred = startPred
            route = ""
            lastEdge = None
            firstEdge = None
            while vertex != startVertex:
                firstEdge = pred.edge
                if not lastEdge:
                    lastEdge = pred.edge
                route = pred.edge.getID() + " " + route
                vertex = pred.edge._from
                pred = pred.pred
            if lastEdge != firstEdge:
                routes.append((startPred.distance, '%s_%s" %s="%s" edges="%s"/>'\
                              % (firstEdge.getID(), lastEdge.getID(),
                                 weight, startPred.distance, route[:-1])))
        for num, route in enumerate(sorted(routes)):
            print '    <route id="route%s_%s' % (num, route[1])
        print
    print "</routes>"


optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-k", "--num-paths", type="int", dest="k", metavar="NUM",
                     default=3, help="calculate the shortest k paths [default: %default]")
optParser.add_option("-s", "--start-edge", dest="start", default="",
                     help="start at the start vertex of this edge")
optParser.add_option("-t", "--travel-time", action="store_true", dest="traveltime",
                     help="use minimum travel time instead of length")
optParser.add_option("-l", "--no-loops", action="store_true", dest="noloops",
                     default=False, help="omit routes which travel an edge twice")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.netfile:
    optParser.print_help()
    sys.exit()
parser = make_parser()
if options.verbose:
    print "Reading net"
net = sumolib.net.readNet(options.netfile)
if options.verbose:
    print len(net.getEdges()), "edges read"
calcPaths(net, options.start)
