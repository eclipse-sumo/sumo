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

# @file    gtfs2osm.py
# @author  Giuliana Armellini
# @date    2021-02-18

"""
Import public transport from GTFS (schedules) and OSM (routes) data
"""

import os
import sys
import subprocess
import datetime
import time
import math
import io
from collections import defaultdict
# from pprint import pprint

import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse_fast_nested  # noqa
from sumolib.miscutils import benchmark  # noqa

# ----------------------- gtfs, osm and sumo modes ----------------------------
OSM2SUMO_MODES = {
    'bus': 'bus',
    'train': 'rail',
    'tram': 'tram',
    'light_rail': 'rail_urban',
    'subway': 'rail_urban',
    'ferry': 'ship'
}

GTFS2OSM_MODES = {
    # https://developers.google.com/transit/gtfs/reference/#routestxt
    '0':  'tram',
    '1':  'subway',
    '2':  'train',
    '3':  'bus',
    '4':  'ferry',
    # '5':  'cableTram',
    # '6':  'aerialLift',
    # '7':  'funicular',
    # https://developers.google.com/transit/gtfs/reference/extended-route-types
    '100':  'train',        # DB
    '109':  'light_rail',   # S-Bahn
    '400':  'subway',       # U-Bahn
    '1000': 'ferry',        # Faehre
    # additional modes used in Hamburg
    '402':  'subway',       # U-Bahn
    '1200': 'ferry',        # Faehre
    # modes used by hafas
    's': 'train',
    'RE': 'train',
    'RB': 'train',
    'IXB': 'train',        # tbd
    'ICE': 'train',
    'IC': 'train',
    'IRX': 'train',        # tbd
    'EC': 'train',
    'NJ': 'train',        # tbd
    'RHI': 'train',        # tbd
    'DPN': 'train',        # tbd
    'SCH': 'train',        # tbd
    'Bsv': 'train',        # tbd
    'KAT': 'train',        # tbd
    'AIR': 'train',        # tbd
    'DPS': 'train',        # tbd
    'lt': 'train',  # tbd
    'BUS': 'bus',        # tbd
    'Str': 'tram',        # tbd
    'DPF': 'train',        # tbd
}
# https://developers.google.com/transit/gtfs/reference/extended-route-types
for i in range(700, 717):
    GTFS2OSM_MODES[str(i)] = 'bus'
for i in range(900, 907):
    GTFS2OSM_MODES[str(i)] = 'tram'


