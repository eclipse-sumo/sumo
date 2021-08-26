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
import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse_fast_nested  # noqa

# ----------------------- gtfs, osm and sumo modes ----------------------------
OSM2SUMO_MODES = {
    'bus': 'bus',
    'train': 'rail',
    'tram': 'tram',
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
    '109':  'train',  # S-Bahn
    '400':  'subway',      # U-Bahn
    '1000': 'ferry',        # Faehre
    # additional modes used in Hamburg
    '402':  'subway',      # U-Bahn
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
    shapes = pd.read_csv(gtfsZip.open('shapes.txt'), dtype=str)
    calendar_dates = pd.read_csv(gtfsZip.open('calendar_dates.txt'), dtype=str)
    calendar = pd.read_csv(gtfsZip.open('calendar.txt'), dtype=str)

    stop_times['stop_sequence'] = stop_times['stop_sequence'].astype(float)

    # filter trips within given begin and end time
    # first adapt stop times to a single day (from 00:00:00 to 23:59:59)
    stop_times['arrival_timedelta'] = pd.to_timedelta(stop_times['arrival_time'])
    stop_times = stop_times.assign(arrival_fixed=[x - pd.to_timedelta("24:00:00")
                                   if x >= pd.to_timedelta("24:00:00")
                                   else x
                                   for x in stop_times['arrival_timedelta']])
    stop_times['departure_timedelta'] = pd.to_timedelta(stop_times['departure_time'])
    stop_times = stop_times.assign(departure_fixed=[x - pd.to_timedelta("24:00:00")
                                   if x >= pd.to_timedelta("24:00:00")
                                   else x
                                   for x in stop_times['departure_timedelta']])

    time_interval = options.end - options.begin
    # if time_interval >= 86400 (24 hs), no filter needed
    if time_interval < 86400 and options.end <= 86400:
        # if simulation time end on the same day
        start_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.begin)))
        end_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.end)))
        stop_times = stop_times[(start_time <= stop_times['departure_fixed']) &
                                (stop_times['departure_fixed'] <= end_time)]
    elif time_interval < 86400 and options.end > 86400:
        # if simulation time includes next day trips
        start_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.begin)))
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

    return routes, trips_on_day, shapes, stops, stop_times


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
    gtfs_data = pd.merge(pd.merge(pd.merge(trips_on_day, stop_times, on='trip_id'),  # noqa
                         stops, on='stop_id'), routes, on='route_id')

    # filter relevant information
    gtfs_data = gtfs_data[['route_id', 'shape_id', 'trip_id', 'stop_id',
                           'route_short_name', 'route_type', 'trip_headsign',
                           'direction_id', 'stop_name', 'stop_lat', 'stop_lon',
                           'stop_sequence', 'arrival_fixed', 'departure_fixed']]

    # replace characters
    gtfs_data['stop_name'] = gtfs_data['stop_name'].str.replace('[/|\'\";,!<>&*?\t\n\r]', ' ')  # noqa
    gtfs_data['trip_headsign'] = gtfs_data['trip_headsign'].str.replace('[/|\'\";,!<>&*?\t\n\r]', ' ')  # noqa

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
    trip_list = gtfs_data[gtfs_data["stop_sequence"] == 0]

    # add new column for unambiguous stop_id and edge in sumo
    gtfs_data["stop_item_id"] = None
    gtfs_data["edge_id"] = None

    # search main and secondary shapes for each pt line (route and direction)
    filtered_stops = gtfs_data.groupby(['route_id', 'direction_id', 'shape_id']
                                       ).agg({'stop_sequence': 'max'}).reset_index()  # noqa
    group_shapes = filtered_stops.groupby(['route_id', 'direction_id']
                                          ).shape_id.aggregate(lambda x: set(x)).reset_index()  # noqa
    filtered_stops = filtered_stops.loc[filtered_stops.groupby(['route_id', 'direction_id'])['stop_sequence'].idxmax()][[  # noqa
                                    'route_id', 'shape_id', 'direction_id']]
    filtered_stops = pd.merge(filtered_stops, group_shapes, on=['route_id', 'direction_id'])  # noqa

    # create dict with shapes and their main shape
    shapes_dict = {}
    for row in filtered_stops.itertuples():
        for sec_shape in row.shape_id_y:
            shapes_dict[sec_shape] = row.shape_id_x

    # create data frame with main shape for stop location
    filtered_stops = gtfs_data[gtfs_data['shape_id'].isin(filtered_stops.shape_id_x)]  # noqa
    filtered_stops = filtered_stops[['route_id', 'shape_id', 'stop_id',
                                     'route_short_name', 'route_type',
                                     'trip_headsign', 'direction_id',
                                     'stop_name', 'stop_lat', 'stop_lon']].drop_duplicates()  # noqa

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
    with open("dua_input.xml", 'w+', encoding="utf8") as dua_file:
        dua_file.write("<routes>\n")
        for key, value in OSM2SUMO_MODES.items():
            dua_file.write('\t<vType id="%s" vClass="%s"/>\n' % (key, value))

        sumo_edges = [sumo_edge.getID() for sumo_edge in net.getEdges()]
        for ptline, ptline_route in parse_fast_nested(options.osm_routes,
                                                      "ptLine", ("id", "name", "line", "type"),  # noqa
                                                      "route", "edges"):
            if ptline.type not in options.modes:
                continue

            route_edges = ptline_route.edges.split(" ")
            # search ptLine origin
            index = 0
            line_orig = route_edges[index]
            while line_orig not in sumo_edges and index+1 < len(route_edges):
                # search for first route edge included in the sumo network
                index += 1
                line_orig = route_edges[index]
            if line_orig not in sumo_edges:
                # if no edge found, discard ptLine
                continue
            # adapt osm route to sumo network
            route_edges = route_edges[index:]

            # search ptLine destination
            index = -1
            line_dest = route_edges[index]
            while line_dest not in sumo_edges and index-1 < -len(route_edges):
                # search for last route edge included in the sumo network
                index += -1
                line_orig = route_edges[index]
            if line_dest not in sumo_edges:
                # if no edges found, discard ptLine
                continue
            # adapt osm route to sumo network
            route_edges = route_edges[: index-1]

            # consider only edges in sumo network
            route_edges = [edge for edge in route_edges if edge in sumo_edges]
            if not route_edges:
                # if no edges found, discard ptLine
                continue

            # transform ptLine origin and destination to geo coordinates
            x, y = net.getEdge(line_orig).getFromNode().getCoord()
            line_orig = net.convertXY2LonLat(x, y)
            x, y = net.getEdge(line_dest).getFromNode().getCoord()
            line_dest = net.convertXY2LonLat(x, y)

            # find ptLine direction
            line_dir = get_line_dir(line_orig, line_dest)

            osm_routes[ptline.id] = (ptline.attr_name, ptline.line,
                                     ptline.type, line_dir)
            dua_file.write("""\t<trip id="%s" type="%s" depart="0" via="%s"/>\n""" %  # noqa
                           (ptline.id, ptline.type, (" ").join(route_edges)))
        dua_file.write("</routes>\n")

    # run duarouter
    run_dua = subprocess.call([sumolib.checkBinary('duarouter'),
                               '-n', options.network,
                               '--route-files', 'dua_input.xml', '--repair',
                               '-o', 'dua_output.xml', '--ignore-errors',
                               '--error-log', options.dua_repair_output])
    if run_dua == 1:
        # exit the program
        sys.exit("Traying to repair OSM routes failed. Duarouter quits with error, see %s" % options.dua_repair_output)  # noqa

    # parse repaired routes
    n_routes = len(osm_routes)

    for ptline, ptline_route in parse_fast_nested("dua_output.xml",
                                                  "vehicle", "id",
                                                  "route", "edges"):
        if len(ptline_route.edges) > 2:
            osm_routes[ptline.id] += (ptline_route.edges, )

    # remove dua files
    os.remove("dua_input.xml")
    os.remove("dua_output.xml")
    os.remove("dua_output.alt.xml")

    # remove invalid routes from dict
    [osm_routes.pop(line) for line in list(osm_routes)
     if len(osm_routes[line]) < 5]

    if n_routes != len(osm_routes):
        print("Not all given routes have been imported, see 'invalid_osm_routes.txt' for more information")  # noqa

    return osm_routes


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
                                                      "ptLine", ("id", "name", "line", "type"),  # noqa
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


