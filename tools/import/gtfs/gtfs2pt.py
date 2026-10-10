#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
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
# @author  Robert Hilbrich
# @date    2018-08-28

"""
Maps GTFS data to a given network, generating routes, stops and vehicles
"""

from __future__ import print_function
from __future__ import absolute_import
from __future__ import division
import os
import sys
import random
import subprocess
import collections
import rtree
import pandas as pd
from collections import namedtuple
pd.options.mode.chained_assignment = None  # default='warn'

sys.path += [os.path.join(os.environ["SUMO_HOME"], "tools"),
             os.path.join(os.environ['SUMO_HOME'], 'tools', 'route')]
import route2poly  # noqa
import sumolib  # noqa
from sumolib.miscutils import euclidean, getBaseName, flattenPath  # noqa
from sumolib.miscutils import benchmark  # noqa

import gtfs2fcd  # noqa
import gtfs2osm  # noqa
import gtfsutils  # noqa
from gtfsutils import OSM2SUMO_MODES, GTFS2OSM_MODES  # noqa


def get_options(args=None):
    ap = gtfs2fcd.add_options()
    # ----------------------- general options ---------------------------------
    ap.add_argument("-n", "--network", category="input", required=True, type=ap.net_file,
                    help="sumo network to use")
    ap.add_argument("--route-output", category="output", type=ap.route_file,
                    help="file to write the generated public transport vehicles to")
    ap.add_argument("--additional-output", category="output", type=ap.additional_file,
                    help="file to write the generated public transport stops and routes to")
    ap.add_argument("--duration", default=10, category="input",
                    type=int, help="minimum time to wait on a stop")
    ap.add_argument("--write-arrival", action="store_true", default=False, dest="writeArrival",
                    help="write stop arrival times (estimated from duration if GTFS gives arrival_time equal to departure_time)")  # noqa
    ap.add_argument("--bus-parking", action="store_true", default=False, dest="busParking",
                    help="set parking to true for bus mode")
    ap.add_argument("--bus-stop-length", default=13, category="input", type=float,
                    help="length for a bus stop")
    ap.add_argument("--train-stop-length", default=110, category="input", type=float,
                    help="length for a train stop")
    ap.add_argument("--tram-stop-length", default=60, category="input", type=float,
                    help="length for a tram stop")
    ap.add_argument("--center-stops", action="store_true", default=False,
                    help="use stop position as center not as front")
    ap.add_argument("--overtake-right", action="store_true", default=False,
                    help="allow right lane overtaking at bus / train stops which are not at the outermost lane")
    ap.add_argument("--skip-access", action="store_true", default=False,
                    help="do not create access links")
    ap.add_argument("--access-radius", default=100, category="input", type=float,
                    help="maximum radius for finding pedestrian access")
    ap.add_argument("--sort", action="store_true", default=False,
                    help="sorting the output-file")
    ap.add_argument("--stops", category="input", type=ap.file_list,
                    help="files with candidate stops (selected by proxmity)")
    ap.add_argument("--patched-stops", category="input", dest="patchedStops", type=ap.file,
                    help="file with replacement stops (based on stop ids)")
    ap.add_argument("--rail-priority-factor", type=float, dest="rpFactor",
                    help="Take into account edge routingType values scaled by FLOAT (routingTypes must correspond to integers in [0,4])")  # noqa
    ap.add_argument("--radius", default=150, category="input", type=float,
                    help="maximum matching radius for candidate edges and stops")
    ap.add_argument("--distance-penalty", default=None, category="input", type=float, dest="distPenalty",
                    help=("Raise the distance between mapped location and input location to FLOAT power and add as penalty when comparing path costs." +  # noqa
                          "Defaults to '1' when setting option --stops and '2' otherwise."))
    ap.add_argument("--warn-detour-factor", default=5, type=float, dest="detourWarnFactor",
                    help="Warn about detours where path distance exceeds airline distance by factor FLOAT")
    ap.add_argument("--remove-detour-factor", default=0, type=float, dest="detourRemoveFactor",
                    help="Disable trips with implausible routes (path distance exceeds airline distance by factor FLOAT)")  # noqa
    ap.add_argument("--use-gtfs-stopids", action="store_true", default=False, category="input",
                    help="use stop identifiers from GTFS for readability")
    # ----------------------- fcd options -------------------------------------
    ap.add_argument("--network-split", category="input",
                    help="directory to write generated networks to")
    ap.add_argument("--network-split-vclass", action="store_true", default=True,
                    help="use the allowed vclass instead of the edge type to split the network (always active, option kept for backward compatibility")  # noqa
    ap.add_argument("--warn-unmapped", action="store_true", default=False,
                    help="warn about unmapped routes")
    ap.add_argument("--mapperlib", category="input",
                    help="mapping library to use (obsolete)")
    ap.add_argument("--map-output", category="output",
                    help="directory to write the generated mapping files to")
    ap.add_argument("--map-output-config", default="conf/output_configuration_template.xml", category="output",
                    type=ap.file, help="output configuration template for the mapper library")
    ap.add_argument("--map-input-config", default="conf/input_configuration_template.xml", category="input",
                    type=ap.file, help="input configuration template for the mapper library")
    ap.add_argument("--map-parameter", default="conf/parameters_template.xml", category="input", type=ap.file,
                    help="parameter template for the mapper library")
    ap.add_argument("--poly-output", category="output", type=ap.file,
                    help="file to write the generated polygon files to")
    ap.add_argument("--poi-output", category="output", type=ap.file, dest="poiOut",
                    help="file to write the input stop coordinates to")
    ap.add_argument("--parking-threshold", type=float, dest="parkingThreshold",
                    help="If set, trips with consecutive stops in the same spot for more than FLOAT seconds are parked")
    ap.add_argument("--fill-gaps", default=5000, type=float, category="input",
                    help="maximum distance between stops")
    ap.add_argument("--skip-fcd", action="store_true", default=False,
                    help="skip generating fcd data")
    ap.add_argument("--skip-map", action="store_true", default=False,
                    help="skip network mapping")

    # ----------------------- osm options -------------------------------------
    ap.add_argument("--osm-routes", category="input", type=ap.route_file, help="osm routes file")
    ap.add_argument("--warning-output", category="output", type=ap.file,
                    help="file to write the unmapped elements from gtfs")
    ap.add_argument("--dua-repair-output", category="output", type=ap.file,
                    help="file to write the osm routes with errors")
    ap.add_argument("--repair", help="repair osm routes", action='store_true', category="processing")
    ap.add_argument("--min-stops", default=1, type=int, category="input",
                    help="minimum number of stops a public transport line must have to be imported")
    ap.add_argument("--maxcache", default=1000, type=int,
                    help="Set maximum cache size for route computation")
    ap.add_argument("-s", "--seed", default=42, type=int,
                    help="random seed for coloring of pois and polygons")

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
    if options.detourRemoveFactor > 0 and options.detourRemoveFactor < options.detourWarnFactor:
        options.detourWarnFactor = options.detourRemoveFactor
    if options.distPenalty is None:
        options.distPenalty = 1 if options.stops else 2
    if options.sbahnLR:
        GTFS2OSM_MODES['109'] = 'light_rail'

    random.seed(options.seed)
    return options


