#!/usr/bin/env python3
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
import subprocess
from collections import defaultdict
import zipfile
import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'

sys.path += [os.path.join(os.environ["SUMO_HOME"], "tools"), os.path.join(os.environ['SUMO_HOME'], 'tools', 'route')]  # noqa
import route2poly  # noqa
import sumolib  # noqa
import tracemapper  # noqa

import gtfs2fcd  # noqa
import gtfs2osm  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap = gtfs2fcd.add_options()
    # ----------------------- general options ---------------------------------
    ap.add_argument("-n", "--network", fix_path=True,
                    help="sumo network to use")
    ap.add_argument("--route-output",
                    help="file to write the generated public transport vehicles to")  # noqa
    ap.add_argument("--additional-output",
                    help="file to write the generated public transport stops and routes to")  # noqa
    ap.add_argument("--duration", default=10,
                    type=int, help="minimum time to wait on a stop")
    ap.add_argument("--bus-stop-length", default=13, type=float,
                    help="length for a bus stop")
    ap.add_argument("--train-stop-length", default=110, type=float,
                    help="length for a train stop")
    ap.add_argument("--tram-stop-length", default=60, type=float,
                    help="length for a tram stop")
    ap.add_argument("--sort", action="store_true", default=False,
                    help="sorting the output-file")

    # ----------------------- fcd options -------------------------------------
    ap.add_argument("--network-split",
                    help="directory to write generated networks to")
    # ap.add_argument("--network-split.vclass", action="store_true", default=False,
    #                        help="use the allowed vclass instead of the edge type to split the network")
    ap.add_argument("--warn-unmapped", action="store_true", default=False,
                    help="warn about unmapped routes")
    ap.add_argument("--mapperlib", default="lib/fcd-process-chain-2.2.2.jar",
                    help="mapping library to use")
    ap.add_argument("--map-output",
                    help="directory to write the generated mapping files to")  # noqa
    ap.add_argument("--map-output-config", default="conf/output_configuration_template.xml",  # noqa
                    help="output configuration template for the mapper library")  # noqa
    ap.add_argument("--map-input-config", default="conf/input_configuration_template.xml",  # noqa
                    help="input configuration template for the mapper library")
    ap.add_argument("--map-parameter", default="conf/parameters_template.xml",
                    help="parameter template for the mapper library")
    ap.add_argument("--poly-output", help="file to write the generated polygon files to")  # noqa
    ap.add_argument("--fill-gaps", default=5000, type=float,
                    help="maximum distance between stops")
    ap.add_argument("--skip-fcd", action="store_true", default=False,
                    help="skip generating fcd data")
    ap.add_argument("--skip-map", action="store_true", default=False,
                    help="skip network mapping")

    # ----------------------- osm options -------------------------------------
    ap.add_argument("--osm-routes", help="osm routes file")
    ap.add_argument("--warning-output",
                    help="file to write the unmapped elements from gtfs")
    ap.add_argument("--dua-repair-output",
                    help="file to write the osm routes with errors")
    ap.add_argument("--bbox",
                    help="define the bounding box to filter the gtfs data, format: W,S,E,N")  # noqa
    ap.add_argument("--repair", help="repair osm routes", action='store_true')
    ap.add_argument("--min-stops", default=1, type=int,
                    help="minimum number of stops a public transport line must have to be imported")  # noqa

    options = ap.parse_args(args)

    options = gtfs2fcd.check_options(options)

    if options.network is None:
        sys.exit("Please give a network file using --network FILE.")
    if options.additional_output is None:
        options.additional_output = options.region + "_publictransport.add.xml"
    if options.route_output is None:
        options.route_output = options.region + "_publictransport.rou.xml"
    if options.warning_output is None:
        options.warning_output = options.region + "_missing.xml"
    if options.dua_repair_output is None:
        options.dua_repair_output = options.region + "_repair_errors.txt"
    if options.map_output is None:
        options.map_output = os.path.join('output', options.region)
    if options.network_split is None:
        options.network_split = os.path.join('resources', options.region)

    return options


