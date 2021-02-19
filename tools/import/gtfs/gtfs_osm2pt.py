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

# @file    gtfs_osm2pt.py
# @author  Giuliana Armellini
# @date    2021-02-18

"""
Import public transport from GTFS (schedules) and OSM (routes) data
"""

import os
import sys
import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'
import numpy as np
import zipfile
import subprocess
import time
import math
from argparse import ArgumentParser

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib
import pyproj
import rtree

def initOptions():
    argParser = ArgumentParser()
    argParser.add_argument("-n", "--network", help="SUMO network file", required=True)
    argParser.add_argument("--osm-routes", help="osm routes file", required=True)
    argParser.add_argument("--gtfs", help="define gtfs zip file to load (mandatory)", required=True)
    argParser.add_argument("--region", help="define the region to filter gtfs data, format: W,S,E,N", required=True)
    argParser.add_argument("--date", help="define the day to import, format: 'YYYYMMDD'") #date = "20200521"
    argParser.add_argument("--repair", help="repair osm routes", action='store_true')
    argParser.add_argument("--debug", action='store_true')
    argParser.add_argument("--bus-stop-length", default=13, type=float, help="length for a bus stop")
    argParser.add_argument("--train-stop-length", default=110, type=float, help="length for a train stop")
    argParser.add_argument("--tram-stop-length", default=60, type=float, help="length for a tram stop")
    argParser.add_argument("--duration", default=10, type=int, help="minimum time to wait on a stop")
    return argParser

def get_line_dir(line_orig, line_dest):
    # get direction of the route
    
    lat_dif = float(line_dest[1]) - float(line_orig[1])
    lon_dif = float(line_dest[0]) - float(line_orig[0])

    if lon_dif == 0: # avoid dividing by 0
        line_dir = 90
    else:
        line_dir = math.degrees(math.atan(abs(lat_dif/lon_dif)))

    if lat_dif >= 0 and lon_dif >= 0: # 1° quadrant
        line_dir = 90 - line_dir
    elif lat_dif < 0 and lon_dif > 0: # 2° quadrant
        line_dir = 90 + line_dir
    elif lat_dif <= 0 and lon_dif <= 0: # 3° quadrant
        line_dir = 90 - line_dir + 180
    else: # 4° quadrant
        line_dir = 270 + line_dir
    
    return line_dir

def repair_routes(options):
    # use duarouter to repair the given osm routes #TODO

    dua_input = "dua_input.xml"
    dua_output = "dua_output.xml"
    dua_error = "dua_error.xml"
    
    # write dua input file
    count = 0
    with open(dua_input, 'w+', encoding="utf8") as dua_file:
        dua_file.write("<routes>\n")
        for key, value in sumo_vClass.items():
            dua_file.write('\t<vType id="%s" vClass="%s"/>\n' % (key, value))

        for ptline, ptline_route in sumolib.xml.parse_fast_nested(options.osm_routes, "ptLine", ("id", "name", "line", "type"), "route", ("edges")):        
            count += 1
            #if not any([ptline.type == "bus", ptline.type == "tram"]): #TODO enable pt_type filter
            #    continue
            if osm_routes.get(ptline.id, False):
                continue
            
            route_edges = ptline_route.edges.split(" ")
            # search ptLine origin
            index = 0
            line_orig = route_edges[index]
            while not line_orig in net.getEdges() and index+1 < len(route_edges):
                # search for first route edge included in the sumo network
                index += 1
                line_orig = route_edges[index]
            if not line_orig in net.getEdges():
                # if no edge found, discard ptLine
                continue
            # adapt osm route to sumo network
            route_edges = route_edges[index: ]

            # search ptLine destination
            index = -1
            line_dest = route_edges[index]
            while not line_dest in net.getEdges() and index-1 < -len(route_edges):
                # search for last route edge included in the sumo network
                index += -1
                line_orig = route_edges[index]
            if not line_dest in net.getEdges():
                # if not edge found, discard ptLine
                continue
            # adapt osm route to sumo network
            route_edges = route_edges[ : index-1]

            # transform ptLine origin and destination to geo coordinates
            x, y = net.getEdge(line_orig).getFromNode().getCoord()
            line_orig = net.convertXY2LonLat(x, y)
            x, y = net.getEdge(line_dest).getFromNode().getCoord()
            line_dest = net.convertXY2LonLat(x, y)

            # find ptLine direction
            line_dir = get_line_dir(line_orig, line_dest)

            # consider only edges in sumo network
            route_edges = [edge for edge in route_edges if edge in net.getEdges()]

            osm_routes[ptline.id] = [ptline.attr_name, ptline.line, ptline.type, line_dir]
            dua_file.write("""\t<vehicle id="%s" type="%s" depart="0">\n""" % (ptline.id, ptline.type))
            dua_file.write("""\t\t<route edges="%s"/>\n""" % (" ").join(route_edges))
            dua_file.write("""\t</vehicle>\n""")
        dua_file.write("</routes>\n")
    
    # run duarouter
    run_dua = subprocess.call([sumolib.checkBinary('duarouter'), '-n', options.network, '--route-files', dua_input ,'--repair', '-o', dua_output, '--error-log', dua_error, '--repair.from', '--repair.to', '--verbose'])
    if run_dua == 1:
        # if not succesfull run with "ignore-errors"
        print("duarouter found errors in routes, keep only possible routes")
        subprocess.call([sumolib.checkBinary('duarouter'), '-n', options.network, '--route-files', dua_input ,'--repair', '-o', dua_output, '--repair.from', '--repair.to', '--ignore-errors', '--verbose'])

    # search ptLines with errors 
    with open(dua_error, 'r') as error_file:
        error_file = error_file.read()
        error_lines = [line.split("'")[1] for line in error_file.splitlines() if line.startswith("Error:")]

    return dua_output, osm_routes, set(error_lines)