def ensureNoInternal(options):
    if not sumolib.net.hasInternal(options.network):
        return options.network
    netcCall = [sumolib.checkBinary("netconvert"), "--no-internal-links", "--no-turnarounds",
                "--no-warnings",
                "--aggregate-warnings", "1",
                "--junctions.corner-detail", "0"]

    if not os.path.exists(options.network_split):
        os.makedirs(options.network_split)
    noIntNet = os.path.join(options.network_split, flattenPath(getBaseName(options.network)) + "_noInt.net.xml")
    if os.path.exists(noIntNet) and os.path.getmtime(noIntNet) > os.path.getmtime(options.network):
        print("Reusing old", noIntNet)
    else:
        subprocess.call(netcCall + ["-s", options.network, "-o", noIntNet])
    return noIntNet


@benchmark
def traceMap(options, net, veh2mode, fixedStops, stopLookup, radius, geoRoutes):
    if options.poiOut is not None:
        colorgen = sumolib.miscutils.Colorgen(('random', 1, 1))
        outf = open(options.poiOut, 'w')
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)

    routes = collections.OrderedDict()
    for mode in sorted(geoRoutes.keys()):
        vclass = OSM2SUMO_MODES.get(mode)
        if options.verbose:
            print("mapping", mode)
        mode_edges = set([e.getID() for e in net.getEdges() if e.allows(vclass)])
        netBox = net.getBBoxXY()
        numTraces = 0
        numRoutes = 0
        cacheHits = 0
        traceCache = {}
        preferences = {}
        if mode in ['train', 'light_rail', 'subway', 'tram'] and options.rpFactor is not None:
            for i in range(5):
                alpha = (4 - i) / 4
                preferences[str(i)] = alpha * 1 / (1 + options.rpFactor) + (1 - alpha)

        for tid, stops in geoRoutes[mode].items():
            trace = tuple(net.convertLonLat2XY(s.lon, s.lat) for s in stops)
            if options.poiOut is not None:
                for idx, pos in enumerate(trace):
                    outf.write('    <poi id="%s:%s" x="%.2f" y="%.2f" color="%s"/>\n' % (
                        tid, idx, pos[0], pos[1], colorgen()))
            numTraces += 1
            minX, minY, maxX, maxY = sumolib.geomhelper.addToBoundingBox(trace)
            if (minX < netBox[1][0] + radius and minY < netBox[1][1] + radius and
                    maxX > netBox[0][0] - radius and maxY > netBox[0][1] - radius):
                vias = {}
                if stopLookup.hasCandidates():
                    for idx, xy in enumerate(trace):
                        candidates = stopLookup.getCandidates(xy, options.radius)
                        if candidates:
                            all_edges = [sumolib._laneID2edgeID(stop.lane) for stop in candidates]
                            vias[idx] = [e for e in all_edges if e in mode_edges]
                for idx in range(len(trace)):
                    fixed = fixedStops.get("%s.%s" % (tid, idx))
                    if fixed:
                        vias[idx] = [sumolib._laneID2edgeID(fixed.lane)]
                if trace in traceCache:
                    mappedRoute, indices = traceCache[trace]
                    # use an indepedent copy in case the route gets repaired and indices updated later in map_stops
                    indices = indices[:]
                    cacheHits += 1
                else:
                    detours = []
                    indices = []
                    mappedRoute = sumolib.route.mapTrace(trace, net, radius, verbose=options.verbose,
                                                         fillGaps=options.fill_gaps, gapPenalty=5000.,
                                                         vClass=vclass, vias=vias,
                                                         fastest=True,
                                                         reversalPenalty=1000.,
                                                         resultDetours=detours,
                                                         preferences=preferences,
                                                         distPenalty=options.distPenalty,
                                                         resultIndices=indices)
                    assert len(detours) == len(trace)
                    assert len(indices) == len(trace)
                    for i in range(1, len(trace)):
                        detour = detours[i]
                        if detour > options.detourWarnFactor:
                            airLine = euclidean(trace[i - 1], trace[i])
                            fx, fy = trace[i - 1]
                            tx, ty = trace[i]
                            msgStart = "Trip"
                            if options.detourRemoveFactor > 0 and detour > options.detourRemoveFactor:
                                msgStart = "Removing trip"
                                mappedRoute = ()
                            print("%s %s (%s): detour (factor %.2f) to stop index %s, fromPos=%.2f,%.2f toPos=%.2f,%.2f (airLine=%.2f path=%.2f)" %  # noqa
                                  (msgStart, tid, mode, detour, i, fx, fy, tx, ty, airLine, detour * airLine), file=sys.stderr)  # noqa

                    traceCache[trace] = mappedRoute, indices

                if mappedRoute:
                    numRoutes += 1
                    routes[tid] = [e.getID() for e in mappedRoute], indices
                    veh2mode[tid] = mode
        if options.verbose:
            print("mapped %s traces to %s routes (%s cacheHits)" % (
                numTraces, numRoutes, cacheHits))

    if options.poiOut is not None:
        outf.write('</additional>\n')
        outf.close()
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
        for vehID, (edges, indices) in routes.items():
            route2poly.generate_poly(PolyOptions, net, vehID, colorgen(), edges, outf)
        outf.write('</polygons>\n')