@benchmark
def import_gtfs(options, gtfsZip):
    """
    Imports the gtfs-data and filters it by the specified date and modes.
    """
    if options.verbose:
        print('Loading GTFS data "%s"' % options.gtfs)

    routes = pd.read_csv(gtfsZip.open('routes.txt'), dtype=str)
    stops = pd.read_csv(gtfsZip.open('stops.txt'), dtype=str)
    stop_times = pd.read_csv(gtfsZip.open('stop_times.txt'), dtype=str)
    trips = pd.read_csv(gtfsZip.open('trips.txt'), dtype=str)
    shapes = pd.read_csv(gtfsZip.open('shapes.txt'), dtype=str) if 'shapes.txt' in gtfsZip.namelist() else None
    calendar_dates = pd.read_csv(gtfsZip.open('calendar_dates.txt'), dtype=str)
    calendar = pd.read_csv(gtfsZip.open('calendar.txt'), dtype=str)

    if 'trip_headsign' not in trips:
        trips['trip_headsign'] = ''
    if 'route_short_name' not in routes:
        routes['route_short_name'] = routes['route_long_name']

    # for some obscure reason there are GTFS files which have the sequence index as a float
    stop_times['stop_sequence'] = stop_times['stop_sequence'].astype(float, copy=False)

    # filter trips within given begin and end time
    # first adapt stop times to a single day (from 00:00:00 to 23:59:59)
    full_day = pd.to_timedelta("24:00:00")

    stop_times['arrival_fixed'] = pd.to_timedelta(stop_times.arrival_time)
    stop_times['departure_fixed'] = pd.to_timedelta(stop_times.departure_time)

    # avoid trimming trips starting before midnight but ending after
    fix_trips = stop_times[(stop_times['arrival_fixed'] >= full_day) &
                           (stop_times['stop_sequence'] == 0)].trip_id.values.tolist()

    stop_times.loc[stop_times.trip_id.isin(fix_trips), 'arrival_fixed'] = stop_times.loc[stop_times.trip_id.isin(
        fix_trips), 'arrival_fixed'] % full_day
    stop_times.loc[stop_times.trip_id.isin(fix_trips), 'departure_fixed'] = stop_times.loc[stop_times.trip_id.isin(
        fix_trips), 'departure_fixed'] % full_day

    extra_stop_times = stop_times.loc[stop_times.arrival_fixed > full_day, ]
    extra_stop_times.loc[:, 'arrival_fixed'] = extra_stop_times.loc[:, 'arrival_fixed'] % full_day
    extra_stop_times.loc[:, 'departure_fixed'] = extra_stop_times.loc[:, 'departure_fixed'] % full_day
    extra_trips_id = extra_stop_times.trip_id.values.tolist()
    extra_stop_times.loc[:, 'trip_id'] = extra_stop_times.loc[:, 'trip_id'] + ".trimmed"
    stop_times = pd.concat((stop_times, extra_stop_times))

    extra_trips = trips.loc[trips.trip_id.isin(extra_trips_id), :]
    extra_trips.loc[:, 'trip_id'] = extra_trips.loc[:, 'trip_id'] + ".trimmed"
    trips = pd.concat((trips, extra_trips))

    time_interval = options.end - options.begin
    start_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.begin)))

    # if time_interval >= 86400 (24 hs), no filter needed
    if time_interval < 86400 and options.end <= 86400:
        # if simulation time end on the same day
        end_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.end)))
        stop_times = stop_times[(start_time <= stop_times['departure_fixed']) &
                                (stop_times['departure_fixed'] <= end_time)]
    elif time_interval < 86400 and options.end > 86400:
        # if simulation time includes next day trips
        end_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.end - 86400)))
        stop_times = stop_times[~((stop_times['departure_fixed'] > end_time) &
                                  (stop_times['departure_fixed'] < start_time))]

    # filter trips for a representative date
    weekday = 'monday tuesday wednesday thursday friday saturday sunday'.split(
    )[datetime.datetime.strptime(options.date, "%Y%m%d").weekday()]
    removed = calendar_dates[(calendar_dates.date == options.date) &
                             (calendar_dates.exception_type == '2')]
    services = calendar[(calendar.start_date <= options.date) &
                        (calendar.end_date >= options.date) &
                        (calendar[weekday] == '1') &
                        (~calendar.service_id.isin(removed.service_id))]
    added = calendar_dates[(calendar_dates.date == options.date) &
                           (calendar_dates.exception_type == '1')]
    trips_on_day = trips[trips.service_id.isin(services.service_id) |
                         trips.service_id.isin(added.service_id)]

    # filter routes by modes
    filter_gtfs_modes = [key for key, value in GTFS2OSM_MODES.items()
                         if value in options.modes]
    routes = routes[routes['route_type'].isin(filter_gtfs_modes)]
    if routes.empty:
        print("Warning! No GTFS data found for the given modes %s." % options.modes)
    if trips_on_day.empty:
        print("Warning! No GTFS data found for the given date %s." % options.date)

    return routes, trips_on_day, shapes, stops, stop_times


