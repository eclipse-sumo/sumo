#!/usr/bin/python
# This script compares two route sets by calculating
# a similarity for any two routes based on the number of common edges
# and determining a maximum weighted matching between the route sets.
# It needs at least two parameters, which are the route sets to compare.
import sys, optparse
from xml.sax import make_parser, handler

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
                self._routeID = "for vehicle " + self._vID
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


optParser = optparse.OptionParser()
optParser.add_option("-d", "--districts-file", dest="districts",
                     default="", help="read districts from FILE", metavar="FILE")
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
	routeMatrix["dummySource"]["dummySink"] = []
	for routeID in routes.iterkeys():
	    routeMatrix["dummySource"]["dummySink"].append(routeID)

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
	print routeIDs1, routeIDs2
