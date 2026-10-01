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

# @file    gtfsutils.py
# @author  Jakob Erdmann Armellini
# @date    2026-10-01

"""
Helper functions for gtfs2pt.py
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

import zipfile
import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse_fast_nested  # noqa
from sumolib.miscutils import benchmark, parseTime, humanReadableTime  # noqa

# ----------------------- gtfs, osm and sumo modes ----------------------------
OSM2SUMO_MODES = {
    'bus': 'bus',
    'train': 'rail',
    'tram': 'tram',
    'light_rail': 'rail_urban',
    'monorail': 'rail_urban',
    'subway': 'subway',
    'aerialway': 'cable_car',
    'ferry': 'ship',
    'trolleybus': 'bus'  # Assume trolleybus access to edges is the same as for bus vehicles
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
    '11': 'trolleybus',  # used in Pilsen
    '800': 'trolleybus',  # used in Prague, as per Extended GTFS Route Types
    # https://developers.google.com/transit/gtfs/reference/extended-route-types
    '100':  'train',        # DB
    '101':  'train',
    '102':  'train',
    '103':  'train',
    '106':  'train',
    '109':  'train',        # S-Bahn
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

def md5hash(s):
    return hashlib.md5(s.encode('utf-8')).hexdigest()


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
    if 'direction_id' not in trips:
        trips = discover_direction(routes, trips, stop_times)
    if 'route_short_name' not in routes:
        routes['route_short_name'] = routes['route_long_name']

    # for some obscure reason there are GTFS files which have the sequence index as a float
    stop_times['stop_sequence'] = stop_times['stop_sequence'].astype(float)

    # filter trips within given begin and end time
    # first adapt stop times to a single day (from 00:00:00 to 23:59:59)
    full_day = pd.to_timedelta("24:00:00")

    stop_times['arrival_fixed'] = pd.to_timedelta(stop_times.arrival_time)
    stop_times['departure_fixed'] = pd.to_timedelta(stop_times.departure_time)

    # avoid trimming trips starting before midnight but ending after
    fix_trips = stop_times[(stop_times['arrival_fixed'] >= full_day) &  # gg/ here i arrive at or after midnight
                           (stop_times['stop_sequence'] == stop_times['stop_sequence'].min())].trip_id.values.tolist()

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
    if 'block_id' in extra_trips.columns:
        extra_trips.loc[:, 'block_id'] = ""
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
    # We need to address difference between `trolleybus` and `bus`, therefore we will split
    # the comma-separated options.modes string into a tuple of strings denoting particular modes
    split_modes = options.modes.split(',')
    filter_gtfs_modes = [key for key, value in GTFS2OSM_MODES.items()
                         if value in split_modes]
    routes = routes[routes['route_type'].isin(filter_gtfs_modes)]
    if routes.empty:
        print("Warning! No GTFS data found for the given modes %s." % options.modes)
    if trips_on_day.empty:
        print("Warning! No GTFS data found for the given date %s." % options.date)

    return routes, trips_on_day, shapes, stops, stop_times


@benchmark
def discover_direction(routes, trips, stop_times):
    """
    Sets the direction value if it is not present in the GTFS data to identify separate
    directions of the same PT line.
    """
    # create a direction_id identifier from the stop sequence
    enhancedStopTimes = pd.merge(stop_times, pd.merge(trips, routes, on='route_id', how='left'), on='trip_id')
    groupedStopTimes = enhancedStopTimes.groupby(["trip_id"], as_index=False).agg({'stop_id': ' '.join})
    groupedStopTimes['direction_id'] = groupedStopTimes['stop_id'].apply(md5hash)
    # copy the direction_id back to the trips file / join the DataFrame
    return pd.merge(trips, groupedStopTimes[['trip_id', 'direction_id']], on='trip_id', how='left')


@benchmark
def filter_gtfs(options, routes, trips_on_day, shapes, stops, stop_times):
    """
    Filters the gtfs-data by the given bounding box.

    If using shapes, searches the main shapes of route. A main shape represents the
    trip that is most often taken in a given public transport route. Only the paths
    (also referred to as routes) and stops of trips with main shapes will be mapped.
    Trips with secondary shapes will be defined by the start and end edge belonging
    to the main shape (if they a part of the main shape).
    """
    stops['stop_lat'] = stops['stop_lat'].astype(float)
    stops['stop_lon'] = stops['stop_lon'].astype(float)

    if shapes is not None:
        shapes['shape_pt_lat'] = shapes['shape_pt_lat'].astype(float)
        shapes['shape_pt_lon'] = shapes['shape_pt_lon'].astype(float)
        shapes['shape_pt_sequence'] = shapes['shape_pt_sequence'].astype(float)

        shapes = shapes[(options.bbox[1] <= shapes['shape_pt_lat']) &
                        (shapes['shape_pt_lat'] <= options.bbox[3]) &
                        (options.bbox[0] <= shapes['shape_pt_lon']) &
                        (shapes['shape_pt_lon'] <= options.bbox[2])]

    # merge gtfs data from stop_times / trips / routes / stops
    gtfs_data = pd.merge(pd.merge(pd.merge(trips_on_day, stop_times, on='trip_id'),
                         stops, on='stop_id'), routes, on='route_id')
    if shapes is None:
        gtfs_data['shape_id'] = gtfs_data['route_id'] + "_" + gtfs_data['direction_id']

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

    # get list of trips with departure time to allow a sorted output
    trip_list = gtfs_data.loc[gtfs_data.groupby('trip_id').stop_sequence.idxmin()]

    # add new column for unambiguous stop_id and edge in sumo
    gtfs_data["stop_item_id"] = None
    gtfs_data["edge_id"] = None
    # create dict with shapes and their main shape
    shapes_dict = {}

    if shapes is not None:
        # search main and secondary shapes for each pt line (route and direction)
        filtered_stops = gtfs_data.groupby(['route_id', 'direction_id', 'shape_id'])[
            "shape_id"].size().reset_index(name='counts')
        group_shapes = filtered_stops.groupby(['route_id', 'direction_id']).shape_id.aggregate(set).reset_index()

        filtered_stops = filtered_stops.loc[filtered_stops.groupby(['route_id', 'direction_id'])['counts'].idxmax()][[  # noqa
                                            'route_id', 'shape_id', 'direction_id']]
        filtered_stops = pd.merge(filtered_stops, group_shapes, on=['route_id', 'direction_id'])

        for row in filtered_stops.itertuples():
            for sec_shape in row.shape_id_y:
                shapes_dict[sec_shape] = row.shape_id_x

        # create data frame with main shape for stop location
        filtered_stops = gtfs_data[gtfs_data['shape_id'].isin(filtered_stops.shape_id_x)]
        filtered_stops = filtered_stops[['route_id', 'shape_id', 'stop_id',
                                        'route_short_name', 'route_type',
                                         'trip_headsign', 'direction_id',
                                         'stop_name', 'stop_lat', 'stop_lon']].drop_duplicates()
    else:
        # If not using shapes, searches for the most common sequence of stops in a route.
        # Only the paths and stops of these main sequences are mapped. Creates 'shapes' with
        # the coordinates of first and last stop in main route sequences, used for mapping later.

        # create a new stop id with their trip sequence
        gtfs_data['new_stop_id'] = gtfs_data['stop_sequence'].astype(str) + '_' + gtfs_data['stop_id']

        # for a given trip, put the stops into a list and then into a string
        group_stops = gtfs_data.groupby(['trip_id', 'shape_id']).new_stop_id.aggregate(list).reset_index()
        group_stops['new_stop_id'] = group_stops['new_stop_id'].str.join(' ')

        # for a given shape (route and direction),
        # count the number of times the particular stop sequence (sequence and stop_id) is used
        group_size = group_stops.groupby(['shape_id', 'new_stop_id']).new_stop_id.size().reset_index(name='counts')

        # get one main route (most common sequence of stops) for each shape
        group_routes = group_size.loc[group_size.groupby(['shape_id']).counts.idxmax()]

        # split string of stops into list again
        group_routes['new_stop_id'] = group_routes['new_stop_id'].str.split(' ')

        # get all stops in all the main routes
        routes_stops = group_routes.explode('new_stop_id', ignore_index=True)
        routes_stops[['stop_sequence', 'stop_id']] = routes_stops.new_stop_id.str.split('_', expand=True)
        routes_stops['stop_sequence'] = routes_stops['stop_sequence'].astype(float)

        stop_indexes = []
        # loop through all unique shapes and collect the first and last stop in sequence
        for shape in routes_stops['shape_id'].unique():
            first_stop_index = routes_stops.loc[routes_stops['shape_id'] == shape, 'stop_sequence'].idxmin()
            last_stop_index = routes_stops.loc[routes_stops['shape_id'] == shape, 'stop_sequence'].idxmax()
            stop_indexes.append(first_stop_index)
            stop_indexes.append(last_stop_index)

        # drop indexes that have duplicates (i.e. first and last stop are the same)
        end_stops_index = [x for x in stop_indexes if stop_indexes.count(x) == 1]

        # create new 'shapes' file with the coordinates of first and last stop
        stop_info = gtfs_data[['shape_id', 'stop_id', 'stop_lat', 'stop_lon']].drop_duplicates()
        stop_shape = routes_stops.loc[end_stops_index, ['shape_id', 'stop_id', 'stop_sequence']]
        shapes = pd.merge(stop_shape, stop_info, on=['shape_id', 'stop_id'])
        shapes = shapes.rename(columns={"stop_sequence": "shape_pt_sequence",
                               "stop_lon": "shape_pt_lon", "stop_lat": "shape_pt_lat"})

        # all stops of main routes, dropped routes with only 1 stop
        routes_stops = routes_stops.loc[routes_stops['shape_id'].isin(shapes['shape_id'])]

        # shapes dictionary is just the shape id in both columns
        for shape in shapes['shape_id'].unique():
            shapes_dict[shape] = shape

        # all stops of main routes, with other infos.
        # stop_sequence is used in merge because some stops are repeated twice in a route
        filtered_stops = pd.merge(routes_stops, gtfs_data,
                                  on=['shape_id', 'stop_id', 'stop_sequence'],
                                  how='left')[['route_id', 'stop_id', 'shape_id',
                                               'route_short_name', 'route_type', 'trip_headsign', 'direction_id',
                                               'stop_name', 'stop_lat', 'stop_lon', 'stop_sequence']
                                              ].drop_duplicates(['shape_id', 'stop_id', 'stop_sequence'])

    return gtfs_data, trip_list, filtered_stops, shapes, shapes_dict


def write_vtypes(options, seen=None):
    if options.vtype_output:
        with sumolib.openz(options.vtype_output, mode='w') as vout:
            sumolib.xml.writeHeader(vout, root="additional", options=options)
            for osm_type, sumo_class in sorted(OSM2SUMO_MODES.items()):
                if osm_type in options.modes and (seen is None or osm_type in seen):
                    vout.write(u'    <vType id="%s" vClass="%s"/>\n' %
                               (osm_type, sumo_class))
            vout.write(u'</additional>\n')

def time2sec(s):
    t = s.split(":")
    return int(t[0]) * 3600 + int(t[1]) * 60 + int(t[2])


def joinBlocks(data):
    """For trips that have the same non-empty block_id:
       - sort trips by first depart
       - swap trip_id and block_id (old trip_id can be written as stop attribute tripId)
       - concatenate route_ids to form a descriptive route id for the joined trips
       - renumber stop_sequence
    """
    blocks = []
    for block_id, block in data.groupby('block_id', dropna=False):
        if not pd.isna(block_id) and block_id != "":
            departs = block.groupby('trip_id')['departure_time'].min().rename('trip_departure_time')
            if len(departs) > 1:
                block = block.join(departs, on='trip_id')
                block.sort_values(by=['trip_departure_time', 'stop_sequence'], inplace=True)
                block.reset_index(drop=True, inplace=True)
                block['stop_sequence'] = block.index
                # block.to_csv('debug_%s.csv' % block_id, sep=";", index=False)
                del block['trip_departure_time']
                # swap columns so later code will treat the block like a single trip (but preserve the original trip_id)
                block[['trip_id', 'block_id']] = block[['block_id', 'trip_id']].values
                # concatenate route_ids if they differ within the block
                block['route_id'] = "_".join(block['route_id'].unique())
        blocks.append(block)
    return pd.concat(blocks)


def get_merged_data(options):
    gtfsZip = zipfile.ZipFile(sumolib.openz(options.gtfs, mode="rb", tryGZip=False, printErrors=True))
    routes, trips_on_day, shapes, stops, stop_times = import_gtfs(options, gtfsZip)
    gtfsZip.fp.close()

    if options.bbox:
        stops['stop_lat'] = stops['stop_lat'].astype(float)
        stops['stop_lon'] = stops['stop_lon'].astype(float)
        stops = stops[(options.bbox[1] <= stops['stop_lat']) & (stops['stop_lat'] <= options.bbox[3]) &
                      (options.bbox[0] <= stops['stop_lon']) & (stops['stop_lon'] <= options.bbox[2])]
    stop_times['arrival_time'] = stop_times['arrival_time'].map(time2sec)
    stop_times['departure_time'] = stop_times['departure_time'].map(time2sec)

    if 'fare_stops.txt' in gtfsZip.namelist():
        zones = pd.read_csv(gtfsZip.open('fare_stops.txt'), dtype=str)
        stops_merged = pd.merge(pd.merge(stops, stop_times, on='stop_id'), zones, on='stop_id')
    else:
        stops_merged = pd.merge(stops, stop_times, on='stop_id')
        stops_merged['fare_zone'] = ''
        stops_merged['fare_token'] = ''
        stops_merged['start_char'] = ''

    trips_routes_merged = pd.merge(trips_on_day, routes, on='route_id')
    merged = pd.merge(stops_merged, trips_routes_merged, on='trip_id').drop_duplicates()
    cols = ['trip_id', 'block_id', 'route_id', 'route_short_name', 'route_type',
            'stop_id', 'stop_name', 'stop_lat', 'stop_lon', 'stop_sequence',
            'fare_zone', 'fare_token', 'start_char', 'trip_headsign',
            'arrival_time', 'departure_time']
    # 'block_id' is optional
    if 'block_id' not in merged.columns:
        cols.remove('block_id')
        options.joinBlocks = False
    merged = merged[cols]
    return merged


def getBestLane(net, lon, lat, radius, stop_length, center, edge_set, pt_class, last_pos=None):
    # get edges near stop location
    x, y = net.convertLonLat2XY(lon, lat)
    edges = [e for e in net.getNeighboringEdges(x, y, radius, includeJunctions=False) if e[0].getID() in edge_set]
    # sort by distance but have edges longer than stop length first
    # TODO we should rather go for maximum overlap but it is unclear how to weight this against distance
    for edge, _ in sorted(edges, key=lambda x: (x[0].getLength() <= stop_length, x[1])):
        for lane in edge.getLanes():
            if lane.allows(pt_class):
                pos = lane.getClosestLanePosAndDist((x, y))[0]
                start = max(0, pos - (stop_length / 2. if center else stop_length))
                end = min(start + stop_length, lane.getLength())
                if last_pos is None or end >= last_pos[1] or edge.getID() != last_pos[0]:
                    return lane.getID(), start, end
    return None


def getAccess(net, lon, lat, radius, lane_id, max_access=10):
    x, y = net.convertLonLat2XY(lon, lat)
    lane = net.getLane(lane_id)
    access = []
    if not lane.getEdge().allows("pedestrian"):
        for access_edge, _ in sorted(net.getNeighboringEdges(x, y, radius), key=lambda i: (i[1], i[0].getID())):
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
