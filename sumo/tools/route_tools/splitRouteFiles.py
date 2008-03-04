#!/usr/bin/env python
"""
@file    splitRouteFiles.py
@author  Michael.Behrisch@dlr.de
@date    2007-07-18
@version $Id: splitRouteFiles.py 494 2007-12-12 00:43:29Z behr_mi $

Splits a list of route files (e.g. coming from the duarouter)
by start time and by edges which contain detectors from an optional
detector file.

Copyright (C) 2007 DLR/FS, Germany
All rights reserved
"""
import os, sys, re

from xml.sax import make_parser, handler
from optparse import OptionParser

def checkDirOpen(path):
    dirName = os.path.dirname(path)
    if dirName != '' and not os.path.exists(dirName):
        os.makedirs(dirName)
    return open(path, 'w')

class DetectorRouteReader(handler.ContentHandler):

    def __init__(self, collectFile, routeDistPrefix, edgeCountFile):
        handler.ContentHandler.__init__(self)
        self._vehID = None
        self._routeString = ''
        self._edges = {}
        self._routeDistPrefix = routeDistPrefix
        self._calibRoutes = checkDirOpen(collectFile)
        print >> self._calibRoutes, "<routes>"
        self._edgeCountFile = edgeCountFile
        self._edgeCount = {}

    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vehID = attrs['id']
        if name == 'route':
            self._routeString = ''
        elif name == 'group':
            edge = attrs['orig_edge']
            self._edges[edge] = checkDirOpen(self._routeDistPrefix+edge+'.xml')

    def characters(self, content):
        self._routeString += content

    def endElement(self, name):
        if name == 'route':
            route = self._routeString.split()
            calibRouteIndex = None
            for index, edge in enumerate(route):
                if self._edgeCountFile:
                    if not edge in self._edgeCount:
                        self._edgeCount[edge] = 0
                    self._edgeCount[edge] += 1
                if edge in self._edges:
                    if calibRouteIndex == None and index < len(route) - 1:
                        calibRouteIndex = index
                    if calibRouteIndex != None:
                        print >> self._edges[edge], '   <routedistelem id="route_%s"/>' % self._vehID
            if calibRouteIndex != None:
                print >> self._calibRoutes, '   <route id="route_%s" multi_ref="x">%s</route>' % (self._vehID, ' '.join(route[calibRouteIndex:]))
            
    def closeAll(self):
        print >> self._calibRoutes, "</routes>"
        self._calibRoutes.close()
        for f in self._edges.itervalues():
            f.close()
        if self._edgeCountFile:
            edgeCountOut = checkDirOpen(self._edgeCountFile)
            print >> edgeCountOut, "<netstats>"
            print >> edgeCountOut, '   <interval begin="0" end="%s">' % (24*3600)
            for edge, weight in self._edgeCount.iteritems():
                print >> edgeCountOut, '      <edge id="%s" traveltime="%s"/>' % (edge, weight)
            print >> edgeCountOut, '   </interval>'
            print >> edgeCountOut, "</netstats>"
            
def splitFiles(routeFiles, typesFile, routesPrefix, step, verbose):
    vtypes = checkDirOpen(typesFile)
    print >> vtypes, """<vtypes>
   <vtype id="PKW" accel="2.6" decel="4.5" sigma="0.5" length="7" maxspeed="41.6" color="0,1,0"/>
   <vtype id="LKW" accel="2.6" decel="4.5" sigma="0.5" length="15" maxspeed="25" color="1,0,0"/>"""
    currentTime = 0
    routesOut = None
    files = []
    departs = []
    for routesIn in routeFiles:
        files.append(open(routesIn, 'rb'))
        departs.append(0)
    index = 0
    while len(files) > 0:
        pos = files[index].tell()
        line = files[index].readline()
        if not line:
            del files[index]
            del departs[index]
            if len(files) > 0:
                index %= len(files)
        else:
            match = re.match(' *<vehicle.*depart="([0-9]+)"', line)
            if match:
                currentDepart = int(match.group(1))
                departs[index] = currentDepart
                if currentDepart <= min(departs):
                    while currentDepart >= currentTime or not routesOut:
                        if routesOut:
                            print >> routesOut, "</routes>"
                            routesOut.close()
                        routeFilename = routesPrefix+"%s.rou.xml" % currentTime
                        if verbose:
                            print "Writing", routeFilename
                        routesOut = checkDirOpen(routeFilename)
                        print >> routesOut, "<routes>"
                        currentTime += step
                    routesOut.write(line)
                    routesOut.write(files[index].readline())
                    routesOut.write(files[index].readline())
                else:
                    files[index].seek(pos)
                    index = (index+1) % len(files)
            elif re.match(' *<vtype', line):
                print >> vtypes, line
            elif not re.match(' *</?routes>', line):
                print >> sys.stderr, "Warning! Unknown line in file."
    if routesOut:
        print >> routesOut, "</routes>"
        routesOut.close()
    print >> vtypes, "</vtypes>"
    vtypes.close()
    
def main():
    optParser = OptionParser()
    optParser.add_option("-d", "--detector-file", dest="detfile",
                         help="read detectors from FILE", metavar="FILE")
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-s", "--step", dest="step", type="int", default=900,
                         help="time split step in seconds")
    optParser.add_option("-t", "--types-file", dest="typesfile", default="vehtypes.xml",
                         help="write vehicle types to FILE", metavar="FILE")
    optParser.add_option("-c", "--route-collection", dest="collectfile", default="calibrator_routes.rou.xml",
                         help="write route collection for the distributions to FILE", metavar="FILE")
    optParser.add_option("-o", "--routes-prefix", dest="routesprefix", default="validate/validate_mofr_",
                         help="let time splitted route files start with PREFIX", metavar="PREFIX")
    optParser.add_option("-r", "--route-dist-prefix", dest="routedistprefix", default="bab_route_dists/routedists_",
                         help="let route distribution files start with PREFIX", metavar="PREFIX")
    optParser.add_option("-e", "--edge-count", dest="edgecount",
                         help="dump number of routes for each edge to FILE", metavar="FILE")
    (options, args) = optParser.parse_args()
    if len(args) == 0:
        optParser.print_help()
        sys.exit()
    splitFiles(args, options.typesfile, options.routesprefix, options.step, options.verbose)
    if options.detfile:
        if options.verbose:
            print "Reading detectors"
        reader = DetectorRouteReader(options.collectfile, options.routedistprefix,
                                     options.edgecount)
        parser = make_parser()
        parser.setContentHandler(reader)
        parser.parse(options.detfile)
        for time in range(0, 24*3600, options.step):
            routeFilename = options.routesprefix+"%s.rou.xml" % time
            if os.path.exists(routeFilename):
                if options.verbose:
                    print "Parsing", routeFilename
                parser.parse(routeFilename)
        reader.closeAll()

if __name__ == '__main__':
    main()
