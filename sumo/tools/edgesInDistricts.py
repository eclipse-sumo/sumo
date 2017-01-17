#!/usr/bin/env python
"""
@file    edgesInDistricts.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2007-07-26
@version $Id$

Parsing a number of networks and taz (district) files with shapes
this script writes a taz file with all the edges which are inside
the relevant taz.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2007-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
from __future__ import absolute_import
import sys
import collections
from optparse import OptionParser
from xml.sax import parse
import sumolib
import random


# written into the net. All members are "private".
class DistrictEdgeComputer:

    def __init__(self, net):
        self._net = net
        self._districtEdges = collections.defaultdict(list)
        self._edgeDistricts = collections.defaultdict(list)
        self._invalidatedEdges = set()

    def computeWithin(self, polygons, options):
        districtBoxes = {}
        for district in polygons:
            districtBoxes[district.id] = district.getBoundingBox()
        for idx, edge in enumerate(self._net.getEdges()):
            shape = edge.getShape()
            if edge.getSpeed() < options.maxspeed and edge.getSpeed() > options.minspeed and (options.internal or edge.getFunction() != "internal"):
                if options.vclass is None or edge.allows(options.vclass):
                    if options.assign_from:
                        xmin, ymin = shape[0]
                        xmax, ymax = shape[0]
                    else:
                        xmin, ymin, xmax, ymax = edge.getBoundingBox()
                    for district in polygons:
                        dxmin, dymin, dxmax, dymax = districtBoxes[district.id]
                        if dxmin <= xmax and dymin <= ymax and dxmax >= xmin and dymax >= ymin:
                            if options.assign_from:
                                if sumolib.geomhelper.isWithin(shape[0], district.shape):
                                    self._districtEdges[district].append(edge)
                                    self._edgeDistricts[edge].append(district)
                                    break
                            else:
                                for pos in shape:
                                    if sumolib.geomhelper.isWithin(pos, district.shape):
                                        self._districtEdges[
                                            district].append(edge)
                                        self._edgeDistricts[
                                            edge].append(district)
                                        break
            if options.verbose and idx % 100 == 0:
                sys.stdout.write("%s/%s\r" % (idx, len(self._net.getEdges())))
        if options.complete:
            for edge in self._edgeDistricts:
                if len(self._edgeDistricts[edge]) > 1:
                    self._invalidatedEdges.add(edge)

    def getEdgeDistrictMap(self):
        result = {}
        for edge, districts in self._edgeDistricts.iteritems():
            if len(districts) == 1:
                result[edge] = districts[0]
        return result

    def writeResults(self, options):
        fd = open(options.output, "w")
        fd.write("<tazs>\n")
        for district, edges in sorted(self._districtEdges.items()):
            filtered = [
                edge for edge in edges if edge not in self._invalidatedEdges]
            if len(filtered) == 0:
                print("District '" + district.id + "' has no edges!")
            else:
                if options.weighted:
                    if options.shapeinfo:
                        fd.write('    <taz id="%s" shape="%s">\n' %
                                 (district.id, district.getShapeString()))
                    else:
                        fd.write('    <taz id="%s">\n' % district.id)
                    for edge in filtered:
                        weight = edge.getSpeed() * edge.getLength()
                        fd.write(
                            '        <tazSource id="%s" weight="%.2f"/>\n' % (edge.getID(), weight))
                        fd.write(
                            '        <tazSink id="%s" weight="%.2f"/>\n' % (edge.getID(), weight))
                    fd.write("    </taz>\n")
                else:
                    if options.shapeinfo:
                        fd.write('    <taz id="%s" shape="%s" edges="%s"/>\n' %
                                 (district.id, district.getShapeString(), " ".join([e.getID() for e in filtered])))
                    else:
                        fd.write('    <taz id="%s" edges="%s"/>\n' %
                                 (district.id, " ".join([e.getID() for e in filtered])))
        fd.write("</tazs>\n")
        fd.close()

    def getTotalLength(self, edgeID):
        edge = self._net.getEdge(edgeID)
        return edge.getLength() * edge.getLaneNumber()


def fillOptions(optParser):
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-c", "--complete", action="store_true",
                         default=False, help="assign edges only if they are not in more than one district")
    optParser.add_option("-n", "--net-file",
                         help="read SUMO network from FILE (mandatory)", metavar="FILE")
    optParser.add_option("-t", "--taz-files",
                         help="read districts from FILEs", metavar="FILE")
    optParser.add_option("-o", "--output", default="districts.taz.xml",
                         help="write results to FILE (default: %default)", metavar="FILE")
    optParser.add_option("-x", "--max-speed", type="float", dest="maxspeed",
                         default=1000.0, help="use lanes where speed is not greater than this (m/s) (default: %default)")
    optParser.add_option("-m", "--min-speed", type="float", dest="minspeed",
                         default=0., help="use lanes where speed is greater than this (m/s) (default: %default)")
    optParser.add_option("-w", "--weighted", action="store_true",
                         default=False, help="Weights sources/sinks by lane number and length")
    optParser.add_option("-f", "--assign-from", action="store_true",
                         default=False, help="Assign the edge always to the district where the \"from\" node is located")
    optParser.add_option("-i", "--internal", action="store_true",
                         default=False, help="Include internal edges in output")
    optParser.add_option(
        "-l", "--vclass", help="Include only edges allowing VCLASS")
    optParser.add_option("-s", "--shapeinfo", action="store_true",
                         default=False, help="write also the shape info in the file")


if __name__ == "__main__":
    optParser = OptionParser()
    fillOptions(optParser)
    (options, args) = optParser.parse_args()
    if not options.net_file:
        optParser.print_help()
        optParser.exit("Error! Providing a network is mandatory")

    if options.verbose:
        print("Reading net '" + options.net_file + "'")
    nets = options.net_file.split(",")
    if len(nets) > 1:
        print(
            "Warning! Multiple networks specified. Parsing the first one for edges and tazs, the others for taz only.")
    reader = DistrictEdgeComputer(sumolib.net.readNet(nets[0]))
    tazFiles = nets + options.taz_files.split(",")
    polyReader = sumolib.shapes.polygon.PolygonReader(True)
    for tf in tazFiles:
        parse(tf, polyReader)
    if options.verbose:
        print("Calculating")
    reader.computeWithin(polyReader.getPolygons(), options)
    if options.verbose:
        print("Writing results")
    reader.writeResults(options)
