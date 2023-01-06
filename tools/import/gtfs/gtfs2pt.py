#!/usr/bin/env python3
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

# @file    gtfs2pt.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2018-08-28

"""
Maps GTFS data to a given network, generating routes, stops and vehicles
"""

from __future__ import print_function
from __future__ import absolute_import
from __future__ import division
import os
import sys
import glob
import subprocess
import collections
import zipfile
import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'

sys.path += [os.path.join(os.environ["SUMO_HOME"], "tools"),
             os.path.join(os.environ['SUMO_HOME'], 'tools', 'route')]
import route2poly  # noqa
import sumolib  # noqa
import tracemapper  # noqa

import gtfs2fcd  # noqa
import gtfs2osm  # noqa


def get_options(args=None):
    ap = gtfs2fcd.add_options()
    # ----------------------- general options ---------------------------------
    ap.add_argument("-n", "--network", fix_path=True, required=True,
                    help="sumo network to use")
    ap.add_argument("--route-output",
                    help="file to write the generated public transport vehicles to")
    ap.add_argument("--additional-output",
                    help="file to write the generated public transport stops and routes to")
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
                    help="directory to write the generated mapping files to")
    ap.add_argument("--map-output-config", default="conf/output_configuration_template.xml",
                    help="output configuration template for the mapper library")
    ap.add_argument("--map-input-config", default="conf/input_configuration_template.xml",
                    help="input configuration template for the mapper library")
    ap.add_argument("--map-parameter", default="conf/parameters_template.xml",
                    help="parameter template for the mapper library")
    ap.add_argument("--poly-output", help="file to write the generated polygon files to")
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
    ap.add_argument("--repair", help="repair osm routes", action='store_true')
    ap.add_argument("--min-stops", default=1, type=int,
                    help="minimum number of stops a public transport line must have to be imported")

    options = ap.parse_args(args)

    options = gtfs2fcd.check_options(options)

    if options.additional_output is None:
        options.additional_output = options.region + "_pt_stops.add.xml"
    if options.route_output is None:
        options.route_output = options.region + "_pt_vehicles.add.xml"
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
    doNavteqOut = os.path.exists(options.mapperlib)
    if not os.path.exists(options.network_split):
        os.makedirs(options.network_split)
    numIdNet = os.path.join(options.network_split, "numerical.net.xml")
    if os.path.exists(numIdNet) and os.path.getmtime(numIdNet) > os.path.getmtime(options.network):
        print("Reusing old", numIdNet)
    else:
        subprocess.call(netcCall + ["-s", options.network, "-o", numIdNet,
                                    "--discard-params", "origId,origFrom,origTo"])
    edgeMap = {}
    seenTypes = set()
    for e in sumolib.net.readNet(numIdNet).getEdges():
        edgeMap[e.getID()] = e.getLanes()[0].getParam("origId", e.getID())
        seenTypes.add(e.getType())
    typedNets = {}
    for inp in sorted(glob.glob(os.path.join(options.fcd, "*.fcd.xml"))):
        mode = os.path.basename(inp)[:-8]
        if not options.modes or mode in options.modes.split(","):
            netPrefix = os.path.join(options.network_split, mode)
            edgeFilter = ["--keep-edges.by-type", mode] if mode in seenTypes else None
            if "rail" in mode or mode == "subway":
                if "railway." + mode in seenTypes:
                    edgeFilter = ["--keep-edges.by-type", "railway." + mode]
            elif mode == "train":
                edgeFilter = ["--keep-edges.by-type", "railway.rail,railway.light_rail"]
            elif mode in ("tram", "bus"):
                edgeFilter = ["--keep-edges.by-vclass", mode]
            if edgeFilter:
                if (os.path.exists(netPrefix + ".net.xml") and
                        os.path.getmtime(netPrefix + ".net.xml") > os.path.getmtime(numIdNet)):
                    print("Reusing old", netPrefix + ".net.xml")
                else:
                    subprocess.call(netcCall + ["-s", numIdNet, "-o", netPrefix + ".net.xml"] + edgeFilter)
                    if doNavteqOut:
                        subprocess.call(netcCall + ["-s", netPrefix + ".net.xml", "-o", "NUL", "--dismiss-vclasses"
                                                    "--dlr-navteq-output", netPrefix])
                typedNets[mode] = (inp, netPrefix)
    return edgeMap, typedNets


def mapFCD(options, typedNets):
    for o in glob.glob(os.path.join(options.map_output, "*.dat")):
        os.remove(o)
    outConf = os.path.join(os.path.dirname(options.map_output_config), "output_configuration.xml")
    with open(options.map_output_config) as inp, open(outConf, "w") as outp:
        outp.write(inp.read() % {"output": options.map_output})
    for mode, (gpsdat, netPrefix) in typedNets.items():
        conf = os.path.join(os.path.dirname(options.map_input_config), "input_configuration_%s.xml") % mode
        with open(options.map_input_config) as inp, open(conf, "w") as outp:
            outp.write(inp.read() % {"input": gpsdat, "net_prefix": netPrefix})
        param = os.path.join(os.path.dirname(options.map_parameter), "parameters_%s.xml") % mode
        with open(options.map_parameter) as inp, open(param, "w") as outp:
            outp.write(inp.read() % {"radius": 100 if mode in ("bus", "tram") else 1000})
        call = "java -mx16000m -jar %s %s %s %s" % (options.mapperlib, conf, outConf, param)
        if options.verbose:
            print(call)
        sys.stdout.flush()
        subprocess.call(call, shell=True)


