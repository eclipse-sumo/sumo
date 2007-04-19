#!/usr/bin/python
# This script recreates a flow file from routes and emitters.
import math, string, sys

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

MAX_POS_DEVIATION = 1

class DetectorData:

    def __init__(self):
        self.detPos = []
        self.detGroup = []
        self.routeFlow = []
        self.detFlow = []

    def addDet(self, pos, det):
        for index, compPos in enumerate(self.detPos):
            if abs(compPos - pos) <= MAX_POS_DEVIATION:
                self.detGroup[index].append(det)
                return
        self.detPos.append(pos)
        self.detGroup.append([det])
        self.routeFlow.append(0)
        self.detFlow.append(0)

    def addDetFlow(self, det, flow):
        for index, group in enumerate(self.detGroup):
            if det in group:
                self.detFlow[index] += flow
                return
        assert False

    def addRouteFlow(self, flow):
        for index in range(len(self.routeFlow)):
            self.routeFlow[index] += flow


class DetectorRouteEmitterReader(handler.ContentHandler):

    def __init__(self):
        self._routeID = ''
        self._routeString = ''
        self._edge2det = {}
        self._route2dets = {}
        self._detData = {}
        self._parser = make_parser()
        self._parser.setContentHandler(self)

    def startElement(self, name, attrs):
        if name == 'detector_definition':
            edge = attrs['lane'][0:-2]
            if not edge in self._edge2det:
                self._edge2det[edge] = DetectorData()
            self._edge2det[edge].addDet(float(attrs['pos']), attrs['id'])
            self._detData[attrs['id']] = self._edge2det[edge]
        if name == 'route':
            self._routeID = attrs['id']
            self._route2dets[attrs['id']] = []
        if name == 'trigger':
            self._parser.parse(attrs['file'])
        if name == 'routedistelem':
            if not options.dfrstyle:
                flow = int(attrs['probability'])
                for det in self._route2dets[attrs['id']]:
                    det.addRouteFlow(flow)
        if name == 'emit':
            if options.dfrstyle:
                for det in self._route2dets[attrs['route']]:
                    det.addRouteFlow(1)

    def characters(self, content):
        if self._routeID != '':
            self._routeString += content

    def endElement(self, name):
        if name == 'route':
            for edge in self._routeString.split()[:-1]:
                if edge in self._edge2det:
                    self._route2dets[self._routeID].append(self._edge2det[edge])
            self._routeID = ''
            self._routeString = ''

    def readDetFlows(self, flowFile):
        headerSeen = False
        for l in file(flowFile):
            flowDef = l.split(';')
            det = flowDef[0]
            if not headerSeen and det == "Detector":
                headerSeen = True
            else:
                if flowDef[0] in self._detData:
                    self._detData[flowDef[0]].addDetFlow(flowDef[0], int(flowDef[2]))

    def calcStatistics(self):
        rSum = 0
        dSum = 0
        sumAbsDev = 0
        sumSquaredDev = 0
        sumSquaredPercent = 0
        n = 0
        for det in self._edge2det.itervalues():
            for rFlow, dFlow in zip(det.routeFlow, det.detFlow):
                if dFlow > 0 or not options.ignorezero:
                    rSum += rFlow
                    dSum += dFlow
                    dev = float(abs(rFlow - dFlow))
                    sumAbsDev += dev
                    sumSquaredDev += dev * dev
                    if dFlow > 0:
                        sumSquaredPercent += dev * dev / dFlow / dFlow
                    n += 1
        print '# avgRouteFlow avgDetFlow avgDev RMSE RMSPE'
        print '#', rSum/n, dSum/n, sumAbsDev/n, math.sqrt(sumSquaredDev/n), math.sqrt(sumSquaredPercent/n)

    def printFlows(self, includeDets):
        if includeDets:
            print '# detNames RouteFlow DetFlow'
        else:
            print '# detNames RouteFlow'
        output = []
        for det in self._edge2det.itervalues():
            det.detString = []
            for group in det.detGroup:
                det.detString.append(string.join(sorted(group), ';'))
            if includeDets:
                output.extend(zip(det.detString, det.routeFlow, det.detFlow))
            else:
                output.extend(zip(det.detString, det.routeFlow))
        if includeDets:
            for group, rflow, dflow in sorted(output):
                if dflow > 0 or not options.ignorezero:
                    print group, rflow, dflow
        else:
            for group, flow in sorted(output):
                print group, flow


optParser = OptionParser()
optParser.add_option("-d", "--detector-file", dest="detfile",
                     help="read detectors from FILE (mandatory)", metavar="FILE")
optParser.add_option("-r", "--routes", dest="routefile",
                     help="read routes from FILE (mandatory)", metavar="FILE")
optParser.add_option("-e", "--emitters", dest="emitfile",
                     help="read emitters from FILE (mandatory)", metavar="FILE")
optParser.add_option("-f", "--detector-flow-file", dest="flowfile",
                     help="read detector flows to compare to from FILE", metavar="FILE")
optParser.add_option("-i", "--ignore-zero", action="store_true", dest="ignorezero",
                     default=False, help="ignore detectors with zero flow in the flow input file")
optParser.add_option("-D", "--dfrouter-style", action="store_true", dest="dfrstyle",
                     default=False, help="emitter files in dfrouter style (explicit routes)")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.detfile or not options.routefile or not options.emitfile:
    optParser.print_help()
    sys.exit()
parser = make_parser()
reader = DetectorRouteEmitterReader()
parser.setContentHandler(reader)
if options.verbose:
    print "Reading detectors"
parser.parse(options.detfile)
if options.verbose:
    print "Reading routes"
parser.parse(options.routefile)
if options.verbose:
    print "Reading emitters"
parser.parse(options.emitfile)
if options.flowfile:
    reader.readDetFlows(options.flowfile)
reader.printFlows(bool(options.flowfile))
if options.flowfile:
    reader.calcStatistics()
