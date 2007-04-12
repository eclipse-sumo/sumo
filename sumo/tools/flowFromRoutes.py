#!/usr/bin/python
# This script recreates a flow file from routes and emitters.
import math, os, string, sys

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

class DetectorRouteEmitterReader(handler.ContentHandler):

    def __init__(self):
        self._routeID = ''
        self._routeString = ''
        self._edge2det = {}
        self._route2dets = {}
        self._detFlow = {}
        self._parser = make_parser()
        self._parser.setContentHandler(self)

    def startElement(self, name, attrs):
        if name == 'detector_definition':
            edge = attrs['lane'][0:-2]
            self._edge2det[edge] = attrs['id']
            self._detFlow[attrs['id']] = 0
        if name == 'route':
            self._routeID = attrs['id']
            self._route2dets[attrs['id']] = []
        if name == 'trigger':
            self._parser.parse(attrs['file'])
        if name == 'routedistelem':
            if not options.dfrstyle:
                flow = int(attrs['probability'])
                for det in self._route2dets[attrs['id']]:
                    self._detFlow[det] += flow
        if name == 'emit':
            if options.dfrstyle:
                for det in self._route2dets[attrs['route']]:
                    self._detFlow[det] += 1
            

    def characters(self, content):
        if self._routeID != '':
            self._routeString += content

    def endElement(self, name):
        if name == 'route':
            for edge in self._routeString.split():
                if edge in self._edge2det:
                    self._route2dets[self._routeID].append(self._edge2det[edge])
            self._routeID = ''
            self._routeString = ''

    def calcRMS(self, flowFile):
        headerSeen = False
        compareDetFlow = {}
        sumSquared = 0
        n = 0
        for l in file(flowFile):
            flowDef = l.split(';')
            det = flowDef[0]
            if not headerSeen and det == "Detector":
                headerSeen = True
            else:
                flow = int(flowDef[2])
                if flow > 0 or not options.ignorezero:
                    if det in self._detFlow:
                        sumSquared += (flow - self._detFlow[det]) * (flow - self._detFlow[det])
                    else:
                        sumSquared += flow * flow
                    n += 1
        print math.sqrt(sumSquared/n)

    def printFlows(self):
        for det, flow in self._detFlow.iteritems():
            print det, flow


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
optParser.add_option("-q", "--quiet", action="store_true", dest="quiet",
                     default=False, help="suppress warnings")
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
    reader.calcRMS(options.flowfile)
else:
    reader.printFlows()