def map_stops(options, net, routes, rout, fixedStops, stopLookup, geoRoutes):
    MappedStop = namedtuple("MappedStop", ["id", "arrival", "until", "name", "block", "isParking"])
    stops = collections.defaultdict(list)
    stopDesc = collections.defaultdict(list)  # laneID -> [(typ, id, start, end, stopName, childs)]
    stopID2Lane = dict()
    rid = None
    for mode in sorted(geoRoutes.keys()):
        vclass = OSM2SUMO_MODES.get(mode)
        seen = set()
        fixed = {}
        lastUntil = None
        lastStop = None
        for rid, locations in geoRoutes[mode].items():
            if rid not in routes:
                if options.warn_unmapped and rid not in seen:
                    print("Warning! Not mapped", rid, file=sys.stderr)
                    seen.add(rid)
                continue
            lastIndex = 0
            lastPos = -1
            for stopIndex, s in enumerate(locations):
                childs = []
                if s.fareZone:
                    childs += ['        <param key="fareZone" value="%s"/>\n' % s.fareZone,
                               '        <param key="fareSymbol" value="%s"/>\n' % s.fareSymbol,
                               '        <param key="startFare" value="%s"/>\n' % s.startFare]
                if rid not in fixed:
                    route, indices = routes[rid]
                    routeFixed = [route[0]]
                    startIndex = 0
                    i = 1
                    for routeEdgeID in route[1:]:
                        path, _ = net.getShortestPath(net.getEdge(routeFixed[-1]),
                                                                  net.getEdge(routeEdgeID),
                                                                  vClass=vclass)
                        if path is None or len(path) > options.fill_gaps + 2:
                            error = "no path found" if path is None else "path too long (%s)" % len(path)
                            print("Warning! Disconnected route '%s' between '%s' and '%s', %s. Keeping longer part." %
                                  (rid, routeFixed[-1], routeEdgeID, error), file=sys.stderr)
                            if len(routeFixed) > len(route) // 2:
                                break
                            routeFixed = [routeEdgeID]
                            startIndex = i
                        else:
                            added = len(path) - 2
                            if len(path) > 2:
                                print("Warning! Fixed route %s between %s and %s (added edges: %s)" % (
                                    rid, routeFixed[-1], routeEdgeID, len(path)),
                                    file=sys.stderr)
                                if added > 0:
                                    for j, index in enumerate(indices):
                                        if index is not None and index >= i:
                                            indices[j] += added
                                    i += added
                            routeFixed += [e.getID() for e in path[1:]]
                        i += 1
                    for j, index in enumerate(indices):
                        if index is not None:
                            newIndex = index - startIndex
                            if 0 <= newIndex < len(routeFixed):
                                indices[j] = newIndex
                            else:
                                indices[j] = None
                    routes[rid] = routeFixed, indices
                    fixed[rid] = routeFixed, indices
                route, indices = fixed[rid]
                if mode in ("bus", "trolleybus"):
                    stopLength = options.bus_stop_length
                elif mode == "tram":
                    stopLength = options.tram_stop_length
                else:
                    stopLength = options.train_stop_length
                if options.use_gtfs_stopids:
                    stop = "gtfs_%s" % s.gtfsid
                else:
                    # This is the original
                    stop = "%s.%s" % (rid, stopIndex)
                if stop in fixedStops:
                    fs = fixedStops[stop]
                    laneID, start, end = fs.lane, float(fs.startPos), float(fs.endPos)
                else:
                    result = None
                    candidate_edges = route[lastIndex:]
                    if stopIndex < len(indices):
                        if indices[stopIndex] is None:
                            candidate_edges = []
                        else:
                            candidate_edges = [route[indices[stopIndex]]]
                            e = net.getEdge(candidate_edges[0])
                            if indices[stopIndex] + 1 < len(route) and len(e.getOutgoing()) == 1:
                                # also accept the next downstream edge if it is the only follower
                                # this helps to prevent matching to very short junction edges
                                candidate_edges += [route[indices[stopIndex] + 1]]
                    if stopLookup.hasCandidates():
                        xy = net.convertLonLat2XY(s.lon, s.lat)
                        candidates = stopLookup.getCandidates(xy, options.radius)
                        if candidates:
                            on_route = [s for s in candidates if sumolib._laneID2edgeID(s.lane) in candidate_edges]
                            if on_route:
                                bestDist = 1e3 * options.radius
                                for stopObj in on_route:
                                    lane = net.getLane(stopObj.lane)
                                    if not lane.allows(vclass):
                                        for lane2 in lane.getEdge().getLanes():
                                            if lane2.allows(vclass):
                                                print("Warning! Fixed lane of loaded stop", stopObj.id, file=sys.stderr)
                                                lane = lane2
                                    if not lane.allows(vclass):
                                        print("Warning! Unable to fix lane of loaded stop", stopObj.id, file=sys.stderr)
                                        continue

                                    endPos = float(stopObj.endPos)
                                    if (stopIndex > 0
                                            and sumolib._laneID2edgeID(stopObj.lane) == route[lastIndex]
                                            and lane.interpretOffset(endPos) < lane.interpretOffset(lastPos)):
                                        continue
                                    dist = sumolib.geomhelper.distance(stopObj.center_xy, xy)
                                    if dist < bestDist:
                                        bestDist = dist
                                        result = (lane.getID(), float(stopObj.startPos), endPos)
                    if result is None and candidate_edges:
                        result = gtfsutils.getBestLane(net, s.lon, s.lat, options.radius, stopLength,
                                                       options.center_stops, candidate_edges, OSM2SUMO_MODES[mode],
                                                       (route[lastIndex], lastPos))
                        if options.warn_unmapped and result is not None and stopLookup.hasCandidates():
                            print("Warning! Adding stop at index %s that was not loaded for %s %s." % (
                                stopIndex, rid, s), file=sys.stderr)
                    if result is None:
                        if options.warn_unmapped:
                            print("Warning! No stop at index %s for %s %s." % (stopIndex, rid, s), file=sys.stderr)
                        continue
                    laneID, start, end = result
                edgeID = laneID.rsplit("_", 1)[0]
                lastIndex = route.index(edgeID, lastIndex)
                lastPos = end
                keep = True
                typ = "busStop" if mode in ("bus", "trolleybus") else "trainStop"
                isParking = options.busParking and mode == "bus"
                for stopItem in stopDesc[laneID]:
                    otherStop, otherStart, otherEnd = stopItem[1:4]
                    if start < otherEnd <= end or otherStart < end <= otherEnd:  # stops overlap
                        if end - start > otherEnd - otherStart:  # keep type and dimensions of the longer one
                            stopItem[0] = typ
                            stopItem[2] = start
                            stopItem[3] = end
                        keep = False
                        stop = otherStop
                        break
                if ((options.parkingThreshold is not None
                     and lastStop == stop
                     and s.until - lastUntil >= options.parkingThreshold)):
                    isParking = True
                if keep:
                    if stop in stopID2Lane:
                        oldID = stop
                        if '#' in stop and stop.rsplit('#')[-1].isdigit():
                            stop = "%s#%s" % (stop.rsplit('#')[0], int(stop.rsplit('#')[-1]) + 1)
                        else:
                            stop += "#1"
                        print("Warning: GTFS stop_id '%s' occurs on lane '%s' and '%s', assigning new id '%s'." % (
                            oldID, stopID2Lane[oldID], laneID, stop), file=sys.stderr)
                    if not options.skip_access:
                        childs += gtfsutils.getAccess(net, s.lon, s.lat, options.access_radius, laneID)
                    if not options.overtake_right:
                        lane = net.getLane(laneID)
                        idx = lane.getIndex()
                        edge = lane.getEdge()
                        if not all([edge.getLane(i).allows("pedestrian") for i in range(idx)]):
                            childs.append(u'        <param key="allowOvertakeRight" value="false"/>\n')
                    stopDesc[laneID].append([typ, stop, start, end, s.name, childs])
                    stopID2Lane[stop] = laneID
                stops[rid].append(MappedStop(stop, s.arrival, s.until, s.name, s.block, isParking))
                lastUntil = s.until
                lastStop = stop
    for laneID, stopList in stopDesc.items():
        for typ, stop, start, end, stopName, childs in stopList:
            rout.write(u'    <%s id="%s" lane="%s" startPos="%.2f" endPos="%.2f" friendlyPos="true" name="%s"%s>\n' %
                       (typ, stop, laneID, start, end, stopName, "" if childs else "/"))
            for a in sorted(childs):
                rout.write(a)
            if childs:
                rout.write(u'    </%s>\n' % typ)
    return stops


