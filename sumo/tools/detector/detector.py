#!/usr/bin/python
import sys

from xml.sax import make_parser, handler

MAX_POS_DEVIATION = 10

class DetectorGroupData:

    def __init__(self, pos, isValid, id=None):
        self.ids = []
        self.pos = pos
        self.isValid = isValid
        self.totalFlow = 0
        self.avgSpeed = 0
        self.entryCount = 0
        if id != None:
            self.ids.append(id)

    def addDetFlow(self, flow, speed):
        oldFlow = self.totalFlow
        self.totalFlow += flow
        count = self.entryCount
        if flow > 0:
            self.avgSpeed = (self.avgSpeed * oldFlow + speed) / self.totalFlow
        self.entryCount += 1


class DetectorReader(handler.ContentHandler):

    def __init__(self, detFile=None, laneMap={}):
        self._edge2DetData = {}
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
            print >> sys.stderr, "Warning! Detector %s already known." % id
            return
        if not edge in self._edge2DetData:
            self._edge2DetData[edge] = []
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
                self._edge2DetData[edge].append(DetectorGroupData(pos, True, id))
        self._det2edge[id] = edge

    def startElement(self, name, attrs):
        if name == 'detector_definition':
            self.addDetector(attrs['id'], float(attrs['pos']),
                             self._laneMap.get(attrs['lane'], self._currentEdge))
        elif name == 'group':
            self._currentGroup = DetectorGroupData(float(attrs['pos']),
                                                   attrs.get('valid', "1") == "1")
            edge = attrs['orig_edge']
            self._currentEdge = edge
            if not edge in self._edge2DetData:
                self._edge2DetData[edge] = []
            self._edge2DetData[edge].append(self._currentGroup)

    def endElement(self, name):
        if name == 'group':
            self._currentGroup = None

    def addFlow(self, det, flow, speed=0.0):
        if det in self._det2edge:
            edge = self._det2edge[det]
            for group in self._edge2DetData[edge]:
                if det in group.ids:
                    group.addDetFlow(flow, speed)
                    break

    def readFlows(self, flowFile):
        headerSeen = False
        for l in file(flowFile):
            flowDef = l.split(';')
            if not headerSeen and flowDef[0] == "Detector":
                headerSeen = True
            else:
                self.addFlow(flowDef[0], float(flowDef[2]))