if __name__ == "__main__":

    # read inputs
    argParser = initOptions()
    options = argParser.parse_args()

    ######################################## Import SUMO net ########################################

    print("import net")
    net = sumolib.net.readNet(options.network)

    ######################################## gtfs, osm and sumo modes ########################################
    sumo_vClass = {
        'bus': 'bus',
        'train': 'rail',
        'tram': 'tram',
        'subway': 'rail_urban',
        'ferry': 'ship'
        }

    gtfs_modes = {
        # modes according to https://developers.google.com/transit/gtfs/reference/#routestxt
        '0':  'tram',
        '1':  'subway',
        '2':  'train',
        '3':  'bus',
        '4':  'ferry',
        # '5':  'cableTram',
        # '6':  'aerialLift',
        # '7':  'funicular',
        # modes used in Berlin and MDV see https://developers.google.com/transit/gtfs/reference/extended-route-types
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
    # bus and tram modes from https://developers.google.com/transit/gtfs/reference/extended-route-types
    for i in range(700, 717):
        gtfs_modes[str(i)] = 'bus'
    for i in range(900, 907):
        gtfs_modes[str(i)] = 'tram'

    ######################################## Import route-paths from OSM ########################################

    print("import osm routes")

    osm_routes = {}
    if options.repair:
        print("repair osm routes")
        osm_repair_routes, osm_routes, error_lines = repair_routes(options)

        for ptline, ptline_route in sumolib.xml.parse_fast_nested(osm_repair_routes, "vehicle", ("id"), "route", ("edges")):        
            if len(ptline_route.edges) > 2:
                osm_routes[ptline.id].append(ptline_route.edges)
            else:
                error_lines.append(ptline.id)
        
        # remove invalid routes from dict
        for line in error_lines:
            if osm_routes.get(line, False):
                del osm_routes[line]
    else:        
        for ptline, ptline_route in sumolib.xml.parse_fast_nested(options.osm_routes, "ptLine", ("id", "name", "line", "type"), "route", ("edges")):
            if len(ptline_route.edges) > 2:
                line_orig = ptline_route.edges.split(" ")[0]
                x, y = net.getEdge(line_orig).getFromNode().getCoord()
                line_orig = net.convertXY2LonLat(x, y)
                
                line_dest = ptline_route.edges.split(" ")[-1]
                x, y = net.getEdge(line_dest).getFromNode().getCoord()
                line_dest = net.convertXY2LonLat(x, y)

                line_dir = get_line_dir(line_orig, line_dest)

                osm_routes[ptline.id] = (ptline.attr_name, ptline.line, ptline.type, line_dir, ptline_route.edges)


    ######################################## Import GTFS data ########################################
    print("import gtfs data")

    gtfsZip = zipfile.ZipFile(options.gtfs)         
    routes = pd.read_csv(gtfsZip.open('routes.txt'), dtype=str)
    stops = pd.read_csv(gtfsZip.open('stops.txt'), dtype=str)
    stop_times = pd.read_csv(gtfsZip.open('stop_times.txt'), dtype=str)
    trips = pd.read_csv(gtfsZip.open('trips.txt'), dtype=str)
    shapes = pd.read_csv(gtfsZip.open('shapes.txt'), dtype=str)
    calendar_dates = pd.read_csv(gtfsZip.open('calendar_dates.txt'), dtype=str)

    # change col types
    stops['stop_lat'] = stops['stop_lat'].astype(float)
    stops['stop_lon'] = stops['stop_lon'].astype(float)
    shapes['shape_pt_lat'] = shapes['shape_pt_lat'].astype(float)
    shapes['shape_pt_lon'] = shapes['shape_pt_lon'].astype(float)
    shapes['shape_pt_sequence'] = shapes['shape_pt_sequence'].astype(float)
    stop_times['stop_sequence'] = stop_times['stop_sequence'].astype(float)

    # filter trips for a representative date
    gtfs_data = pd.merge(trips, calendar_dates, on='service_id')
    gtfs_data = gtfs_data[gtfs_data['date'] == options.date]

    # merge gtfs data from stop_times / trips / routes / stops
    gtfs_data = pd.merge(pd.merge(pd.merge(gtfs_data, stop_times, on='trip_id'), stops, on='stop_id'), routes, on='route_id')

    # Filter relevant information
    gtfs_data = gtfs_data[['route_id', 'shape_id', 'trip_id', 'stop_id','route_short_name', 'route_type', 'trip_headsign', 'direction_id', 'stop_name', 'stop_lat', 'stop_lon', 'stop_sequence', 'arrival_time', 'departure_time']]
    
    # filter data inside SUMO net by stop location and shape
    net_boundary = [float(boundary) for boundary in options.region.split(",")]
    gtfs_data = gtfs_data[(net_boundary[1] <= gtfs_data['stop_lat']) & (gtfs_data['stop_lat'] <= net_boundary[3]) & (net_boundary[0] <= gtfs_data['stop_lon']) & (gtfs_data['stop_lon'] <= net_boundary[2])]
    shapes = shapes[(net_boundary[1] <= shapes['shape_pt_lat']) & (shapes['shape_pt_lat'] <= net_boundary[3]) & (net_boundary[0] <= shapes['shape_pt_lon']) & (shapes['shape_pt_lon'] <= net_boundary[2])]
    
    # times to sec to enable sorting 
    trip_list = gtfs_data[gtfs_data["stop_sequence"] == 0]
    trip_list['departure'] = pd.to_timedelta(trip_list['arrival_time'])
    
    # add column for unambiguous stop_id and sumo edge
    gtfs_data["stop_item_id"] = None
    gtfs_data["edge_id"] = None
    
    # search main and secondary shapes for each pt line (route and direction)
    filter_stops = gtfs_data.groupby(['route_id','direction_id','shape_id']).agg({'stop_sequence': 'max'}).reset_index()
    group_shapes = filter_stops.groupby(['route_id','direction_id']).shape_id.aggregate(lambda x: set(x)).reset_index()
    filter_stops = filter_stops.loc[filter_stops.groupby(['route_id', 'direction_id'])['stop_sequence'].idxmax()][['route_id','shape_id', 'direction_id']]
    filter_stops = pd.merge(filter_stops, group_shapes, on=['route_id', 'direction_id'])

    # create dict with shape and main shape
    shapes_dict = {}
    for row in filter_stops.itertuples():
        for sec_shape in row.shape_id_y:
            shapes_dict[sec_shape] = row.shape_id_x

    # create data frame with main shape for stop location
    filter_stops = gtfs_data[gtfs_data['shape_id'].isin(filter_stops.shape_id_x)]
    filter_stops = filter_stops[['route_id', 'shape_id', 'stop_id', 'route_short_name', 'route_type', 'trip_headsign', 'direction_id', 'stop_name', 'stop_lat', 'stop_lon']].drop_duplicates()

    ######################################## Define Stops and Routes ########################################
    print("map stops and routes")
    
    map_routes = {}
    map_stops = {}
    radius = 200 # gtfs stops are grouped (no in exact geo position), so a large radius is needed

    missing_stops = []
    missing_lines = []

    for row in filter_stops.itertuples():

        # check if pt type is considered
        pt_type = gtfs_modes.get(row.route_type, False)
        if not pt_type:
            continue # if pt type not considered go to next row

        # check if gtfs route already mapped to osm route 
        if not map_routes.get(row.shape_id, False):
            # if route not mapped, find the osm route for shape id
            pt_line = row.route_short_name
            
            # get shape definition and define pt direction
            aux_shapes = shapes[shapes['shape_id'] == row.shape_id]
            pt_orig = aux_shapes[aux_shapes.shape_pt_sequence == aux_shapes.shape_pt_sequence.min()]
            pt_dest = aux_shapes[aux_shapes.shape_pt_sequence == aux_shapes.shape_pt_sequence.max()]
            line_dir = get_line_dir((pt_orig.shape_pt_lon, pt_orig.shape_pt_lat), (pt_dest.shape_pt_lon, pt_dest.shape_pt_lat))
            
            # get osm lines with same route name and pt type
            osm_lines = [key for key in osm_routes.keys() if osm_routes[key][1] == pt_line and osm_routes[key][2] == pt_type]
            if len(osm_lines) > 1:
                # get the direction for the found routes and take the route with lower difference
                aux_dif = [abs(line_dir-osm_routes[key][3]) for key in osm_lines]
                osm_id = osm_lines[aux_dif.index(min(aux_dif))]
                
                # add mapped osm route to dict
                map_routes[row.shape_id] = [osm_id, osm_routes.get(osm_id)[4].split(" ")]
            else:
                # no osm route found
                missing_lines.append(pt_line)
        
        # check if route was mapped
        if not map_routes.get(row.shape_id, False):
            continue # route not found, pt stops can not be mapped

        # check if stop already mapped
        stop_mapped = [key for key in map_stops.keys() if key.split("_")[0] == row.stop_id]
        stop_item_id = 0 # for pt stops with different stop points
        
        if stop_mapped:
            # get maximum item for stop
            stop_item_id = [int(stop.split("_")[1]) for stop in stop_mapped]
            stop_item_id = max(stop_item_id) + 1
            
            # check if the stop is already define #TODO merge stops on same edges
            for key in stop_mapped:
                # for item of mapped stop
                stop_edge = map_stops[key][1].split("_")[0]
                if stop_edge in map_routes[row.shape_id][1]:
                    # if edge in route, the stops are the same
                    # add the shape id to the stop
                    map_stops[key][5].append(row.shape_id)
                    # add to data frame
                    shape_list = [key for key, value in shapes_dict.items() if value == row.shape_id]
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) & (gtfs_data["shape_id"].isin(shape_list)), "stop_item_id"] = key
                    gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) & (gtfs_data["shape_id"].isin(shape_list)), "edge_id"] = stop_edge

                    stop_mapped = True
                    break
                else:
                    # check if the wrong edge was adopted
                    # get edges near stop location
                    x, y = net.convertLonLat2XY(row.stop_lon, row.stop_lat)
                    edges = net.getNeighboringEdges(x, y, radius, includeJunctions=False)
                    edges.sort(key = lambda x: x[1]) # sort by distance

                    for edge in edges:
                        edge_inter = set(map_routes[row.shape_id][1]) # current route edges
                        for shape_item in map_stops[key][5]: # shapes id of stop
                            # define the interseccion between the route edges of all shapes in stop
                            edge_inter = set(edge_inter) & set(map_routes[shape_item][1])
                        if edge[0].getID() in edge_inter:
                            # if the edge is in all routes
                            for lane in edge[0].getLanes():
                                # update the lane id
                                pt_class = sumo_vClass.get(pt_type, False)
                                if lane.allows(pt_class):
                                    lane_id = lane.getID()
                            map_stops[key][1] = lane_id
                            # update edge in data frame
                            gtfs_data.loc[gtfs_data["stop_item_id"] == key, "edge_id"] = edge[0].getID()
                            
                            # add to data frame              
                            shape_list = [key for key, value in shapes_dict.items() if value == row.shape_id]
                            gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) & (gtfs_data["shape_id"].isin(shape_list)), "stop_item_id"] = key
                            gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) & (gtfs_data["shape_id"].isin(shape_list)), "edge_id"] = edge[0].getID()

                            stop_mapped = 'yes'
                            break
                    if stop_mapped == 'yes':
                        break # if already found the stop, don't keep searching
            if stop_mapped != 'yes':
                stop_mapped = None # if stop not the same, search stop
                    

        # if stop not mapped
        if not stop_mapped:
            # get edges near stop location
            x, y = net.convertLonLat2XY(row.stop_lon, row.stop_lat)
            edges = net.getNeighboringEdges(x, y, radius, includeJunctions=False)
            edges.sort(key = lambda x: x[1]) # sort by distance

            for edge in edges:
                if not edge[0].getID() in map_routes[row.shape_id][1]:
                    # if edge not in pt line route
                    continue
                
                for lane in edge[0].getLanes():
                    pt_class = sumo_vClass.get(pt_type, False)
                    if lane.allows(pt_class):
                        lane_id = lane.getID()                            
                        if pt_class == "bus":
                            stop_length = options.bus_stop_length
                        elif pt_class == "tram":
                            stop_length = options.tram_stop_length
                        else:
                            stop_length = options.train_stop_length
                        pos = lane.getClosestLanePosAndDist((x, y))[0]
                        start = max(0, pos-stop_length)
                        end = min(start+stop_length, lane.getLength())
                        stop_item_id = "%s_%s" % (row.stop_id, stop_item_id)
                        map_stops[stop_item_id] = [row.stop_name, lane_id, start, end, pt_type, [row.shape_id]]
                        # add data to data frame
                        shape_list = [key for key, value in shapes_dict.items() if value == row.shape_id]
                        gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) & (gtfs_data["shape_id"].isin(shape_list)), "stop_item_id"] = stop_item_id
                        gtfs_data.loc[(gtfs_data["stop_id"] == row.stop_id) & (gtfs_data["shape_id"].isin(shape_list)), "edge_id"] = edge[0].getID()
                        
                        stop_mapped = True
                        break
                break
                
        # if stop not mapped, add to missing stops
        if not stop_mapped:
            missing_stops.append((row.stop_id, row.shape_id, row.stop_name, row.stop_lat, row.stop_lon))


    ######################################## Save mapped and missing data ########################################
    if options.debug:
        file_map_stops = "debug_map_stops.csv"
        with open(file_map_stops, 'w', encoding="utf8") as output_file:
            output_file.write("stop_item_id;stop_name;lane_id;start;end;pt_type;shape_id\n")
            for key, value in map_stops.items():
                output_file.write("%s;%s;%s;%s;%s;%s;%s\n" % (key, value[0], value[1], value[2], value[3], value[4], value[5]))

        file_miss_stops = "debug_missing_stops.csv"
        with open(file_miss_stops, 'w', encoding="utf8") as output_file:
            output_file.write("stop_id;shape_id;stop_name;stop_lat;stop_lon\n")
            for stop in set(missing_stops):
                output_file.write("%s;%s;%s;%s;%s\n" % (stop[0], stop[1], stop[2], stop[3], stop[4]))

        file_map_routes = "debug_map_routes.csv"
        with open(file_map_routes, 'w', encoding="utf8") as output_file:
            output_file.write("shape_id;osm_id;edges\n")
            for key, value in map_routes.items():
                output_file.write("%s;%s;%s\n" % (key, value[0], value[1]))

        file_miss_routes = "debug_missing_routes.csv"
        with open(file_miss_routes, 'w', encoding="utf8") as output_file:
            output_file.write("line_id\n")
            for line in set(missing_lines):
                output_file.write("%s\n" % (line))

    ######################################## Write Stops Output ########################################

    print("generates stops output")

    stop_output = "gtfs_stops.add.xml"
    with open(stop_output, 'w', encoding="utf8") as output_file:
        sumolib.xml.writeHeader(output_file, stop_output, "additional")
        for key, value in map_stops.items():
            if value[4] == "bus":
                output_file.write('    <busStop id="%s" lane="%s" startPos="%s" endPos="%s" name="%s" friendlyPos="true"/>\n' %
                            (key, value[1], value[2], value[3], value[0] ))
            else:
                # from gtfs2pt.py
                output_file.write('    <trainStop id="%s" lane="%s" startPos="%s" endPos="%s" name="%s" friendlyPos="true">\n' %
                            (key, value[1], value[2], value[3], value[0] ))
                            
                ap = sumolib.geomhelper.positionAtShapeOffset(net.getLane(value[1]).getShape(), value[2])
                numAccess = 0
                for accessEdge, _ in sorted(net.getNeighboringEdges(*ap, r=100), key=lambda i: i[1]):
                    if accessEdge.getID() != key.split("_")[0] and accessEdge.allows("passenger"):
                        _, accessPos, accessDist = accessEdge.getClosestLanePosDist(ap)
                        output_file.write(('        <access friendlyPos="true" ' +
                                    'lane="%s_0" pos="%s" length="%s"/>\n') %
                                    (accessEdge.getID(), accessPos, 1.5 * accessDist))
                        numAccess += 1
                        if numAccess == 5:
                            break
                output_file.write('    </trainStop>\n')
        output_file.write('</additional>\n')

    print("generates routes output")
    
    route_output = "gtfs_ptline.rou.xml"
    route_error = "gtfs_ptline_error.txt"
    
    with open(route_output, 'w', encoding="utf8") as output_file, open(route_error, 'w', encoding="utf8") as error_file:
        sumolib.xml.writeHeader(output_file, route_output, "routes")
        for value in set(gtfs_modes.values()):
            output_file.write('    <vType id="%s" vClass="%s"/>\n' % (value, sumo_vClass.get(value, False)))

        for row in trip_list.sort_values("departure").itertuples():

            # check if pt type is considered
            pt_type = gtfs_modes.get(row.route_type, False)
            if not pt_type:
                continue # if pt type not considered go to next row
            
            main_shape = shapes_dict.get(row.shape_id, None)
            if not map_routes.get(main_shape, None):
                # if route not mapped
                continue

            edges_list = map_routes[main_shape][1]
            stop_list = gtfs_data[gtfs_data["trip_id"] == row.trip_id].sort_values("stop_sequence")
            stop_index = [edges_list.index(stop.edge_id) for stop in stop_list.itertuples() if stop.edge_id in edges_list]

            if len(set(stop_index)) < 3:
                # Not enough stops mapped
                error_file.write("route id= '%s' line= '%s' trip= '%s' not mapped stops\n" % (row.route_id, row.route_short_name, row.trip_id))
                continue

            output_file.write('    <vehicle id="%s_%s" line="%s_%s" depart="%s" departEdge="%s" arrivalEdge="%s" type="%s">\n' 
                % (row.route_short_name, row.trip_id, row.route_id, row.direction_id, row.arrival_time, min(stop_index), max(stop_index), pt_type))
            output_file.write('        <route edges="%s"/>\n' % (" ".join(edges_list)))
            
            check_seq = -1
            for stop in stop_list.itertuples():
                if not stop.stop_item_id:
                    # if stop not mapped
                    continue
                if not stop.edge_id in edges_list:
                    # stop not in route
                    error_file.write("route error stop item %s %s (sequence %s) not in route %s (trip %s)\n" % (stop.stop_item_id, stop.stop_name, stop.stop_sequence, stop.route_id, stop.trip_id))
                    continue

                stop_index = edges_list.index(stop.edge_id)
                if stop_index > check_seq :
                    check_seq = stop_index
                    output_file.write('        <stop busStop="%s" arrival="%s" duration="%s" until="%s"/><!--%s %s-->\n' %
                        (stop.stop_item_id, stop.arrival_time, options.duration, stop.departure_time, stop.edge_id, stop_index))
                elif stop_index < check_seq:
                    # stop not downstream
                    error_file.write("sequence error in stop item %s %s (sequence %s) of route %s (trip %s)\n" % (stop.stop_item_id, stop.stop_name, stop.stop_sequence, stop.route_id, stop.trip_id))
                
                #else check_seq == stop_index:
                #    "stops are the same"

            output_file.write('    </vehicle>\n')
        output_file.write('</routes>\n')