def splitNet(options):
    netcCall = [sumolib.checkBinary("netconvert"), "--no-internal-links", "--numerical-ids", "--no-turnarounds",
                "--offset.disable-normalization", "--output.original-names", "--aggregate-warnings", "1",
                "--junctions.corner-detail", "0", "--dlr-navteq.precision", "0", "--geometry.avoid-overlap", "false"]
    if not os.path.exists(options.network_split):
        os.makedirs(options.network_split)
    numIdNet = os.path.join(options.network_split, "numerical.net.xml")
    subprocess.call(netcCall + ["-s", options.network, "-o", numIdNet, "--discard-params", "origId,origFrom,origTo"])
    edgeMap = {}
    seenTypes = set()
    for e in sumolib.net.readNet(numIdNet).getEdges():
        edgeMap[e.getID()] = e.getLanes()[0].getParam("origId", e.getID())
        seenTypes.add(e.getType())
    typedNets = {}
    for inp in glob.glob(os.path.join(options.gpsdat, "gpsdat_*.csv")):
        mode = os.path.basename(inp)[7:-4]
        if not options.modes or mode in options.modes.split(","):
            netPrefix = os.path.join(options.network_split, mode)
            edgeTypes = [mode]
            if "rail" in mode or mode == "subway":
                edgeTypes = ["railway." + mode]
            elif mode in ("tram", "bus"):
                edgeTypes = ["railway.tram"] if mode == "tram" else []
                for hwType in ("bus_guideway", "living_street", "motorway", "motorway_link", "primary", "primary_link",
                               "residential", "secondary", "secondary_link", "tertiary", "tertiary_link",
                               "trunk", "trunk_link", "unclassified", "unsurfaced"):
                    if mode == "tram":
                        edgeTypes.append("highway.%s|railway.tram" % hwType)
                    else:
                        edgeTypes.append("highway." + hwType)
            edgeType = ",".join(filter(lambda t: t in seenTypes, edgeTypes))
            if edgeType:
                subprocess.call(netcCall + ["-s", numIdNet, "-o", netPrefix + ".net.xml",
                                            "--dlr-navteq-output", netPrefix,
                                            "--dismiss-vclasses", "--keep-edges.by-type", edgeType])
                typedNets[mode] = (inp, netPrefix)
    return edgeMap, typedNets


def mapFCD(options, typedNets):
    for o in glob.glob(os.path.join(options.map_output, "*.dat")):
        os.remove(o)
    outConf = os.path.join(os.path.dirname(options.map_output_config), "output_configuration.xml")
    with open(options.map_output_config) as inp, open(outConf, "w") as outp:
        outp.write(inp.read() % {"output": options.map_output})
    for railType, (gpsdat, netPrefix) in typedNets.items():
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


