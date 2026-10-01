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

# @file    gtfs2osm.py
# @author  Giuliana Armellini
# @author  Mirko Barthauer
# @date    2021-02-18

"""
Helper functions for mapping GTFS data to OSM public transport lines (--osm-routes).
This is a legacy code path. Instead it is recommended to map GTFS to OSM stops with gtfs2pt.py option --stops
"""

import os
import sys
import subprocess
import datetime
import time
import math
import io
import re
from collections import defaultdict
import hashlib

# from pprint import pprint

import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse_fast_nested  # noqa
from sumolib.miscutils import benchmark, parseTime, humanReadableTime  # noqa
from gtfsutils import OSM2SUMO_MODES, GTFS2OSM_MODES, import_gtfs, getBestLane, getAccess, write_vtypes  # noqa


def get_line_dir(line_orig, line_dest):
    """
    Calculates the direction of the public transport line based on the start
    and end nodes of the osm route.
    """
    lat_dif = float(line_dest[1]) - float(line_orig[1])
    lon_dif = float(line_dest[0]) - float(line_orig[0])

    if lon_dif == 0:  # avoid dividing by 0
        line_dir = 90
    else:
        line_dir = math.degrees(math.atan(abs(lat_dif/lon_dif)))

    if lat_dif >= 0 and lon_dif >= 0:  # 1 quadrant
        line_dir = 90 - line_dir
    elif lat_dif < 0 and lon_dif > 0:  # 2 quadrant
        line_dir = 90 + line_dir
    elif lat_dif <= 0 and lon_dif <= 0:  # 3 quadrant
        line_dir = 90 - line_dir + 180
    else:  # 4 quadrant
        line_dir = 270 + line_dir

    return line_dir


def repair_routes(options, net):
    """
    Runs duarouter to repair the given osm routes.
    """
    osm_routes = {}
    # write dua input file
    with io.open("dua_input.xml", 'w+', encoding="utf8") as dua_file:
        dua_file.write(u"<routes>\n")
        for key, value in OSM2SUMO_MODES.items():
            dua_file.write(u'    <vType id="%s" vClass="%s"/>\n' % (key, value))
        num_read = discard_type = discard_net = 0
        sumo_edges = set([sumo_edge.getID() for sumo_edge in net.getEdges()])
        for ptLine in sumolib.xml.parse(options.osm_routes, "ptLine"):
            num_read += 1
            if ptLine.type not in options.modes:
                discard_type += 1
                continue

            if not ptLine.route:
                discard_net += 1
                continue
            route_edges = [edge for edge in ptLine.route[0].edges.split() if edge in sumo_edges]
            if not route_edges:
                discard_net += 1
                continue

            # transform ptLine origin and destination to geo coordinates
            x, y = net.getEdge(route_edges[0]).getFromNode().getCoord()
            line_orig = net.convertXY2LonLat(x, y)
            x, y = net.getEdge(route_edges[-1]).getFromNode().getCoord()
            line_dest = net.convertXY2LonLat(x, y)

            # find ptLine direction
            line_dir = get_line_dir(line_orig, line_dest)

            osm_routes[ptLine.id] = [ptLine.attr_name, ptLine.line, ptLine.type, line_dir, ptLine.color,
                                     None, [s.attr_name for s in (ptLine.stops or [])]]
            dua_file.write(u'    <trip id="%s" type="%s" depart="0" via="%s"/>\n' %
                           (ptLine.id, ptLine.type, (" ").join(route_edges)))
        dua_file.write(u"</routes>\n")

    if options.verbose:
        print("%s routes read, discarded for wrong mode: %s, outside of net %s, keeping %s" %
              (num_read, discard_type, discard_net, len(osm_routes)))
    # run duarouter
    subprocess.check_call([sumolib.checkBinary('duarouter'),
                           '-n', options.network,
                           '--route-files', 'dua_input.xml', '--repair',
                           '-o', 'dua_output.xml', '--ignore-errors',
                           '--error-log', options.dua_repair_output])

    # parse repaired routes
    n_routes = len(osm_routes)
    broken = set(osm_routes.keys())
    for ptline, ptline_route in parse_fast_nested("dua_output.xml", "vehicle", "id", "route", "edges"):
        osm_routes[ptline.id][5] = ptline_route.edges
        broken.remove(ptline.id)

    # remove dua files
    os.remove("dua_input.xml")
    os.remove("dua_output.xml")
    os.remove("dua_output.alt.xml")

    # remove invalid routes from dict
    [osm_routes.pop(line) for line in list(osm_routes) if line in broken]

    if n_routes != len(osm_routes):
        print("%s of %s routes have been imported, see '%s' for more information." %
              (len(osm_routes), n_routes, options.dua_repair_output))

    return osm_routes