def traceMap(options, typedNets, radius=100):
    routes = collections.OrderedDict()
    for mode in sorted(typedNets.keys()):
        if options.verbose:
            print("mapping", mode)
        net = sumolib.net.readNet(os.path.join(options.network_split, mode + ".net.xml"))
        netBox = net.getBBoxXY()
        numTraces = 0
        filePath = os.path.join(options.fcd, mode + ".fcd.xml")
        if not os.path.exists(filePath):
            return []
        traces = tracemapper.readFCD(filePath, net, True)
        for tid, trace in traces:
            numTraces += 1
            minX, minY, maxX, maxY = sumolib.geomhelper.addToBoundingBox(trace)
            if (minX < netBox[1][0] + radius and minY < netBox[1][1] + radius and
                    maxX > netBox[0][0] - radius and maxY > netBox[0][1] - radius):
                mappedRoute = sumolib.route.mapTrace(trace, net, radius, fillGaps=options.fill_gaps)
                if mappedRoute:
                    routes[tid] = [e.getID() for e in mappedRoute]
        if options.verbose:
            print("mapped", numTraces, "traces to", len(routes), "routes.")
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


def map_stops(options, net, routes, rout, edgeMap):
    stops = collections.defaultdict(list)
    stopDef = set()
    rid = None
    for inp in sorted(glob.glob(os.path.join(options.fcd, "*.fcd.xml"))):
        mode = os.path.basename(inp)[:-8]
        typedNetFile = os.path.join(options.network_split, mode + ".net.xml")
        if not os.path.exists(typedNetFile):
            print("Warning! No net", typedNetFile, file=sys.stderr)
            continue
        if options.verbose:
            print("Reading", typedNetFile)
        typedNet = sumolib.net.readNet(typedNetFile)
        seen = set()
        fixed = {}
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
                    print("Warning! Not mapped", rid, file=sys.stderr)
                    seen.add(rid)
                continue
            if rid not in fixed:
                routeFixed = [routes[rid][0]]
                for routeEdgeID in routes[rid][1:]:
                    path, _ = typedNet.getShortestPath(typedNet.getEdge(routeFixed[-1]), typedNet.getEdge(routeEdgeID))
                    if path is None or len(path) > options.fill_gaps + 2:
                        error = "no path found" if path is None else "path too long (%s)" % len(path)
                        print("Warning! Disconnected route '%s' between '%s' and '%s', %s. Keeping longer part." %
                              (rid, edgeMap.get(routeFixed[-1]), edgeMap.get(routeEdgeID), error), file=sys.stderr)
                        if len(routeFixed) > len(routes[rid]) // 2:
                            break
                        routeFixed = [routeEdgeID]
                    else:
                        if len(path) > 2:
                            print("Warning! Fixed connection", rid, len(path), file=sys.stderr)
                        routeFixed += [e.getID() for e in path[1:]]
                if rid not in routes:
                    continue
                routes[rid] = routeFixed
                fixed[rid] = [edgeMap[e] for e in routeFixed]
            route = fixed[rid]
            if mode == "bus":
                stopLength = options.bus_stop_length
            elif mode == "tram":
                stopLength = options.tram_stop_length
            else:
                stopLength = options.train_stop_length
            result = gtfs2osm.getBestLane(net, veh.x, veh.y, 200, stopLength,
                                          route[lastIndex:], gtfs2osm.OSM2SUMO_MODES[mode], lastPos)
            if result is None:
                if options.warn_unmapped:
                    print("Warning! No stop for %s." % str(veh), file=sys.stderr)
                continue
            laneID, start, end = result
            edgeID = laneID.rsplit("_", 1)[0]
            lastIndex = route.index(edgeID, lastIndex)
            lastPos = end
            stop = "%s:%.2f" % (edgeID, end)
            if stop not in stopDef:
                stopDef.add(stop)
                access = gtfs2osm.getAccess(net, veh.x, veh.y, 100, laneID)
                if not access and not params:
                    addAttrs += "/"
                typ = "busStop" if mode == "bus" else "trainStop"
                rout.write(u'    <%s id="%s" lane="%s" startPos="%.2f" endPos="%.2f"%s>\n%s' %
                           (typ, stop, laneID, start, end, addAttrs, params))
                if access or params:
                    for a in sorted(access):
                        rout.write(a)
                    rout.write(u'    </%s>\n' % typ)
            stops[rid].append((stop, int(veh.until)))
    return stops