def traceMap(options, typedNets, radius=100):
    routes = defaultdict(list)
    for railType in typedNets.keys():
        if options.verbose:
            print("mapping", railType)
        net = sumolib.net.readNet(os.path.join(options.network_split, railType + ".net.xml"))
        netBox = net.getBBoxXY()
        traces = tracemapper.readFCD(os.path.join(options.fcd, railType + ".fcd.xml"), net, True)
        for tid, trace in traces:
            minX, minY, maxX, maxY = sumolib.geomhelper.addToBoundingBox(trace)
            if (minX < netBox[1][0] + radius and minY < netBox[1][1] + radius and
                    maxX > netBox[0][0] - radius and maxY > netBox[0][1] - radius):
                mappedRoute = sumolib.route.mapTrace(trace, net, radius, fillGaps=options.fill_gaps)
                if mappedRoute:
                    routes[tid] = [e.getID() for e in mappedRoute]
    return routes


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
    stops = defaultdict(list)
    stopDef = set()
    rid = None
    for inp in sorted(glob.glob(os.path.join(options.fcd, "*.fcd.xml"))):
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
        for veh in sumolib.xml.parse_fast(inp, "vehicle", ("id", "x", "y", "until", "name",
                                                           "fareZone", "fareSymbol", "startFare")):
            addAttrs = ' friendlyPos="true" name="%s"' % veh.attr_name
            params = ""
            if veh.fareZone:
                params = "".join(['        <param key="%s" value="%s"/>\n' %
                                  p for p in (('fareZone', veh.fareZone), ('fareSymbol', veh.fareSymbol),
                                              ('startFare', veh.startFare))])
            if rid != veh.id:
                lastIndex = 0
                lastPos = -1
                rid = veh.id
            if rid not in routes:
                if options.warn_unmapped and rid not in seen:
                    print("Warning! Not mapped", rid)
                    seen.add(rid)
                continue
            route = routes[rid]
            if rid not in fixed:
                routeFixed = [route[0]]
                for routeEdgeID in route[1:]:
                    path, _ = typedNet.getShortestPath(typedNet.getEdge(routeFixed[-1]), typedNet.getEdge(routeEdgeID))
                    if path is None or len(path) > options.fill_gaps + 2:
                        error = "no path found" if path is None else "path too long (%s)" % len(path)
                        print("Warning! Skipping disconnected route '%s', %s." % (rid, error))
                        seen.add(rid)
                        del routes[rid]
                        break
                    else:
                        if len(path) > 2:
                            print("Warning! Fixed connection", rid, len(path))
                        routeFixed += [e.getID() for e in path[1:]]
                if rid not in routes:
                    continue
                route = routes[rid] = routeFixed
                fixed.add(rid)
            p = typedNet.convertLonLat2XY(float(veh.x), float(veh.y))
            if railType == "bus":
                stopLength = options.bus_stop_length
            elif railType == "tram":
                stopLength = options.tram_stop_length
            else:
                stopLength = options.train_stop_length
            candidates = []
            for edge, dist in typedNet.getNeighboringEdges(*p, r=200):
                if edge.getID() in route[lastIndex:]:
                    if edge.getLength() < stopLength:
                        dist += stopLength - edge.getLength()  # penalty for short edges
                    candidates.append((edge, dist))
            found = False
            for edge, dist in sorted(candidates, key=lambda i: i[1]):
                pos = edge.getClosestLanePosDist(p)[1]
                if edge.getID() != route[lastIndex] or pos > lastPos:
                    lastIndex = route.index(edge.getID(), lastIndex)
                    lastPos = pos
                    origEdgeID = edge.getLanes()[0].getParam("origId", edge.getID())
                    stop = "%s:%.2f" % (origEdgeID, pos)
                    if stop not in stopDef:
                        stopDef.add(stop)
                        startPos = max(0, pos - stopLength)
                        if railType == "bus":
                            for l in edge.getLanes():
                                if l.allows(railType):
                                    break
                            rout.write('    <busStop id="%s" lane="%s_%s" startPos="%.2f" endPos="%.2f"%s>\n%s' %
                                       (stop, origEdgeID, l.getIndex(),
                                        startPos, pos + stopLength, addAttrs, params))
                            rout.write('    </busStop>\n')
                        else:
                            rout.write('    <trainStop id="%s" lane="%s_0" startPos="%.2f" endPos="%.2f"%s>\n%s' %
                                       (stop, origEdgeID,
                                        startPos, pos + stopLength, addAttrs, params))
                            ap = net.convertLonLat2XY(float(veh.x), float(veh.y))
                            numAccess = 0
                            for accessEdge, _ in sorted(net.getNeighboringEdges(*ap, r=100), key=lambda i: i[1]):
                                if accessEdge.getID() != edge.getID() and accessEdge.allows("pedestrian"):
                                    _, accessPos, accessDist = accessEdge.getClosestLanePosDist(ap)
                                    rout.write(('        <access friendlyPos="true" ' +
                                                'lane="%s_0" pos="%.2f" length="%.2f"/>\n') %
                                               (accessEdge.getID(), accessPos, 1.5 * accessDist))
                                    numAccess += 1
                                    if numAccess == 10:
                                        break
                            rout.write('    </trainStop>\n')
                    stops[rid].append((stop, int(veh.until)))
                    found = True
                    break
            if not found:
                if candidates or options.warn_unmapped:
                    print("Warning! No stop for coordinates %.2f, %.2f" % p, "on", veh)
    return stops


