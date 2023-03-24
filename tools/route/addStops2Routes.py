#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
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
import random
from collections import defaultdict

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.output import parse, parse_fast  # noqa
from sumolib.options import ArgumentParser  # noqa
import sumolib  # noqa


def get_options(args=None):
    op = ArgumentParser()
    op.add_option("-n", "--net-file", dest="netfile",
                  help="define the net filename (mandatory)")
    op.add_option("-r", "--route-files", dest="routefiles",
                  help="define the route file separated by comma (mandatory)")
    op.add_option("-o", "--output-file", dest="outfile",
                  help="define the output filename")
    op.add_option("-t", "--typesfile", dest="typesfile",
                  help="Give a typesfile")
    op.add_option("-d", "--duration",
                  help="Define duration of vehicle stop (setting 'X-Y' picks randomly from [X,Y[)")
    op.add_option("-u", "--until",
                  help="Define end time of vehicle stop")
    op.add_option("-p", "--parking", dest="parking", action="store_true",
                  default=False, help="where is the vehicle parking")
    op.add_option("--relpos",
                  help="relative stopping position along the edge [0,1] or 'random'")
    op.add_option("--lane", default="0",
                  help="set index of stop lane or 'random' (unusable lanes are not counted)")
    op.add_option("--reledge", default="1",
                  help="relative stopping position along the route [0,1] or 'random' (1 indicates the last edge)")
    op.add_option("--probability", type=float, default=1,
                  help="app stop with the given probability ]0, 1]")
    op.add_option("--parking-areas", dest="parkingareas", default=False,
                  help="load parkingarea definitions and stop at parkingarea on the arrival edge if possible")
    op.add_option("--start-at-stop", dest="startAtStop", action="store_true",
                  default=False, help="shorten route so it starts at stop")
    op.add_option("--rel-occupancy", dest="relOccupancy", type=float,
                  help="fill all parkingAreas to relative occupancy")
    op.add_option("--abs-occupancy", dest="absOccupancy", type=int, default=1,
                  help="fill all parkingAreas to absolute occupancy")
    op.add_option("--abs-free", dest="absFree", type=int,
                  help="fill all parkingAreas to absolute remaining capacity")
    op.add_option("-D", "--person-duration", dest="pDuration",
                  help="Define duration of person stop (setting 'X-Y' picks randomly from [X,Y[)")
    op.add_option("-U", "--person-until", dest="pUntil",
                  help="Define end time of person stop")
    op.add_option("-s", "--seed", type=int, default=42, help="random seed")
    op.add_option("-v", "--verbose", dest="verbose", action="store_true",
                  default=False, help="tell me what you are doing")

    (options, args) = op.parse_known_args(args=args)

    if options.parkingareas:
        options.parkingareas = options.parkingareas.split(",")

    if not options.routefiles:
        if not options.startAtStop:
            op.print_help()
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
        op.print_help()
        sys.exit("--net-file missing")

    if not options.typesfile:
        options.typesfile = options.routefiles
    else:
        options.typesfile = options.typesfile.split(",")

    if not options.duration and not options.until:
        op.print_help()
        sys.exit("stop duration or until missing")

    if options.relpos is not None:
        try:
            options.relpos = max(0, min(1, float(options.relpos)))
        except ValueError:
            if options.relpos != 'random':
                sys.exit("option --relpos must be set to 'random' or to a float value from [0,1]")
            pass

    if options.lane is not None:
        try:
            options.lane = int(options.lane)
            if options.lane < 0:
                sys.exit("option --lane must be set to 'random' or to a non-negative integer value")
        except ValueError:
            if options.lane != 'random':
                sys.exit("option --lane must be set to 'random' or to an integer value")
            pass

    if options.reledge is not None:
        try:
            options.reledge = max(0, min(1, float(options.reledge)))
        except ValueError:
            if options.reledge != 'random':
                sys.exit("option --reledge must be set to 'random' or to a float value from [0,1]")
            pass

    return options


def readTypes(options):
    vtypes = {None: "passenger"}
    for file in options.typesfile:
        for vtype in sumolib.output.parse(file, 'vType'):
            vtypes[vtype.id] = vtype.getAttributeSecure("vClass", "passenger")
    # print(vtypes)
    return vtypes


def getEdgeIDs(obj):
    result = []
    if obj.route:
        return obj.route[0].edges.split()
    if obj.attr_from:
        result.append(obj.attr_from)
    if obj.to:
        result.append(obj.to)
    return result


def interpretDuration(duration):
    if '-' in duration:
        start, stop = duration.split('-')
        return random.randrange(int(start), int(stop))
    else:
        return duration


def loadRouteFiles(options, routefile, edge2parking, outf):
    net = sumolib.net.readNet(options.netfile)
    vtypes = readTypes(options)
    numSkipped = defaultdict(lambda: 0)

    for routefile in options.routefiles:
        for obj in sumolib.xml.parse(routefile, ['vehicle', 'trip', 'flow', 'person', 'vType']):
            if (obj.name == 'vType' or
                    options.probability < 1 and random.random() > options.probability):
                outf.write(obj.toXML(' '*4))
                continue
            edgeIDs = getEdgeIDs(obj)
            reledge = options.reledge
            if reledge == 'random':
                reledge = random.random()
            lastEdgeID = None
            if edgeIDs:
                lastEdgeID = edgeIDs[int(round(reledge * (len(edgeIDs) - 1)))]

            if lastEdgeID is None:
                if obj.name == 'person' and (
                        options.pDuration is not None
                        or options.pUntil is not None):
                    stopAttrs = {}
                    if options.pDuration:
                        stopAttrs["duration"] = interpretDuration(options.pDuration)
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
                skip = True
                lanes = lastEdge.getLanes()
                usable = [lane for lane in lanes if lane.allows(vtypes[obj.type])]
                if usable:
                    lane = None
                    if options.lane == 'random':
                        lane = random.choice(usable)
                    elif options.lane < len(usable):
                        lane = usable[options.lane]

                    if lane:
                        skip = False
                        stopAttrs["lane"] = lane.getID()
                        if options.relpos:
                            relpos = options.relpos
                            if options.relpos == 'random':
                                relpos = random.random()
                            stopAttrs["endPos"] = "%.2f" % (lane.getLength() * relpos)
                if skip:
                    numSkipped[obj.name] += 1
                    print("Warning: no allowed lane found on edge '%s' for vehicle '%s' (%s)" % (
                        lastEdgeID, obj.id, vtypes[obj.type]), file=sys.stderr)

            if options.parking:
                stopAttrs["parking"] = "true"
            if options.duration:
                stopAttrs["duration"] = interpretDuration(options.duration)
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
        attrs += ' duration="%s"' % interpretDuration(options.duration)
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
    random.seed(options.seed)

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