def map_gtfs_osm(options, net, osm_routes, gtfs_data, shapes, shapes_dict, filtered_stops):  # noqa
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

    for row in filtered_stops.itertuples():
        # check if route already discarded
        if row.shape_id in missing_lines:
            continue

        # check if gtfs route already mapped to osm route
        if not map_routes.get(row.shape_id, False):
            # if route not mapped, find the osm route for shape id
            pt_line_name = row.route_short_name
            pt_type = GTFS2OSM_MODES[row.route_type]

            # get shape definition and define pt direction
            aux_shapes = shapes[shapes['shape_id'] == row.shape_id]
            pt_orig = aux_shapes[aux_shapes.shape_pt_sequence ==
                                 aux_shapes.shape_pt_sequence.min()]
            pt_dest = aux_shapes[aux_shapes.shape_pt_sequence ==
                                 aux_shapes.shape_pt_sequence.max()]
            line_dir = get_line_dir((pt_orig.shape_pt_lon,
                                     pt_orig.shape_pt_lat),
                                    (pt_dest.shape_pt_lon,
                                     pt_dest.shape_pt_lat))

            # get osm lines with same route name and pt type
            osm_lines = [ptline_id for ptline_id, value in osm_routes.items()
                         if value[1] == pt_line_name and value[2] == pt_type]
            if len(osm_lines) > 1:
                # get the direction for the found routes and take the route
                # with lower difference
                aux_dif = [abs(line_dir-osm_routes[ptline_id][3])
                           for ptline_id in osm_lines]
                osm_id = osm_lines[aux_dif.index(min(aux_dif))]

                # add mapped osm route to dict
                map_routes[row.shape_id] = (osm_id, osm_routes[osm_id][4].split(" "))  # noqa
            else:
                # no osm route found, do not map stops of route
                missing_lines.append((pt_line_name, row.trip_headsign))
                continue

        # check if stop already mapped
        stop_mapped = [stop_item_id for stop_item_id in map_stops.keys()
                       if stop_item_id.split("_")[0] == row.stop_id]
        stop_item_id = 0  # for pt stops with different stop points

        # set stop's type, class and length
        pt_type = GTFS2OSM_MODES[row.route_type]
        pt_class = OSM2SUMO_MODES[pt_type]
        if pt_class == "bus":
            stop_length = options.bus_stop_length
        elif pt_class == "tram":
            stop_length = options.tram_stop_length
        else:
            stop_length = options.train_stop_length

        if stop_mapped:
            # get maximum item for stop
            stop_item_id = [int(stop.split("_")[1]) for stop in stop_mapped]
            stop_item_id = max(stop_item_id) + 1

            # check if the stop is already define
            for stop in stop_mapped:
                # for item of mapped stop
                stop_edge = map_stops[stop][1].split("_")[0]
                if stop_edge in map_routes[row.shape_id][1]:
                    # if edge in route, the stops are the same
                    # add the shape id to the stop
                    map_stops[stop][5].append(row.shape_id)
                    # add to data frame
                    shape_list = [sec_shape for sec_shape, main_shape in shapes_dict.items()  # noqa
                                  if main_shape == row.shape_id]
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                                  (gtfs_data["shape_id"].isin(shape_list)),
                                  "stop_item_id"] = stop
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                                  (gtfs_data["shape_id"].isin(shape_list)),
                                  "edge_id"] = stop_edge

                    stop_mapped = True
                    break
                else:
                    # check if the wrong edge was adopted
                    # get edges near stop location
                    x, y = net.convertLonLat2XY(row.stop_lon, row.stop_lat)
                    edges = net.getNeighboringEdges(x, y, radius, includeJunctions=False)  # noqa
                    edges.sort(key=lambda x: x[1])  # sort by distance

                    # interseccion between route edges of all shapes in stop
                    edge_inter = set(map_routes[row.shape_id][1])
                    for shape_item in map_stops[stop][5]:  # shapes id of stop
                        edge_inter = set(edge_inter) & set(map_routes[shape_item][1])  # noqa

                    # find edge
                    new_edge = [edge[0] for edge in edges if edge[0].getID()
                                in edge_inter and edge[0].getLength()
                                >= stop_length*1.20]  # filter length
                    if not new_edge:
                        new_edge = [edge[0] for edge in edges
                                    if edge[0].getID() in edge_inter]
                    if not new_edge:
                        continue  # stops are not same

                    # if the edge is in all routes
                    for lane in new_edge[0].getLanes():
                        # update the lane id, start and end and add shape
                        if lane.allows(pt_class):
                            lane_id = lane.getID()
                            pos = int(lane.getClosestLanePosAndDist((x, y))[0])
                            start = max(0, pos-stop_length)
                            end = min(start+stop_length, lane.getLength())
                            map_stops[stop][1:4] = [lane_id, start, end]
                            map_stops[stop][5].append(row.shape_id)
                            break
                    # update edge in data frame
                    gtfs_data.loc[gtfs_data["stop_item_id"] == stop, "edge_id"] = new_edge[0].getID()  # noqa
                    # add to data frame
                    shape_list = [sec_shape for sec_shape, main_shape in shapes_dict.items()  # noqa
                                  if main_shape == row.shape_id]
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                                  (gtfs_data["shape_id"].isin(shape_list)),
                                  "stop_item_id"] = stop
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                                  (gtfs_data["shape_id"].isin(shape_list)),
                                  "edge_id"] = new_edge[0].getID()

                    stop_mapped = True
                    break

            if stop_mapped is not True:
                stop_mapped = None  # if stop not the same, search stop

        # if stop not mapped
        if not stop_mapped:
            # get edges near stop location
            x, y = net.convertLonLat2XY(row.stop_lon, row.stop_lat)
            edges = net.getNeighboringEdges(x, y, radius, includeJunctions=False)  # noqa
            # filter edges by length
            edges = [edge for edge in edges
                     if edge[0].getLength() >= stop_length*1.20]
            edges.sort(key=lambda x: x[1])  # sort by distance

            for edge in edges:
                if not edge[0].getID() in map_routes[row.shape_id][1]:
                    # if edge not in pt line route
                    continue

                for lane in edge[0].getLanes():
                    if not lane.allows(pt_class):
                        continue
                    lane_id = lane.getID()
                    pos = int(lane.getClosestLanePosAndDist((x, y))[0])
                    start = max(0, pos-stop_length)
                    end = min(start+stop_length, lane.getLength())
                    stop_item_id = "%s_%s" % (row.stop_id, stop_item_id)
                    map_stops[stop_item_id] = [row.stop_name, lane_id, start,
                                               end, pt_type, [row.shape_id]]
                    # add data to data frame
                    shape_list = [sec_shape for sec_shape, main_shape in shapes_dict.items()  # noqa
                                  if main_shape == row.shape_id]
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                                  (gtfs_data["shape_id"].isin(shape_list)),
                                  "stop_item_id"] = stop_item_id
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) &
                                  (gtfs_data["shape_id"].isin(shape_list)),
                                  "edge_id"] = edge[0].getID()

                    stop_mapped = True
                    break
                break

        # if stop not mapped, add to missing stops
        if not stop_mapped:
            missing_stops.append((row.stop_id, row.stop_name,
                                  row.route_short_name))

    return map_routes, map_stops, missing_stops, missing_lines


