# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    matsim_network_update.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

from .matsim_base import *
import math
from shapely.ops import unary_union, polygonize
from shapely.geometry import Point, MultiPoint, Polygon, MultiLineString, LineString
from matplotlib.path import Path
from scipy.spatial import ConvexHull, Delaunay
from collections import deque
from coremodules.network.routing import get_mincostroute_edge2edge
import matplotlib.pyplot as plt
import networkx as nx
from xml.sax import saxutils, parse, handler
import os
import sys
import shutil
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.processes import Process
from xml.sax import parse, handler
import sys
import os
BASE_DIR = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, os.path.dirname(__file__))


# I think geonetworkx is not used
# try:
#    import geonetworkx as gnx
# except:
#    gnx = None
try:
    from concave_hull import concave_hull
except:
    concave_hull = None


try:
    import pyproj
except:
    from mpl_toolkits.basemap import pyproj

pathsep = os.path.sep


class MapUpdater(Process):
    def __init__(self,  matsim, ids_mode=None,
                 logger=None, **kwargs):
        print('MapUpdater.__init__', matsim)
        self._init_common('MapUpdater', name='SUMO MATSim MapUpdater',
                          logger=logger,
                          info='Updates the MATSim network according to the SUMO network',
                          )

        self._matsim = matsim
        self._scenario = matsim.get_scenario()
        self._net = self._scenario.net
        self.rootname = self._scenario.get_rootfilename()
        self.rootdirpath = self._scenario.get_workdirpath()
        self.convex_hull = None
        self.concave_hull = None
        self.street_polygon = None
        self.roadTypes = {'highway.cycleway': False,  # NO
                          'highway.path': False,  # NO
                          'highway.living_street': False,  # YES
                          'highway.residential': True,  # YES
                          'highway.trunk': True,  # YES
                          'highway.service': False,  # NO
                          'highway.primary': True,  # YES
                          'highway.secondary': True,  # NO
                          'highway.tertiary': True,  # NO
                          'highway.primary_link': True,  # Yes
                          'highway.secondary_link': True,  # NO
                          'highway.motorway': True,  # Yes
                          'highway.motorway_link': True,  # YES
                          'highway.trunk_link': True,  # YES
                          'highway.unclassified': True,  # YES
                          'highway.track': False,  # NO
                          'railway.tram': False,
                          '': False,  # NO
                          }
        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.search_ortho = attrsman.add(cm.AttrConf('search_ortho', False,
                                                     groupnames=['options'],
                                                     name='search_ortho',
                                                     info='Search for vectors using orthogonal projection',
                                                     ))
        self.use_shape_for_matching = attrsman.add(cm.AttrConf('use_shape_for_matching', True,
                                                               groupnames=['options'],
                                                               name='use_shape_for_matching',
                                                               info='Use the shapes of the vector instead of the nodes ',
                                                               ))
        self.radius_start = attrsman.add(cm.AttrConf('radius_start', 4,
                                                     groupnames=['options'],
                                                     name='radius_start',
                                                     info='Search radius for SUMO vectors',
                                                     ))
        self.cosine_threshold = attrsman.add(cm.AttrConf('cosine_threshold', 0.95,
                                                         groupnames=['options'],
                                                         name='cosine_threshold',
                                                         info='Cosine Similarity threshold',
                                                         ))
        self.ortho_dist = attrsman.add(cm.AttrConf('ortho_dist', 20,
                                                   groupnames=['options'],
                                                   name='ortho_dist',
                                                   info='Allowed orthogonal distance',
                                                   ))
        self.ortho_cosine_threshold = attrsman.add(cm.AttrConf('ortho_cosine_threshold', 0.9,
                                                               groupnames=['options'],
                                                               name='ortho_cosine_threshold',
                                                               info='Cosine Similarity threshold during orthogonal projection',
                                                               ))
        self.ortho_tol = attrsman.add(cm.AttrConf('ortho_tol', 0.2,
                                                  groupnames=['options'],
                                                  name='ortho_tol',
                                                  info='Allowed differenze in orthogonal projection',
                                                  ))

        self.tol_node = attrsman.add(cm.AttrConf('tol_node', 1,
                                                 groupnames=['options'],
                                                 name='tol_node',
                                                 info='Difference in incoming / out-going edges',
                                                 ))
        self.min_length = attrsman.add(cm.AttrConf('min_length', 30,
                                                   groupnames=['options'],
                                                   name='min_length',
                                                   info='Minimal length of a MATSim Edge to be matched',
                                                   ))

    def do(self):
        if concave_hull is not None:
            self.network_output()
            return True
        else:
            print('WARNING: no execusion possible, package "concave_hull" is not installed.')
            return False

    # def orthogonal_distance_and_check_projection(self, point, start_point, end_point):
    #     # Vektor vom Startpunkt zum Endpunkt
    #     line_vector = end_point - start_point
    #     # Vektor vom Startpunkt zum Punkt
    #     point_vector = point - start_point
    #     # Orthogonaler Abstand berechnen
    #     orthogonal_distance = np.abs(np.cross(line_vector, point_vector)) / np.linalg.norm(line_vector)
    #     # Projektierter Punkt auf der Linie
    #     projected_point = start_point + np.dot(point_vector, line_vector) / np.dot(line_vector, line_vector) * line_vector
    #     # Überprüfen, ob der projizierte Punkt zwischen Start- und Endpunkt liegt
    #     is_between = np.all(np.logical_and(projected_point >= start_point, projected_point <= end_point))
    #     return orthogonal_distance, projected_point, is_between
    def get_highway_defaults(self, highway_type):
        use_vsp_adjustments = True
        highway_type = str(highway_type)
        if highway_type == '':
            highway_type = "highway.unclassified"

        highway_defaults = {
            "highway.motorway": (1, 2, 120.0/3.6, 1.0, 2000, True),
            "highway.motorway_link": (1, 1, 80.0/3.6, 1.0, 1500, True),
            "highway.trunk": (2, 1, 80.0/3.6, 1.0, 2000),
            "highway.trunk_link": (2, 1, 50.0/3.6, 1.0, 1500),
            "highway.primary": (3, 1, 80.0/3.6, 1.0, 1000) if use_vsp_adjustments else (3, 1, 80.0/3.6, 1.0, 1500),
            "highway.primary_link": (3, 1, 60.0/3.6, 1.0, 1000) if use_vsp_adjustments else (3, 1, 60.0/3.6, 1.0, 1500),
            "highway.secondary": (4, 1, 30.0/3.6, 1.0, 800) if use_vsp_adjustments else (4, 1, 30.0/3.6, 1.0, 1000),
            "highway.secondary_link": (4, 1, 30.0/3.6, 1.0, 800) if use_vsp_adjustments else (4, 1, 30.0/3.6, 1.0, 1000),
            "highway.tertiary": (5, 1, 25.0/3.6, 1.0, 600),
            "highway.tertiary_link": (5, 1, 25.0/3.6, 1.0, 600),
            "highway.unclassified": (6, 1, 15.0/3.6, 1.0, 600),
            "highway.residential": (7, 1, 15.0/3.6, 1.0, 600),
            "highway.living_street": (8, 1, 10.0/3.6, 1.0, 300)
        }
        return highway_defaults.get(highway_type, (6, 1, 15.0/3.6, 1.0, 600))

    def check_connections(self):

        print('Analyze connections')
        network = self._net
        edges = network.edges
        lanes = network.lanes
        nodes = network.nodes
        connections = network.connections
        ids_connections = connections.get_ids()
        ids_node = nodes.get_ids()
        # n_connections = np.zeros(np.max(ids_node)+1, dtype = np.int32)
        # nodes.n_connections[ids_node] = 0
        # dictionary with lists
        connections_of_node = {}

        for connection in ids_connections:

            incoming_lane = connections.ids_fromlane[connection]
            outgoing_lane = connections.ids_tolane[connection]
            incoming_edge = lanes.ids_edge[incoming_lane]
            outgoing_edge = lanes.ids_edge[outgoing_lane]

            node = edges.ids_tonode[incoming_edge]

            # Check for car - only
            if self._net.lanes.get_accesslevel([incoming_lane], 4) >= 0 and self._net.lanes.get_accesslevel([outgoing_lane], 4) >= 0:
                # Handelt es sich um parallele Straßen
                if self._net.edges.get_row(incoming_edge)['ids_fromnode'] == self._net.edges.get_row(outgoing_edge)['ids_tonode']:
                    print("ignore")
                else:
                    if node in connections_of_node:
                        connections_of_node[node].append((incoming_edge, outgoing_edge))
                    else:
                        connections_of_node[node] = [(incoming_edge, outgoing_edge)]

        for nodes in ids_node:
            list_incoming = self._net.nodes.get_row(nodes)['ids_incoming']
            list_outgoing = self._net.nodes.get_row(nodes)['ids_outgoing']

            number_of_desired = 0
            if list_incoming is not None and list_outgoing is not None:
                for incoming in list_incoming:
                    for outgoing in list_outgoing:
                        if not (self._net.edges.get_row(incoming)['ids_fromnode'] == self._net.edges.get_row(outgoing)['ids_tonode']) and self._net.edges.get_accesslevel(incoming, 4) >= 0 and self._net.edges.get_accesslevel(outgoing, 4) >= 0:
                            number_of_desired += 1
            else:
                number_of_desired = 0
            # Node nicht in der Liste
            if nodes in connections_of_node:
                if number_of_desired == len(set(connections_of_node[nodes])):
                    connections_of_node.pop(nodes)

        return connections_of_node

    def rework_connections(self, nodes, edges, connections_to_rework):

        for node in connections_to_rework:
            connections = connections_to_rework[node]
            node_id_sumo = self._net.nodes.get_row(node)['ids_sumo']
            list_incoming = self._net.nodes.get_row(node)['ids_incoming']
            list_outgoing = self._net.nodes.get_row(node)['ids_outgoing']

            # filter for modes
            list_incoming_filtered = []
            for edge in list_incoming:
                if self._net.edges.get_accesslevel(edge, 4) >= 0:
                    list_incoming_filtered.append(edge)
            list_incoming = list_incoming_filtered

            list_outgoing_filtered = []
            for edge in list_outgoing:
                if self._net.edges.get_accesslevel(edge, 4) >= 0:
                    list_outgoing_filtered.append(edge)
            list_outgoing = list_outgoing_filtered

            for incoming in list_incoming:

                new_cord = self._net.edges.get_coord_from_pos(incoming, self._net.edges.get_row(incoming)[
                                                              'lengths']-self._net.nodes.get_row(node)['radii'])
                edge_id_sumo = self._net.edges.get_row(incoming)['ids_sumo']
                nodes["I_"+str(node_id_sumo)+"-"+str(edge_id_sumo)] = [new_cord[0], new_cord[1]]
                # get values
                [from_node, to_node, capacity, modes, length, num_lanes, speed] = edges[edge_id_sumo]
                to_node = "I_"+str(node_id_sumo)+"-"+str(edge_id_sumo)
                # update
                edges[edge_id_sumo] = [from_node, to_node, capacity, modes, length, num_lanes, speed]

            for outgoing in list_outgoing:
                new_cord = self._net.edges.get_coord_from_pos(outgoing, self._net.nodes.get_row(node)['radii'])
                edge_id_sumo = self._net.edges.get_row(outgoing)['ids_sumo']
                nodes["I_"+str(node_id_sumo)+"-"+str(edge_id_sumo)] = [new_cord[0], new_cord[1]]
                # get values
                [from_node, to_node, capacity, modes, length, num_lanes, speed] = edges[edge_id_sumo]
                from_node = "I_"+str(node_id_sumo)+"-"+str(edge_id_sumo)
                # update
                edges[edge_id_sumo] = [from_node, to_node, capacity, modes, length, num_lanes, speed]

            for connection in set(connections):
                # possible improvement: lane count inside edge
                count_of_lanes = connections.count(connection)

                start = connection[0]
                end = connection[1]
                start_edge_id_sumo = self._net.edges.get_row(start)['ids_sumo']
                end_edge_id_sumo = self._net.edges.get_row(end)['ids_sumo']

                [from_node, to_node, capacity, modes, length, num_lanes, speed] = edges[start_edge_id_sumo]
                # possible improvement: lane count inside edge
                if count_of_lanes < num_lanes:
                    capacity = int((capacity/num_lanes)*count_of_lanes)

                from_node = "I_"+str(node_id_sumo)+"-"+str(start_edge_id_sumo)
                to_node = "I_"+str(node_id_sumo)+"-"+str(end_edge_id_sumo)
                coord_start = nodes[from_node]
                coord_end = nodes[to_node]
                dist = np.sqrt(np.square(coord_start[0] - coord_end[0]) + np.square(coord_start[1] - coord_end[1]))

                edges["I_"+str(node_id_sumo)+"-"+str(start_edge_id_sumo)+"-"+str(end_edge_id_sumo)
                      ] = [from_node, to_node, capacity, modes, dist, num_lanes, speed]

        return nodes, edges

    def np_graph_matsim(self, nodes, edges, boundaries=None, x_border=0, y_border=0):
        # .get_ids()
        # .get_row(id)['ids_fromnode']
        # ['coords'][0]
        # ['ids_fromnode']

        # ['ids_tonode']

        # ACHTUNG: Hier ist es wichtig auf die Modes zu achten!
        relevant_nodes = []
        vector_set_start = []
        vector_set_end = []
        vector_set_id = []
        vector_prev = []
        vector_after = []
        for id in nodes.get_ids():
            coords = nodes.get_row(id)['coords']
            if ((boundaries[0]-x_border <= coords[0]) & (boundaries[2]+x_border >= coords[0]) & (boundaries[1]-y_border <= coords[1]) & (boundaries[3]+y_border >= coords[1])):
                relevant_nodes.append(id)
        for id in edges.get_ids():
            if edges.get_row(id)['ids_fromnode'] in relevant_nodes or edges.get_row(id)['ids_tonode'] in relevant_nodes:
                vector_set_start.append(nodes.get_row(edges.get_row(id)['ids_fromnode'])['coords'])
                vector_set_end.append(nodes.get_row(edges.get_row(id)['ids_tonode'])['coords'])
                vector_set_id.append(id)
                # Anzahl an Edges, welche in den Vektor führen
                vector_prev.append(len(edges.select_ids(edges.ids_tonode.value == edges.get_row(id)['ids_fromnode'])))
                vector_after.append(len(edges.select_ids(edges.ids_fromnode.value == edges.get_row(id)['ids_tonode'])))
        return np.array(vector_set_start), np.array(vector_set_end), np.array(vector_set_id), np.array(vector_prev), np.array(vector_after)

    def filter_matsim(self, nodes, edges, boundaries=None, x_border=0, y_border=0):
        # .get_ids()
        # .get_row(id)['ids_fromnode']
        # ['coords'][0]
        # ['ids_fromnode']

        # ['ids_tonode']

        # ACHTUNG: Hier ist es wichtig auf die Modes zu achten!

        vector_set_id = []
        relevant_nodes = set()
        for id in nodes.get_ids():
            coords = nodes.get_row(id)['coords']
            if ((boundaries[0]-x_border <= coords[0]) & (boundaries[2]+x_border >= coords[0]) & (boundaries[1]-y_border <= coords[1]) & (boundaries[3]+y_border >= coords[1])):
                relevant_nodes.add(id)
        for id in edges.get_ids():
            if edges.get_row(id)['ids_fromnode'] in relevant_nodes or edges.get_row(id)['ids_tonode'] in relevant_nodes:
                vector_set_id.append(id)
        return np.array(vector_set_id)

    def prepare_routing_matsim(self, nodes, edges, boundaries=None, x_border=10000, y_border=10000):

        relevant_nodes = []
        check_nodes = []
        check_edges = []
        relevant_edges = []
        # Format of Boundary box
        # [MinX, MinY ,MaxX, MaxY ]
        # x_border, y_border sind buffer drum rum!

        for id in nodes.get_ids():
            coords = nodes.get_row(id)['coords']
            if ((boundaries[0]-x_border <= coords[0]) & (boundaries[2]+x_border >= coords[0]) & (boundaries[1]-y_border <= coords[1]) & (boundaries[3]+y_border >= coords[1])):
                relevant_nodes.append(id)

                if not ((boundaries[0] <= coords[0]) & (boundaries[2] >= coords[0]) & (boundaries[1] <= coords[1]) & (boundaries[3] >= coords[1])):

                    check_nodes.append(id)
        fstar = {}
        ids = edges.get_ids()
        # Improve performance!
        check_nodes = set(check_nodes)
        relevant_nodes = set(relevant_nodes)
        ###
        for id_edge, id_tonode, from_node in zip(ids, edges.ids_tonode[ids], edges.ids_fromnode[ids]):
            # beide nodes im
            if id_tonode in relevant_nodes or from_node in relevant_nodes:

                relevant_edges.append(id_edge)
                # remove key

        # Nachfolger muss auch im Netzwerk sein!
        relevant_edges = set(relevant_edges)

        for id_edge, id_tonode, from_node in zip(ids, edges.ids_tonode[ids], edges.ids_fromnode[ids]):
            # beide nodes im
            if id_tonode in relevant_nodes or from_node in relevant_nodes:
                ids_edge_outgoing = edges.select_ids(edges.ids_fromnode.value == id_tonode)
                ids_edge_outgoing_filtered = []
                for edge_to_filter in ids_edge_outgoing:
                    if edge_to_filter in relevant_edges:
                        ids_edge_outgoing_filtered.append(edge_to_filter)

                if ids_edge_outgoing is not None:
                    fstar[id_edge] = set(ids_edge_outgoing_filtered)
                else:
                    fstar[id_edge] = set()

            if id_tonode in check_nodes and from_node in check_nodes:
                # remove key
                check_edges.append(id_edge)

        print(len(fstar.keys()))
        return fstar, check_edges

    def np_graph_sumo(self, nodes, edges, filter=True):
        # ACHTUNG: Hier ist es wichtig auf die Modes zu achten!
        vector_set_start = []
        vector_set_end = []
        vector_set_id = []
        vector_prev = []
        vector_after = []

        for id in edges.get_ids():
            if filter != None:
                if self.roadTypes.get(edges.get_row(id)['types']):
                    # g.add_edge(str(edges.get_row(id)['ids_fromnode']),str(edges.get_row(id)['ids_tonode']),weight=edges.get_row(id)['lengths'],id=str(id))
                    # node_list.append(edges.get_row(id)['ids_fromnode'])
                    # node_list.append(edges.get_row(id)['ids_tonode'])

                    # Achtung: shapes der vektoren und Knoten / Kanten varrieren stark!

                    vector_set_start.append(nodes.get_row(edges.get_row(id)['ids_fromnode'])['coords'])
                    vector_set_end.append(nodes.get_row(edges.get_row(id)['ids_tonode'])['coords'])

                    vector_set_id.append(id)

                    in_coming = []
                    in_coming = edges.select_ids(edges.ids_tonode.value == edges.get_row(id)['ids_fromnode'])
                    # Detect fringe edges!
                    if len(in_coming) == 0:
                        vector_prev.append(-1)
                    elif len(in_coming) == 1 and edges.get_row(in_coming[0])['ids_fromnode'] == edges.get_row(id)['ids_tonode']:
                        vector_prev.append(-1)
                    else:
                        vector_prev.append(len(in_coming))

                    out_going = []
                    out_going = edges.select_ids(edges.ids_fromnode.value == edges.get_row(id)['ids_tonode'])
                    # Detect fringe edges!
                    if len(out_going) == 0:
                        vector_after.append(-1)
                    elif len(out_going) == 1 and edges.get_row(out_going[0])['ids_tonode'] == edges.get_row(id)['ids_fromnode']:
                        vector_after.append(-1)
                    else:
                        vector_after.append(len(out_going))

            else:

                vector_set_start.append(nodes.get_row(edges.get_row(id)['ids_fromnode'])['coords'])
                vector_set_end.append(nodes.get_row(edges.get_row(id)['ids_tonode'])['coords'])

                vector_set_id.append(id)

                vector_prev.append(len(edges.select_ids(edges.ids_tonode.value == edges.get_row(id)['ids_fromnode'])))
                vector_after.append(len(edges.select_ids(edges.ids_fromnode.value == edges.get_row(id)['ids_tonode'])))

        return np.array(vector_set_start), np.array(vector_set_end), np.array(vector_set_id), np.array(vector_prev), np.array(vector_after)

    def detect_finge_edges_sumo(self):
        network = self._net
        edges = network.edges

        fringe_edges_from = []
        fringe_edges_to = []
        for id in edges.get_ids():
            if self.roadTypes.get(edges.get_row(id)['types']):
                in_coming = []
                in_coming = edges.select_ids(edges.ids_tonode.value == edges.get_row(id)['ids_fromnode'])
                # Detect fringe edges!
                if len(in_coming) == 0:
                    fringe_edges_from.append(id)
                elif len(in_coming) == 1 and edges.get_row(in_coming[0])['ids_fromnode'] == edges.get_row(id)['ids_tonode']:
                    fringe_edges_from.append(id)

                out_going = edges.select_ids(edges.ids_fromnode.value == edges.get_row(id)['ids_tonode'])
                # Detect fringe edges!
                if len(out_going) == 0:
                    fringe_edges_to.append(id)
                elif len(out_going) == 1 and edges.get_row(out_going[0])['ids_tonode'] == edges.get_row(id)['ids_fromnode']:
                    fringe_edges_to.append(id)
        return fringe_edges_from, fringe_edges_to

    def detect_fringe_nodes_sumo(self, id_mode=4):
        network = self._net
        edges = network.edges
        nodes = network.edges

        fringe_nodes = set()

        for id in nodes.get_ids():
            in_coming = []
            out_going = []
            in_coming_raw = []
            out_going_raw = []
            ignore = False

            in_coming_raw = edges.select_ids(edges.ids_tonode.value == id)

            for edge_id in in_coming_raw:
                if self._net.edges.get_accesslevel(edge_id, 4) >= 0:
                    in_coming.append(edge_id)

            out_going_raw = edges.select_ids(edges.ids_fromnode.value == id)

            for edge_id in out_going_raw:
                if self._net.edges.get_accesslevel(edge_id, 4) >= 0:
                    out_going.append(edge_id)

            # Detect fringe edges!
            if len(in_coming) == 0 and len(out_going) > 0:

                fringe_nodes.add((id, 'incoming_sumo'))

            if len(out_going) == 0 and len(in_coming) > 0:

                fringe_nodes.add((id, 'outgoing_sumo'))

            if len(in_coming) == 1 and len(out_going) == 1:
                incoming_edge_shape = edges.shapes[in_coming[0]]

                outgoing_edge_shape = edges.shapes[out_going[0]]
                # coord_node = nodes.coords[node]
                coord_incoming_last = incoming_edge_shape[-1]
                coord_incoming_penultimate = incoming_edge_shape[-2]
                coord_outgoing_first = outgoing_edge_shape[0]
                coord_outgoing_second = outgoing_edge_shape[1]

                # print 'coord_node:',coord_node, 'coord_incoming:',coord_incoming, 'coord_outgoing:',coord_outgoing
                azimut_incoming = self.get_azimut(coord_incoming_last, coord_incoming_penultimate)
                azimut_outgoing = self.get_azimut(coord_outgoing_first, coord_outgoing_second)
                # print 'azimut_incoming:', azimut_incoming, 'azimut_outgoing:' ,azimut_outgoing
                diff_azimut = azimut_outgoing-azimut_incoming

                if np.absolute(diff_azimut) <= np.pi/18 or np.absolute(diff_azimut) >= np.pi*35/18:
                    fringe_nodes.add((id, 'multi_lane'))

        return fringe_nodes

    def get_azimut(self, point1, point2):
        # point1 = [x1,y1], point2 = [x2,y2]. Return the azimut of the second
        # point, respect to the first one, considered as the center.

        x_nod = point1[0]
        y_nod = point1[1]
        x_inc = point2[0]
        y_inc = point2[1]

        if (x_inc-x_nod) >= 0 and (y_inc-y_nod) > 0:
            azimut_incoming = np.arctan((x_inc-x_nod)/(y_inc-y_nod))
        elif (x_inc-x_nod) > 0 and (y_inc-y_nod) <= 0:
            azimut_incoming = np.pi/2 + np.arctan((y_nod-y_inc)/(x_inc-x_nod))
        elif (x_inc-x_nod) <= 0 and (y_inc-y_nod) < 0:
            azimut_incoming = np.pi + np.arctan((x_nod-x_inc)/(y_nod-y_inc))
        elif (x_inc-x_nod) < 0 and (y_inc-y_nod) >= 0:
            azimut_incoming = np.pi*3/2 + np.arctan((y_inc-y_nod)/(x_nod-x_inc))
        else:
            print('Warning, the two points are the same')
            # print point1, point2
            azimut_incoming = 0

        return azimut_incoming

    def breiten_suche(self, id, fstar):
        visited = set()
        # next_edges = fstar[id]
        queue = deque()
        if id in fstar:
            queue.extend(fstar[id])
            while queue:
                current_vertex = queue.popleft()
                if current_vertex in fstar:
                    for next in fstar[current_vertex]:
                        if not (next in visited):
                            visited.add(next)
                            queue.append(next)

                if len(visited) > 200:
                    queue = None
        return len(visited)

    def network_output(self):
        relevant_sumo_nodes = set()
        edges = {}
        for id in self._net.edges.get_ids():

            if self._net.edges.get_accesslevel(id, 4) >= 0:

                from_node = self._net.nodes.get_row(self._net.edges.get_row(id)['ids_fromnode'])['ids_sumo']
                to_node = self._net.nodes.get_row(self._net.edges.get_row(id)['ids_tonode'])['ids_sumo']
                # Achtung!!!
                # self._net.lanes.get_accesslevel_lanes(self._net.edges.ids_lanes[id], 4)
                # self._net.edges.ids_lanes[id_edge]
                numlanes_with_car_access = sum(1 for x in self._net.lanes.get_accesslevel_lanes(
                    self._net.edges.ids_lanes[id], 4) if x >= 0)
                # int(self._net.edges.get_row(id)['nums_lanes'])
                capacity = numlanes_with_car_access * self.get_highway_defaults(self._net.edges.get_row(id)['types'])[4]
                modes = "car"  # self._net.edges.get_row(id)['modes_matsim']
                id_sumo = self._net.edges.get_row(id)['ids_sumo']
                length = self._net.edges.get_row(id)['lengths']
                # num_lanes= self._net.edges.get_row(id)['nums_lanes']
                num_lanes = numlanes_with_car_access
                speed = self._net.edges.get_row(id)['speeds_max']

                edges[id_sumo] = [from_node, to_node, capacity, modes, length, num_lanes, speed]
                relevant_sumo_nodes.add(self._net.edges.get_row(id)['ids_fromnode'])
                relevant_sumo_nodes.add(self._net.edges.get_row(id)['ids_tonode'])

        nodes = {}
        for id in self._net.nodes.get_ids():
            if id in relevant_sumo_nodes:
                x_sumo, y_sumo = self._net.nodes.get_row(id)['coords'][0], self._net.nodes.get_row(id)['coords'][1]
                id_sumo = self._net.nodes.get_row(id)['ids_sumo']

                nodes[id_sumo] = [x_sumo, y_sumo]

        connections_to_rework = self.check_connections()

        # network to insert to MATSim
        nodes, edges = self.rework_connections(nodes, edges, connections_to_rework)