def filter_trips(options, routes, stops, outf, begin, end, vehicles):
    numDays = int(end) // 86400
    if end % 86400 != 0:
        numDays += 1
    vehDeparts = collections.defaultdict(lambda: [])
    for mode in sorted(vehicles.keys()):
        for veh in vehicles[mode]:
            tripID, routeID, orig_depart, line, params = veh
            if routeID in routes and len(routes[routeID][0]) > 0 and len(stops.get(routeID, [])) > 1:
                until = stops[routeID][0].until
                for d in range(numDays):
                    depart = max(0, d * 86400 + orig_depart + until - options.duration)
                    if begin <= depart < end:
                        if d != 0 and tripID.endswith(".trimmed"):
                            # only add trimmed trips the first day
                            continue
                        vehDeparts[depart].append((mode, "%s.%s" % (tripID, d), routeID, orig_depart, line, params))

    vehDeparts = list(vehDeparts.items())
    if options.sort:
        vehDeparts.sort()
    for depart, vehs in vehDeparts:
        for mode, tripID, routeID, orig_depart, line, params in vehs:
            outf.write(u'    <vehicle id="%s" route="%s" type="%s" depart="%s" line="%s">\n' %
                       (tripID, routeID, mode, options.ft(depart), line))
            for k, v in params:
                outf.write(u'        <param key="%s" value=%s/>\n' % (k, sumolib.xml.quoteattr(str(v), True)))
            outf.write(u'    </vehicle>\n')