@benchmark
def import_osm(options, net):
    """
    Imports the routes of the public transport lines from osm.
    """
    if options.repair:
        if options.verbose:
            print("Import and repair osm routes")
        osm_routes = repair_routes(options, net)
    else:
        if options.verbose:
            print("Import osm routes")
        osm_routes = {}
        for ptLine in sumolib.xml.parse(options.osm_routes, "ptLine"):
            if ptLine.type not in options.modes or not ptLine.route:
                continue
            route_edges = ptLine.route[0].edges.split()
            route_edges = [e for e in route_edges if net.hasEdge(e)]
            if route_edges:
                # TODO recheck what happens if it is only one edge
                x, y = net.getEdge(route_edges[0]).getFromNode().getCoord()
                line_orig = net.convertXY2LonLat(x, y)

                x, y = net.getEdge(route_edges[-1]).getFromNode().getCoord()
                line_dest = net.convertXY2LonLat(x, y)

                line_dir = get_line_dir(line_orig, line_dest)

                osm_routes[ptLine.id] = (ptLine.attr_name, ptLine.line,
                                         ptLine.type, line_dir, ptLine.color,
                                         ptLine.route[0].edges, [s.attr_name for s in (ptLine.stops or [])])
    return osm_routes


def _addToDataFrame(gtfs_data, row, shapes_dict, stop, edge):
    shape_list = [sec_shape for sec_shape, main_shape in shapes_dict.items() if main_shape == row.shape_id]
    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                  (gtfs_data["shape_id"].isin(shape_list)),
                  "stop_item_id"] = stop
    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                  (gtfs_data["shape_id"].isin(shape_list)),
                  "edge_id"] = edge


@benchmark
def map_gtfs_osm(options, net, osm_routes, gtfs_data, shapes, shapes_dict, filtered_stops):
    """
    Maps the routes from gtfs with the sumo routes imported from osm and maps
    the gtfs stops with the lane and position in sumo.
    """
    if options.verbose:
        print("Map stops and routes")

    map_routes = {}
    map_stops = {}
    # gtfs stops are grouped (not in exact geo position), so a large radius
    # for mapping is needed
    radius = 200

    missing_stops = []
    missing_lines = []
    stop_items = defaultdict(list)

    # get different permutations of stop names, and assign the collection of all stop names in route to the stop
    filtered_stops['stop_name'] = [[x] + re.split(r', | ,|,', x) + [x.replace(',', '')]
                                   for x in filtered_stops['stop_name']]
    filtered_shapes = filtered_stops.groupby(['shape_id', 'route_short_name',
                                              'route_type', 'direction_id']).stop_name.aggregate("sum").reset_index(
        name='stop_name_all')
    filtered_stops = pd.merge(filtered_stops, filtered_shapes)

    for row in filtered_stops.itertuples():
        # check if gtfs route already mapped to osm route
        if row.shape_id not in map_routes:
            # if route not mapped, find the osm route for shape id
            pt_line_name = row.route_short_name
            pt_type = GTFS2OSM_MODES[row.route_type]

            # get shape definition and define pt direction
            aux_shapes = shapes[shapes['shape_id'] == row.shape_id]
            if len(aux_shapes) == 0:
                print("Warning! Missing shape data for shape_id '%s'" % row.shape_id, file=sys.stderr)
                line_dir = 90
            else:
                pt_orig = aux_shapes[aux_shapes.shape_pt_sequence == aux_shapes.shape_pt_sequence.min()]
                pt_dest = aux_shapes[aux_shapes.shape_pt_sequence == aux_shapes.shape_pt_sequence.max()]
                line_dir = get_line_dir((pt_orig.shape_pt_lon.iloc[0], pt_orig.shape_pt_lat.iloc[0]),
                                        (pt_dest.shape_pt_lon.iloc[0], pt_dest.shape_pt_lat.iloc[0]))

            # get osm lines with same route name and pt type,
            # and if they have at least one matching stop name in osm and gtfs routes
            osm_lines = [(abs(line_dir - value[3]), ptline_id, value[4], value[5])
                         for ptline_id, value in osm_routes.items()
                         if value[1] == pt_line_name and value[2] == pt_type]