def filter_trips(options, routes, stops, outfile, begin, end):
    numDays = end // 86400
    if end % 86400 != 0:
        numDays += 1
    with open(outfile, 'w', encoding="utf8") as outf:
        sumolib.xml.writeHeader(outf, os.path.basename(__file__), "routes")
        for inp in glob.glob(os.path.join(options.fcd, "*.rou.xml")):
            for veh in sumolib.xml.parse_fast(inp, "vehicle", ("id", "route", "type", "depart", "line")):
                if len(routes.get(veh.route, [])) > 0 and len(stops.get(veh.route, [])) > 1:
                    until = stops[veh.route][0][1]
                    for d in range(numDays):
                        depart = max(0, d * 86400 + int(veh.depart) + until - options.duration)
                        if begin <= depart < end:
                            if d != 0 and veh.id.endswith(".trimmed"):
                                # only add trimmed trips the first day
                                continue
                            outf.write('    <vehicle id="%s.%s" route="%s" type="%s" depart="%s" line="%s"/>\n' %
                                       (veh.id, d, veh.route, veh.type, depart, veh.line))
        outf.write('</routes>\n')
    if options.sort:
        path = os.path.join(os.environ["SUMO_HOME"], "tools", "route", "sort_routes.py")
        subprocess.call(["python", path, outfile, "-o", "temp.xml"])
        os.remove(outfile)
        os.rename("temp.xml", outfile)


def main(options):
    if options.verbose:
        print('Loading net')
    net = sumolib.net.readNet(options.network)

    if options.osm_routes:
        # Import PT from GTFS and OSM routes
        if not options.bbox:
            BBoxXY = net.getBBoxXY()
            BBoxLonLat = (net.convertXY2LonLat(BBoxXY[0][0], BBoxXY[0][1]),
                          net.convertXY2LonLat(BBoxXY[1][0], BBoxXY[1][1]))
            options.bbox = (BBoxLonLat[0][0], BBoxLonLat[0][1],
                            BBoxLonLat[1][0], BBoxLonLat[1][1])
        else:
            options.bbox = [float(coord) for coord in options.bbox.split(",")]

        gtfsZip = zipfile.ZipFile(sumolib.open(options.gtfs, False))
        routes, trips_on_day, shapes, stops, stop_times = gtfs2osm.import_gtfs(options, gtfsZip)

        if shapes is None:
            print('Warning: Importing OSM routes currently requires a GTFS file with shapes.')
            options.osm_routes = None
        else:
            (gtfs_data, trip_list,
             filtered_stops,
             shapes, shapes_dict) = gtfs2osm.filter_gtfs(options, routes,
                                                         trips_on_day, shapes,
                                                         stops, stop_times)

            osm_routes = gtfs2osm.import_osm(options, net)

            (mapped_routes, mapped_stops,
            missing_stops, missing_lines) = gtfs2osm.map_gtfs_osm(options, net, osm_routes,  # noqa
                                                                gtfs_data, shapes,  # noqa
                                                                shapes_dict, filtered_stops)  # noqa

            gtfs2osm.write_gtfs_osm_outputs(options, mapped_routes, mapped_stops,
                                            missing_stops, missing_lines,
                                            gtfs_data, trip_list, shapes_dict, net)
    if not options.osm_routes:
        # Import PT from GTFS
        if not options.skip_fcd:
            gtfs2fcd.main(options)
        edgeMap, typedNets = splitNet(options)
        if os.path.exists(options.mapperlib):
            if not options.skip_map:
                mapFCD(options, typedNets)
            routes = defaultdict(lambda: [])
            for o in glob.glob(os.path.join(options.map_output, "*.dat")):
                for line in open(o):
                    time, edge, speed, coverage, id, minute_of_week = line.split('\t')[:6]
                    routes[id].append(edge)
        else:
            if options.mapperlib != "tracemapper":
                print("Warning! No mapping library found, falling back to tracemapper.")
            routes = traceMap(options, typedNets)

        if options.poly_output:
            generate_polygons(net, routes, options.poly_output)
        with open(options.additional_output, 'w', encoding="utf8") as rout:
            sumolib.xml.writeHeader(rout, os.path.basename(__file__), "additional")
            stops = map_stops(options, net, routes, rout)
            for vehID, edges in routes.items():
                if edges:
                    rout.write('    <route id="%s" edges="%s">\n' % (vehID, " ".join([edgeMap[e] for e in edges])))
                    offset = None
                    for stop in stops[vehID]:
                        if offset is None:
                            offset = stop[1]
                        rout.write('        <stop busStop="%s" duration="%s" until="%s"/>\n' %
                                   (stop[0], options.duration, stop[1] - offset))
                    rout.write('    </route>\n')
                else:
                    print("Warning! Empty route", vehID)
            rout.write('</additional>\n')
        filter_trips(options, routes, stops, options.route_output, options.begin, options.end)


if __name__ == "__main__":
    main(get_options())