@benchmark
def filter_gtfs(options, routes, trips_on_day, shapes, stops, stop_times):
    """
    Filters the gtfs-data by the given bounding box and searches the main
    shapes along with their secondary shapes. A main shape represents the trip
    with the longest distance of a public transport route. Only the paths (also
    referred to as routes) and stops of trips with main shapes will be mapped.
    Trips with secondary shapes will be defined by the start and end edge
    belonging to the main shape.
    """
    stops['stop_lat'] = stops['stop_lat'].astype(float)
    stops['stop_lon'] = stops['stop_lon'].astype(float)
    shapes['shape_pt_lat'] = shapes['shape_pt_lat'].astype(float)
    shapes['shape_pt_lon'] = shapes['shape_pt_lon'].astype(float)
    shapes['shape_pt_sequence'] = shapes['shape_pt_sequence'].astype(float)

    # merge gtfs data from stop_times / trips / routes / stops
    gtfs_data = pd.merge(pd.merge(pd.merge(trips_on_day, stop_times, on='trip_id'),
                         stops, on='stop_id'), routes, on='route_id')

    # filter relevant information
    gtfs_data = gtfs_data[['route_id', 'shape_id', 'trip_id', 'stop_id',
                           'route_short_name', 'route_type', 'trip_headsign',
                           'direction_id', 'stop_name', 'stop_lat', 'stop_lon',
                           'stop_sequence', 'arrival_fixed', 'departure_fixed']]

    # filter data inside SUMO net by stop location and shape
    gtfs_data = gtfs_data[(options.bbox[1] <= gtfs_data['stop_lat']) &
                          (gtfs_data['stop_lat'] <= options.bbox[3]) &
                          (options.bbox[0] <= gtfs_data['stop_lon']) &
                          (gtfs_data['stop_lon'] <= options.bbox[2])]
    shapes = shapes[(options.bbox[1] <= shapes['shape_pt_lat']) &
                    (shapes['shape_pt_lat'] <= options.bbox[3]) &
                    (options.bbox[0] <= shapes['shape_pt_lon']) &
                    (shapes['shape_pt_lon'] <= options.bbox[2])]

    # get list of trips with departure time to allow a sorted output
    trip_list = gtfs_data.loc[gtfs_data.groupby('trip_id').stop_sequence.idxmin()]

    # add new column for unambiguous stop_id and edge in sumo
    gtfs_data["stop_item_id"] = None
    gtfs_data["edge_id"] = None

    # search main and secondary shapes for each pt line (route and direction)
    filtered_stops = gtfs_data.groupby(['route_id', 'direction_id', 'shape_id']
                                       ).agg({'stop_sequence': 'max'}).reset_index()
    group_shapes = filtered_stops.groupby(['route_id', 'direction_id']).shape_id.aggregate(set).reset_index()
    filtered_stops = filtered_stops.loc[filtered_stops.groupby(['route_id', 'direction_id'])['stop_sequence'].idxmax()][[  # noqa
                                    'route_id', 'shape_id', 'direction_id']]
    filtered_stops = pd.merge(filtered_stops, group_shapes, on=['route_id', 'direction_id'])

    # create dict with shapes and their main shape
    shapes_dict = {}
    for row in filtered_stops.itertuples():
        for sec_shape in row.shape_id_y:
            shapes_dict[sec_shape] = row.shape_id_x

    # create data frame with main shape for stop location
    filtered_stops = gtfs_data[gtfs_data['shape_id'].isin(filtered_stops.shape_id_x)]
    filtered_stops = filtered_stops[['route_id', 'shape_id', 'stop_id',
                                     'route_short_name', 'route_type',
                                     'trip_headsign', 'direction_id',
                                     'stop_name', 'stop_lat', 'stop_lon']].drop_duplicates()

    return gtfs_data, trip_list, filtered_stops, shapes, shapes_dict


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
        for ptline, ptline_route in parse_fast_nested(options.osm_routes,
                                                      "ptLine", ("id", "name", "line", "type"),
                                                      "route", "edges"):
            num_read += 1
            if ptline.type not in options.modes:
                discard_type += 1
                continue

            route_edges = [edge for edge in ptline_route.edges.split() if edge in sumo_edges]
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

            osm_routes[ptline.id] = (ptline.attr_name, ptline.line, ptline.type, line_dir)
            dua_file.write(u'    <trip id="%s" type="%s" depart="0" via="%s"/>\n' %
                           (ptline.id, ptline.type, (" ").join(route_edges)))
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
    for ptline, ptline_route in parse_fast_nested("dua_output.xml", "vehicle", "id", "route", "edges"):
        osm_routes[ptline.id] += (ptline_route.edges, )

    # remove dua files
    os.remove("dua_input.xml")
    os.remove("dua_output.xml")
    os.remove("dua_output.alt.xml")

    # remove invalid routes from dict
    [osm_routes.pop(line) for line in list(osm_routes) if len(osm_routes[line]) < 5]

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
        for ptline, ptline_route in parse_fast_nested(options.osm_routes,
                                                      "ptLine", ("id", "name", "line", "type"),
                                                      "route", "edges"):
            if ptline.type not in options.modes:
                continue
            if len(ptline_route.edges) > 2:
                line_orig = ptline_route.edges.split(" ")[0]
                x, y = net.getEdge(line_orig).getFromNode().getCoord()
                line_orig = net.convertXY2LonLat(x, y)

                line_dest = ptline_route.edges.split(" ")[-1]
                x, y = net.getEdge(line_dest).getFromNode().getCoord()
                line_dest = net.convertXY2LonLat(x, y)

                line_dir = get_line_dir(line_orig, line_dest)

                osm_routes[ptline.id] = (ptline.attr_name, ptline.line,
                                         ptline.type, line_dir,
                                         ptline_route.edges)
    return osm_routes