# if value[1] == pt_line_name and value[2] in OSM2OSM_MODES[pt_type] and set(value[6]) & set(row.stop_name_all)]
            if osm_lines:
                # get the direction for the found routes and take the route
                # with lower difference
                diff, osm_id, color, edges = min(osm_lines, key=lambda x: x[0] if x[0] < 180 else 360 - x[0])
                d = diff if diff < 180 else 360 - diff
                if d < 160:  # to prevent mapping to route going the opposite direction
                    # add mapped osm route to dict
                    map_routes[row.shape_id] = (osm_id, edges.split(), color)
                else:
                    missing_lines.append((row.route_id, pt_line_name,  sumolib.xml.quoteattr(
                        str(row.trip_headsign), True), row.direction_id))
                    continue
            else:
                # no osm route found, do not map stops of route
                missing_lines.append((row.route_id, pt_line_name,  sumolib.xml.quoteattr(
                    str(row.trip_headsign), True), row.direction_id))
                continue

        # set stop's type, class and length
        pt_type = GTFS2OSM_MODES[row.route_type]
        pt_class = OSM2SUMO_MODES[pt_type]
        if pt_class == "bus":
            stop_length = options.bus_stop_length
        elif pt_class == "tram":
            stop_length = options.tram_stop_length
        else:
            stop_length = options.train_stop_length

        stop_mapped = False
        for stop in stop_items[row.stop_id]:
            # for item of mapped stop
            stop_edge = map_stops[stop][1].rsplit("_", 1)[0]
            if stop_edge in map_routes[row.shape_id][1]:
                # if edge in route, the stops are the same
                # intersect the edge set
                map_stops[stop][6] = map_stops[stop][6] & set(map_routes[row.shape_id][1])
            else:
                # check if the wrong edge was adopted
                edge_inter = set(map_routes[row.shape_id][1]) & map_stops[stop][6]
                best = getBestLane(net, row.stop_lon, row.stop_lat, radius,
                                   stop_length, options.center_stops, edge_inter, pt_class)
                if best is None:
                    continue
                # update the lane id, start and end and add shape
                lane_id, start, end = best
                access = getAccess(net, row.stop_lon, row.stop_lat, options.access_radius, lane_id)
                map_stops[stop][1:7] = [lane_id, start, end, access, pt_type, edge_inter]
                # update edge in data frame
                stop_edge = lane_id.rsplit("_", 1)[0]
                gtfs_data.loc[gtfs_data["stop_item_id"] == stop, "edge_id"] = stop_edge
            # add to data frame
            _addToDataFrame(gtfs_data, row, shapes_dict, stop, stop_edge)
            stop_mapped = True
            break

        # if stop not mapped
        if not stop_mapped:
            edge_inter = set(map_routes[row.shape_id][1])
            best = getBestLane(net, row.stop_lon, row.stop_lat, radius,
                               stop_length, options.center_stops, edge_inter, pt_class)
            if best is not None:
                lane_id, start, end = best
                access = getAccess(net, row.stop_lon, row.stop_lat, options.access_radius, lane_id)
                stop_item_id = "%s_%s" % (row.stop_id, len(stop_items[row.stop_id]))
                stop_items[row.stop_id].append(stop_item_id)
                map_stops[stop_item_id] = [sumolib.xml.quoteattr(row.stop_name[0], True),
                                           lane_id, start, end, access, pt_type, edge_inter]
                _addToDataFrame(gtfs_data, row, shapes_dict, stop_item_id, lane_id.split("_")[0])
                stop_mapped = True

        # if stop not mapped, add to missing stops
        if not stop_mapped:
            missing_stops.append((row.stop_id, sumolib.xml.quoteattr(
                row.stop_name[0], True), row.route_short_name, row.direction_id))
