#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    detector.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2007-06-28
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import sys
from collections import defaultdict
from xml.sax import make_parser, handler

MAX_POS_DEVIATION = 10

def relError(actual, expected):
    if expected == 0:
        if actual == 0:
            return 0
        else:
            return 1
    else:
        return (actual - expected) / expected

def parseFlowFile(flowFile, detCol="Detector", timeCol="Time", flowCol="qPKW", speedCol="vPKW", begin=None, end=None):
        detIdx = -1
        flowIdx = -1
        speedIdx = -1
        timeIdx = -1
        with open(flowFile) as f:
            for l in f:
                if ';' not in l:
                    continue
                flowDef = [e.strip() for e in l.split(';')]
                if detIdx == -1 and detCol in flowDef:
                    # init columns
                    detIdx = flowDef.index(detCol)
                    if flowCol in flowDef:
                        flowIdx = flowDef.index(flowCol)
                    if speedCol in flowDef:
                        speedIdx = flowDef.index(speedCol)
                    if timeCol in flowDef:
                        timeIdx = flowDef.index(timeCol)
                elif flowIdx != -1:
                    # columns are initialized
                    if timeIdx == -1 or begin is None:
                        curTime = None
                        timeIsValid = True
                    else:
                        curTime = float(flowDef[timeIdx])
                        timeIsValid = (end is None and curTime == begin) or (
                            curTime >= begin and curTime < end)
                    if timeIsValid:
                        speed = float(flowDef[speedIdx]) if speedIdx != -1 else None
                        yield (flowDef[detIdx], curTime, float(flowDef[flowIdx]), speed)


class DetectorGroupData:

    def __init__(self, pos, isValid, id=None):
        self.ids = []
        self.pos = pos
        self.isValid = isValid
        self.totalFlow = 0
        self.avgSpeed = 0
        self.entryCount = 0
        if id is not None:
            self.ids.append(id)
        # timeline data (see readFlowsTimeline)
        self.begin = 0
        self.timeline = []
        self.interval = None

    def addDetFlow(self, flow, speed):
        oldFlow = self.totalFlow
        self.totalFlow += flow
        if flow > 0 and speed is not None:
            self.avgSpeed = (
                self.avgSpeed * oldFlow + speed * flow) / self.totalFlow
        self.entryCount += 1

    def clearFlow(self):
        self.totalFlow = 0
        self.avgSpeed = 0
        self.entryCount = 0


class DetectorReader(handler.ContentHandler):

    def __init__(self, detFile=None, laneMap={}):
        self._edge2DetData = defaultdict(list)
        self._det2edge = {}
        self._currentGroup = None
        self._currentEdge = None
        self._laneMap = laneMap
        if detFile:
            parser = make_parser()
            parser.setContentHandler(self)
            parser.parse(detFile)

    def addDetector(self, id, pos, edge):
        if id in self._det2edge:
            print("Warning! Detector %s already known." % id, file=sys.stderr)
            return
        if edge is None:
            raise RuntimeError("Detector '%s' has no edge" % id)
        if self._currentGroup:
            self._currentGroup.ids.append(id)
        else:
            haveGroup = False
            for data in self._edge2DetData[edge]:
                if abs(data.pos - pos) <= MAX_POS_DEVIATION:
                    data.ids.append(id)
                    haveGroup = True
                    break
            if not haveGroup:
                self._edge2DetData[edge].append(
                    DetectorGroupData(pos, True, id))
        self._det2edge[id] = edge

    def getEdgeDetGroups(self, edge):
        return self._edge2DetData[edge]

    def startElement(self, name, attrs):
        if name == 'detectorDefinition' or name == 'e1Detector':
            self.addDetector(attrs['id'], float(attrs['pos']),
                             self._laneMap.get(attrs['lane'], self._currentEdge))
        elif name == 'group':
            self._currentGroup = DetectorGroupData(float(attrs['pos']),
                                                   attrs.get('valid', "1") == "1")
            edge = attrs['orig_edge']
            self._currentEdge = edge
            self._edge2DetData[edge].append(self._currentGroup)

    def endElement(self, name):
        if name == 'group':
            self._currentGroup = None

    def addFlow(self, det, flow, speed=0.0):
        #print("addFlow det=%s flow=%s" % (det, flow))
        if det in self._det2edge:
            edge = self._det2edge[det]
            for group in self._edge2DetData[edge]:
                if det in group.ids:
                    group.addDetFlow(flow, speed)
                    break

    def clearFlows(self):
        for groupList in self._edge2DetData.itervalues():
            for group in groupList:
                group.clearFlow()


    def readFlows(self, flowFile, det="Detector", flow="qPKW", speed=None, time=None, timeVal=None, timeMax=None):
        values = parseFlowFile(flowFile, 
                detCol=det, timeCol=time, flowCol=flow, 
                speedCol=speed, begin=timeVal, end=timeMax)
        hadFlow = False
        for det, time, flow, speed in values:
            hadFlow = True
            self.addFlow(det, flow, speed)
        return hadFlow

    def findTimes(self, flowFile, tMin, tMax, det="Detector", time="Time"):
        timeIdx = 1
        with open(flowFile) as f:
            for l in f:
                if ';' not in l:
                    continue
                flowDef = [e.strip() for e in l.split(';')]
                if det in flowDef:
                    if time in flowDef:
                        timeIdx = flowDef.index(time)
                elif len(flowDef) > timeIdx:
                    curTime = float(flowDef[timeIdx])
                    if tMin is None or tMin > curTime:
                        tMin = curTime
                    if tMax is None or tMax < curTime:
                        tMax = curTime
        return tMin, tMax

