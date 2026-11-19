#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    visum_convertRoutes.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2007-02-21


from __future__ import print_function
from __future__ import absolute_import
from collections import defaultdict
import os
import sys
import random

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
import sumolib  # noqa


class Statistics:
    def __init__(self):
        self.found = 0
        self.foundN = 0
        self.missing = 0
        self.missingN = 0


def get_options(args=None):
    op = sumolib.options.ArgumentParser()
    op.add_option("-n", "--net-file", dest="netfile", category="input", required=True, type=op.net_file,
                  help="SUMO net file to work with")
    op.add_option("-r", "--visum-routes", dest="routes", category="input", required=True, type=op.file,
                  help="The VISUM-routes files to parse")
    op.add_option("-o", "--output", category="output", type=op.file, required=True,
                  help="Name of the file to write")
    op.add_option("--taz-output", dest="tazOutput", category="output", type=op.file,
                  help="Write traffic zones (TAZ) with source and sink edges to FILE")
    op.add_option("-b", "--begin", category="time", type=op.time,
                  help="The begin time of the routes to generate", default=0)
    op.add_option("-e", "--end", category="time", type=op.time,
                  help="The end time (+1) of the routes to generate", default=3600)
    op.add_option("-p", "--prefix", category="input",
                  help="ID prefix to use")
    op.add_option("-t", "--type", category="input",
                  help="The type to use for vehicles")
    op.add_option("-u", "--uniform",
                  help="Whether departures shall be distributed uniform in each interval", action="store_true",
                  default=False)
    op.add_option("-l", "--timeline", category="input",
                  help="Percentages over a day")
    op.add_option("-a", "--tabs", action="store_true",
                  default=False, help="tab separated route file")
    op.add_option("-v", "--verbose", action="store_true",
                  default=False, help="tell me what you are doing")
    op.add_option("-2", "--net2", category="input", type=op.net_file,
                  help="immediately match routes to a second network", metavar="FILE")
    op.add_option("-s", "--step", default=10, category="input",
                  type=float, help="distance between successive trace points")
    op.add_option("-d", "--delta", default=1, category="input",
                  type=float, help="maximum distance between edge and trace points when matching to the second net")
    op.add_option("-i", "--distribution", action="store_true",
                  default=False, help="write route distributions only")
    op.add_option("--cutoff", category="input",
                  help="Keep only one route when less than CUTOFF vehicles drive the OD", type=int,
                  default=0)
    op.add_option("--seed", type=int, default=42, help="random seed")
    return op.parse_args(args=args)


def write_taz_output(options, tazSources, tazSinks):
    allZones = sorted(set(tazSources.keys()) | set(tazSinks.keys()))
    with sumolib.openz(options.tazOutput, "w") as tf:
        sumolib.writeXMLHeader(tf, "$Id$", "additional", options=options)
        for zoneID in allZones:
            tf.write('    <taz id=%s>\n' % sumolib.xml.quoteattr(zoneID))
            for edgeID, weight in sorted(tazSources[zoneID].items()):
                tf.write('        <tazSource id=%s weight="%s"/>\n' % (sumolib.xml.quoteattr(edgeID), weight))
            for edgeID, weight in sorted(tazSinks[zoneID].items()):
                tf.write('        <tazSink id=%s weight="%s"/>\n' % (sumolib.xml.quoteattr(edgeID), weight))
            tf.write('    </taz>\n')
        tf.write('</additional>\n')