#    pprint(map_routes)
#    pprint(map_stops)
    return map_routes, map_stops, missing_stops, missing_lines


def write_gtfs_osm_outputs(options, map_routes, map_stops, missing_stops, missing_lines,
                           gtfs_data, trip_list, shapes_dict, net):
    """
    Generates stops and routes for sumo and saves the unmapped elements.
    """
    if options.verbose:
        print("Generates stops and routes output")

    # determine if we need to format times (depart, duration, until) to be human readable or whole seconds
    ft = humanReadableTime if "hrtime" in options and options.hrtime else int

    with sumolib.openz(options.additional_output, mode='w') as output_file:
        sumolib.xml.writeHeader(output_file, root="additional", options=options)
        for stop, value in sorted(map_stops.items()):
            name, lane, start_pos, end_pos, access, v_type = value[:6]
            typ = "busStop" if v_type == "bus" else "trainStop"
            output_file.write(u'    <%s id="%s" lane="%s" startPos="%.2f" endPos="%.2f" name=%s friendlyPos="true"%s>\n' %  # noqa
                              (typ, stop, lane, start_pos, end_pos, name, "" if access else "/"))
            for a in access:
                output_file.write(a)
            if access:
                output_file.write(u'    </%s>\n' % typ)
        output_file.write(u'</additional>\n')

    sequence_errors = []
    write_vtypes(options)

    with sumolib.openz(options.route_output, mode='w') as output_file:
        sumolib.xml.writeHeader(output_file, root="routes", options=options)
        numDays = int(options.end) // 86401
        start_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.begin)))
        shapes_written = set()

        for day in range(numDays+1):
            if day == numDays and options.end % 86400 > 0:
                # if last day, filter trips until given end time
                end_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.end-86400*numDays)))
                trip_list = trip_list[trip_list["arrival_fixed"] <= end_time]

            seqs = {}
            for row in trip_list.sort_values("arrival_fixed").itertuples():

                if day != 0 and row.trip_id.endswith(".trimmed"):
                    # only add trimmed trips the first day
                    continue

                if day == 0 and row.arrival_fixed < start_time:
                    # avoid writing first day trips that not applied
                    continue

                main_shape = shapes_dict.get(row.shape_id)
                if main_shape not in map_routes:
                    # if route not mapped
                    continue
                pt_color = map_routes[main_shape][2]
                if pt_color is None:
                    pt_color = ""
                else:
                    pt_color = ' color="%s"' % pt_color
                pt_type = GTFS2OSM_MODES[row.route_type]
                edges_list = map_routes[main_shape][1]
                stop_list = gtfs_data[gtfs_data["trip_id"] == row.trip_id].sort_values("stop_sequence")
                stop_index = [edges_list.index(stop.edge_id)
                              for stop in stop_list.itertuples()
                              if stop.edge_id in edges_list]

                if len(stop_index) < options.min_stops:
                    # Not enough stops mapped
                    continue

                if main_shape not in shapes_written:
                    output_file.write(u'    <route id="%s" edges="%s"/>\n' % (main_shape, " ".join(edges_list)))
                    shapes_written.add(main_shape)

                stopSeq = tuple([stop.stop_item_id for stop in stop_list.itertuples()])
                if stopSeq not in seqs:
                    seqs[stopSeq] = row.trip_id

                # determine departure from first valid stop
                depart = None
                for stop in stop_list.itertuples():
                    if stop.stop_item_id:
                        depart = ft(parseTime(str(stop.arrival_fixed.days + day) +
                                    ":" + str(stop.arrival_fixed).split(' ')[2]))
                        break

                veh_attr = (row.trip_id, day,
                            main_shape, row.route_id, seqs[stopSeq], depart,
                            min(stop_index), max(stop_index), pt_type, pt_color)
                output_file.write(u'    <vehicle id="%s.%s" route="%s" line="%s_%s" depart="%s" departEdge="%s" arrivalEdge="%s" type="%s"%s>\n' % veh_attr)  # noqa
                params = [("gtfs.route_name", row.route_short_name)]
                if row.trip_headsign:
                    params.append(("gtfs.trip_headsign", row.trip_headsign))
                if options.writeTerminals:
                    firstStop = stop_list.iloc[0]
                    lastStop = stop_list.iloc[-1]
                    firstDepart = parseTime(str(firstStop.departure_fixed.days + day) +
                                            ":" + str(firstStop.departure_fixed).split(' ')[2])
                    lastArrival = parseTime(str(lastStop.arrival_fixed.days + day) +
                                            ":" + str(lastStop.arrival_fixed).split(' ')[2])
                    params += [("gtfs.origin_stop", firstStop.stop_name),
                               ("gtfs.origin_depart", ft(firstDepart)),
                               ("gtfs.destination_stop", lastStop.stop_name),
                               ("gtfs.destination_arrrival", ft(lastArrival))]
                for k, v in params:
                    output_file.write(u'        <param key="%s" value=%s/>\n' % (
                        k, sumolib.xml.quoteattr(str(v), True)))

                check_seq = -1
                for stop in stop_list.itertuples():
                    if not stop.stop_item_id:
                        # if stop not mapped
                        continue
                    stop_index = edges_list.index(stop.edge_id)
                    if stop_index >= check_seq:
                        check_seq = stop_index
                        # TODO check stop position if we are on the same edge as before
                        stop_attr = (stop.stop_item_id,
                                     ft(parseTime(str(stop.arrival_fixed.days + day) +
                                        ":" + str(stop.arrival_fixed).split(' ')[2])),
                                     ft(options.duration) if options.duration > 60 else options.duration,
                                     ft(parseTime(str(stop.departure_fixed.days + day) +
                                        ":" + str(stop.departure_fixed).split(' ')[2])),
                                     stop.stop_sequence, stop_list.stop_sequence.max(),
                                     sumolib.xml.quoteattr(stop.stop_name, True))
                        output_file.write(u'        <stop busStop="%s" arrival="%s" duration="%s" until="%s"/><!--stopSequence="%s/%s" %s-->\n' % stop_attr)  # noqa
                    elif stop_index < check_seq:
                        # stop not downstream
                        sequence_errors.append((stop.stop_item_id, sumolib.xml.quoteattr(stop.stop_name, True),
                                                row.route_short_name,
                                                sumolib.xml.quoteattr(str(row.trip_headsign), True), row.direction_id,
                                                stop.trip_id))

                output_file.write(u'    </vehicle>\n')
        output_file.write(u'</routes>\n')

    # -----------------------   Save missing data ------------------
    if any([missing_stops, missing_lines, sequence_errors]):
        print("Not all given gtfs elements have been mapped, see %s for more information" % options.warning_output)
        with io.open(options.warning_output, 'w', encoding="utf8") as output_file:
            sumolib.xml.writeHeader(output_file, root="missingElements", rootAttrs=None, options=options)
            for stop in sorted(set(missing_stops)):
                output_file.write(u'    <stop id="%s" name=%s ptLine="%s" direction_id="%s"/>\n' % stop)
            for line in sorted(set(missing_lines)):
                output_file.write(u'    <ptLine id="%s" name="%s" trip_headsign=%s direction_id="%s"/>\n' % line)
            for stop in sorted(set(sequence_errors)):
                output_file.write(u'    <stopSequence stop_id="%s" stop_name=%s ptLine="%s" trip_headsign=%s direction_id="%s" trip_id="%s"/>\n' % stop)  # noqa
            output_file.write(u'</missingElements>\n')