def _addToDataFrame(gtfs_data, row, shapes_dict, stop, edge):
    shape_list = [sec_shape for sec_shape, main_shape in shapes_dict.items() if main_shape == row.shape_id]
    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                  (gtfs_data["shape_id"].isin(shape_list)),
                  "stop_item_id"] = stop
    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                  (gtfs_data["shape_id"].isin(shape_list)),
                  "edge_id"] = edge


def getBestLane(net, lon, lat, radius, stop_length, edge_set, pt_class, last_pos=-1):
    # get edges near stop location
    x, y = net.convertLonLat2XY(lon, lat)
    edges = [e for e in net.getNeighboringEdges(x, y, radius, includeJunctions=False) if e[0].getID() in edge_set]
    # sort by distance but have edges longer than stop length first
    for edge, _ in sorted(edges, key=lambda x: (x[0].getLength() <= stop_length, x[1])):
        for lane in edge.getLanes():
            if lane.allows(pt_class):
                pos = lane.getClosestLanePosAndDist((x, y))[0]
                if pos > last_pos or edge.getID() != edge_set[0]:
                    start = max(0, pos - stop_length)
                    end = min(start + stop_length, lane.getLength())
                    return lane.getID(), start, end
    return None


def getAccess(net, lon, lat, radius, lane_id, max_access=10):
    x, y = net.convertLonLat2XY(lon, lat)
    lane = net.getLane(lane_id)
    access = []
    if not lane.getEdge().allows("pedestrian"):
        for access_edge, _ in sorted(net.getNeighboringEdges(x, y, radius), key=lambda i: i[1]):
            if access_edge.allows("pedestrian"):
                access_lane_idx, access_pos, access_dist = access_edge.getClosestLanePosDist((x, y))
                if not access_edge.getLane(access_lane_idx).allows("pedestrian"):
                    for idx, lane in enumerate(access_edge.getLanes()):
                        if lane.allows("pedestrian"):
                            access_lane_idx = idx
                            break
                access.append((u'        <access friendlyPos="true" lane="%s_%s" pos="%.2f" length="%.2f"/>\n') %
                              (access_edge.getID(), access_lane_idx, access_pos, 1.5 * access_dist))
                if len(access) == max_access:
                    break
    return access


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

    for row in filtered_stops.itertuples():
        # check if gtfs route already mapped to osm route
        if row.shape_id not in map_routes:
            # if route not mapped, find the osm route for shape id
            pt_line_name = row.route_short_name
            pt_type = GTFS2OSM_MODES[row.route_type]

            # get shape definition and define pt direction
            aux_shapes = shapes[shapes['shape_id'] == row.shape_id]
            pt_orig = aux_shapes[aux_shapes.shape_pt_sequence == aux_shapes.shape_pt_sequence.min()]
            pt_dest = aux_shapes[aux_shapes.shape_pt_sequence == aux_shapes.shape_pt_sequence.max()]
            line_dir = get_line_dir((pt_orig.shape_pt_lon, pt_orig.shape_pt_lat),
                                    (pt_dest.shape_pt_lon, pt_dest.shape_pt_lat))

            # get osm lines with same route name and pt type
            osm_lines = [(abs(line_dir - value[3]), ptline_id, value[4])
                         for ptline_id, value in osm_routes.items()
                         if value[1] == pt_line_name and value[2] == pt_type]

            if osm_lines:
                # get the direction for the found routes and take the route
                # with lower difference
                diff, osm_id, edges = min(osm_lines, key=lambda x: x[0] if x[0] < 180 else 360 - x[0])
                # add mapped osm route to dict
                map_routes[row.shape_id] = (osm_id, edges.split())
            else:
                # no osm route found, do not map stops of route
                missing_lines.append((pt_line_name, sumolib.xml.quoteattr(str(row.trip_headsign), True)))
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
                best = getBestLane(net, row.stop_lon, row.stop_lat, radius, stop_length, edge_inter, pt_class)
                if best is None:
                    continue
                # update the lane id, start and end and add shape
                lane_id, start, end = best
                access = getAccess(net, row.stop_lon, row.stop_lat, 100, lane_id)
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
            best = getBestLane(net, row.stop_lon, row.stop_lat, radius, stop_length, edge_inter, pt_class)
            if best is not None:
                lane_id, start, end = best
                access = getAccess(net, row.stop_lon, row.stop_lat, 100, lane_id)
                stop_item_id = "%s_%s" % (row.stop_id, len(stop_items[row.stop_id]))
                stop_items[row.stop_id].append(stop_item_id)
                map_stops[stop_item_id] = [sumolib.xml.quoteattr(row.stop_name, True),
                                           lane_id, start, end, access, pt_type, edge_inter]
                _addToDataFrame(gtfs_data, row, shapes_dict, stop_item_id, lane_id.split("_")[0])
                stop_mapped = True

        # if stop not mapped, add to missing stops
        if not stop_mapped:
            missing_stops.append((row.stop_id, sumolib.xml.quoteattr(row.stop_name, True), row.route_short_name))