def filter_trips(options, routes, stops, outf, begin, end):
    numDays = end // 86400
    if end % 86400 != 0:
        numDays += 1
    if options.sort:
        vehs = collections.defaultdict(lambda: "")
    for inp in sorted(glob.glob(os.path.join(options.fcd, "*.rou.xml"))):
        for veh in sumolib.xml.parse_fast_structured(inp, "vehicle", ("id", "route", "type", "depart", "line"),
                                                     {"param": ["key", "value"]}):
            if len(routes.get(veh.route, [])) > 0 and len(stops.get(veh.route, [])) > 1:
                until = stops[veh.route][0][1]
                for d in range(numDays):
                    depart = max(0, d * 86400 + int(veh.depart) + until - options.duration)
                    if begin <= depart < end:
                        if d != 0 and veh.id.endswith(".trimmed"):
                            # only add trimmed trips the first day
                            continue
                        line = (u'    <vehicle id="%s.%s" route="%s" type="%s" depart="%s" line="%s">\n' %
                                (veh.id, d, veh.route, veh.type, depart, veh.line))
                        for p in veh.param:
                            line += u'        <param key="%s" value="%s"/>\n' % p
                        line += u'    </vehicle>\n'
                        if options.sort:
                            vehs[depart] += line
                        else:
                            outf.write(line)
    if options.sort:
        for _, vehs in sorted(vehs.items()):
            outf.write(vehs)


def main(options):
    if options.verbose:
        print('Loading net')
    net = sumolib.net.readNet(options.network)

    if not options.bbox:
        bboxXY = net.getBBoxXY()
        options.bbox = net.convertXY2LonLat(*bboxXY[0]) + net.convertXY2LonLat(*bboxXY[1])
    else:
        options.bbox = [float(coord) for coord in options.bbox.split(",")]
    if options.osm_routes:
        # Import PT from GTFS and OSM routes
        gtfsZip = zipfile.ZipFile(sumolib.openz(options.gtfs, mode="rb", tryGZip=False))
        routes, trips_on_day, shapes, stops, stop_times = gtfs2osm.import_gtfs(options, gtfsZip)

        if routes.empty or trips_on_day.empty:
            return
        if shapes is None:
            print('Warning: Importing OSM routes currently requires a GTFS file with shapes.', file=sys.stderr)
            options.osm_routes = None
        else:
            (gtfs_data, trip_list,
             filtered_stops,
             shapes, shapes_dict) = gtfs2osm.filter_gtfs(options, routes,
                                                         trips_on_day, shapes,
                                                         stops, stop_times)

            osm_routes = gtfs2osm.import_osm(options, net)

            (mapped_routes, mapped_stops,
             missing_stops, missing_lines) = gtfs2osm.map_gtfs_osm(options, net, osm_routes, gtfs_data, shapes,
                                                                   shapes_dict, filtered_stops)

            gtfs2osm.write_gtfs_osm_outputs(options, mapped_routes, mapped_stops,
                                            missing_stops, missing_lines,
                                            gtfs_data, trip_list, shapes_dict, net)
    if not options.osm_routes:
        # Import PT from GTFS
        if not options.skip_fcd:
            if not os.path.exists(options.mapperlib):
                options.gpsdat = None
            if not gtfs2fcd.main(options):
                return
        edgeMap, typedNets = splitNet(options)
        if os.path.exists(options.mapperlib):
            if not options.skip_map:
                mapFCD(options, typedNets)
            routes = collections.OrderedDict()
            for o in glob.glob(os.path.join(options.map_output, "*.dat")):
                for line in open(o):
                    time, edge, speed, coverage, id, minute_of_week = line.split('\t')[:6]
                    routes.setdefault(id, []).append(edge)
        else:
            if not gtfs2fcd.dataAvailable(options):
                print("Warning! No infrastructure for the given modes %s." % options.modes)
                return
            if options.mapperlib != "tracemapper":
                print("Warning! No mapping library found, falling back to tracemapper.", file=sys.stderr)
            routes = traceMap(options, typedNets)

        if options.poly_output:
            generate_polygons(net, routes, options.poly_output)
        with sumolib.openz(options.additional_output, mode='w') as aout:
            sumolib.xml.writeHeader(aout, os.path.basename(__file__), "additional", options=options)
            stops = map_stops(options, net, routes, aout, edgeMap)
            aout.write(u'</additional>\n')
        with sumolib.openz(options.route_output, mode='w') as rout:
            sumolib.xml.writeHeader(rout, os.path.basename(__file__), "routes", options=options)
            for vehID, edges in routes.items():
                if edges:
                    rout.write(u'    <route id="%s" edges="%s">\n' % (vehID, " ".join([edgeMap[e] for e in edges])))
                    offset = None
                    for stop in stops[vehID]:
                        if offset is None:
                            offset = stop[1]
                        rout.write(u'        <stop busStop="%s" duration="%s" until="%s"/>\n' %
                                   (stop[0], options.duration, stop[1] - offset))
                    rout.write(u'    </route>\n')
                else:
                    print("Warning! Empty route", vehID, file=sys.stderr)
            filter_trips(options, routes, stops, rout, options.begin, options.end)
            rout.write(u'</routes>\n')


if __name__ == "__main__":
    main(get_options())