def main(args=None):
    options = get_options(args=args)
    random.seed(options.seed)
    stats = Statistics()
    routes = []
    tazSources = defaultdict(lambda: defaultdict(float))
    tazSinks = defaultdict(lambda: defaultdict(float))

    if options.verbose:
        print("Reading net...")
    net = sumolib.net.readNet(options.netfile)
    net2 = None
    if options.net2:
        net.move(-net.getLocationOffset()[0], -net.getLocationOffset()[1])
        net2 = sumolib.net.readNet(options.net2)
        net2.move(-net2.getLocationOffset()[0], -net2.getLocationOffset()[1])

    emap = {}
    for e in net._edges:
        if e._from._id not in emap:
            emap[e._from._id] = {}
        if e._to._id not in emap[e._from._id]:
            emap[e._from._id][e._to._id] = e._id

    def map_route_edges(route_str):
        if not net2:
            return route_str
        trace = []
        for e in route_str.split():
            edge = net.getEdge(e)
            numSteps = int(edge.getLength() / options.step)
            for p in range(numSteps):
                trace.append(sumolib.geomhelper.positionAtShapeOffset(edge.getShape(), p * options.step))
        path = sumolib.route.mapTrace(trace, net2, options.delta)
        return " ".join(path)

    def record_taz_edges(route_str, count_val, orig_z, dest_z):
        edges = route_str.split()
        if not edges:
            return
        if orig_z:
            tazSources[orig_z][edges[0]] += count_val
        if dest_z:
            tazSinks[dest_z][edges[-1]] += count_val

    def flush_distribution(fdo):
        if not routes:
            return
        distID = routes[0][0][:routes[0][0].rfind("_")]
        total = 0
        r_max = routes[0]
        for r in routes:
            total += r[1]
            if r_max[1] < r[1]:
                r_max = r
        if total < options.cutoff:
            del routes[:]
            routes.append((r_max[0], total, r_max[2], r_max[3], r_max[4]))
        if len(routes) > 1:
            fdo.write('    <routeDistribution id="%s">\n' % distID)
        else:
            routes[0] = (distID, routes[0][1], routes[0][2], routes[0][3], routes[0][4])
        for r in routes:
            mapped_edges = map_route_edges(r[2])
            fdo.write('        <route id="%s" probability="%s" edges="%s"/>\n' % (r[0], r[1], mapped_edges))
        if len(routes) > 1:
            fdo.write('    </routeDistribution>\n')
        del routes[:]

    def addRouteChecking(route_str, rid, rcount, rok, orig_z, dest_z, fdo):
        route_str = route_str.strip()
        if route_str != "":
            if rok:
                mapped_for_taz = map_route_edges(route_str) if options.distribution else route_str
                if not options.distribution and net2:
                    route_str = map_route_edges(route_str)
                    mapped_for_taz = route_str
                record_taz_edges(mapped_for_taz, rcount, orig_z, dest_z)
                if options.distribution and routes:
                    distID = routes[0][0][:routes[0][0].rfind("_")]
                    if distID != rid[:rid.rfind("_")]:
                        flush_distribution(fdo)
                routes.append((rid, rcount, route_str, orig_z, dest_z))
                stats.found += 1
                stats.foundN += rcount
            else:
                stats.missing += 1
                stats.missingN += rcount

    if options.verbose:
        print("Reading routes...")
    separator = "\t" if options.tabs else ";"
    parse = False
    ok = True
    route = ""
    rid = ""
    origZone = ""
    destZone = ""
    attributes = []
    count = 0
    routeNo = 0
    with sumolib.openz(options.routes) as fd, sumolib.openz(options.output, "w") as fdo:
        sumolib.writeXMLHeader(fdo, "$Id$", "routes", options=options)
        for idx, line in enumerate(fd):
            if options.verbose and idx % 10000 == 0 and idx > 0:
                sys.stdout.write("%s lines read\r" % "{:,}".format(idx))
            if line.find("$") == 0 or line.find("*") == 0 or line.find(separator) < 0:
                parse = False
                addRouteChecking(route, rid, count, ok, origZone, destZone, fdo)
                route = ""
            if parse:
                values = line.strip('\n\r').split(separator)
                amap = {}
                for i in range(0, len(attributes)):
                    amap[attributes[i]] = values[i]
                if amap["origzoneno"] != "":
                    addRouteChecking(route, rid, count, ok, origZone, destZone, fdo)
                    origZone = amap["origzoneno"]
                    destZone = amap["destzoneno"]
                    rid = origZone + "_" + destZone + "_" + amap["pathindex"]
                    count = float(amap["prtpath\\vol(ap)"])
                    route = " "
                    ok = True
                    routeNo += 1
                else:
                    if not ok:
                        continue
                    fromnode = amap["fromnodeno"]
                    tonode = amap["tonodeno"]
                    link = amap["linkno"]
                    if fromnode not in emap:
                        if routeNo != 0:
                            print("Missing from-node '" + fromnode + "'; skipping")
                        ok = False
                        continue
                    if tonode not in emap[fromnode]:
                        if routeNo != 0:
                            print("No connection between from-node '" +
                                  fromnode + "' and to-node '" + tonode + "'; skipping")
                        ok = False
                        continue
                    edge = emap[fromnode][tonode]
                    if link != edge and link != edge[1:]:
                        if routeNo != 0:
                            print("Mismatching edge '" + link + "' (from '" +
                                  fromnode + "', to '" + tonode + "'); skipping")
                        ok = False
                        continue
                    route = route + edge + " "

            if line.find("$PRTPATHLINK:") == 0 or line.find("$IVTEILWEG:") == 0:
                attributes = line[line.find(":") + 1:].strip().lower().split(separator)
                for i in range(0, len(attributes)):
                    if attributes[i] == "qbeznr":
                        attributes[i] = "origzoneno"
                    if attributes[i] == "zbeznr":
                        attributes[i] = "destzoneno"
                    if attributes[i] == "iv-weg\\bel(ap)":
                        attributes[i] = "prtpath\\vol(ap)"
                    if attributes[i] == "wegind":
                        attributes[i] = "pathindex"
                    if attributes[i] == "vonknotnr":
                        attributes[i] = "fromnodeno"
                    if attributes[i] == "nachknotnr":
                        attributes[i] = "tonodeno"
                    if attributes[i] == "strnr":
                        attributes[i] = "linkno"
                parse = True

        addRouteChecking(route, rid, count, ok, origZone, destZone, fdo)

        if options.verbose:
            print(" %s routes found (%s vehs)" % (stats.found, stats.foundN))
            print(" %s routes missing (%s vehs)" % (stats.missing, stats.missingN))

        if options.tazOutput:
            write_taz_output(options, tazSources, tazSinks)

        if options.distribution:
            flush_distribution(fdo)
            fdo.write("</routes>\n")
            return

        timeline = None
        if options.timeline:
            timeline = []
            vals = options.timeline.split(",")
            for v in vals:
                timeline.append(float(v))
            if len(timeline) != 24:
                print("The timeline must have 24 entries")
                sys.exit()

        if options.verbose:
            print("Generating vehicles...")
        emissions = []
        begin = int(options.begin)
        end = int(options.end)

        if not timeline:
            for r in routes:
                for i in range(0, int(r[1])):
                    if options.uniform:
                        t = float(begin) + float(end - begin) / float(r[1]) * float(i)
                    else:
                        t = float(begin) + float(end - begin) * random.random()
                    emissions.append((int(t), r[0] + "__" + str(i), r[2], r[3], r[4]))
        else:
            nNo = 0
            for r in routes:
                left = 0.
                tbeg = 0
                j = 0
                for tp in timeline:
                    fno = (float(r[1]) + left) * tp / 100.
                    no = int(fno)
                    left += fno - no
                    if left >= 1.:
                        left -= 1
                        no += 1
                    for i in range(0, no):
                        if options.uniform:
                            t = tbeg + float(3600) / float(r[1]) * float(i)
                        else:
                            t = tbeg + float(3600) * random.random()
                        emissions.append((int(t), r[0] + "__" + str(j), r[2], r[3], r[4]))
                        j = j + 1
                    nNo += no
                    tbeg += 3600
            if options.verbose:
                print(" %s vehicles after applying timeline" % nNo)

        if options.verbose:
            print("Sorting routes...")
        emissions.sort(key=lambda x: (x[0], x[1]))

        if options.verbose:
            print("Writing routes...")
        for emission in emissions:
            veh_id = (options.prefix + "_" if options.prefix else "") + emission[1]
            attrs = ['id=%s' % sumolib.xml.quoteattr(veh_id), 'depart="%s"' % emission[0]]
            if options.type:
                attrs.append('type=%s' % sumolib.xml.quoteattr(options.type))
            if emission[3]:
                attrs.append('fromTaz=%s' % sumolib.xml.quoteattr(emission[3]))
            if emission[4]:
                attrs.append('toTaz=%s' % sumolib.xml.quoteattr(emission[4]))
            fdo.write('    <vehicle %s><route edges="%s"/></vehicle>\n' % (" ".join(attrs), emission[2]))
        fdo.write("</routes>\n")
        if options.verbose:
            print(" %s vehicles written" % len(emissions))


if __name__ == "__main__":
    main()