class StopLookup:
    def __init__(self, fnames, net):
        self._candidates = []
        self._net = net
        self._rtree = rtree.index.Index()
        if fnames:
            for fname in fnames.split(','):
                self._candidates += list(sumolib.xml.parse(fname, ("busStop", "trainStop")))
            for ri, stop in enumerate(self._candidates):
                lane = net.getLane(stop.lane)
                middle = (lane.interpretOffset(float(stop.startPos)) +
                          lane.interpretOffset(float(stop.endPos))) / 2
                x, y = sumolib.geomhelper.positionAtShapeOffset(lane.getShape(), middle)
                bbox = (x - 1, y - 1, x + 1, y + 1)
                self._rtree.add(ri, bbox)
                stop.center_xy = (x, y)

    def hasCandidates(self):
        return len(self._candidates) > 0

    def getCandidates(self, xy, r=150):
        if self._candidates:
            stops = []
            x, y = xy
            for i in self._rtree.intersection((x - r, y - r, x + r, y + r)):
                stops.append(self._candidates[i])
            return stops
        else:
            return []


def removeDoubleHypen(string):
    while '--' in string:
        string = string.replace('--', '- -')
    return string


def main(options):
    legacy_osm_routes = options.osm_routes and not options.stops
    noIntNet = options.network if legacy_osm_routes else ensureNoInternal(options)

    if options.verbose:
        print('Loading net')
    orignet = sumolib.net.readNet(options.network, maxcache=options.maxcache)
    net = orignet if noIntNet == options.network else sumolib.net.readNet(noIntNet)

    if not options.bbox:
        bboxXY = net.getBBoxXY()
        options.bbox = net.convertXY2LonLat(*bboxXY[0]) + net.convertXY2LonLat(*bboxXY[1])
    else:
        options.bbox = [float(coord) for coord in options.bbox.split(",")]
    fixedStops = {}
    stopLookup = StopLookup(options.stops, net)
    if options.patchedStops:
        for stop in sumolib.xml.parse(options.patchedStops, ("busStop", "trainStop")):
            fixedStops[stop.id] = stop
    if legacy_osm_routes:
        # Import PT from GTFS and OSM routes
        gtfs2osm.process(options, orignet)
    else:
        veh2mode = {}
        full_data_merged = gtfsutils.loadGTFS(options)
        if full_data_merged.empty:
            print("Warning! GTFS data did not contain any trips with stops within the given bounding box area.",
                  file=sys.stderr)
            return
        vehicles, geoRoutes = gtfs2fcd.groupRoutes(options, full_data_merged)

        if not geoRoutes:
            print("Warning! No infrastructure for the given modes %s." % options.modes, file=sys.stderr)
            return
        gtfsutils.write_vtypes(options, sorted(vehicles.keys()))

        routes = traceMap(options, net, veh2mode, fixedStops, stopLookup, options.radius, geoRoutes)

        if options.poly_output:
            generate_polygons(net, routes, options.poly_output)
        with sumolib.openz(options.additional_output, mode='w') as aout:
            sumolib.xml.writeHeader(aout, os.path.basename(__file__), "additional", options=options)
            stops = map_stops(options, orignet, routes, aout, fixedStops, stopLookup, geoRoutes)
            aout.write(u'</additional>\n')
        with sumolib.openz(options.route_output, mode='w') as rout:
            sumolib.xml.writeHeader(rout, os.path.basename(__file__), "routes", options=options)
            for vehID, (edges, indices) in routes.items():
                if edges:
                    writeRoute(options, rout, vehID, edges, stops)
                else:
                    print("Warning! Empty route for %s." % vehID, file=sys.stderr)
            filter_trips(options, routes, stops, rout, options.begin, options.end, vehicles)
            rout.write(u'</routes>\n')