def write_gtfs_osm_outputs(options, map_routes, map_stops, missing_stops, missing_lines, gtfs_data, trip_list, shapes_dict, net):   # noqa
    """
    Generates stops and routes for sumo and saves the unmapped elements.
    """
    if options.verbose:
        print("Generates stops and routes output")

    with open(options.additional_output, 'w', encoding="utf8") as output_file:
        sumolib.xml.writeHeader(output_file, root="additional")
        for stop, value in map_stops.items():
            name, lane, start_pos, end_pos, v_type = value[:5]
            if v_type == "bus":
                output_file.write('    <busStop id="%s" lane="%s" startPos="%s" endPos="%s" name="%s" friendlyPos="true"/>\n' %  # noqa
                                  (stop, lane, start_pos, end_pos, name))
            else:
                # from gtfs2pt.py
                output_file.write('    <trainStop id="%s" lane="%s" startPos="%s" endPos="%s" name="%s" friendlyPos="true">\n' %  # noqa
                                  (stop, lane, start_pos, end_pos, name))

                ap = sumolib.geomhelper.positionAtShapeOffset(net.getLane(lane).getShape(), start_pos)  # noqa
                numAccess = 0
                for accessEdge, _ in sorted(net.getNeighboringEdges(*ap, r=100), key=lambda i: i[1]):  # noqa
                    if accessEdge.getID() != stop.split("_")[0] and accessEdge.allows("pedestrian"):  # noqa
                        lane_id = [lane.getID() for lane in accessEdge.getLanes() if lane.allows("pedestrian")][0]  # noqa
                        _, accessPos, accessDist = accessEdge.getClosestLanePosDist(ap)  # noqa
                        output_file.write(('        <access friendlyPos="true" lane="%s" pos="%s" length="%s"/>\n') %  # noqa
                                          (lane_id, int(accessPos), 1.5 * int(accessDist)))  # noqa
                        numAccess += 1
                        if numAccess == 5:
                            break
                output_file.write('    </trainStop>\n')
        output_file.write('</additional>\n')

    sequence_errors = []

    if options.vtype_output:
        with open(options.vtype_output, 'w', encoding="utf8") as vout:
            sumolib.xml.writeHeader(vout, root="additional")
            for osm_type, sumo_class in OSM2SUMO_MODES.items():
                if osm_type in options.modes:
                    vout.write('    <vType id="%s" vClass="%s"/>\n' %
                               (osm_type, sumo_class))
            vout.write(u'</additional>\n')

    with open(options.route_output, 'w', encoding="utf8") as output_file:
        sumolib.xml.writeHeader(output_file, root="routes")
        numDays = options.end // 86401
        start_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.begin)))

        for day in range(numDays+1):
            if day == numDays and options.end % 86400 > 0:
                # if last day, filter trips until given end time
                end_time = pd.to_timedelta(time.strftime('%H:%M:%S', time.gmtime(options.end-86400*numDays)))  # noqa
                trip_list = trip_list[trip_list["arrival_fixed"] <= end_time]

            seqs = {}
            for row in trip_list.sort_values("arrival_fixed").itertuples():

                if day == 0 and row.arrival_fixed < start_time:
                    # avoid writing first day trips that not applied
                    continue

                main_shape = shapes_dict.get(row.shape_id, None)
                if not map_routes.get(main_shape, None):
                    # if route not mapped
                    continue

                pt_type = GTFS2OSM_MODES[row.route_type]
                edges_list = map_routes[main_shape][1]
                stop_list = gtfs_data[gtfs_data["trip_id"] ==
                                      row.trip_id].sort_values("stop_sequence")
                stop_index = [edges_list.index(stop.edge_id)
                              for stop in stop_list.itertuples()
                              if stop.edge_id in edges_list]

                if len(set(stop_index)) < options.min_stops:
                    # Not enough stops mapped
                    continue
                stopSeq = tuple([stop.stop_item_id for stop in stop_list.itertuples()])
                if stopSeq not in seqs:
                    seqs[stopSeq] = row.trip_id
                veh_attr = (row.route_short_name, row.trip_id, day,
                            row.route_id, seqs[stopSeq],
                            day, str(row.arrival_fixed).split(' ')[2],
                            min(stop_index), max(stop_index), pt_type,
                            row.trip_headsign)
                output_file.write('    <vehicle id="%s_%s.%s" line="%s_%s" depart="%s:%s" departEdge="%s" arrivalEdge="%s" type="%s"><!--%s-->\n' % veh_attr)  # noqa
                output_file.write('        <route edges="%s"/>\n' % (" ".join(edges_list)))  # noqa

                check_seq = -1
                for stop in stop_list.itertuples():
                    if not stop.stop_item_id:
                        # if stop not mapped
                        continue
                    stop_index = edges_list.index(stop.edge_id)
                    if stop_index > check_seq:
                        check_seq = stop_index
                        stop_attr = (stop.stop_item_id, day,
                                     str(stop.arrival_fixed).split(' ')[2],
                                     options.duration, day,
                                     str(stop.departure_fixed).split(' ')[2],
                                     stop.stop_name)
                        output_file.write('        <stop busStop="%s" arrival="%s:%s" duration="%s" until="%s:%s"/><!--%s-->\n' % stop_attr)  # noqa
                    elif stop_index < check_seq:
                        # stop not downstream
                        sequence_errors.append((stop.stop_item_id,
                                                row.route_short_name,
                                                row.trip_headsign,
                                                stop.trip_id))

                output_file.write('    </vehicle>\n')
        output_file.write('</routes>\n')

    # -----------------------   Save missing data ------------------
    if any([missing_stops, missing_lines, sequence_errors]):
        print("Not all given gtfs elements have been mapped, see %s for more information" % options.warning_output)  # noqa
        with open(options.warning_output, 'w', encoding="utf8") as output_file:
            output_file.write('<missingElements>\n')
            for stop in sorted(set(missing_stops)):
                output_file.write('    <stop id="%s" name="%s" ptLine="%s"/>\n'
                                  % stop)
            for line in sorted(set(missing_lines)):
                output_file.write('    <ptLine id="%s" trip_headsign="%s"/>\n'
                                  % line)
            for stop in sorted(set(sequence_errors)):
                output_file.write(
                    '    <stopSequence stop_id="%s" ptLine="%s" trip_headsign="%s" trip_id="%s"/>\n' % stop)  # noqa
            output_file.write('</missingElements>\n')