#    pprint(map_routes)
#    pprint(map_stops)
    return map_routes, map_stops, missing_stops, missing_lines


def write_vtypes(options, seen=None):
    if options.vtype_output:
        with sumolib.openz(options.vtype_output, mode='w') as vout:
            sumolib.xml.writeHeader(vout, root="additional")
            for osm_type, sumo_class in sorted(OSM2SUMO_MODES.items()):
                if osm_type in options.modes and (seen is None or osm_type in seen):
                    vout.write(u'    <vType id="%s" vClass="%s"/>\n' %
                               (osm_type, sumo_class))
            vout.write(u'</additional>\n')


def write_gtfs_osm_outputs(options, map_routes, map_stops, missing_stops, missing_lines,
                           gtfs_data, trip_list, shapes_dict, net):
    """
    Generates stops and routes for sumo and saves the unmapped elements.
    """
    if options.verbose:
        print("Generates stops and routes output")

    with sumolib.openz(options.additional_output, mode='w') as output_file:
        sumolib.xml.writeHeader(output_file, root="additional")
        for stop, value in map_stops.items():
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
        sumolib.xml.writeHeader(output_file, root="routes")
        numDays = options.end // 86401
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
                veh_attr = (row.trip_id, day,
                            main_shape, row.route_id, seqs[stopSeq],
                            row.arrival_fixed.days + day,
                            str(row.arrival_fixed).split(' ')[2],
                            min(stop_index), max(stop_index), pt_type)
                output_file.write(u'    <vehicle id="%s.%s" route="%s" line="%s_%s" depart="%s:%s" departEdge="%s" arrivalEdge="%s" type="%s">\n' % veh_attr)  # noqa
                output_file.write(u'        <param key="gtfs.route_name" value=%s/>\n' %
                                  sumolib.xml.quoteattr(str(row.route_short_name), True))
                if row.trip_headsign:
                    output_file.write(u'        <param key="gtfs.trip_headsign" value=%s/>\n' %
                                      sumolib.xml.quoteattr(str(row.trip_headsign), True))
                check_seq = -1
                for stop in stop_list.itertuples():
                    if not stop.stop_item_id:
                        # if stop not mapped
                        continue
                    stop_index = edges_list.index(stop.edge_id)
                    if stop_index >= check_seq:
                        check_seq = stop_index
                        # TODO check stop position if we are on the same edge as before
                        stop_attr = (stop.stop_item_id, stop.arrival_fixed.days + day,
                                     str(stop.arrival_fixed).split(' ')[2],
                                     options.duration, stop.departure_fixed.days + day,
                                     str(stop.departure_fixed).split(' ')[2],
                                     sumolib.xml.quoteattr(stop.stop_name, True))
                        output_file.write(u'        <stop busStop="%s" arrival="%s:%s" duration="%s" until="%s:%s"/><!--%s-->\n' % stop_attr)  # noqa
                    elif stop_index < check_seq:
                        # stop not downstream
                        sequence_errors.append((stop.stop_item_id,
                                                row.route_short_name,
                                                sumolib.xml.quoteattr(str(row.trip_headsign), True),
                                                stop.trip_id))

                output_file.write(u'    </vehicle>\n')
        output_file.write(u'</routes>\n')

    # -----------------------   Save missing data ------------------
    if any([missing_stops, missing_lines, sequence_errors]):
        print("Not all given gtfs elements have been mapped, see %s for more information" % options.warning_output)
        with io.open(options.warning_output, 'w', encoding="utf8") as output_file:
            output_file.write(u'<missingElements>\n')
            for stop in sorted(set(missing_stops)):
                output_file.write(u'    <stop id="%s" name=%s ptLine="%s"/>\n' % stop)
            for line in sorted(set(missing_lines)):
                output_file.write(u'    <ptLine id="%s" trip_headsign=%s/>\n' % line)
            for stop in sorted(set(sequence_errors)):
                output_file.write(u'    <stopSequence stop_id="%s" ptLine="%s" trip_headsign=%s trip_id="%s"/>\n' % stop)  # noqa
            output_file.write(u'</missingElements>\n')