def writeRoute(options, rout, vehID, edges, stops):
    ft = options.ft
    rout.write(u'    <route id="%s" edges="%s">\n' % (vehID, " ".join(edges)))
    offset = None
    isJoined = False
    lastTripId = None
    if options.joinBlocks:
        # if multiple trip_id share the same block_id, this is treated by swapping trip_id and block_id, hence we can
        # read the changing trip_ids out of the block attribute
        blocks = set([s.block for s in stops[vehID]])
        isJoined = len(blocks) > 1
    for s in stops[vehID]:
        tripId = ' tripId="%s"' % s.block if isJoined and lastTripId != s.block else ''
        parking = ' parking="true"' if s.isParking else ""
        if offset is None:
            offset = s.until
        # ensure arrival preceeds until by at least duration
        arrival = 'arrival="%s" ' % ft(
            max(0, min(s.arrival - offset, s.until - options.duration))) if options.writeArrival else ""
        rout.write(u'        <stop busStop="%s" %sduration="%s" until="%s"%s%s/> <!-- %s -->\n' %
                   (s.id, arrival, ft(options.duration), ft(s.until - offset), parking, tripId,
                    removeDoubleHypen(s.name)))
    rout.write(u'    </route>\n')


if __name__ == "__main__":
    main(get_options())
