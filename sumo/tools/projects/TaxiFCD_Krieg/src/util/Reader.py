# -*- coding: Latin-1 -*-
"""
@file    Reader.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-18
@version $Id$

Contains reader modules which are needed frequently

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import util.Path as path
from analysis.Taxi import *
from cPickle import dump
from cPickle import load
from os.path import exists


def readVLS_Edges():
    """Reads the net file and returns a list of all edges."""
    inputFile = open(path.net, 'r')
    for line in inputFile:
        if line.find(" <edges") != -1:
            #         delete edges tag at start and end
            words = line[line.find(">") + 1:line.find("</")].split(" ")
            break
    inputFile.close()
    return words


def readRoute_Edges(buildNew=False):
    """Reads the route file of the 'normal' traffic. 
       Goal is to fetch all edges of the net where traffic was generated."""
    edgesSet = set()
    if not exists(path.drivenEdges) or buildNew:
        inputFile = open(path.normalTrafficRoutes, 'r')
        for line in inputFile:
            if line.find("<route>") != -1:
                words = line[line.find(">") + 1:line.find("</")].split(" ")
                # add edges to set but ignore the first and the last edge
                edgesSet = edgesSet | set(words[1:-1])
        inputFile.close()
        dump(edgesSet, open(path.drivenEdges, 'w'))
    else:
        edgesSet = load(open(path.drivenEdges, 'r'))
    return edgesSet


def readRoute_EdgesMod(buildNew=False):
    """Reads a selectedLane file which contains all edges of the net with 'good' traffic."""
    edgesSet = set()
    if not exists(path.drivenEdges) or buildNew:
        inputFile = open(
            "D:/Krieg/Projekte/Diplom/Daten/auswertung/used2.txt", 'r')
        for line in inputFile:
            edge = line[5:-3]
            edgesSet.add(edge)
        inputFile.close()
        dump(edgesSet, open(path.drivenEdges, 'w'))
    else:
        edgesSet = load(open(path.drivenEdges, 'r'))
    return edgesSet


def readAnalysisInfo(WEE=False):
    """Reads the analysis file and returns a list off taxis with all available information."""
    taxiList = []
    i = -1
    if WEE:
        inputFile = open(path.analysisWEE, 'r')
    else:
        inputFile = open(path.analysis, 'r')
    for line in inputFile:
        words = line.split('"')
        if words[0] == '\t<vehicle id=':
            taxiList.append(Taxi(words[1]))
            i += 1
        if words[0] == '\t\t<step time=':
            taxiList[i].addStep(
                Step(words[1], words[3], words[5], words[7], words[9], words[11], words[13]))

    inputFile.close()
    return taxiList


def readEdgesLength():
    """Reads the length of each edge with occurs in the analysis-File and saves them in a pickled file."""
    edgeDict = {}

    # search the edges
    taxiList = readAnalysisInfo()
    for taxi in taxiList:
        for step in taxi.getSteps():
            if step.source == SOURCE_FCD or step.source == SOURCE_SIMFCD:
                edgeDict[step.edge] = 0.0

    # search the length  (in m)
    inputFile = open(path.net, 'r')

    for line in inputFile:
        words = line.split('"')
        try:
            index = words.index(" length=")
            if words[1][:-2] in edgeDict and words[1][-2:] == '_0':
                edgeDict[words[1][:-2]] = float(words[index + 1])
        except ValueError:
            # do nothing
            None

    # pickles the edgeDict
    dump(edgeDict, open(path.edgeLengthDict, 'w'))
    inputFile.close()
