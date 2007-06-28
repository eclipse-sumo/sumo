#!/usr/bin/python
import pickle, sys

from xml.sax import make_parser, handler

MAX_POS_DEVIATION = 10

class DetectorGroupData:

    def __init__(self, pos, id=None):
        self.ids = []
        self.pos = pos
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

    def __init__(self, detFile=None, isPickled=False):
        self._edge2DetData = {}
        self._det2edge = {}
        self._currentGroup = None
        self._currentEdge = None
        if detFile:
            if isPickled:
                pklFile = open(detFile, 'rb')
                self._edge2DetData = pickle.load(pklFile)
                self._det2edge = pickle.load(pklFile)
                pklFile.close()
            else:
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
                self._edge2DetData[edge].append(DetectorGroupData(pos, id))
        self._det2edge[id] = edge

    def startElement(self, name, attrs):
        if name == 'detector_definition':
            self.addDetector(attrs['id'], float(attrs['pos']),
                             attrs.get('orig_edge', self._currentEdge))
        elif name == 'group':
            self._currentGroup = DetectorGroupData(float(attrs['pos']))
            edge = attrs['orig_edge']
            self._currentEdge = edge
            if not edge in self._edge2DetData:
                self._edge2DetData[edge] = []
            self._edge2DetData[edge].append(self._currentGroup)

    def endElement(self, name):
        if name == 'group':
            self._currentGroup = None

    def pickle(self):
        pickle.dump(self._edge2DetData, sys.stdout, -1)
        pickle.dump(self._det2edge, sys.stdout, -1)

    def addFlow(self, det, flow, speed):
        if det in self._det2edge:
            edge = self._det2edge[det]
            for group in self._edge2DetData[edge]:
                if det in group.ids:
                    group.addDetFlow(flow, speed)
                    break


if __name__ == "__main__":
    DetectorReader(sys.argv[1]).pickle()
