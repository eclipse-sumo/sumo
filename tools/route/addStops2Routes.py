#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    addStops2Routes.py
# @author  Yun-Pang Floetteroed
# @author  Jakob Erdmann
# @date    2019-04-25

"""
add stops to vehicle routes
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.output import parse, parse_fast  # noqa
from sumolib.options import ArgumentParser  # noqa
import sumolib  # noqa


def get_options(args=None):
    optParser = ArgumentParser()
    optParser.add_option("-n", "--net-file", dest="netfile",
                         help="define the net filename (mandatory)")
    optParser.add_option("-r", "--route-files", dest="routefiles",
                         help="define the route file seperated by comma (mandatory)")
    optParser.add_option("-o", "--output-file", dest="outfile",
                         help="define the output filename")
    optParser.add_option("-t", "--typesfile", dest="typesfile",
                         help="Give a typesfile")
    optParser.add_option("-d", "--duration",
                         help="Define duration of vehicle stop")
    optParser.add_option("-u", "--until",
                         help="Define end time of vehicle stop")
    optParser.add_option("-p", "--parking", dest="parking", action="store_true",
                         default=False, help="where is the vehicle parking")
    optParser.add_option("--parking-areas", dest="parkingareas", default=False,
                         help="load parkingarea definitions and stop at parkingarea on the arrival edge if possible")
    optParser.add_option("--start-at-stop", dest="startAtStop", action="store_true",
                         default=False, help="shorten route so it starts at stop")
    optParser.add_option("--rel-occupancy", dest="relOccupancy", type=float,
                         help="fill all parkingAreas to relative occupancy")
    optParser.add_option("--abs-occupancy", dest="absOccupancy", type=int, default=1,
                         help="fill all parkingAreas to absolute occupancy")
    optParser.add_option("--abs-free", dest="absFree", type=int,
                         help="fill all parkingAreas to absolute remaining capacity")
    optParser.add_option("-D", "--person-duration", dest="pDuration",
                         help="Define duration of person stop")
    optParser.add_option("-U", "--person-until", dest="pUntil",
                         help="Define end time of person stop")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="tell me what you are doing")

    (options, args) = optParser.parse_known_args(args=args)

    if options.parkingareas:
        options.parkingareas = options.parkingareas.split(",")

    if not options.routefiles:
        if not options.startAtStop:
            optParser.print_help()
            sys.exit("--route-files missing")
        elif not options.parkingareas:
            sys.exit("--parking-areas needed to generation stationary traffic without route-files")
        else:
            options.routefiles = []
            if not options.outfile:
                options.outfile = options.parkingareas[0][:-4] + ".stops.xml"
    else:
        options.routefiles = options.routefiles.split(',')
        if not options.outfile:
            options.outfile = options.routefiles[0][:-4] + ".stops.xml"

    if not options.netfile:
        optParser.print_help()
        sys.exit("--net-file missing")

    if not options.typesfile:
        options.typesfiles = options.routefiles

    if not options.duration and not options.until:
        optParser.print_help()
        sys.exit("stop duration or until missing")
    return options


def readTypes(options):
    vtypes = {None: "passenger"}
    for file in options.typesfile.split(','):
        for vtype in sumolib.output.parse(file, 'vType'):
            vtypes[vtype.id] = vtype.vClass
    # print(vtypes)
    return vtypes


def getLastEdge(obj):
    if obj.route:
        edgesList = obj.route[0].edges.split()
        return edgesList[-1]
    elif obj.to:
        return obj.to
    else:
        return None


def loadRouteFiles(options, routefile, edge2parking, outf):
    net = sumolib.net.readNet(options.netfile)
    vtypes = readTypes(options)
    numSkipped = defaultdict(lambda: 0)

    for routefile in options.routefiles:
        for obj in sumolib.xml.parse(routefile, ['vehicle', 'trip', 'flow', 'person']):
            lastEdgeID = getLastEdge(obj)
            if lastEdgeID is None:
                if obj.name == 'person' and (
                        options.pDuration is not None
                        or options.pUntil is not None):
                    stopAttrs = {}
                    if options.pDuration:
                        stopAttrs["duration"] = options.pDuration
                    if options.pUntil:
                        stopAttrs["until"] = options.pUntil
                    # stop location is derived automatically from previous plan element
                    obj.addChild("stop", attrs=stopAttrs)
                else:
                    numSkipped[obj.name] += 1
                outf.write(obj.toXML(' '*4))
                continue

            lastEdge = net.getEdge(lastEdgeID)
            skip = False
            stopAttrs = {}
            if options.parkingareas:
                if lastEdgeID in edge2parking:
                    stopAttrs["parkingArea"] = edge2parking[lastEdgeID]
                else:
                    skip = True
                    numSkipped[obj.name] += 1
                    print("Warning: no parkingArea found on edge '%s' for vehicle '%s'" % (
                        lastEdgeID, obj.id), file=sys.stderr)
            else:
                # find usable lane
                lanes = lastEdge.getLanes()
                for lane in lanes:
                    if lane.allows(vtypes[obj.type]):
                        stopAttrs["lane"] = lane.getID()
                        break

            if options.parking:
                stopAttrs["parking"] = "true"
            if options.duration:
                stopAttrs["duration"] = options.duration
            if options.until:
                stopAttrs["until"] = options.until
            if not skip:
                obj.addChild("stop", attrs=stopAttrs)
                if options.startAtStop:
                    obj.setAttribute("departPos", "stop")
                    if obj.route:
                        obj.route[0].setAttribute("edges", lastEdgeID)
                    elif obj.attr_from:
                        obj.attr_from = obj.to

            outf.write(obj.toXML(' '*4))

    for objType, n in numSkipped.items():
        print("Warning: No stop added for %s %ss" % (n, objType))


def generateStationary(options, edge2parking, outf):
    paCapacity = {}
    if options.parkingareas:
        for pafile in options.parkingareas:
            for pa in sumolib.xml.parse(pafile, "parkingArea"):
                paCapacity[pa.id] = int(pa.getAttributeSecure("roadsideCapacity", 0))

    attrs = ""
    if options.duration:
        attrs += ' duration="%s"' % options.duration
    if options.until:
        attrs += ' until="%s"' % options.until

    for edge, pa in edge2parking.items():
        n = 0
        if options.relOccupancy:
            n = paCapacity[pa] * options.relOccupancy
        elif options.absFree:
            n = paCapacity[pa] - options.absFree
        else:
            n = options.absOccupancy
        for i in range(n):
            id = "%s.%s" % (pa, i)
            outf.write('    <vehicle id="%s" depart="0" departPos="stop">\n' % id)
            outf.write('       <route edges="%s"/>\n' % edge)
            outf.write('       <stop parkingArea="%s"%s/>\n' % (pa, attrs))
            outf.write('    </vehicle>\n')


def main(options):

    edge2parking = {}
    if options.parkingareas:
        for pafile in options.parkingareas:
            for pa in sumolib.xml.parse(pafile, "parkingArea"):
                edge = '_'.join(pa.lane.split('_')[:-1])
                edge2parking[edge] = pa.id

    # with io.open(options.outfile, 'w', encoding="utf8") as outf:
    # with open(options.outfile, 'w', encoding="utf8") as outf:
    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", options=options)  # noqa
        if options.routefiles:
            loadRouteFiles(options, options.routefiles, edge2parking, outf)
        else:
            generateStationary(options, edge2parking, outf)
        outf.write('</routes>\n')
    outf.close()


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