# Hier haben wir nun das SUMO Netzwerk als MATSim

        # detect fringe nodes in sumo
        # Fringe_edges_from = outgoing (Fringe node is in the back!)
        # Fringe_edges_to = incoming (Fringe node is in the front!)
        fringe_edges_from, fringe_edges_to = self.detect_finge_edges_sumo()
        print("Fringe Node detection -> Systemschnittstellen zu MATSim")
        fringe_nodes = self.detect_fringe_nodes_sumo()
        print(fringe_nodes)

        # filter MATSim network
        matsim_set_id = self.filter_matsim(self._matsim.net.nodes, self._matsim.net.edges,
                                           self._net.get_boundaries(is_netboundaries=True), x_border=3000, y_border=3000)
        # um die funktion get_closest_edge zu verwenden => make_segment_edge_map notwendig => Reduktion des Gebiets notwendig, da sonst zu rechenaufwändig
        self._matsim.net.edges.make_segment_edge_map(ids=matsim_set_id)

        # sumo_set_start,sumo_set_end,sumo_set_id,sumo_prev, sumo_after = self.np_graph_sumo(self._net.nodes,self._net.edges)

        # matsim_set_start,matsim_set_end,matsim_set_id,matsim_set_prev, matsim_set_after = self.np_graph_matsim(self._matsim.net.nodes,self._matsim.net.edges,self._net.get_boundaries(is_netboundaries = True))

        # ersetze die Nodes im Netzwerk!
        rename_nodes = {}
        rename_edges = {}

        delete_list = []
        # 'rename_matsim_node':{}, 'rename_to_node':{}, 'rename_from_node'
        fringe_nodes_matsim = set()
        fringe_edges_matsim = set()
        # incoming geht nach sumo rein
        print("Node assignment -> Systemschnittstellen zu SUMO")
        print("Node assignment -> Assignment erfolgt über das Umbennen der alten MATSim Nodes")
        for sumo_node, categorie in fringe_nodes:
            result = self.find_suitable_matsim_node(sumo_node, mode=categorie)

            if result != None:
                matsim_node, order, action = result

                print(str(categorie) + ' | ' + str(sumo_node) + " -> " + str(matsim_node))
                # new_node_name ist anschließend sumo
                new_node_name = sumo_node
                old_node_name = matsim_node
                fringe_nodes_matsim.add(new_node_name)

                if action == 'multi_lane_rename':
                    # ACHTUNG: sumo_node verändert sich!
                    for edge_detail, action_detail in order:
                        rename_edges[edge_detail] = {'old': old_node_name,
                                                     'new': new_node_name, 'action': action_detail}
                        fringe_edges_matsim.add(edge_detail)

                if action == 'rename_matsim_node':

                    rename_nodes[old_node_name] = new_node_name

                if action == 'rename_to_node' or action == 'rename_from_node':

                    if len(order) > 1:
                        print('ERROR')
                    else:
                        edge_detail, action_detail = order[0]

                    rename_edges[edge_detail] = {'old': old_node_name, 'new': new_node_name, 'action': action_detail}
                    fringe_edges_matsim.add(edge_detail)

                # 'rename_to_node':{}
                # 'rename_from_node'

                #  suche nach edges
                # delete_list.append(matsim_set_id[edge_matsim])
                # if old_node_name in rename_dict:
                #     if not rename_dict[old_node_name] == new_node_name:
                #         print("ERROR! - " + str(rename_dict[old_node_name]) + str(new_node_name))
                #     else:


#############################################################################################################################################################################
        print("Clean-Up -> Löschen des alten Ausschnitts von MATSim")
        matsim_set_id_check = []
        # cleanup old !
        exclude_from_deletion = fringe_edges_from + fringe_edges_to
        self._matsim.mapmatch.clear()
        ids_remove = []
        for matsim_id in matsim_set_id:
            delete = self.is_covered_by_sumo_area(matsim_id)
            # delete = self.is_covered_by_sumo_lane_wise(matsim_id)

            if delete and not matsim_id in fringe_edges_matsim:
                ids_remove.append(matsim_id)

        ids_remove_clean = []
        for id in ids_remove:
            if id in self._matsim.net.edges.get_ids():
                ids_remove_clean.append(id)
            else:
                print("ERROR")

        self._matsim.net.edges.del_rows(ids_remove_clean)
#############################################################################################################################################################################

        # matsim_set_id_check = [x for x in matsim_set_id if x not in ids_remove]

        # ids_remove_clean = []
        # for id in delete_list:
        #     if id in self._matsim.net.edges.get_ids():
        #         ids_remove_clean.append(id)
        #     else:
        #         print("ERROR")

        # self._matsim.net.edges.del_rows(ids_remove_clean)

        # matsim_set_id_check = [x for x in matsim_set_id_check if x not in delete_list]

        # ids_remove = []

        print("Insert -> Einfügen der neuen SUMO nodes als MATSim nodes")
        # nodes ist definiert aus den alten SUMO nodes!
        for node in nodes:
            # rename_node beinhaltet die fringe nodes!
            if True:  # not (node in rename_dict):
                # check if already exists:
                if node in self._matsim.net.nodes.ids_matsim._index_to_id:
                    # update with sumo coords!
                    # TODO: Sind Egdes mit dem Knoten verbunden? -> rausfiltern!
                    id = self._matsim.net.nodes.ids_matsim._index_to_id[node]

                    # if not(id in fringe_nodes_matsim):

                    #     edges_to_delete = self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_tonode.value == id)
                    #     if len(edges_to_delete) > 0:
                    #         ids_remove.extend(edges_to_delete)
                    #     edges_to_delete = self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_fromnode.value == id)
                    #     if len(edges_to_delete) > 0:
                    #         ids_remove.extend(edges_to_delete)

                    coords_matsim = np.zeros((1, 3), dtype=np.float64)
                    [x_sumo, y_sumo] = nodes[node]
                    coords_matsim[0][0] = x_sumo
                    coords_matsim[0][1] = y_sumo
                    self._matsim.net.nodes.coords[id] = coords_matsim
                    self._matsim.net.nodes.ids_matsim[id] = node
                else:
                    ids_node_matsim = np.zeros(1, dtype=object)
                    coords_matsim = np.zeros((1, 3), dtype=np.float64)

                    [x_sumo, y_sumo] = nodes[node]

                    ids_node_matsim[0] = node
                    coords_matsim[0][0] = x_sumo
                    coords_matsim[0][1] = y_sumo

                    self._matsim.net.nodes.add_rows(n=1,
                                                    ids_matsim=ids_node_matsim,
                                                    coords=coords_matsim,
                                                    )

        # ids_remove_clean = []
        # for id in ids_remove:
        #     if id in self._matsim.net.edges.get_ids():
        #         ids_remove_clean.append(id)
        #     else:
        #         print("ERROR")

        # self._matsim.net.edges.del_rows(ids_remove_clean)

        # insert new network -> block insert performanter!
        print("Insert -> Einfügen der neuen SUMO Kanten")
        n = len(edges.keys())
        ids_edge_matsim = np.zeros(n, dtype=object)
        ids_fromnode_matsim = np.zeros(n, dtype=object)
        ids_tonode_matsim = np.zeros(n, dtype=object)
        lengths = np.zeros(n, dtype=np.float32)
        speeds_max = np.zeros(n, dtype=np.float32)
        capacities = np.zeros(n, dtype=np.float32)
        nums_lane = np.zeros(n, dtype=np.float32)
        modes_array = np.zeros(n, dtype=object)
        types = np.zeros(n, dtype=object)
        shapes_edge = np.zeros(n, dtype=object)
        # couple new edges to old nodes!
        index = 0

        for edge in edges:

            [from_node, to_node, capacity, modes, length, num_lanes, speed] = edges[edge]

            # if from_node in rename_dict:
            #     # interne ID bereits bekannt, da alt
            #     ids_fromnode_matsim[index] = rename_dict[from_node]
            # else:
            #     # interne ID nicht bekannt, da neu
            ids_fromnode_matsim[index] = self._matsim.net.nodes.ids_matsim.get_id_from_index(from_node)

            # if to_node in rename_dict:
            #     # interne ID bereits bekannt, da alt
            #     ids_tonode_matsim[index] = rename_dict[to_node]
            # else:
            #     # interne ID nicht bekannt, da neu

            ids_tonode_matsim[index] = self._matsim.net.nodes.ids_matsim.get_id_from_index(to_node)

            lengths[index] = length
            speeds_max[index] = speed
            capacities[index] = capacity
            modes_array[index] = modes
            types[index] = modes
            shapes_edge[index] = [self._matsim.net.nodes.coords[ids_fromnode_matsim[0]],
                                  self._matsim.net.nodes.coords[ids_tonode_matsim[0]]]
            ids_edge_matsim[index] = edge
            nums_lane[index] = num_lanes
            index += 1

            # edges.shapes[ids_edge] = shapes_edge[inds_valid]

        ids_edge = self._matsim.net.edges.add_rows(n=n,
                                                   ids_matsim=ids_edge_matsim,
                                                   ids_fromnode=ids_fromnode_matsim,
                                                   ids_tonode=ids_tonode_matsim,
                                                   lengths=lengths,
                                                   speeds_max=speeds_max,
                                                   capacities=capacities,
                                                   nums_lane=nums_lane,
                                                   types=types,
                                                   modes_matsim=modes_array,
                                                   shapes=shapes_edge,
                                                   )
        self._matsim.net.edges.shapes[ids_edge] = shapes_edge
        ################################################################################################################################################################################
        print("Connect Networks -> Umbennen der MATSim Teile wie definiert")
        # if action == 'rename_matsim_node':
        #     rename_nodes[old_node_name] = new_node_name

        # if action == 'rename_to_node' or action == 'rename_from_node':
        #     rename_edges[edge] = {'old': old_node_name, 'new':new_node_name, 'action':action}
        # for node in rename_nodes:

        # ACHTUNG: sumo_node verändert sich!

        for id in self._matsim.net.edges.get_ids():
            # no length Update neccessary, da gleiche Position der Nodes
            if self._matsim.net.edges.get_row(id)['ids_fromnode'] in rename_nodes:
                sumo_interne_id = rename_nodes[self._matsim.net.edges.get_row(id)['ids_fromnode']]
                sumo_name = self._net.nodes.get_row(sumo_interne_id)['ids_sumo']
                matsim_interne_id = self._matsim.net.nodes.ids_matsim._index_to_id[sumo_name]
                self._matsim.net.edges.set_row(id, ids_fromnode=matsim_interne_id)

            if self._matsim.net.edges.get_row(id)['ids_tonode'] in rename_nodes:

                sumo_interne_id = rename_nodes[self._matsim.net.edges.get_row(id)['ids_tonode']]
                sumo_name = self._net.nodes.get_row(sumo_interne_id)['ids_sumo']
                matsim_interne_id = self._matsim.net.nodes.ids_matsim._index_to_id[sumo_name]
                self._matsim.net.edges.set_row(id, ids_tonode=matsim_interne_id)

            # to_node / from_node wird verändert. Länge des Vektors muss angepasst werden
            if id in rename_edges:
                if rename_edges[id]['action'] == 'rename_to_node':

                    sumo_interne_id = rename_edges[id]['new']
                    sumo_name = self._net.nodes.get_row(sumo_interne_id)['ids_sumo']
                    matsim_interne_id = self._matsim.net.nodes.ids_matsim._index_to_id[sumo_name]
                    self._matsim.net.edges.set_row(id, ids_tonode=matsim_interne_id)

                    from_node = self._matsim.net.edges.get_row(id)['ids_fromnode']
                    to_node = self._matsim.net.edges.get_row(id)['ids_tonode']

                    # edge_matsim_edge_shape = self._matsim.net.edges.shapes[matsim_edge]
                    matsim_start = self._matsim.net.nodes.coords[from_node][0:2]
                    matsim_end = self._matsim.net.nodes.coords[to_node][0:2]
                    distance = np.linalg.norm(matsim_end - matsim_start)
                    self._matsim.net.edges.set_row(id, lengths=distance)

                if rename_edges[id]['action'] == 'rename_from_node':

                    sumo_interne_id = rename_edges[id]['new']
                    sumo_name = self._net.nodes.get_row(sumo_interne_id)['ids_sumo']
                    matsim_interne_id = self._matsim.net.nodes.ids_matsim._index_to_id[sumo_name]

                    self._matsim.net.edges.set_row(id, ids_fromnode=matsim_interne_id)

                    from_node = self._matsim.net.edges.get_row(id)['ids_fromnode']
                    to_node = self._matsim.net.edges.get_row(id)['ids_tonode']

                    # edge_matsim_edge_shape = self._matsim.net.edges.shapes[matsim_edge]
                    matsim_start = self._matsim.net.nodes.coords[from_node][0:2]
                    matsim_end = self._matsim.net.nodes.coords[to_node][0:2]
                    distance = np.linalg.norm(matsim_end - matsim_start)
                    self._matsim.net.edges.set_row(id, lengths=distance)

        # clean edges! #################################################################################################################################################################
        # Müss ich prüfen, ob SUMO die MATSim Eigenschaften erfüllt?
        if True:

            nodes_to_keep = self.global_cleaner()
            # clean nodes
            ids_remove_clean = []
            for id in self._matsim.net.edges.get_ids():
                if self._matsim.net.edges.get_row(id)['ids_fromnode'] in nodes_to_keep and self._matsim.net.edges.get_row(id)['ids_tonode'] in nodes_to_keep:
                    pass
                else:
                    ids_remove_clean.append(id)

            self._matsim.net.edges.del_rows(ids_remove_clean)
            # clean nodes
            ids_remove_clean = []
            for id in self._matsim.net.nodes.get_ids():
                if not id in nodes_to_keep:
                    ids_remove_clean.append(id)

            self._matsim.net.nodes.del_rows(ids_remove_clean)

        else:

            print("LOAD ROUTING")
            weights = self._matsim.net.edges.get_times()
            fstar, check_ids = self.prepare_routing_matsim(self._matsim.net.nodes, self._matsim.net.edges, self._net.get_boundaries(
                is_netboundaries=True), x_border=10000, y_border=10000)
            print("START EDGE CHECK")

            ids_remove = []
            ids_unconnected = []
            # Wähle 20 zufällige edges und probiere zu diesen eine route her zu stellen
            routing_destinations = np.random.choice(check_ids, size=3, replace=False)
            print(len(routing_destinations))
            print(len(matsim_set_id_check))

            print("filter unconnected edges")

            # Filter unconnected edges
            for id in matsim_set_id_check:
                # wir wissen: Ensures that each link in the network can be reached by any other link

                from_node = self._matsim.net.edges.get_row(id)['ids_fromnode']
                to_node = self._matsim.net.edges.get_row(id)['ids_tonode']

                test_1 = []
                test_2 = []
                ids_unconnected = set()
                # Anschlüsse
                test_1 = self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_tonode.value == from_node)
                test_2 = self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_fromnode.value == to_node)

                if (len(test_1) == 0) and (len(test_2) == 0):
                    ids_remove.append(id)

                # TODO: wenn test_1 == 1 und test_2 == 1 und len combined_set = set(list1) | set(list2) -> löschen
                elif (len(test_1) == 1) and (len(test_2) == 1) and len(set(test_1) | set(test_2)) == 1:

                    ids_remove.append(id)

                    # Hier filtern wir noch nicht alle raus! Muss auch für connected edges ausgeführt werden!
                else:
                    found_route = False
                    index = 0
                    stop_searching = False

                    combined_list = set(test_1) | set(test_2)
                    # Gefährlicher Passus?

                    for element in combined_list:
                        if element in ids_unconnected:
                            stop_searching = True
                            ids_remove.append(id)
                            ids_unconnected.add(id)
                        # finden wir eine Route zu den Anschlusstellen? -> Route darf in MATsim oder in SUMO verlaufen
                    while found_route == False and stop_searching == False:

                        # cost, tmp_routes = get_mincostroute_edge2edge(id,routing_destinations[index],weights= weights,fstar = fstar)
                        number_of_following = self.breiten_suche(id, fstar)

                        # if len(tmp_routes) > 0:
                        #    found_route = True
                        #    print("found_route")
                        # else:
                        #    index+=1

                        # if index == len(routing_destinations):
                        if number_of_following < 30:
                            stop_searching = True
                            ids_remove.append(id)
                            ids_unconnected.add(id)
                        else:
                            stop_searching = True
                            # print("connected")

            # self._matsim.net.edges.del_rows(ids_remove)

            ids_remove_clean = []
            for id in ids_remove:
                if id in self._matsim.net.edges.get_ids():
                    ids_remove_clean.append(id)
                else:
                    print("ERROR")

            self._matsim.net.edges.del_rows(ids_remove_clean)

            print("filter unidirectional edges")

            # TODO: clean up unidirectional edges, on fringe node!! -> check all! -> Effizienter über fstar + bstar und Gebietseinschränkung!
            matsim_set_start, matsim_set_end, matsim_set_id, matsim_set_prev, matsim_set_after = self.np_graph_matsim(
                self._matsim.net.nodes, self._matsim.net.edges, self._net.get_boundaries(is_netboundaries=True), x_border=1000, y_border=1000)

            ids_remove = []
            initial = True

            while ids_remove != [] or initial == True:

                ids_remove = []
                initial = False

                for id in matsim_set_id:

                    # update with sumo coords!
                    from_node = self._matsim.net.edges.get_row(id)['ids_fromnode']
                    to_node = self._matsim.net.edges.get_row(id)['ids_tonode']

                    test_1 = self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_tonode.value == from_node)
                    test_2 = self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_fromnode.value == to_node)

                    if (len(test_1) == 0) != (len(test_2) == 0):

                        ids_remove.append(id)

                    elif (len(test_1) == 0) and (len(test_2) == 0):

                        ids_remove.append(id)
                ids_remove_clean = []
                for id in ids_remove:
                    if id in self._matsim.net.edges.get_ids():
                        ids_remove_clean.append(id)
                    else:
                        print("ERROR")

                self._matsim.net.edges.del_rows(ids_remove_clean)

        # clean matsim ! ##################################################################################################################################################################
            print("START NODE CHECK")
            ids_remove = []
            for id in self._matsim.net.nodes.get_ids():
                if len(self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_tonode.value == id)) == 0 and len(self._matsim.net.edges.select_ids(self._matsim.net.edges.ids_fromnode.value == id)) == 0:
                    ids_remove.append(id)

            self._matsim.net.nodes.del_rows(ids_remove)
        #################################################################################################################################################################################
        simfile = open(self.rootdirpath + '/network.xml', 'w', encoding="utf-8")

        simfile.write(
            """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE network SYSTEM "http://www.matsim.org/files/dtd/network_v1.dtd">\n<network>\n<!-- ====================================================================== -->\n\t<nodes>\n""")
        # <node id="1000056237" x="4422218.544286327" y="5375842.302200025" />

        for id in nodes:
            [x_sumo, y_sumo] = nodes[id]
            # ACHTUNG: Die Koordinaten müssen zurück in EPSG:31468 transformiert werden
            simfile.write('\t\t<node id=\"' + id + '\" x=\"' + str(x_sumo) + '\" y=\"' + str(y_sumo) + '\"/>\n')

        simfile.write('\t</nodes>')
        simfile.write('\t<links capperiod="01:00:00" effectivecellsize="7.5" effectivelanewidth="3.75">')
        # <link id="99999" from="280511774" to="399363" length="269.75324405938596" freespeed="16.666666666666668" capacity="2000.0" permlanes="2.0" oneway="1" modes="car" origid="307544229" type="secondary" />

        for id in edges:
            [from_node, to_node, capacity, modes, length, num_lanes, speed] = edges[id]
            simfile.write('\t\t<link id=\"' + id + '\" from=\"' + from_node + '\" to=\"' + to_node + '\" length=\"' + str(length) + '\" freespeed=\"' + str(
                speed)+'\" capacity=\"' + str(capacity) + '\" permlanes=\"' + str(num_lanes)+'\" oneway=\"1\" modes=\"' + modes + '\" type=\"secondary\"/>\n')

        simfile.write(
            """</links>\n
        <!-- ====================================================================== -->\n
        </network>\n""")
        simfile.close()

    def alpha_shape(self, points, alpha):
        """
        Compute the alpha shape (concave hull) of a set of points.
        @param points: Iterable container of points.
        @param alpha: Alpha value to influence the gooeyness of the border. 
                    Smaller numbers don't fall inward as much as larger numbers.
                    Too large, and you lose everything!
        Inpsired by: https://gist.github.com/dwyerk/10561690
        """

        if len(points) < 4:
            # When you have a triangle, there is no sense in computing an alpha shape.
            return None

        coords = np.array([point for point in points])
        tri = Delaunay(coords)
        triangles = coords[tri.vertices]

        a = np.linalg.norm(triangles[:, 0] - triangles[:, 1], axis=1)
        b = np.linalg.norm(triangles[:, 1] - triangles[:, 2], axis=1)
        c = np.linalg.norm(triangles[:, 2] - triangles[:, 0], axis=1)
        s = (a + b + c) / 2.0
        areas = np.sqrt(s * (s - a) * (s - b) * (s - c))
        circums = a * b * c / (4.0 * areas)

        # Filter edges based on circumcircle radius
        filtered = triangles[circums < (1.0 / alpha)]

        edge1 = filtered[:, (0, 1)]
        edge2 = filtered[:, (1, 2)]
        edge3 = filtered[:, (2, 0)]
        edge_points = np.concatenate((edge1, edge2, edge3))

        m = MultiLineString(edge_points)
        triangles = list(polygonize(m))

        concave_hull = unary_union(triangles)

        return concave_hull

    def calculate_hull(self):
        coord_list = []
        plot = False

        for id in self._net.nodes.get_ids():
            coord_list.append(self._net.nodes.coords[id][0:2])

        for id in self._net.edges.get_ids():
            for coord in self._net.edges.shapes[id][:, 0:2]:
                coord_list.append(coord)

        coord_list = np.array(coord_list)

        concave_hull_points = concave_hull(coord_list, concavity=1, length_threshold=0)

        if (concave_hull_points[0][0] != concave_hull_points[-1][0]) or (concave_hull_points[0][1] != concave_hull_points[-1][1]):
            concave_hull_points = np.append(concave_hull_points, [concave_hull_points[0]], axis=0)

        self.concave_hull = Polygon(concave_hull_points)
        # Trick for cleaning up the polygon
        self.concave_hull = self.concave_hull.buffer(0)

        # x, y = self.concave_hull.exterior.xy
        # plt.figure(figsize=(8, 8))
        # plt.plot(x, y, color='blue', label='Concave Polygon')
        # plt.fill(x, y, alpha=0.3, color='blue')
        if plot:
            x_points = concave_hull_points[:, 0]
            y_points = concave_hull_points[:, 1]
            plt.scatter(x_points, y_points, c='blue', marker='o')
            plt.show()

        edge_list = []

        for sumo_edge in self._net.edges.get_ids():
            sumo_edge_shape = self._net.edges.shapes[sumo_edge]
            num_lanes = self._net.edges.nums_lanes[sumo_edge]
            line = LineString(sumo_edge_shape)
            buffered_line = line.buffer(2*num_lanes)
            edge_list.append(buffered_line)
            self.concave_hull = self.concave_hull.union(buffered_line)

        # Trick for cleaning up the polygon
        self.concave_hull = self.concave_hull.buffer(0)

        for matsim_node in self._net.nodes.get_ids():
            node_shape = self._net.nodes.shapes[matsim_node]
            polygon = Polygon(node_shape)
            edge_list.append(polygon)
            self.concave_hull = self.concave_hull.union(polygon)

        # Trick for cleaning up the polygon
        self.concave_hull = self.concave_hull.buffer(0)

        self.street_polygon = unary_union(edge_list)

        hull = ConvexHull(coord_list)
        hull_vertices = coord_list[hull.vertices]

        self.convex_hull = Delaunay(hull_vertices)

    def is_covered_by_sumo_area(self, matsim_edge):

        if self.concave_hull == None or self.convex_hull == None or self.street_polygon == None:
            self.calculate_hull()
        from_node = self._matsim.net.edges.get_row(matsim_edge)['ids_fromnode']
        to_node = self._matsim.net.edges.get_row(matsim_edge)['ids_tonode']

        # edge_matsim_edge_shape = self._matsim.net.edges.shapes[matsim_edge]
        matsim_start = self._matsim.net.nodes.coords[from_node][0:2]
        matsim_end = self._matsim.net.nodes.coords[to_node][0:2]

        start_inside = False
        end_inside = False

        if False:
            start_inside = self.convex_hull.find_simplex(matsim_start) >= 0
            end_inside = self.convex_hull.find_simplex(matsim_end) >= 0
        else:
            start_inside = self.concave_hull.contains(Point(matsim_start))
            end_inside = self.concave_hull.contains(Point(matsim_end))

        if start_inside and end_inside:
            return True
        else:
            return False

    def is_covered_by_sumo_lane_wise(self, matsim_edge):
        # das Problem hier ist, dass das Löschen nicht präzise genug ist => es werden auch fringe edges gelöscht

        edge_matsim_edge_shape = self._matsim.net.edges.shapes[matsim_edge]
        matsim_start = edge_matsim_edge_shape[0]
        matsim_end = edge_matsim_edge_shape[-1]

        ortho_dist = 15

        result_start = self._net.edges.get_closest_edge(matsim_start, d_max=ortho_dist, n_best=20)
        index = 0
        ignore = False
        delete_start = False
        delete_end = False

        while len(result_start[0]) > index:

            if self.check_same_dir(result_start[0][index], matsim_edge, winkel_vorgabe=np.pi / 3) and result_start[1][index] < ortho_dist:
                # bester Vektor ist fringe edge -> ignore
                # if  index > 0: #result_start[0][index] in exclude_from_deletion and:
                #    ignore = True
                # else:
                delete_start = True

            index += 1

        result_end = self._net.edges.get_closest_edge(matsim_end, d_max=ortho_dist, n_best=20)

        index = 0
        ignore = False

        while len(result_end[0]) > index:

            if self.check_same_dir(result_end[0][index], matsim_edge, winkel_vorgabe=np.pi / 3) and result_end[1][index] < ortho_dist:
                # bester Vektor ist fringe edge -> ignore
                # if  index > 0: #result_start[0][index] in exclude_from_deletion and:
                #    ignore = True
                # else:
                delete_end = True
            index += 1

        if delete_start and delete_end and not ignore:
            return True
        else:
            return False

    def is_covered_by_sumo(self, start_point, end_point, vector_set_start, vector_set_end, vector_set_id, exclude_from_deletion):

        ortho_cosine_threshold = 0.8
        ortho_dist = 15
        # Cosine similarities
        vector_set = vector_set_end - vector_set_start
        target_vector = end_point - start_point

        dot_product = np.dot(vector_set, target_vector)
        norm_vector = np.linalg.norm(target_vector)
        norm_array = np.linalg.norm(vector_set, axis=1)
        cosine_similarities = dot_product / (norm_vector * norm_array)

        result_start = self._net.edges.get_closest_edge(start_point, d_max=ortho_dist, n_best=20)

        index = 0
        ignore = False
        delete_start = False
        delete_end = False
        vector_set_id = list(vector_set_id)
        while len(result_start[0]) > index:
            try:
                cosine = cosine_similarities[vector_set_id.index(result_start[0][index])]
            except:
                cosine = -1

            if cosine > ortho_cosine_threshold and result_start[1][index] < ortho_dist:
                # bester Vektor ist fringe edge -> ignore
                if result_start[0][index] in exclude_from_deletion and index == 0:
                    ignore = True
                else:
                    delete_start = True

            index += 1

        result_end = self._net.edges.get_closest_edge(end_point, d_max=ortho_dist, n_best=20)

        index = 0

        while len(result_end[0]) > index:
            try:
                cosine = cosine_similarities[vector_set_id.index(result_end[0][index])]
            except:
                cosine = -1

            if cosine > ortho_cosine_threshold and result_end[1][index] < ortho_dist:
                # bester Vektor ist fringe edge -> ignore
                if result_end[0][index] in exclude_from_deletion and index == 0:
                    ignore = True
                else:
                    delete_end = True

            index += 1

        if delete_start and delete_end and not ignore:
            return True
        else:
            return False

    def global_cleaner(self):

        next_nodes_forward = {}
        next_nodes_backward = {}

        for edge_id in self._matsim.net.edges.get_ids():

            from_node = self._matsim.net.edges.get_row(edge_id)['ids_fromnode']
            to_node = self._matsim.net.edges.get_row(edge_id)['ids_tonode']

            if from_node in next_nodes_forward:
                next_nodes_forward[from_node].add(to_node)
            else:
                next_nodes_forward[from_node] = set()
                next_nodes_forward[from_node].add(to_node)

            if to_node in next_nodes_backward:
                next_nodes_backward[to_node].add(from_node)
            else:
                next_nodes_backward[to_node] = set()
                next_nodes_backward[to_node].add(from_node)

        visited_nodes = set()

        biggest_cluster = set()

        for node in self._matsim.net.nodes.get_ids():
            if not (node in visited_nodes):
                cluster = self.find_biggest_cluster(next_nodes_forward, next_nodes_backward, node)
                visited_nodes.update(cluster)
                if len(cluster) > len(biggest_cluster):
                    biggest_cluster = cluster
                if len(biggest_cluster) >= (len(self._matsim.net.nodes.get_ids())-len(visited_nodes)):
                    break

        return biggest_cluster

    def find_biggest_cluster(self, next_nodes_forward, next_nodes_backward, node):

        clusternodes = set()
        clusternodes.add(node)

        pendingForward = deque()
        pendingForward.append(node)
        visited_nodes_forward = set()

        pendingBackward = deque()
        pendingBackward.append(node)
        visited_nodes_backward = set()

        while (len(pendingForward) > 0):
            current_node = pendingForward.popleft()
            if current_node in next_nodes_forward:
                next_nodes = next_nodes_forward[current_node]
                for node in next_nodes:
                    if not (node in visited_nodes_forward):
                        visited_nodes_forward.add(node)
                        pendingForward.append(node)

        while (len(pendingBackward) > 0):
            current_node = pendingBackward.popleft()
            if current_node in next_nodes_backward:
                next_nodes = next_nodes_backward[current_node]
                for node in next_nodes:
                    if not (node in visited_nodes_backward):
                        visited_nodes_backward.add(node)
                        pendingBackward.append(node)

                        if node in visited_nodes_forward:
                            clusternodes.add(node)

        return clusternodes

    def check_same_dir(self, edge_sumo, edge_matsim, winkel_vorgabe=np.pi / 6):
        network = self._net
        edges_sumo = network.edges

        network_matsim = self._matsim.net
        edges_matsim = network_matsim.edges
        edge_sumo_edge_shape = edges_sumo.shapes[edge_sumo]

        edge_matsim_edge_shape = edges_matsim.shapes[edge_matsim]
        # coord_node = nodes.coords[node]
        coord_edge_sumo_last = edge_sumo_edge_shape[0]
        coord_edge_sumo_penultimate = edge_sumo_edge_shape[-1]
        coord_edge_matsim_first = edge_matsim_edge_shape[0]
        coord_edge_matsim_second = edge_matsim_edge_shape[-1]

        # print 'coord_node:',coord_node, 'coord_edge_sumo:',coord_edge_sumo, 'coord_edge_matsim:',coord_edge_matsim
        azimut_edge_sumo = self.get_azimut(coord_edge_sumo_last, coord_edge_sumo_penultimate)
        azimut_edge_matsim = self.get_azimut(coord_edge_matsim_first, coord_edge_matsim_second)
        # print 'azimut_edge_sumo:', azimut_edge_sumo, 'azimut_edge_matsim:' ,azimut_edge_matsim
        diff_azimut = azimut_edge_matsim-azimut_edge_sumo

        # Normalize the difference to be within the range -π to π
        # diff_azimut = (diff_azimut + np.pi) % (2 * np.pi) - np.pi

        if np.absolute(diff_azimut) <= winkel_vorgabe:
            return True
        else:
            return False

    def find_suitable_matsim_node(self, sumo_node, mode=None):
        dist_node_assignment = 5

        # incoming: Edge geht in das SUMO Gebiet rein -> (outgoing muss gleich sein)
        if mode == 'incoming_sumo':

            # existiert ein Knoten im MATSim Netzwerk, welcher vergleichbar ist?

            coords_node_sumo = self._net.nodes.get_row(sumo_node)['coords']
            closest_matsim_node, distance = self._matsim.net.nodes.get_closest(coords_node_sumo)
            # hier kann es nur eine geben (per Definition!)
            out_going_sumo = self._net.edges.select_ids(self._net.edges.ids_fromnode.value == sumo_node)

            if distance < dist_node_assignment:

                out_going_matsim = self._matsim.net.edges.select_ids(
                    self._matsim.net.edges.ids_fromnode.value == closest_matsim_node)

                return_it = False
                corresponding_matsim_edge = None

                for matsim_edge in out_going_matsim:
                    if self.check_same_dir(out_going_sumo[0], matsim_edge) and not return_it:
                        return_it = True
                        corresponding_matsim_edge = matsim_edge
                    elif self.check_same_dir(out_going_sumo[0], matsim_edge) and return_it:
                        return_it = False
                        corresponding_matsim_edge = None

                if return_it:
                    return closest_matsim_node, [], 'rename_matsim_node'

            # liegt der Knoten nahe einer MATSim Kante?
            ids_edge, dists = self._matsim.net.edges.get_closest_edge(coords_node_sumo, n_best=3)

            for id_edge, dist in zip(ids_edge, dists):
                if self.check_same_dir(out_going_sumo[0], id_edge) and dist < 40:

                    new_matsim_node = self._matsim.net.edges.get_row(id_edge)['ids_tonode']

                    return new_matsim_node, [[id_edge, 'rename_to_node']], 'rename_to_node'

        # outgoing geht aus SUMO raus (incoming muss gleich sein)
        if mode == 'outgoing_sumo':
            # existiert ein Knoten im MATSim Netzwerk, welcher vergleichbar ist?

            coords_node_sumo = self._net.nodes.get_row(sumo_node)['coords']
            closest_matsim_node, distance = self._matsim.net.nodes.get_closest(coords_node_sumo)
            # hier kann es nur eine geben (per Definition!)
            incoming_sumo = self._net.edges.select_ids(self._net.edges.ids_tonode.value == sumo_node)

            if distance < dist_node_assignment:

                incoming_matsim = self._matsim.net.edges.select_ids(
                    self._matsim.net.edges.ids_tonode.value == closest_matsim_node)

                return_it = False
                corresponding_matsim_edge = None

                for matsim_edge in incoming_matsim:
                    if self.check_same_dir(incoming_sumo[0], matsim_edge) and not return_it:
                        return_it = True
                        corresponding_matsim_edge = matsim_edge
                    elif self.check_same_dir(incoming_sumo[0], matsim_edge) and return_it:
                        return_it = False
                        corresponding_matsim_edge = None

                if return_it:
                    return closest_matsim_node,  [], 'rename_matsim_node'

            # liegt der Knoten nahe einer MATSim Kante?
            ids_edge, dists = self._matsim.net.edges.get_closest_edge(coords_node_sumo, n_best=3)

            for id_edge, dist in zip(ids_edge, dists):
                if self.check_same_dir(incoming_sumo[0], id_edge) and dist < 40:

                    new_matsim_node = self._matsim.net.edges.get_row(id_edge)['ids_fromnode']

                    return new_matsim_node, [[id_edge, 'rename_from_node']], 'rename_from_node'

        if mode == 'multi_lane':
            # Diese Modus repräsentiert eine bidirektionale Kante
            # Find nearst
            coords_node_sumo = self._net.nodes.get_row(sumo_node)['coords']
            closest_matsim_node, distance = self._matsim.net.nodes.get_closest(coords_node_sumo)

            if distance < dist_node_assignment:

                return closest_matsim_node,  [], 'rename_matsim_node'

            # hier kann es nur eine geben (per Definition!)
            ids_edge, dists = self._matsim.net.edges.get_closest_edge(coords_node_sumo, n_best=3)

            incoming_sumo = self._net.edges.select_ids(self._net.edges.ids_tonode.value == sumo_node)

            best_1 = []

            for id_edge, dist in zip(ids_edge, dists):

                if self.check_same_dir(incoming_sumo[0], id_edge) and dist < 20:
                    # take best solution
                    if len(best_1) == 0:

                        new_matsim_node = self._matsim.net.edges.get_row(id_edge)['ids_fromnode']

                        best_1 = [new_matsim_node, id_edge, 'rename_from_node']

            # liegt der Knoten nahe einer MATSim Kante?
            ids_edge, dists = self._matsim.net.edges.get_closest_edge(coords_node_sumo, n_best=3)
            # hier kann es nur eine geben (per Definition!)
            out_going_sumo = self._net.edges.select_ids(self._net.edges.ids_fromnode.value == sumo_node)

            best_2 = []

            for id_edge, dist in zip(ids_edge, dists):
                if self.check_same_dir(out_going_sumo[0], id_edge) and dist < 20:
                    # take best solution
                    if len(best_2) == 0:

                        new_matsim_node = self._matsim.net.edges.get_row(id_edge)['ids_tonode']

                        best_2 = [new_matsim_node, id_edge, 'rename_to_node']

            if len(best_1) > 0 and len(best_2) > 0:
                if best_1[0] == best_2[0]:
                    return best_1[0], [[best_1[1], best_1[2]], [best_2[1], best_2[2]]], 'multi_lane_rename'

    def find_suitable_matsim(self, start_point, end_point, vector_set_start, vector_set_end, search_beginning=True):

        radius_start = 10
        cosine_threshold = 0.9
        ortho_dist = 5
        ortho_cosine_threshold = 0.8
        ortho_tol = 0.01
        tol_node = 1

        if search_beginning:

            # Suche innerhalb der Knoten -> Eingrenzung
            euclidean_distances = np.linalg.norm(vector_set_start - start_point, axis=1)
        else:
            euclidean_distances = np.linalg.norm(vector_set_end - end_point, axis=1)
            # Cosine similarities

        vector_set = vector_set_end - vector_set_start
        target_vector = end_point - start_point

        dot_product = np.dot(vector_set, target_vector)
        norm_vector = np.linalg.norm(target_vector)
        norm_array = np.linalg.norm(vector_set, axis=1)
        cosine_similarities = dot_product / (norm_vector * norm_array)

        counter = 0
        return_index = None
        stop_searching = False

        indices_less_than = np.where(euclidean_distances < radius_start)[0]

        modified_similarities = np.zeros_like(euclidean_distances) - 1

        modified_similarities[indices_less_than] = cosine_similarities[indices_less_than]

        sorted_results = np.argsort(modified_similarities)[::-1]

        index = 0

        while modified_similarities[sorted_results[index]] > 0:

            # hat ein Knoten zwei ähnliche Ausgänge? -> Ich erwarte eine eindeutige Zuordnung
            # Kleiner Suchradius (circa eine Straßenbreite) und eine kleine Cosine_Similarity -> Hier ist nun die Vektorlänge egal -> Wenn es mehrere Lösungen gibt, dann wird diese Ignoriert!

            if euclidean_distances[sorted_results[index]] < radius_start and cosine_similarities[sorted_results[index]] > cosine_threshold:

                if return_index == None:

                    return_index = sorted_results[index]

                else:
                    if isinstance(return_index, list):
                        return_index.append(sorted_results[index])
                    else:
                        return_index = [return_index, sorted_results[index]]

            index += 1

        # Mehrere Kandidaten gefunden -> nehme den ähnlichsten!
        if isinstance(return_index, list):
            # invertiere die Logik
            if not search_beginning:
                # Suche innerhalb der Knoten -> Eingrenzung
                euclidean_distances = np.linalg.norm(vector_set_start - start_point, axis=1)
            else:
                euclidean_distances = np.linalg.norm(vector_set_end - end_point, axis=1)

            modified_distances = np.zeros_like(euclidean_distances) + 10000

            for index in return_index:

                modified_distances[index] = euclidean_distances[index]

            sorted_results = np.argsort(modified_similarities)[::-1]

            return_index = sorted_results[0]

        return return_index
