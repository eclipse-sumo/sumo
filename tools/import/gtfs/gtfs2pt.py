#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    gtfs2pt.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2018-08-28

"""
Maps GTFS data to a given network, generating routes, stops and vehicles
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import glob
import shutil
import subprocess
from collections import defaultdict

sys.path += [os.path.join(os.environ["SUMO_HOME"], "tools"), os.path.join(os.environ['SUMO_HOME'], 'tools', 'route')] 
import route2poly
import sumolib

import gtfs2fcd


def get_options(args=None):
    argParser = gtfs2fcd.add_options()
    argParser.add_argument("--mapperlib", default="lib/fcd-process-chain-2.2.2.jar",
                           help="mapping library to use")
    argParser.add_argument("--map-output", help="directory to write the generated mapping files to")
    argParser.add_argument("--map-output-config", default="conf/output_configuration_template.xml",
                           help="output configuration template for the mapper library")
    argParser.add_argument("--map-input-config", default="conf/input_configuration_template.xml",
                           help="input configuration template for the mapper library")
    argParser.add_argument("--map-parameter", default="conf/parameters_template.xml",
                           help="parameter template for the mapper library")
    argParser.add_argument("--poly-output", help="file to write the generated polygon files to")
    argParser.add_argument("--route-output", default="publictransport.add.xml",
                           help="file to write the generated public transport stops and routes to")
    argParser.add_argument("--vehicle-output", default="publictransport.rou.xml",
                           help="file to write the generated public transport vehicles to")
    argParser.add_argument("-n", "--network", help="sumo network to use")
    argParser.add_argument("--network-split", help="directory to write generated networks to")
    argParser.add_argument("-b", "--begin", default=0,
                           type=int, help="Defines the begin time to export")
    argParser.add_argument("-e", "--end", default=86400,
                           type=int, help="Defines the end time for the export")
    argParser.add_argument("--skip-fcd", action="store_true", default=False, help="skip generation fcd data")

    options = gtfs2fcd.check_options(argParser.parse_args(args=args))
    if options.map_output is None:
        options.map_output = os.path.join('output', options.region)
    if options.network_split is None:
        options.network_split = os.path.join('resources', options.region)
    return options


def splitNet(options):
    netcCall = [sumolib.checkBinary("netconvert"), "--no-internal-links", "--numerical-ids", "--no-turnarounds", 
                "--offset.disable-normalization", "--output.original-names",
                "--junctions.corner-detail", "0", "--dlr-navteq.precision", "0", "--geometry.avoid-overlap", "false"]
    if not os.path.exists(options.network_split):
        os.makedirs(options.network_split)
    numIdNet = os.path.join(options.network_split, "all.net.xml")
    subprocess.call(netcCall + ["-s", options.network, "-o", numIdNet])
    for inp in glob.glob(os.path.join(options.gpsdat, "gpsdat_*.csv")):
        railType = os.path.basename(inp)[7:-4]
        netPrefix = os.path.join(options.network_split, railType)
        if railType == "bus":
            ret = subprocess.call(netcCall + ["-s", numIdNet, "-o", os.path.join(options.network_split, railType + ".net.xml"),
                                              "--dlr-navteq-output", netPrefix])
        else:
            edgeType = railType
            if "rail" in railType:
                edgeType = "railway." + railType
            if railType == "tram":
                edgeType = "railway.tram,highway.residential\|railway.tram"
            ret = subprocess.call(netcCall + ["-s", numIdNet, "-o", os.path.join(options.network_split, railType + ".net.xml"),
                                              "--dlr-navteq-output", netPrefix, "--dismiss-vclasses", "--keep-edges.by-type", edgeType])
        if ret == 0:
            yield inp, railType, netPrefix


def mapFCD(options):
    for o in glob.glob(os.path.join(options.map_output, "*.dat")):
        os.remove(o)
    outConf = os.path.join(os.path.dirname(options.map_output_config), "output_configuration.xml")
    with open(options.map_output_config) as inp, open(outConf, "w") as outp:
        outp.write(inp.read() % {"output": options.map_output})
    for gpsdat, railType, netPrefix in splitNet(options):
        conf = os.path.join(os.path.dirname(options.map_input_config), "input_configuration_%s.xml") % railType
        with open(options.map_input_config) as inp, open(conf, "w") as outp:
            outp.write(inp.read() % {"input": gpsdat, "net_prefix": netPrefix})
        param = os.path.join(os.path.dirname(options.map_parameter), "parameters_%s.xml") % railType
        with open(options.map_parameter) as inp, open(param, "w") as outp:
            outp.write(inp.read() % {"radius": 100 if railType in ("bus", "tram") else 1000})
        call = "java -mx16000m -jar %s %s %s %s" % (options.mapperlib, conf, outConf, param)
        if options.verbose:
            print(call)
        sys.stdout.flush()
        subprocess.call(call, shell=True)


def traceMap(options):
    for o in glob.glob("output/%s/*.dat" % options.region):
        os.remove(o)


def generate_polygons(net, routes, outfile):
    colorgen = sumolib.miscutils.Colorgen(('random', 1, 1))
    class PolyOptions:
        internal = False
        spread = 0.2
        blur = 0
        geo = True
        layer = 100
        
    with open(outfile, 'w') as outf:
        outf.write('<polygons>\n')
        for vehID, edges in routes.items():
            route2poly.generate_poly(PolyOptions, net, vehID, colorgen(), edges, outf)
        outf.write('</polygons>\n')

def map_stops(options, net, routes, rout):
    stops = defaultdict(lambda:[])
    stopDef = set()
    rid = None
    for inp in glob.glob(os.path.join(options.fcd, "*.fcd.xml")):
        railType = os.path.basename(inp)[:-8]
        typedNetFile = os.path.join(options.network_split, railType + ".net.xml")
        if not os.path.exists(typedNetFile):
            print("Warning! No net", typedNetFile)
            continue
        if options.verbose:
            print("Reading", typedNetFile)
        typedNet = sumolib.net.readNet(typedNetFile)
        seen = set()
        fixed = set()
        for veh in sumolib.xml.parse_fast(inp, "vehicle", ("id", "x", "y", "until", "name", "fareZone", "fareSymbol", "startFare")):
            addAttrs = ' friendlyPos="true" name="%s"' % veh.attr_name
            params = ""
            if options.region == "moin":
                params = "".join(['        <param key="%s" value="%s"/>\n' % p for p in (('fareZone', veh.fareZone), ('fareSymbol', veh.fareSymbol), ('startFare', veh.startFare))])
            if rid != veh.id.split("_")[1]:
                lastIndex = 0
                rid = veh.id.split("_")[1]
            if rid not in routes:
                if rid not in seen:
                    print("Warning! Not mapped", rid)
                    seen.add(rid)
                continue
            route = routes[rid]
            if rid not in fixed:
                routeFixed = [route[0]]
                for routeEdgeID in route[1:]:
                    path, _ = typedNet.getShortestPath(typedNet.getEdge(routeFixed[-1]), typedNet.getEdge(routeEdgeID))
                    if path is None:
                        print("Warning! Not connected", rid)
                        routeFixed.append(routeEdgeID)
                    elif 2 < len(path) < 10:
                        print("Warning! Fixed connection", rid, len(path))
                        routeFixed += [e.getID() for e in path[1:]]
                    else:
                        routeFixed.append(routeEdgeID)
                route = routes[rid] = routeFixed
                fixed.add(rid)
            p = typedNet.convertLonLat2XY(float(veh.x), float(veh.y))
            found = False
            for edge, dist in sorted(typedNet.getNeighboringEdges(*p, r=200), key=lambda i:i[1]):
                if edge.getID() in route[lastIndex:]:
                    lastIndex = route.index(edge.getID(), lastIndex)
                    pos = edge.getClosestLanePosDist(p)[1]
                    stop = "%s:%.2f" % (edge.getID(), pos)
                    if not stop in stopDef:
                        stopDef.add(stop)
                        if railType == "bus":
                            for l in edge.getLanes():
                                if l.allows(railType):
                                    break
                            startPos = max(0, pos - 10)
                            rout.write('    <busStop id="%s" lane="%s" startPos="%s" endPos="%s"%s>\n%s    </busStop>\n' % (stop, l.getID(), startPos, pos + 10, addAttrs, params))
                        else:
                            startPos = max(0, pos - 60)
                            rout.write('    <trainStop id="%s" lane="%s_0" startPos="%s" endPos="%s"%s>\n%s' % (stop, edge.getID(), startPos, pos + 60, addAttrs, params))
                            ap = net.convertLonLat2XY(float(veh.x), float(veh.y))
                            numAccess = 0
                            for accessEdge, _ in sorted(net.getNeighboringEdges(*ap, r=100), key=lambda i:i[1]):
                                if accessEdge.getID() != edge.getID() and accessEdge.allows("passenger") and numAccess < 10:
                                    _, accessPos, accessDist = accessEdge.getClosestLanePosDist(ap)
                                    rout.write('        <access friendlyPos="true" lane="%s_0" pos="%s" length="%s"/>\n' % (accessEdge.getID(), accessPos, 1.5 * accessDist))
                                    numAccess += 1
                            rout.write('    </trainStop>\n')
                    stops[rid].append((stop, veh.until))
                    found = True
                    break
            if not found:
                print("Warning! No stop for", p, "on", veh)
    return stops

def filter_trips(options, routes, stops, outfile, begin, end):
    numDays = end // 86400
    if end % 86400 != 0:
        numDays += 1
    with open(outfile, 'w') as outf:
        sumolib.xml.writeHeader(outf, os.path.basename(__file__), "routes")
        for inp in glob.glob(os.path.join(options.fcd, "*.rou.xml")):
            for veh in sumolib.xml.parse_fast(inp, "vehicle", ("id", "route", "type", "depart", "line")):
                if veh.route in routes and len(routes[veh.route]) > 0 and veh.route in stops and len(stops[veh.route]) > 1:
                    for d in range(numDays):
                         depart = d * 86400 + int(veh.depart)
                         if begin <= depart < end:
                             outf.write('    <vehicle id="%s.%s" route="%s" type="%s" depart="%s" line="%s"/>\n' % (veh.id, d, veh.route, veh.type, depart, veh.line))
        outf.write('</routes>\n')

def main(options):
    if not options.skip_fcd:
        gtfs2fcd.main(options)
    if os.path.exists(options.mapperlib):
        mapFCD(options)
    else:
        print("Warning! No mapping library found, falling back to tracemapper.")
        traceMap(options)
    net = sumolib.net.readNet(options.network)
    routes = defaultdict(lambda:[])
    for o in glob.glob(os.path.join(options.map_output, "*.dat")):
        for line in open(o):
            time, edge, speed, coverage, id, minute_of_week = line.split('\t')[:6]
            routes[id].append(edge)
    if options.poly_output:
        generate_polygons(net, routes, options.poly_output)
    with open(options.route_output, 'w') as rout:
        sumolib.xml.writeHeader(rout, os.path.basename(__file__), "additional")
        stops = map_stops(options, net, routes, rout)
        for vehID, edges in routes.items():
            if edges:
                rout.write('    <route id="%s" edges="%s">\n' % (vehID, " ".join(edges)))
                for stop in stops[vehID]:
                    rout.write('        <stop busStop="%s" duration="10" until="%s"/>\n' % stop)
                rout.write('    </route>\n')
            else:
                print("Warning! Empty route", vehID)
        rout.write('</additional>\n')
    if options.region == gtfs2fcd.URMO:
        filter_trips(options, routes, stops, options.vehicle_output, 23 * 3600, 49 * 3600)
    else:
        filter_trips(options, routes, stops, options.vehicle_output, options.begin, options.end)

if __name__ == "__main__":
    main(get_options())
