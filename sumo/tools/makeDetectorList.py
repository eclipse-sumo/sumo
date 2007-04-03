#!/usr/bin/python
# This script prints a list of detectors to be used with the dfrouter.
# It needs at least three parameters, the SUMO net (.net.xml), a file
# mapping edges which have been joined and a file specifying detectors
# in Elmar's point collection format. Optionally a last parameter "-b"
# can be given which generates output in bab_map format.
import sys

from xml.sax import saxutils, make_parser, handler

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edge = ''
        self._edgeSpeed = {}
        self._det2edge = {}
        self._detPos = {}
        self._edgeMap = {}
        self._edgePos = {}

    def startElement(self, name, attrs):
        if name == 'edge' and (not 'function' in attrs or attrs['function'] != 'internal'):
            self._edge = attrs['id']
        if name == 'lane':
            self._edgeSpeed[self._edge] = 3.6 * float(attrs['maxspeed'])            

    def readMapping(self, mapFile):
        for l in file(mapFile):
            mapDef = l.split()
            if not mapDef[0] in self._edgeSpeed:
                print >> sys.stderr, "Warning! Unknown edge " + mapDef[0]
            cumLength = 0.0
            for mEdge in mapDef[1:]:
                edgeDef = mEdge.split(':')
                self._edgeMap[edgeDef[0]] = mapDef[0]
                self._edgePos[edgeDef[0]] = cumLength
                cumLength = cumLength + float(edgeDef[1])

    def readDetectors(self, detectorFile):
        for l in file(detectorFile):
            if l[0] != '#':
                detDef = l.split()
                if len(detDef) < 6 or detDef[1] != "5":
                    continue
                edge = detDef[5]
                if not edge in self._edgeSpeed and not edge in self._edgeMap:
                    print >> sys.stderr, "Warning! Unknown edge " + detDef[5]
                else:
                    detName = detDef[2].split(';')
                    if detName[0] in self._det2edge:
                        print >> sys.stderr, "Warning! Detector " + detName[0] + " already known"
                    self._det2edge[detName[0]] = edge
                    if len(detName) > 2 and detName[len(detName)-2].startswith('DISTANCE'):
                        self._detPos[detName[0]] = float(detName[len(detName)-2][8:])
                        

    def printBabMap(self):
        for k, v in self._det2edge.iteritems():
            print k, v, self._edgeSpeed[self._edgeMap.get(v, v)]

    def printDetectors(self):
        print "<a>"
        for k, v in self._det2edge.iteritems():
            lane = 0
            if k[len(k)-1] >= "0" and k[len(k)-1] <= "9":
                lane = int(k[len(k)-1]) - 1
            pos = self._detPos.get(k, 0.0)
            if v in self._edgePos:
                pos += self._edgePos[v]
            print '   <detector_definition id="' + k + '"',
            print 'lane="' + self._edgeMap.get(v, v) + '_' + str(lane) + '"',
            print 'pos="' + str(pos) + '"/>'
        print "</a>"

            
if len(sys.argv) < 4:
    print "Usage: " + sys.argv[0] + " <net> <edgemap> <detectors> [-b]"
    sys.exit()
parser = make_parser()
net = NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
net.readMapping(sys.argv[2])
net.readDetectors(sys.argv[3])
if len(sys.argv) == 5 and sys.argv[4] == '-b':
    net.printBabMap()
else:
    net.printDetectors()
