# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    results.py
# @author  Joerg Schweizer
# @date   2012


import os
import sys
import string
import types
from xml.sax import saxutils, parse, handler  # , make_parser
from collections import OrderedDict
import numpy as np


from coremodules.modules_common import *

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.geometry import *

from agilepy.lib_base.processes import Process, CmlMixin, ff, call, P
from coremodules.network.network import SumoIdsConf
from coremodules.demand.demandbase import RouteCounter, RouteReader


def load_results(filepath, parent=None, logger=None):
    # typically parent is the SIMULATION
    results = cm.load_obj(filepath, parent=parent)
    if logger is not None:
        results.set_logger(logger)
    return results


class Connectionresults(am.ArrayObjman):
    def __init__(self, ident, parent, trips, edges,
                 is_add_default=True,
                 name='Connection results',
                 info='Table with simulation results for each used connection. Consider only the finished trips',
                 **kwargs):

        self._init_constants()

        self._trips = trips
        self._edges = edges
        self._vtypes = trips.parent.vtypes

        self._init_objman(ident=ident,
                          parent=parent,  # main results object
                          info=info,
                          name=name,
                          **kwargs)

        connections = edges.parent.connections
        self.add_col(am.IdsArrayConf('ids_connection', connections,
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID connection',
                                     info='ID of connection.',
                                     ))

        self.add_col(am.ArrayConf('total_flows', default=0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total flows',
                                  unit='Veich.',
                                  info='Total flow in the connection.',
                                  ))

        self.add_col(am.ArrayConf('total_od_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total OD flows',
                                  unit='Veich.',
                                  info='Total flow in the connection from OD trips.',
                                  ))

        self.add_col(am.ArrayConf('total_vp_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total vp flows',
                                  unit='Veich.',
                                  info='Total flow in the connection from virtual population.',
                                  ))

        self.add_col(am.ArrayConf('total_ptline_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total pt flows',
                                  unit='Veich.',
                                  info='Total public transport flow in the connection.',
                                  ))

        self.add_col(am.ArrayConf('total_car_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total car flows',
                                  unit='Veich.',
                                  info='Total car flow in the connection, taking into account all the od car and iauto.',
                                  ))

        self.add_col(am.ArrayConf('total_bike_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total bike flows',
                                  unit='Veich.',
                                  info='Total bike flow in the connection, taking into account all the od bike and ibike.',
                                  ))

        self.add_col(am.ArrayConf('total_moto_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total moto flows',
                                  unit='Veich.',
                                  info='Total moto flow in the connection, taking into account all the od moto and imoto.',
                                  ))

        self.add_col(am.ArrayConf('total_iauto_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total iauto flows',
                                  unit='Veich.',
                                  info='Total iauto flow in the connection.',
                                  ))

        self.add_col(am.ArrayConf('total_ibike_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total ibike flows',
                                  unit='Veich.',
                                  info='Total ibike flow in the connection.',
                                  ))

        self.add_col(am.ArrayConf('total_imoto_flows', 0,
                                  dtype=np.float32,
                                  groupnames=['results'],
                                  name='Total imoto flows',
                                  unit='Veich.',
                                  info='Total imoto flow in the connection.',
                                  ))

    def _init_constants(self):
        self._is_keep_right_car = False
        self._is_keep_right_bus = True
        self._is_keep_right_bike = True
        self._is_keep_right_moto = False
        self._is_keep_right_taxi = False

    def evaluate_results(self, sumo, datapaths):
        routeresults = self.parent.routeresults
        ids_routeres = routeresults.get_ids()
        # Consider warmup time
        ids_routeres = ids_routeres[(routeresults.times_depart[ids_routeres] > sumo.time_warmup)]
#        ids_routeres = ids_routeres[700:1000]
        # print ids_routeres
        ids_routeres_od = ids_routeres[(routeresults.ids_trip[ids_routeres] > 0)]
        ids_od_modes = self._trips.parent.vtypes.ids_mode[self._trips.ids_vtype[routeresults.ids_trip[ids_routeres_od]]]
        ids_routeres_od_auto = ids_routeres_od[(
            ids_od_modes == self._edges.parent.modes.names.get_id_from_index('passenger'))]
        ids_routeres_od_taxi = ids_routeres_od[(
            ids_od_modes == self._edges.parent.modes.names.get_id_from_index('taxi'))]
        ids_routeres_od_car = list(ids_routeres_od_auto)+list(ids_routeres_od_taxi)
        ids_routeres_od_bike = ids_routeres_od[(
            ids_od_modes == self._edges.parent.modes.names.get_id_from_index('bicycle'))]
        ids_routeres_od_moto = ids_routeres_od[(
            ids_od_modes == self._edges.parent.modes.names.get_id_from_index('motorcycle'))]

        ids_routeres_iauto = ids_routeres[(routeresults.ids_iauto[ids_routeres] > 0)]
        ids_routeres_ibike = ids_routeres[(routeresults.ids_ibike[ids_routeres] > 0)]
        ids_routeres_imoto = ids_routeres[(routeresults.ids_imoto[ids_routeres] > 0)]
        ids_routeres_vp = list(ids_routeres_iauto) + list(ids_routeres_ibike) + list(ids_routeres_imoto)
        ids_routeres_ptlines = ids_routeres[(routeresults.ids_ptline[ids_routeres] > 0)]
        ids_routeres_car = list(ids_routeres_od_car) + list(ids_routeres_iauto)
        ids_routeres_bike = list(ids_routeres_od_bike) + list(ids_routeres_ibike)
        ids_routeres_moto = list(ids_routeres_od_moto) + list(ids_routeres_imoto)

        ids_edges = routeresults.ids_edges[ids_routeres]
        ids_edges_od_car = routeresults.ids_edges[ids_routeres_od_car]
        ids_edges_od = routeresults.ids_edges[ids_routeres_od]
        ids_edges_iauto = routeresults.ids_edges[ids_routeres_iauto]
        ids_edges_ibike = routeresults.ids_edges[ids_routeres_ibike]
        ids_edges_imoto = routeresults.ids_edges[ids_routeres_imoto]
        ids_edges_car = routeresults.ids_edges[ids_routeres_car]
        ids_edges_bike = routeresults.ids_edges[ids_routeres_bike]
        ids_edges_moto = routeresults.ids_edges[ids_routeres_moto]
        ids_edges_ptline = routeresults.ids_edges[ids_routeres_ptlines]
        ids_edges_vp = routeresults.ids_edges[ids_routeres_vp]
# print 'Routes:', len(ids_edges)
# print 'Car routes:', len(ids_edges_car)
# print 'OD Routes:', len(ids_edges_od)
# print 'VP routes:', len(ids_edges_vp)
# print 'iauto route:', len(ids_edges_iauto)
# print 'ibike routes:', len(ids_edges_ibike)
# print 'imoto routes:', len(ids_edges_imoto)

        modes_od = []
        modes_iauto = []
        modes_ibike = []
        modes_imoto = []
        modes_car = []
        modes_bike = []
        modes_moto = []
        modes_ptline = []
        modes_vp = []
        modes_tot = []

        for trip in ids_routeres_od:
            mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[self._trips.ids_vtype[routeresults.ids_trip[trip]]]]
            modes_od.append(mode)

        for trip, id_iauto in zip(ids_routeres_iauto, routeresults.ids_iauto[ids_routeres_iauto]):
            vtype = self._trips.parent.virtualpop.get_iautos().ids_vtype[id_iauto]
            mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            modes_iauto.append(mode)

        for trip, id_ibike in zip(ids_routeres_ibike, routeresults.ids_ibike[ids_routeres_ibike]):
            vtype = self._trips.parent.virtualpop.get_ibikes().ids_vtype[id_ibike]
            mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            modes_ibike.append(mode)

        for trip, id_imoto in zip(ids_routeres_imoto, routeresults.ids_imoto[ids_routeres_imoto]):
            vtype = self._trips.parent.virtualpop.get_imotos().ids_vtype[id_imoto]
            mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            modes_imoto.append(mode)

        for trip in ids_routeres_car:
            if routeresults.ids_iauto[trip] > 0:
                id_iauto = routeresults.ids_iauto[trip]
                vtype = self._trips.parent.virtualpop.get_iautos().ids_vtype[id_iauto]
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            elif routeresults.ids_trip[trip] > 0:
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[self._trips.ids_vtype[routeresults.ids_trip[trip]]]]
            modes_car.append(mode)

        for trip in ids_routeres_bike:
            if routeresults.ids_ibike[trip] > 0:
                id_ibike = routeresults.ids_ibike[trip]
                vtype = self._trips.parent.virtualpop.get_ibikes().ids_vtype[id_ibike]
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            elif routeresults.ids_trip[trip] > 0:
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[self._trips.ids_vtype[routeresults.ids_trip[trip]]]]
            modes_bike.append(mode)

        for trip in ids_routeres_moto:
            if routeresults.ids_imoto[trip] > 0:
                id_imoto = routeresults.ids_imoto[trip]
                vtype = self._trips.parent.virtualpop.get_imotos().ids_vtype[id_imoto]
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            elif routeresults.ids_trip[trip] > 0:
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[self._trips.ids_vtype[routeresults.ids_trip[trip]]]]
            modes_moto.append(mode)

        for trip in ids_routeres_ptlines:
            mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[self._trips.parent.ptlines.ids_vtype[routeresults.ids_ptline[trip]]]]
            modes_ptline.append(mode)

        for trip in ids_routeres_vp:
            if routeresults.ids_iauto[trip] > 0:
                id_iauto = routeresults.ids_iauto[trip]
                vtype = self._trips.parent.virtualpop.get_iautos().ids_vtype[id_iauto]
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            elif routeresults.ids_ibike[trip] > 0:
                id_ibike = routeresults.ids_ibike[trip]
                vtype = self._trips.parent.virtualpop.get_ibikes().ids_vtype[id_ibike]
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            elif routeresults.ids_imoto[trip] > 0:
                id_imoto = routeresults.ids_imoto[trip]
                vtype = self._trips.parent.virtualpop.get_imotos().ids_vtype[id_imoto]
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]

            modes_vp.append(mode)

        for trip in ids_routeres:
            if routeresults.ids_iauto[trip] > 0 or routeresults.ids_ibike[trip] > 0 or routeresults.ids_imoto[trip] > 0:
                if routeresults.ids_iauto[trip] > 0:
                    id_iauto = routeresults.ids_iauto[trip]
                    vtype = self._trips.parent.virtualpop.get_iautos().ids_vtype[id_iauto]
                    mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
                elif routeresults.ids_ibike[trip] > 0:
                    id_ibike = routeresults.ids_ibike[trip]
                    vtype = self._trips.parent.virtualpop.get_ibikes().ids_vtype[id_ibike]
                    mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
                elif routeresults.ids_imoto[trip] > 0:
                    id_imoto = routeresults.ids_imoto[trip]
                    vtype = self._trips.parent.virtualpop.get_imotos().ids_vtype[id_imoto]
                    mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[vtype]]
            elif routeresults.ids_trip[trip] > 0:
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[self._trips.ids_vtype[routeresults.ids_trip[trip]]]]
            elif routeresults.ids_ptline[trip] > 0:
                mode = self._edges.parent.modes.names[self._trips.parent.vtypes.ids_mode[self._trips.parent.ptlines.ids_vtype[routeresults.ids_ptline[trip]]]]
            else:
                print 'WARNING: there is a not considered route typology'
            modes_tot.append(mode)

        print 'n route', len(modes_tot)
        if len(modes_tot) != len(ids_edges):
            print 'WARNING: modes and ids_edges have a different length - total flow'
        print 'n car routes', len(modes_car)
        if len(modes_car) != len(ids_edges_car):
            print 'WARNING: modes and ids_edges have a different length - car'
        print 'n bike routes', len(modes_bike)
        if len(modes_bike) != len(ids_edges_bike):
            print 'WARNING: modes and ids_edges have a different length - bike'
        print 'n moto routes', len(modes_moto)
        if len(modes_moto) != len(ids_edges_moto):
            print 'WARNING: modes and ids_edges have a different length - moto'
        print 'n od routes', len(modes_od)
        if len(modes_od) != len(ids_edges_od):
            print 'WARNING: modes and ids_edges have a different length - od'
        print 'n od car routes', modes_od.count('passenger')
        print 'n od taxi routes', modes_od.count('taxi')
        print 'n od bike routes', modes_od.count('bicycle')
        print 'n od moto routes', modes_od.count('motorcycle')
        print 'n vp routes',  len(modes_vp)
        if len(modes_vp) != len(ids_edges_vp):
            print 'WARNING: modes and ids_edges have a different length - vp'
        print 'n iauto routes',  len(modes_iauto)
        if len(modes_iauto) != len(ids_edges_iauto):
            print 'WARNING: modes and ids_edges have a different length - iauto'
        print 'n ibike routes',  len(modes_ibike)
        if len(modes_ibike) != len(ids_edges_ibike):
            print 'WARNING: modes and ids_edges have a different length - ibike'
        print 'n imoto routes',  len(modes_imoto)
        if len(modes_imoto) != len(ids_edges_imoto):
            print 'WARNING: modes and ids_edges have a different length - imoto'
        print 'n pt routes',  len(modes_ptline)
        if len(modes_ptline) != len(ids_edges_ptline):
            print 'WARNING: modes and ids_edges have a different length - bus'

        ids_connections_tot, flows_tot = self.evaluate_connection_flows('tot', modes_tot, ids_edges)
        self.add_rows(ids_connection=ids_connections_tot, total_flows=flows_tot)

        ids_connections_car, flows_car = self.evaluate_connection_flows('car', modes_car, ids_edges_car)
        ids_connections_bike, flows_bike = self.evaluate_connection_flows('bike', modes_bike, ids_edges_bike)
        ids_connections_moto, flows_moto = self.evaluate_connection_flows('moto', modes_moto, ids_edges_moto)
        ids_connections_od, flows_od = self.evaluate_connection_flows('od', modes_od, ids_edges_od)
        ids_connections_vp, flows_vp = self.evaluate_connection_flows('vp', modes_vp, ids_edges_vp)
        ids_connections_iauto, flows_iauto = self.evaluate_connection_flows('iauto', modes_iauto, ids_edges_iauto)
        ids_connections_ibike, flows_ibike = self.evaluate_connection_flows('ibike', modes_ibike, ids_edges_ibike)
        ids_connections_imoto, flows_imoto = self.evaluate_connection_flows('imoto', modes_imoto, ids_edges_imoto)
        ids_connections_ptline, flows_ptline = self.evaluate_connection_flows('pt', modes_ptline, ids_edges_ptline)

        for connection, flow in zip(ids_connections_car, flows_car):
            self.total_car_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_bike, flows_bike):
            self.total_bike_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_moto, flows_moto):
            self.total_moto_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_od, flows_od):
            self.total_od_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_vp, flows_vp):
            self.total_vp_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_iauto, flows_iauto):
            self.total_iauto_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_ibike, flows_ibike):
            self.total_ibike_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_imoto, flows_imoto):
            self.total_imoto_flows[self.ids_connection.get_id_from_index(connection)] = flow

        for connection, flow in zip(ids_connections_ptline, flows_ptline):
            self.total_ptline_flows[self.ids_connection.get_id_from_index(connection)] = flow

    def evaluate_connection_flows(self, ident, modes, routes):
        '''
        Evaluate connection flows from a list of routes, each composed by a list of edges.
        Sometimes many maneuvers connect the same couple of edges: you can 
        choose to either keep the rightmost lane or split homogeneously the flow in 
        the available maneuvers, between the connections allowed for the specific MODE.
        Return both the traveled connections and the related flows
        '''
        print 'analyzing', ident, 'routes'
        edges = self._edges
        lanes = edges.parent.lanes
        connections = edges.parent.connections
        ids_connection = connections.get_ids()
        connection_flows = np.zeros(len(ids_connection)+1, dtype=np.float32)
        ids_traveled_connections = []
        n_wrong_connections = 0
        n_good_connections = 0
        n_wrong_connections_moto = 0
        n_wrong_connections_auto = 0
        n_wrong_connections_bike = 0
        n_wrong_connections_bus = 0
        for route, mode in zip(routes, modes):
            for i in range(len(route)-1):
                edge_from = route[i]
                edge_to = route[i+1]
                ids_lane_from_all = edges.ids_lanes[edge_from]
                ids_lane_to_all = edges.ids_lanes[edge_to]
                ids_lane_from = []
                ids_lane_to = []
# print 'ids_lane_from_all', ids_lane_from_all
# print 'ids_lane_to_all', ids_lane_to_all
                for id_lane in ids_lane_from_all:
                    if len(edges.parent.lanes.ids_modes_allow[id_lane]) > 0:
                        if mode in self._edges.parent.modes.names[edges.parent.lanes.ids_modes_allow[id_lane]]:
                            ids_lane_from.append(id_lane)
                    elif len(edges.parent.lanes.ids_modes_disallow[id_lane]) > 0:
                        if mode not in self._edges.parent.modes.names[edges.parent.lanes.ids_modes_disallow[id_lane]]:
                            ids_lane_from.append(id_lane)
                    elif not len(edges.parent.lanes.ids_modes_allow[id_lane]) > 0 and not len(edges.parent.lanes.ids_modes_disallow[id_lane]) > 0:
                        ids_lane_from.append(id_lane)
# print 'allow from lane', id_lane, self._edges.parent.modes.names[edges.parent.lanes.ids_modes_allow[id_lane]]
# print 'disallow from lane', id_lane, self._edges.parent.modes.names[edges.parent.lanes.ids_modes_disallow[id_lane]]
                for id_lane in ids_lane_to_all:
                    if len(edges.parent.lanes.ids_modes_allow[id_lane]) > 0:
                        if mode in self._edges.parent.modes.names[edges.parent.lanes.ids_modes_allow[id_lane]]:
                            ids_lane_to.append(id_lane)
                    elif len(edges.parent.lanes.ids_modes_disallow[id_lane]) > 0:
                        if mode not in self._edges.parent.modes.names[edges.parent.lanes.ids_modes_disallow[id_lane]]:
                            ids_lane_to.append(id_lane)
                    elif not len(edges.parent.lanes.ids_modes_allow[id_lane]) > 0 and not len(edges.parent.lanes.ids_modes_disallow[id_lane]) > 0:
                        ids_lane_to.append(id_lane)
# print 'ids_lane_from', ids_lane_from
# print 'ids_lane_to', ids_lane_to

                possible_connections_from = []
                possible_connections_to = []
                connections_list = []
                for id_lane_from in ids_lane_from:
                    poss_connections_from = ids_connection[(connections.ids_fromlane[ids_connection] == id_lane_from)]
                    for poss_connection_from in poss_connections_from:
                        possible_connections_from.append(poss_connection_from)
                for id_lane_to in ids_lane_to:
                    poss_connections_to = ids_connection[(connections.ids_tolane[ids_connection] == id_lane_to)]
                    for poss_connection_to in poss_connections_to:
                        possible_connections_to.append(poss_connection_to)
                for element in possible_connections_from:
                    if element in possible_connections_to:
                        connections_list.append(element)

                if connections_list == []:
                    print 'Warning: no connections between a couple of successive edges for mode', mode
                    print 'ids_lane_from_all', ids_lane_from_all
                    print 'ids_lane_to_all', ids_lane_to_all
                    n_wrong_connections += 1
                    if mode == 'motorcycle':
                        n_wrong_connections_moto += 1
                    if mode == 'passenger':
                        n_wrong_connections_auto += 1
                    if mode == 'bicycle':
                        n_wrong_connections_bike += 1
                    if mode == 'bus':
                        n_wrong_connections_bus += 1

                else:

                    if mode == 'motorcycle':
                        keep_right = self._is_keep_right_moto
                    elif mode == 'passenger':
                        keep_right = self._is_keep_right_car
                    elif mode == 'bicycle':
                        keep_right = self._is_keep_right_bike
                    elif mode == 'bus':
                        keep_right = self._is_keep_right_bus
                    elif mode == 'taxi':
                        keep_right = self._is_keep_right_taxi
                    else:
                        print 'WARNING - Not recognized mode'
                        keep_right = True

                    n_good_connections += 1
                    if keep_right == False:
                        for connection in connections_list:
                            if connection not in ids_traveled_connections:
                                ids_traveled_connections.append(connection)
                            connection_flows[connection] += 1./len(connections_list)

                    elif keep_right == True:
                        lane_index_connections = lanes.indexes[connections.ids_fromlane[connections_list]
                                                               ] + lanes.indexes[connections.ids_tolane[connections_list]]
                        connection = connections_list[np.argmin(lane_index_connections)]
                        if connection not in ids_traveled_connections:
                            ids_traveled_connections.append(connection)
                        connection_flows[connection] += 1.
# print ids_traveled_connections, connection_flows[ids_traveled_connections]
        print 'n_good_connections:', n_good_connections
        print 'n_wrong_connections:', n_wrong_connections
# print 'n_wrong_connections_moto:', n_wrong_connections_moto
# print 'n_wrong_connections_auto:', n_wrong_connections_auto
# print 'n_wrong_connections_bike:', n_wrong_connections_bike
# print 'n_wrong_connections_bus:', n_wrong_connections_bus
##
        return ids_traveled_connections, connection_flows[ids_traveled_connections]


class Routeresults(am.ArrayObjman):
    def __init__(self, ident, parent, trips, edges, datapathkey='routesdatapath',
                 is_add_default=True,
                 name='Route results',
                 info='Table with simulation results for each route made.Consider only the finished trips',
                 **kwargs):
        self._trips = trips
        self._init_objman(ident=ident,
                          parent=parent,  # main results object
                          info=info,
                          name=name,
                          **kwargs)

        self.add(cm.AttrConf('datapathkey', datapathkey,
                             groupnames=['_private'],
                             name='data pathkey',
                             info="key of data path",
                             ))

        self.add_col(am.IdsArrayConf('ids_trip', trips,
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID trip',
                                     info='ID of trip.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_iauto', trips.parent.virtualpop.get_iautos(),
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID iauto',
                                     info='ID of iauto trip from the virtual population.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_imoto', trips.parent.virtualpop.get_imotos(),
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID imoto',
                                     info='ID of trip imoto from the virtual population.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_ibike', trips.parent.virtualpop.get_ibikes(),
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID ibike',
                                     info='ID of trip ibike from the virtual population.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_ptline', trips.parent.ptlines,
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID ptline',
                                     info='ID of the public transportation line.',
                                     ))

        attrinfos = OrderedDict([

            ('times_depart',   {'name': 'Time depart', 'xmltag': 'times_depart',   'unit': 's',
                                'default': 0.0, 'info': 'Depart time', 'groupnames': ['routedata']}),

            ('times_arrival',   {'name': 'Time arrival', 'xmltag': 'times_arrival',   'unit': 's',
                                 'default': 0.0, 'info': 'Arrival time', 'groupnames': ['routedata']}),

            ('type',   {'name': 'Vehicle type', 'xmltag': 'type',   'unit': None,
                        'default': np.object, 'info': 'Vehicle type', 'groupnames': ['routedata']}),

            ('ids_edge_depart',   {'name': 'ID Edge depart', 'xmltag': 'ids_edge_depart',    'unit': None,
                                   'default': 0, 'info': 'Depart edege', 'groupnames': ['routedata']}),

            ('ids_edge_arrival',   {'name': 'ID Edge arrival', 'xmltag': 'ids_edge_arrival',   'unit': None,
                                    'default': 0, 'info': 'Arrival edge', 'groupnames': ['routedata']}),

            ('inds_lane_depart',   {'name': 'ID lane depart', 'xmltag': 'inds_lane_depart',   'unit': None,
                                    'default': 0, 'info': 'Depart lane ', 'groupnames': ['routedata']}),

            ('positions_depart',   {'name': 'Position depart', 'xmltag': 'positions_depart',   'unit': 'm',
                                    'default': 0.0, 'info': 'Position depart', 'groupnames': ['routedata']}),

            ('speeds_depart',   {'name': 'Speed depart', 'xmltag': 'speeds_depart',   'unit': 'm/s',
                                 'default': 0.0, 'info': 'Depart speed', 'groupnames': ['routedata']}),

            ('inds_lane_arrival',   {'name': 'ID Lane arrival', 'xmltag': 'inds_lane_arrival',   'unit': None,
                                     'default': 0, 'info': 'Arrival lane', 'groupnames': ['routedata']}),

            ('positions_arrival',   {'name': 'Position arrival', 'xmltag': 'positions_arrival',    'unit': 'm',
                                     'default': 0.0, 'info': 'Arrival position', 'groupnames': ['routedata']}),

            ('speeds_arrival',   {'name': 'Speed arrival', 'xmltag': 'speeds_arrival', 'unit': 'm/s',
                                  'default': 0.0, 'info': 'Arrival speed', 'groupnames': ['routedata']}),
        ])

        for attrname, kwargs in attrinfos.iteritems():
            self.add_resultattr(attrname, **kwargs)

        # this is special for route info
        self.add_col(am.IdlistsArrayConf('ids_edges', edges,
                                         name='Edge IDs',
                                         groupnames=['routedata'],
                                         info='List of edge IDs constituting the actually taken route.',
                                         xmltag='edges',
                                         ))

    def add_resultattr(self, attrname, **kwargs):

        # default cannot be kwarg
        default = kwargs['default']
        del kwargs['default']
        if kwargs.has_key('groupnames'):
            kwargs['groupnames'].append('results')
        else:
            kwargs['groupnames'] = ['results']

        self.add_col(am.ArrayConf(attrname, default, **kwargs))

    # def import_routesdata(self, filepath):
    #    # TODO
    #    pass

    # def import_tripdata(self, filepath):
    #    #print 'import_tripdata',filepath,self.get_group('tripdata')
    #    self.import_sumoxml(filepath,self.get_group('tripdata'))

    def import_xml(self, sumo, datapaths):
        datapathkey = self.datapathkey.get_value()
        if datapaths.has_key(datapathkey):
            self.import_sumoxml(datapaths[datapathkey], sumo, self.get_group('routedata'))

    def import_sumoxml(self, filepath, sumo, attrconfigs):
        element = 'route'
        # print 'Tripresults.import_sumoxml',self.get_trips().ident, element,filepath
        #id_type = 'edge',
        #reader = 'interval',

        results = read_routeresult(filepath, sumo, self._trips, element, attrconfigs)
        ids_raw = results['ids_sumo']

        # print '  ids_raw',ids_raw
        # print '  results.keys()',results.keys()
        # print '  results',results

        # this procedure is necessary to create new result ids only
        # for trips that are not yet in the database
        n = len(ids_raw[(ids_raw != '')])
        # print '  number of rows',n
        ind_range = np.arange(n, dtype=np.int32)
        ids = np.zeros(n, dtype=np.int32)
        for i in ind_range:

            id_trip = ids_raw[i]
# if self.ids_trip.has_index(id_trip):
##                ids[i] =  self.ids_trip.get_id_from_index(id_trip)
# else:
            if 'iauto' in id_trip:
                ids[i] = self.add_row(ids_iauto=int(id_trip.split('.')[1]))
# print int(id_trip.split('.')[1]), id_trip.split('.')[1], id_trip
            elif 'imoto' in id_trip:
                ids[i] = self.add_row(ids_imoto=int(id_trip.split('.')[1]))
            elif 'ibike' in id_trip:
                ids[i] = self.add_row(ids_ibike=int(id_trip.split('.')[1]))
            elif 'ptline' in id_trip:

                ids[i] = self.add_row(ids_ptline=int(id_trip.split('.')[1]))
            else:
                # print 'WARNING:', id_trip, 'not recognized trip'
                ids[i] = self.add_row(ids_trip=int(id_trip))

        for attrconfig in attrconfigs:
            attrname = attrconfig.attrname
            if attrname != 'ids_sumo':
                default = attrconfig.get_default()
                if type(default) in (types.IntType, types.LongType):
                    conversion = 'i'  # int
                    values_attr = np.zeros(n, int)
                elif type(default) in (types.FloatType, types.ComplexType):
                    conversion = 'f'  # float
                    values_attr = np.zeros(n, dtype=np.float32)
                if type(default) in (types.BooleanType,):
                    conversion = 'b'  # str
                    values_attr = np.zeros(n, dtype=np.bool)
                else:
                    conversion = 's'  # str
                    values_attr = np.zeros(n, dtype=np.object)

                is_average = False
                if hasattr(attrconfig, 'is_average'):
                    is_average = attrconfig.is_average
                # this is a tricky way to read the data stored in
                # dictionarie into array tructures as used in results
                # problem is that not all dictionaries have all ids
                for i in ind_range:
                    # print '  check',ids_raw[i],results[attrname].get(ids_raw[i],'No data')

                    val = results[attrname][i]
                    # print val, results[attrname][i]
                    if conversion == 'i':
                        val = int(val)
                    else:
                        values_attr[i] = val
                    # print '   ',i,ids[i],attrname,conversion,val,type(val),is_average
                    values_attr[i] = val
                if attrname == 'ids_edges':
                    for i in ind_range:
                        values_attr[i] = results[attrname][i]
                # print '  attrname',attrname
                # print '  ids',type(ids),ids
                # print '  values_attr',type(values_attr),values_attr
                attrconfig.set(ids, values_attr)


class Tripresults(am.ArrayObjman):
    def __init__(self, ident, parent, trips, edges,  # datapathkey = 'tripdatapath',
                 is_add_default=True,
                 name='Trip results',
                 info='Table with simulation results for each trip made.',
                 **kwargs):

        self._init_objman(ident=ident,
                          parent=parent,  # main results object
                          info=info,
                          name=name,
                          **kwargs)

        # self.add(cm.AttrConf(  'datapathkey',datapathkey,
        #                        groupnames = ['_private'],
        #                        name = 'data pathkey',
        #                        info = "key of data path",
        #                        ))

        self.add_col(am.IdsArrayConf('ids_trip', trips,
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID trip',
                                     info='ID of trip.',
                                     ))
        attrinfos = OrderedDict([
            ('duration', {'name': 'Duration', 'xmltag': 'duration',    'unit': 's',
                          'default': 0, 'info': 'Trip duration', 'groupnames': ['tripdata']}),
            ('depart',   {'name': 'Dep. time', 'xmltag': 'depart',   'unit': 's',     'default': 0,
                          'info': 'Departure time', 'groupnames': ['tripdata'], 'is_average': True}),
            ('arrival',   {'name': 'Arr. time', 'xmltag': 'arrival',   'unit': 's',    'default': 0,
                           'info': 'Arrival time', 'groupnames': ['tripdata'], 'is_average': True}),
            ('routeLength',   {'name': 'Length', 'xmltag': 'routeLength',    'unit': 'm',
                               'default': 0.0, 'info': 'Route length', 'groupnames': ['tripdata']}),
            ('departdelays',   {'name': 'Dep. delay', 'xmltag': 'departDelay',   'unit': 's',    'default': 0,
                                'info': 'The time the vehicle had to wait before it could start his journey', 'groupnames': ['tripdata'], 'is_average': True}),
            ('waittimes',   {'name': 'Wait time', 'xmltag': 'waitingTime',   'unit': 's',    'default': 0,
                             'info': 'The time in which the vehicle speed was below 0.1m/s (scheduled stops do not count) ', 'groupnames': ['tripdata'], 'is_average': True}),
            ('stoptimes',   {'name': 'Stop time', 'xmltag': 'stopTime',   'unit': 's',    'default': 0,
                             'info': 'The time in which the vehicle was taking a planned stop', 'groupnames': ['tripdata'], 'is_average': True}),
            ('timelosses',   {'name': 'Timeloss', 'xmltag': 'timeLoss',   'unit': 's',    'default': 0,
                              'info': 'The time lost due to driving below the ideal speed. (ideal speed includes the individual speedFactor; slowdowns due to intersections etc. will incur timeLoss, scheduled stops do not count)', 'groupnames': ['tripdata'], 'is_average': True}),
            ('departPos',   {'name': 'depart pos', 'xmltag': 'departPos',   'unit': 'm',
                             'default': 0.0, 'info': 'depart position', 'groupnames': ['tripdata'], 'is_average': True}),
            ('arrivalPos',   {'name': 'arrival pos', 'xmltag': 'arrivalPos',    'unit': 'm',
                              'default': 0.0, 'info': 'arrival position', 'groupnames': ['tripdata'], 'is_average': True}),
            ('speedfactors',   {'name': 'Speedfactor', 'xmltag': 'speedFactor',    'default': 0.0,
                                'info': 'The individual speed factor of the vehicle (possibly drawn from a speed distribution at the start of the simulation)', 'groupnames': ['tripdata'], }),
            ('are_vaporized',   {'name': 'vaporized', 'xmltag': 'vaporized',    'default': False,
                                 'info': 'Whether the vehicle was removed from the simulation before reaching its destination', 'groupnames': ['tripdata'], }),
            ('waitSteps',   {'name': 'wait steps', 'xmltag': 'waitingCount',   'unit': None,    'default': 0,
                             'info': 'Count of time steps, the vehicle has been waiting during its trip', 'groupnames': ['tripdata']}),
            ('rerouteNo',   {'name': 'reroute No', 'xmltag': 'rerouteNo',   'unit': None,
                             'default': 0, 'info': 'Number of re-routes', 'groupnames': ['tripdata']}),
            ('waitSteps',   {'name': 'wait steps', 'xmltag': 'waitSteps',   'unit': None,    'default': 0,
                             'info': 'Time steps, the vehicle has been waiting during its trip', 'groupnames': ['tripdata']}),
            ('consumptions', {'name': 'Consumption', 'xmltag': 'Consum',    'unit': 'Ws',
                              'default': 0.0, 'info': 'Electrical energy consumption', 'groupnames': ['electricenergydata']}),
            #('speeds_av',   {'name':'Average speeds', 'xmltag':'speed',   'unit':'m/s',     'default':0, 'info':'Average speed','groupnames':['tripdata'],'is_average' : True}),
        ])

        for attrname, kwargs in attrinfos.iteritems():
            self.add_resultattr(attrname, **kwargs)

        # this is special for route info
        self.add_col(am.IdlistsArrayConf('ids_edges', edges,
                                         name='Edge IDs',
                                         groupnames=['routeinfo'],
                                         info='List of edge IDs constituting the actually taken route.',
                                         xmltag='edges',
                                         ))

    def get_trips(self):
        return self.ids_trip.get_linktab()

    def add_resultattr(self, attrname, **kwargs):

        # default cannot be kwarg
        default = kwargs['default']
        del kwargs['default']
        if kwargs.has_key('groupnames'):
            kwargs['groupnames'].append('results')
        else:
            kwargs['groupnames'] = ['results']

        self.add_col(am.ArrayConf(attrname, default, **kwargs))

    # def import_routesdata(self, filepath):
    #    # TODO
    #    pass

    # def import_tripdata(self, filepath):
    #    #print 'import_tripdata',filepath,self.get_group('tripdata')
    #    self.import_sumoxml(filepath,self.get_group('tripdata'))

    def import_xml(self, sumo, datapaths):
        print 'Tripresults.import_xml datapaths', datapaths
        datapathkey = 'tripdatapath'
        if datapaths.has_key(datapathkey):
            self.import_tripdata_sumoxml(datapaths[datapathkey], sumo)

        #datapathkey = 'electricenergypath'
        # if datapaths.has_key(datapathkey):
        #    self.import_electricenergy_sumoxml(datapaths[datapathkey],sumo)

    def import_electricenergy_sumoxml_broke(self, filepath, sumo):
        element = 'vehicle'
        print 'Tripresults.import_electricenergy_sumoxml', self.get_trips().ident, element, filepath
        #id_type = 'edge',
        #reader = 'interval',
        attrconfigs = self.get_group('electricenergydata')
        # print 'import_sumoxml',element
        #id_type = 'edge',
        #reader = 'interval',
        ids_sumo, results, interval = read_interval2(
            filepath, sumo, element, attrconfigs)
        # print '  ids_sumo',ids_sumo
        # print '  results.keys()',results.keys()
        # print '  results',results
        # create ids for all colums
        # if fileinfo['id_type']=='edge':

        # this procedure is necessary to create new result ids only
        # for edges that are not yet in the database
        ids_sumotrip = self.ids_trip.get_linktab().ids_sumo
        n = len(ids_sumo)
        # print '  n',n
        ind_range = np.arange(n, dtype=np.int32)
        ids = np.zeros(n, dtype=np.int32)
        for i in ind_range:
            id_trip = ids_sumotrip.get_id_from_index(ids_sumo[i])
            if self.ids_trip.has_index(id_trip):
                ids[i] = self.ids_trip.get_id_from_index(id_edge)
            else:
                ids[i] = self.add_row(ids_trip=id_edge)

        # ids = self.add_row()# here the ids_sumo can be strings too
        # elif fileinfo['id_type']=='trip':
        #    ids = self.tripresults.add_rows_keyrecycle(keys = ids_sumo)#
        # print '  ids=',ids

        for attrconfig in attrconfigs:

            attrname = attrconfig.attrname

            default = attrconfig.get_default()
            if type(default) in (types.IntType, types.LongType):
                conversion = 'i'  # int
                values_attr = np.zeros(n, int)
            elif type(default) in (types.FloatType, types.ComplexType):
                conversion = 'f'  # float
                values_attr = np.zeros(n, float)
            else:
                conversion = 's'  # str
                values_attr = np.zeros(n, obj)

            is_average = False
            if hasattr(attrconfig, 'is_average'):
                is_average = attrconfig.is_average
            # print ' copy',attrname,'is_average',is_average
            # this is a tricky way to read the data stored in
            # dictionarie into array tructures as used in results
            # problem is that not all dictionaries have all ids
            for i in ind_range:
                if is_average:
                    valcum, num = results[attrname].get(ids_sumo[i], (default, 1))
                    val = valcum/float(num)  # average over measurements!
                else:
                    val = results[attrname].get(ids_sumo[i], default)

                if conversion == 'i':
                    val = int(val)
                # else:
                #    values_attr[i]=val
                # print '   attrname',attrname,conversion,val,is_average,type(val)
                values_attr[i] = val

            # print '  attrname',attrname
            # print '  ids',type(ids),ids
            # print '  values_attr',type(values_attr),values_attr
            attrconfig.set(ids, values_attr)

    def import_tripdata_sumoxml(self, filepath, sumo):
        element = 'tripinfo'
        print 'Tripresults.import_tripdata_sumoxml', self.get_trips().ident, 'element', element, filepath
        #id_type = 'edge',
        #reader = 'interval',
        attrconfigs = self.get_group('tripdata')
        ids_raw, results = read_tripresult(filepath, sumo, self.get_trips(), element, attrconfigs)

        # print '  ids_raw',ids_raw
        # print '  results.keys()',results.keys()
        # print '  results',results

        # this procedure is necessary to create new result ids only
        # for trips that are not yet in the database
        n = len(ids_raw)
        # print '  number of rows',n
        ind_range = np.arange(n, dtype=np.int32)
        ids = np.zeros(n, dtype=np.int32)
        for i in ind_range:

            id_trip = int(ids_raw[i])
            if self.ids_trip.has_index(id_trip):
                ids[i] = self.ids_trip.get_id_from_index(id_trip)
            else:
                ids[i] = self.add_row(ids_trip=id_trip)

        for attrconfig in attrconfigs:
            attrname = attrconfig.attrname
            default = attrconfig.get_default()
            if type(default) in (types.IntType, types.LongType):
                conversion = 'i'  # int
                values_attr = np.zeros(n, int)
            elif type(default) in (types.FloatType, types.ComplexType):
                conversion = 'f'  # float
                values_attr = np.zeros(n, dtype=np.float32)
            if type(default) in (types.BooleanType,):
                conversion = 'b'  # str
                values_attr = np.zeros(n, dtype=np.bool)
            else:
                conversion = 's'  # str
                values_attr = np.zeros(n, dtype=np.object)

            is_average = False
            if hasattr(attrconfig, 'is_average'):
                is_average = attrconfig.is_average
            # this is a tricky way to read the data stored in
            # dictionarie into array tructures as used in results
            # problem is that not all dictionaries have all ids
            for i in ind_range:
                # print '  check',ids_raw[i],results[attrname].get(ids_raw[i],'No data')
                if is_average:
                    valcum, num = results[attrname].get(ids_raw[i], (default, 1))
                    val = valcum/float(num)  # average!
                else:
                    val = results[attrname].get(ids_raw[i], default)

                if conversion == 'i':
                    val = int(val)
                else:
                    values_attr[i] = val
                # print '   ',i,ids[i],attrname,conversion,val,type(val),is_average
                values_attr[i] = val

            # print '  attrname',attrname
            # print '  ids',type(ids),ids
            # print '  values_attr',type(values_attr),values_attr
            attrconfig.set(ids, values_attr)


class Edgeresults(am.ArrayObjman):
    def __init__(self, parent, edges, is_add_default=True, **kwargs):

        self._init_objman(ident='edgeresults',
                          parent=parent,  # main results object
                          name='Edge results',
                          info='Table with simulation results for each network edge.',
                          #xmltag = ('vtypes','vtype','ids_sumo'),
                          **kwargs)

        self.add_col(am.IdsArrayConf('ids_edge', edges,
                                     groupnames=['state'],
                                     is_index=True,
                                     name='ID edge',
                                     info='ID of edge.',
                                     ))
        self._init_attributes()

    def _init_attributes(self):
        # print 'Edgeresults._init_attributes'
        # print '  ',self,id(self),hasattr(self,'import_marouterxml')

        attrinfos = OrderedDict([


            ('detectorflows',  {'name': 'Detector flows',      'unit': None,    'default': 0.,
                                'info': 'Flows measured by detectors', 'xmltag': 'detectorflows', 'groupnames': ['edgedata']}),
            ('entered',  {'name': 'Entered',      'unit': None,    'default': 0,
                          'info': 'Entered number of vehicles', 'xmltag': 'entered', 'groupnames': ['edgedata']}),
            ('entered_est', {'name': 'Entered est',                'default': 0,
                             'info': 'Estimated number of entered vehicles.', 'groupnames': ['analysis'], }),
            ('left',     {'name': 'Left',         'unit': None,    'default': 0,
                          'info': 'Left number of vehicles', 'xmltag': 'left', 'groupnames': ['edgedata']}),
            ('arrived',  {'name': 'Arrived',      'unit': None,    'default': 0,
                          'info': 'Arrived number of vehicles', 'xmltag': 'arrived', 'groupnames': ['edgedata']}),
            ('departed', {'name': 'Departed',     'unit': None,    'default': 0,
                          'info': 'Departed number of vehicles', 'xmltag': 'departed', 'groupnames': ['edgedata']}),
            ('traveltime', {'name': 'Av. times',   'unit': 's',     'default': 0.0, 'info': 'Av. Travel times',
                            'xmltag': 'traveltime', 'groupnames': ['edgedata'], 'is_average': True}),
            ('density',  {'name': 'Av. Densities', 'unit': 'veh/km', 'default': 0.0,
                          'info': 'Av. Density in vehicles of vehicles on this Edge/Lane', 'xmltag': 'density', 'groupnames': ['edgedata'], 'is_average': True}),
            ('waitingTime', {'name': 'Av. waits',  'unit': 's',     'default': 0.0, 'info': 'Av. Waiting times of vehicles on this Edge/Lane',
                             'xmltag': 'waitingTime', 'groupnames': ['edgedata'], 'is_average': True}),
            ('speed',    {'name': 'Av. speeds',   'unit': 'm/s',   'default': 0.0,
                          'info': 'Av. velocity of vehicles on this Edge/Lane', 'xmltag': 'speed', 'groupnames': ['edgedata'], 'is_average': True}),
            ('fuel_abs', {'name': 'Abs. Fuel',    'unit': 'ml',    'default': 0.0,
                          'info': 'Absolute fuel consumption of vehicles on this Edge/Lane', 'xmltag': 'fuel_abs', 'groupnames': ['edgeemissions']}),
            ('CO_abs',   {'name': 'Abs. CO',      'unit': 'mg',    'default': 0.0,
                          'info': 'Absolute CO emission of vehicles on this Edge/Lane', 'xmltag': 'CO_abs', 'groupnames': ['edgeemissions']}),
            ('CO2_abs',  {'name': 'Abs. CO2',     'unit': 'mg',    'default': 0.0,
                          'info': 'Absolute CO2 emission of vehicles on this Edge/Lane', 'xmltag': 'CO2_abs', 'groupnames': ['edgeemissions']}),
            ('NOx_abs',  {'name': 'Abs. NOx',     'unit': 'mg',    'default': 0.0,
                          'info': 'Absolute NOx emission of vehicles on this Edge/Lane', 'xmltag': 'NOx_abs', 'groupnames': ['edgeemissions']}),
            ('PMx_abs',  {'name': 'Abs. PMx',     'unit': 'mg',    'default': 0.0,
                          'info': 'Absolute PMx  emission (Particle matter, all sizes) of vehicles on this Edge/Lane', 'xmltag': 'PMx_abs', 'groupnames': ['edgeemissions']}),
            ('fuel_normed', {'name': 'Specific fuel',       'unit': 'l/km/h', 'default': 0.0,
                             'info': 'Specific fuel consumption of vehicles on this Edge/Lane', 'xmltag': 'fuel_normed', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('CO_normed', {'name': 'Specific CO',           'unit': 'g/km/h', 'default': 0.0,
                           'info': 'Normalized CO emission of vehicles on this Edge/Lane', 'xmltag': 'CO_normed', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('CO2_normed', {'name': 'Specific CO2',         'unit': 'g/km/h', 'default': 0.0,
                            'info': 'Normalized CO2 emission of vehicles on this Edge/Lane', 'xmltag': 'CO2_normed', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('NOx_normed', {'name': 'Specific NOx',         'unit': 'g/km/h', 'default': 0.0,
                            'info': 'Normalized NOx emission of vehicles on this Edge/Lane', 'xmltag': 'NOx_normed', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('PMx_normed', {'name': 'Specific PMx',         'unit': 'g/km/h', 'default': 0.0,
                            'info': 'Normalized PMx emission of vehicles on this Edge/Lane', 'xmltag': 'PMx_normed', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('fuel_perVeh', {'name': 'Fuel per veh.',       'unit': 'ml/veh', 'default': 0.0,
                             'info': 'Absolute fuel consumption of vehicles on this Edge/Lane', 'xmltag': 'fuel_perVeh', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('CO_perVeh', {'name': 'CO per veh.',           'unit': 'mg/veh', 'default': 0.0,
                           'info': 'CO emission per vehicle on this Edge/Lane', 'xmltag': 'CO_perVeh', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('CO2_perVeh', {'name': 'CO2 per veh.',         'unit': 'mg/veh', 'default': 0.0,
                            'info': 'CO2 emission per vehicle on this Edge/Lane', 'xmltag': 'CO2_perVeh', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('NOx_perVeh', {'name': 'NOx per veh.',         'unit': 'mg/veh', 'default': 0.0,
                            'info': 'NOx emission per vehicle on this Edge/Lane', 'xmltag': 'NOx_perVeh', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('PMx_perVeh', {'name': 'PMx per veh.',         'unit': 'mg/veh', 'default': 0.0,
                            'info': 'PMx emission per vehicle on this Edge/Lane', 'xmltag': 'PMx_perVeh', 'groupnames': ['edgeemissions'], 'is_average': True}),
            ('noise',    {'name': 'Noise',         'unit': 'dB',   'default': 0.0,
                          'info': 'Noise of vehicles on this Edge/Lane', 'xmltag': 'noise', 'groupnames': ['edgenoise'], 'is_average': True}),
            ('total_flows',  {'name': 'Total flows',      'unit': None,    'default': 0,
                              'info': 'Total flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_flows']}),
            ('total_od_flows',  {'name': 'Total OD flows',      'unit': None,    'default': 0,
                                 'info': 'Total OD flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_od_flows']}),
            ('total_vp_flows',  {'name': 'Total VP flows',      'unit': None,    'default': 0,
                                 'info': 'Total VP flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_vp_flows']}),
            ('total_ptline_flows',  {'name': 'Total pt flows',      'unit': None,    'default': 0,
                                     'info': 'Total pt flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_ptline_flows']}),
            ('total_car_flows',  {'name': 'Total car flows',      'unit': None,    'default': 0,
                                  'info': 'Total car flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_car_flows']}),
            ('total_bike_flows',  {'name': 'Total bike flows',      'unit': None,    'default': 0,
                                   'info': 'Total bike flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_bike_flows']}),
            ('total_moto_flows',  {'name': 'Total moto flows',      'unit': None,    'default': 0,
                                   'info': 'Total moto flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_moto_flows']}),
            ('total_iauto_flows',  {'name': 'Total iauto flows',      'unit': None,    'default': 0,
                                    'info': 'Total iauto flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_iauto_flows']}),
            ('total_ibike_flows',  {'name': 'Total ibike flows',      'unit': None,    'default': 0,
                                    'info': 'Total ibike flow in the edge', 'xmltag': 'entered', 'groupnames': ['total_ibike_flows']}),
            ('total_imoto_flows',  {'name': 'Total imoto flows',      'unit': None,    'default': 0,
                                    'info': 'Total imoto flow in the edge.', 'xmltag': 'entered', 'groupnames': ['total_imoto_flows']}),

            #
        ])

        for attrname, kwargs in attrinfos.iteritems():
            self.add_resultattr(attrname, **kwargs)

    def add_resultattr(self, attrname, **kwargs):

        # default cannot be kwarg
        default = kwargs['default']
        del kwargs['default']
        if kwargs.has_key('groupnames'):
            kwargs['groupnames'].append('results')
        else:
            kwargs['groupnames'] = ['results']

        self.add_col(am.ArrayConf(attrname, default, **kwargs))

    def add_edgelength(self):
        edges = self.parent.get_scenario().net.edges
        self.add_col(am.ArrayConf('lengths', 0.0,
                                  groupnames=[],
                                  perm='r',
                                  unit='m',
                                  name='Edge length',
                                  info='Edge length.'))
        ids = self.get_ids()
        self.lengths[ids] = edges.lengths[self.ids_edge[ids]]

    def add_entered_est(self, ids_edge, entered_vec):
        self.entered_est.reset()
        ids_results = np.zeros(len(ids_edge), dtype=np.int32)
        i = 0
        for id_edge, entered in zip(ids_edge, entered_vec):
            if self.ids_edge.has_index(id_edge):
                id_res = self.ids_edge.get_id_from_index(id_edge)
                self.entered_est[id_res] = entered
            else:
                id_res = self.add_row(ids_edge=id_edge, entered_est=entered)
            ids_results[i] = id_res
            i += 1

        return ids_results

    def filter_zoneedges(self, ids_zone, is_invert=False):
        """
        Keep only results of edges that belong to zone id_zone
        """
        print 'filter_zoneedges', ids_zone

        zones = self.parent.parent.parent.landuse.zones
        ids_zoneedge = set()
        for id_zone in ids_zone:
            if not (zones.ids_edges_orig[id_zone] is None):
                ids_zoneedge.update(zones.ids_edges_orig[id_zone])

        if len(ids_zoneedge) > 0:

            ids_res = self.get_ids()
            inds = np.zeros(len(ids_res), dtype=np.bool)
            for i, id_res, id_edge in zip(xrange(len(ids_res)), ids_res, self.ids_edge[ids_res]):
                inds[i] = id_edge in ids_zoneedge
            if not is_invert:
                inds = np.logical_not(inds)

            self.del_rows(ids_res[inds])

    def import_edgedata(self, sumo,  filepath):
        print 'import_edgedata', filepath
        # print '  group',self.get_group('edgedata')
        #attrnames_data = ['entered','left','arrived','departed']
        #attrnames_averaged = ['traveltime','density','waitingTime','speed',]
        self.import_sumoxml(filepath, sumo,  self.get_group('edgedata'))

    def import_edgeflows(self,  sumo, filepath):

        routeresults = self.parent.routeresults
        net = self.parent.parent.parent.net
        demand = self.parent.parent.parent.demand
        ids_routeres = routeresults.get_ids()
        # Consider warmup time
        ids_routeres = ids_routeres[(routeresults.times_depart[ids_routeres] > sumo.time_warmup)]
#        ids_routeres = ids_routeres[700:1000]
        # print ids_routeres
        ids_routeres_od = ids_routeres[(routeresults.ids_trip[ids_routeres] > 0)]
        ids_od_modes = demand.vtypes.ids_mode[demand.trips.ids_vtype[routeresults.ids_trip[ids_routeres_od]]]
        ids_routeres_od_auto = ids_routeres_od[(ids_od_modes == net.modes.names.get_id_from_index('passenger'))]
        ids_routeres_od_taxi = ids_routeres_od[(ids_od_modes == net.modes.names.get_id_from_index('taxi'))]
        ids_routeres_od_car = list(ids_routeres_od_auto)+list(ids_routeres_od_taxi)
        ids_routeres_od_bike = ids_routeres_od[(ids_od_modes == net.modes.names.get_id_from_index('bicycle'))]
        ids_routeres_od_moto = ids_routeres_od[(ids_od_modes == net.modes.names.get_id_from_index('motorcycle'))]

        ids_routeres_iauto = ids_routeres[(routeresults.ids_iauto[ids_routeres] > 0)]
        ids_routeres_ibike = ids_routeres[(routeresults.ids_ibike[ids_routeres] > 0)]
        ids_routeres_imoto = ids_routeres[(routeresults.ids_imoto[ids_routeres] > 0)]
        ids_routeres_vp = list(ids_routeres_iauto) + list(ids_routeres_ibike) + list(ids_routeres_imoto)
        ids_routeres_ptlines = ids_routeres[(routeresults.ids_ptline[ids_routeres] > 0)]
        ids_routeres_car = list(ids_routeres_od_car) + list(ids_routeres_iauto)
        ids_routeres_bike = list(ids_routeres_od_bike) + list(ids_routeres_ibike)
        ids_routeres_moto = list(ids_routeres_od_moto) + list(ids_routeres_imoto)

        ids_edges = routeresults.ids_edges[ids_routeres]
        ids_edges_od_car = routeresults.ids_edges[ids_routeres_od_car]
        ids_edges_od = routeresults.ids_edges[ids_routeres_od]
        ids_edges_iauto = routeresults.ids_edges[ids_routeres_iauto]
        ids_edges_ibike = routeresults.ids_edges[ids_routeres_ibike]
        ids_edges_imoto = routeresults.ids_edges[ids_routeres_imoto]
        ids_edges_car = routeresults.ids_edges[ids_routeres_car]
        ids_edges_bike = routeresults.ids_edges[ids_routeres_bike]
        ids_edges_moto = routeresults.ids_edges[ids_routeres_moto]
        ids_edges_ptline = routeresults.ids_edges[ids_routeres_ptlines]
        ids_edges_vp = routeresults.ids_edges[ids_routeres_vp]

        total_flows = np.zeros(len(net.edges.get_ids())+1, dtype=np.float32)
        total_od_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_vp_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_ptline_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_car_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_bike_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_moto_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_iauto_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_ibike_flows = np.zeros(len(total_flows), dtype=np.float32)
        total_imoto_flows = np.zeros(len(total_flows), dtype=np.float32)

        for ids_edge in ids_edges:
            for id_edge in ids_edge:
                total_flows[id_edge] += 1

        for ids_edge in ids_edges_od:
            for id_edge in ids_edge:
                total_od_flows[id_edge] += 1

        for ids_edge in ids_edges_iauto:
            for id_edge in ids_edge:
                total_iauto_flows[id_edge] += 1

        for ids_edge in ids_edges_ibike:
            for id_edge in ids_edge:
                total_ibike_flows[id_edge] += 1

        for ids_edge in ids_edges_imoto:
            for id_edge in ids_edge:
                total_imoto_flows[id_edge] += 1

        for ids_edge in ids_edges_car:
            for id_edge in ids_edge:
                total_car_flows[id_edge] += 1

        for ids_edge in ids_edges_bike:
            for id_edge in ids_edge:
                total_bike_flows[id_edge] += 1

        for ids_edge in ids_edges_moto:
            for id_edge in ids_edge:
                total_moto_flows[id_edge] += 1

        for ids_edge in ids_edges_ptline:
            for id_edge in ids_edge:
                total_ptline_flows[id_edge] += 1

        for ids_edge in ids_edges_vp:
            for id_edge in ids_edge:
                total_vp_flows[id_edge] += 1

        ids_edgeres = self.get_ids()

        self.total_flows[ids_edgeres] = total_flows[self.ids_edge[ids_edgeres]]
        self.total_od_flows[ids_edgeres] = total_od_flows[self.ids_edge[ids_edgeres]]
        self.total_vp_flows[ids_edgeres] = total_vp_flows[self.ids_edge[ids_edgeres]]
        self.total_ptline_flows[ids_edgeres] = total_ptline_flows[self.ids_edge[ids_edgeres]]
        self.total_car_flows[ids_edgeres] = total_car_flows[self.ids_edge[ids_edgeres]]
        self.total_bike_flows[ids_edgeres] = total_bike_flows[self.ids_edge[ids_edgeres]]
        self.total_moto_flows[ids_edgeres] = total_moto_flows[self.ids_edge[ids_edgeres]]
        self.total_iauto_flows[ids_edgeres] = total_iauto_flows[self.ids_edge[ids_edgeres]]
        self.total_ibike_flows[ids_edgeres] = total_ibike_flows[self.ids_edge[ids_edgeres]]
        self.total_imoto_flows[ids_edgeres] = total_imoto_flows[self.ids_edge[ids_edgeres]]

        return True

    def import_edgenoise(self, sumo,  filepath):
        print 'import_edgenoise', filepath
        self.import_sumoxml(filepath, sumo, self.get_group('edgenoise'))

    def import_edgeemissions(self, sumo, filepath):
        print 'import_edgeemissions', filepath
        #attrnames_data = ['fuel_abs','CO_abs','CO2_abs','NOx_abs','PMx_abs']
        #attrnames_averaged = ['fuel_normed','CO_normed','CO2_normed',]
        self.import_sumoxml(filepath, sumo, self.get_group('edgeemissions'))

    def import_sumoxml(self, filepath, sumo, attrconfigs):
        element = 'edge'
        # print 'import_sumoxml',element
        #id_type = 'edge',
        #reader = 'interval',
        ids_sumo, results, interval = read_interval2(
            filepath, sumo, element, attrconfigs)
        # print '  ids_sumo',ids_sumo
        # print '  results.keys()',results.keys()
        # print '  results',results
        # create ids for all colums
        # if fileinfo['id_type']=='edge':

        # this procedure is necessary to create new result ids only
        # for edges that are not yet in the database
        ids_sumoedge = self.ids_edge.get_linktab().ids_sumo
        n = len(ids_sumo)
        # print '  n',n
        ind_range = np.arange(n, dtype=np.int32)
        ids = np.zeros(n, dtype=np.int32)
        for i in ind_range:
            id_edge = ids_sumoedge.get_id_from_index(ids_sumo[i])
            if self.ids_edge.has_index(id_edge):
                ids[i] = self.ids_edge.get_id_from_index(id_edge)
            else:
                ids[i] = self.add_row(ids_edge=id_edge)

        # ids = self.add_row()# here the ids_sumo can be strings too
        # elif fileinfo['id_type']=='trip':
        #    ids = self.tripresults.add_rows_keyrecycle(keys = ids_sumo)#
        # print '  ids=',ids

        for attrconfig in attrconfigs:

            attrname = attrconfig.attrname

            default = attrconfig.get_default()
            if type(default) in (types.IntType, types.LongType):
                conversion = 'i'  # int
                values_attr = np.zeros(n, int)
            elif type(default) in (types.FloatType, types.ComplexType):
                conversion = 'f'  # float
                values_attr = np.zeros(n, float)
            else:
                conversion = 's'  # str
                values_attr = np.zeros(n, obj)

            is_average = False
            if hasattr(attrconfig, 'is_average'):
                is_average = attrconfig.is_average
            # print ' copy',attrname,'is_average',is_average
            # this is a tricky way to read the data stored in
            # dictionarie into array tructures as used in results
            # problem is that not all dictionaries have all ids
            for i in ind_range:
                if is_average:
                    valcum, num = results[attrname].get(ids_sumo[i], (default, 1))
                    val = valcum/float(num)  # average over measurements!
                else:
                    val = results[attrname].get(ids_sumo[i], default)

                if conversion == 'i':
                    val = int(val)
                # else:
                #    values_attr[i]=val
                # print '   attrname',attrname,conversion,val,is_average,type(val)
                values_attr[i] = val

            # print '  attrname',attrname
            # print '  ids',type(ids),ids
            # print '  values_attr',type(values_attr),values_attr
            attrconfig.set(ids, values_attr)

    def import_marouterxml(self, filepath, marouter):

        if not self.has_attrname('entered_mar'):
            # add marouter result attributes
            # traveltime="14.01" speed="5.56" entered="0.00" flowCapacityRatio="0.00"/>
            attrinfos = OrderedDict([
                ('entered_mar',  {'name': 'Entered Marouter',
                                    # 'symbol':'F_{MA}',
                                    'unit': None,
                                    'default': 0,
                                    'info': 'Entered number of vehicles',
                                    'xmltag': 'entered',
                                    'groupnames': ['marouter']
                                    }),
                ('speeds_mar',    {'name': 'Av. speeds Marouter',
                                   'unit': 'm/s',
                                   'default': 0.0,
                                   'info': 'Av. velocity of vehicles on this Edge',
                                   'xmltag': 'speed',
                                   'groupnames': ['marouter'],
                                   'is_average': True
                                   }),
                ('flow_capacity_ratios', {'name': 'Saturation',
                                          'default': 0.0,
                                          'unit': '%',
                                          'info': 'Ratio between simulated flows and edge capacity determined by marouter.',
                                          'xmltag': 'flowCapacityRatio',
                                          'groupnames': ['marouter']
                                          }),
            ])

            for attrname, kwargs in attrinfos.iteritems():
                self.add_resultattr(attrname, **kwargs)

        attrconfigs = self.get_group('marouter')

        self.import_sumoxml(filepath, marouter, attrconfigs)


class EdgeresultFilter(Process):
    def __init__(self, edgeresults,  logger=None, **kwargs):
        print 'EdgeresultFilter.__init__'

        # TODO: let this be independent, link to it or child??

        self._init_common(ident='edgeresultfilter',
                          parent=edgeresults,
                          name='Edgeresult Filter',
                          logger=logger,
                          info='Filters the result of specific edges.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))
        zones = self.parent.parent.parent.parent.landuse.zones
        ids_zone = zones.get_ids()
        zonechoices = {}
        for id_zone, name_zone in zip(ids_zone, zones.ids_sumo[ids_zone]):
            zonechoices[name_zone] = id_zone
        print '  zonechoices', zonechoices
        # make for each possible pattern a field for prob
        # if len(zonechoices) > 0:
        self.ids_zone = attrsman.add(cm.ListConf('ids_zone', [],
                                                 groupnames=['options'],
                                                 choices=zonechoices,
                                                 name='Zones',
                                                 info="""Retain edge result if edge is within these zones.""",
                                                 ))
        # else:
        #    self.ids_zone = []
        # self.id_zone = attrsman.add(cm.AttrConf( 'id_zone',kwargs.get('id_zone',1),
        #                    groupnames = ['options'],
        #                    perm='rw',
        #                    name = 'Zone ID',
        #                    info = """Retain edge result if edge is within this zone.""",
        #                    ))

        self.is_invert = attrsman.add(cm.AttrConf('is_invert', kwargs.get('is_invert', False),
                                                  groupnames=['options'],
                                                  perm='rw',
                                                  name='invert filter',
                                                  info="""Invert filter results.""",
                                                  ))

    def do(self):
        print 'EdgeresultFilter'
        # links
        edgeresults = self.parent
        edgeresults.filter_zoneedges(self.ids_zone, self.is_invert)
        return True

        # TODO: generate and assign  additional vehicles
        # to satisfy prescribes ownership


class TrajectoryResults(am.ArrayObjman):
    """Collects trajectories for all vehicles with a battery device."""

    def __init__(self,  parent,  datapathkey='trajectorypath',
                 is_add_default=True,
                 name='Trajectory results',
                 info='Table with trajectory and speed data  of individual vehicles. Either all vehicles or vehicles with a fcd device.',
                 **kwargs):

        self._init_objman(ident='trajectory_results',
                          parent=parent,  # main results object
                          info=info,
                          name=name,
                          **kwargs)

        self.add(cm.AttrConf('datapathkey', datapathkey,
                             groupnames=['_private'],
                             name='data pathkey',
                             info="key of data path",
                             ))

        self.add(cm.AttrConf('times', np.array([], dtype=np.float32),
                             groupnames=['_private'],
                             name='Times',
                             info="Times steps for energy measurements",
                             ))

        self.add_col(am.ListArrayConf('trajectories',
                                      groupnames=['_private'],
                                      name='Trajectories',
                                      info="x,y,z, trajectories of all vehicles during each time step",
                                      ))

        self.add_col(am.ListArrayConf('angles',
                                      groupnames=['_private'],
                                      name='Angle',
                                      info="Angles of all vehicles during each time step",
                                      ))

        self.add_col(am.ListArrayConf('speeds',
                                      groupnames=['_private'],
                                      name='Speed',
                                      info="Speed of all vehicles during each time step",
                                      ))

        # self.add(cm.FuncConf(  'energy_total','on_energy_total', 0.0,
        #                                groupnames = ['summary'],
        #                                name = 'Total energy consumption',
        #                                unit = 'KWh',
        #                                info = 'Total electrical energy consumpton of all vehicles.',
        #                                ))

        self.add_col(SumoIdsConf('vehicle',
                                 #name ='SUMO vehicle ID',
                                 info='SUMO vehicle ID, as used in route files.',
                                 perm='r'))

    def import_xml(self, sumo, datapaths):
        datapathkey = self.datapathkey.get_value()
        print 'TrajectoryResults.import_xml datapathkey', datapathkey, datapaths.has_key(datapathkey)
        if datapaths.has_key(datapathkey):
            self.import_trajectories_sumoxml(datapaths[datapathkey], sumo)

    def import_trajectories_sumoxml(self, filepath, sumo):
        element = 'vehicle'
        print 'TrajectoryResults.import_trajectories_sumoxml', element, filepath

        ids_sumo, times, trajectories, angles, speeds = read_trajectories(
            filepath, sumo, element)

        n = len(ids_sumo)
        ids_res = self.add_rows(n=n, ids_sumo=ids_sumo)
        self.times.set_value(times)
        self.trajectories[ids_res] = trajectories
        self.angles[ids_res] = angles
        self.speeds[ids_res] = speeds
        # self.print_trajectories()

    def print_trajectories(self):
        ids_res = self.get_ids()
        times = self.times.get_value()
        for i, t in zip(xrange(len(times)), times):
            print 79*'-'
            print 'time=', t, 's', len(times)
            for id_res, id_sumo_veh, traj, a, v in zip(ids_res, self.ids_sumo[ids_res], self.trajectories[ids_res], self.angles[ids_res], self.speeds[ids_res]):
                # print '  id_sumo_veh',id_sumo_veh,id_res
                # print '    v',v[i]
                # print '    traj',traj[i]
                # print '    a',a[i]
                print '  id_sumo_veh', id_sumo_veh, ': (x,y)', traj[i], 'a=%.2f', a[i], ' v=%.2fm/s' % v[i], len(a), len(v), len(traj)


class ElectricEnergyVehicleResults(am.ArrayObjman):
    """Collects electric energy results for all vehicles with a battery device."""

    def __init__(self,  parent,  datapathkey='electricenergypath',
                 is_add_default=True,
                 name='Electric energy vehicle results',
                 info='Table with electric energy consumption data for each vehicle with a battery device.',
                 **kwargs):

        self._init_objman(ident='electricenergy_vehicleresults',
                          parent=parent,  # main results object
                          info=info,
                          name=name,
                          **kwargs)

        self.add(cm.AttrConf('datapathkey', datapathkey,
                             groupnames=['_private'],
                             name='data pathkey',
                             info="key of data path",
                             ))

        self.add(cm.AttrConf('times', np.array([], dtype=np.float32),
                             groupnames=['_private'],
                             name='Times',
                             info="Times steps for energy measurements",
                             ))

        self.add(cm.AttrConf('energies', np.array([], dtype=np.float32),
                             groupnames=['_private'],
                             name='Energies',
                             info="Energy consumptions of all vehicles during each time step",
                             ))

        self.add(cm.FuncConf('energy_total', 'on_energy_total', 0.0,
                             groupnames=['summary'],
                             name='Total energy consumption',
                             unit='KWh',
                             info='Total electrical energy consumpton of all vehicles.',
                             ))

        self.add_col(SumoIdsConf('vehicle',
                                 #name ='SUMO vehicle ID',
                                 info='SUMO vehicle ID, as used in route files.',
                                 perm='r'))

        attrinfos = OrderedDict([
            ('consumptions', {'name': 'Consumption', 'xmltag': 'energyConsumed',    'unit': 'Ws',
                              'default': 0.0, 'info': 'Total electrical energy consumption.', 'groupnames': ['electricenergydata']}),
            ('charges_station', {'name': 'Charged at station', 'xmltag': 'energyCharged',    'unit': 'Ws',     'default': 0.0,
                                 'info': 'Total electrical energy charged at charging stations.', 'groupnames': ['electricenergydata']}),
            ('charges_av',   {'name': 'Average charge', 'xmltag': 'actualBatteryCapacity',   'unit': 'Ws',     'default': 0.0,
                              'info': 'Average level of battery charge', 'groupnames': ['electricenergydata'], 'is_average': True}),
            ('speeds_av',   {'name': 'Average speeds', 'xmltag': 'speed',   'unit': 'm/s',     'default': 0.0,
                             'info': 'Average speed', 'groupnames': ['electricenergydata'], 'is_average': True}),
        ])

        for attrname, kwargs in attrinfos.iteritems():
            self.add_resultattr(attrname, **kwargs)

    def on_energy_total(self):
        """Returns total energy consumption"""
        return np.sum(self.consumptions.get_value())/10.0**3

    def add_resultattr(self, attrname, **kwargs):

        # default cannot be kwarg
        default = kwargs['default']
        del kwargs['default']
        if kwargs.has_key('groupnames'):
            kwargs['groupnames'].append('results')
        else:
            kwargs['groupnames'] = ['results']

        self.add_col(am.ArrayConf(attrname, default, **kwargs))

    # def import_routesdata(self, filepath):
    #    # TODO
    #    pass

    # def import_tripdata(self, filepath):
    #    #print 'import_tripdata',filepath,self.get_group('tripdata')
    #    self.import_sumoxml(filepath,self.get_group('tripdata'))

    def import_xml(self, sumo, datapaths):
        datapathkey = self.datapathkey.get_value()
        if datapaths.has_key(datapathkey):
            self.import_electricenergy_sumoxml(datapaths[datapathkey], sumo)

    def import_electricenergy_sumoxml(self, filepath, sumo):
        element = 'vehicle'
        print 'ElectricEnergyresults.import_electricenergy_sumoxml', element, filepath
        #id_type = 'edge',
        #reader = 'interval',
        attrconfigs = self.get_group('electricenergydata')
        # print 'import_sumoxml',element
        #id_type = 'edge',
        #reader = 'interval',
        ids_sumo, results, interval, times, energies = read_electrical_energy(
            filepath, sumo, element, attrconfigs)

        self.times.set_value(times)
        self.energies.set_value(energies)

        print '  times=\n', self.times.get_value()
        print '  energies=\n', self.energies.get_value()

        # print '  ids_sumo',ids_sumo
        # print '  results.keys()',results.keys()
        # print '  results',results
        # create ids for all colums
        # if fileinfo['id_type']=='edge':

        # this procedure is necessary to create new result ids only
        # for edges that are not yet in the database
        #ids_sumotrip = self.ids_trip.get_linktab().ids_sumo
        n = len(ids_sumo)
        ids = self.add_rows(n=n, ids_sumo=ids_sumo)
        print '  n', n
        ind_range = np.arange(n, dtype=np.int32)

        #ids = np.zeros(n, dtype=np.int32)
        # for i in ind_range:
        #    id_trip = ids_sumotrip.get_id_from_index(ids_sumo[i])
        #    if self.ids_trip.has_index(id_trip):
        #        ids[i] =  self.ids_trip.get_id_from_index(id_edge)
        #    else:
        #        ids[i] =  self.add_row(ids_trip = id_edge)

        # ids = self.add_row()# here the ids_sumo can be strings too
        # elif fileinfo['id_type']=='trip':
        #    ids = self.tripresults.add_rows_keyrecycle(keys = ids_sumo)#
        # print '  ids=',ids

        for attrconfig in attrconfigs:

            attrname = attrconfig.attrname

            default = attrconfig.get_default()
            if type(default) in (types.IntType, types.LongType):
                conversion = 'i'  # int
                values_attr = np.zeros(n, int)
            elif type(default) in (types.FloatType, types.ComplexType):
                conversion = 'f'  # float
                values_attr = np.zeros(n, float)
            else:
                conversion = 's'  # str
                values_attr = np.zeros(n, obj)

            is_average = False
            if hasattr(attrconfig, 'is_average'):
                is_average = attrconfig.is_average
            # print ' copy',attrname,'is_average',is_average
            # this is a tricky way to read the data stored in
            # dictionarie into array tructures as used in results
            # problem is that not all dictionaries have all ids
            for i in ind_range:
                if is_average:
                    valcum, num = results[attrname].get(ids_sumo[i], (default, 1))
                    val = valcum/float(num)  # average over measurements!
                else:
                    val = results[attrname].get(ids_sumo[i], default)

                if conversion == 'i':
                    val = int(val)
                # else:
                #    values_attr[i]=val
                # print '   attrname',attrname,conversion,val,is_average,type(val)
                values_attr[i] = val

            # print '  attrname',attrname
            # print '  ids',type(ids),ids
            # print '  values_attr',type(values_attr),values_attr
            attrconfig.set(ids, values_attr)


class Simresults(cm.BaseObjman):
    def __init__(self, ident='simresults', parent=None,
                 name='Simulation results',
                 info='Results of SUMO simulation run.',
                 outfile_prefix='out',
                 scenario=None,
                 **kwargs):

        # either scenario must be provided or
        # parent must provide method get_scenario
        if scenario is None:
            scenario = parent.get_scenario()
        # print 'Network.__init__',name,kwargs
        rootname = scenario.get_rootfilename()
        rootdirpath = scenario.get_workdirpath()

        self._init_objman(ident=ident, parent=parent, name=name,
                          info=info, **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))

        self._init_attributes()

    def _init_attributes(self):
        attrsman = self.get_attrsman()
        scenario = self.get_scenario()
        self.edgeresults = attrsman.add(cm.ObjConf(Edgeresults(self, scenario.net.edges),
                                                   groupnames=['Edge results'],
                                                   ))
        self.connectionresults = attrsman.add(cm.ObjConf(Connectionresults('connectionresults', self, scenario.demand.trips,
                                                                           scenario.net.edges), groupnames=['Connection results']))

        self.routeresults = attrsman.add(cm.ObjConf(Routeresults('routeresults', self, scenario.demand.trips,
                                                                 scenario.net.edges), groupnames=['Route results']))
        # add trip results from all demand objects
        print 'Simresults._init_attributes'
        print '  scenario.demand.get_demandobjects()', scenario.demand.get_demandobjects()
        for demandobj in scenario.demand.get_demandobjects():
            demandobj.config_results(self)

        for simobj in self.parent.get_simobjects():
            simobj.config_simresults(self)

        self.electricenergy_vehicleresults = attrsman.add(cm.ObjConf(ElectricEnergyVehicleResults(self),
                                                                     groupnames=['Results'],
                                                                     ))

        self.trajectory_results = attrsman.add(cm.ObjConf(TrajectoryResults(self),
                                                          groupnames=['Results'],
                                                          ))

    def clear_results(self):
        for resultobj in self.get_attrsman().get_group('Results'):
            resultobj.clear()
            self.connectionresults.clear_rows()
            self.routeresults.clear_rows()
            self.edgeresults.clear_rows()

    def get_resultobj(self, ident):
        # print 'get_resultobj',hasattr(self,ident)
        if hasattr(self, ident):
            return getattr(self, ident)

        else:
            return None

    def add_resultobj(self, resultobj, **kwargs):
        # print 'RESULTS:add_resultobj',resultobj.ident
        # attention: need to check whether already set
        # because setattr is set explicitely after add
        if hasattr(self, resultobj.get_ident()):
            # self.get_attrsman().delete(resultobj.get_ident())
            getattr(self, resultobj.get_ident()).clear()

        if not hasattr(self, resultobj.get_ident()):
            if kwargs.has_key('groupnames'):
                kwargs['groupnames'].append('Results')
            else:
                kwargs['groupnames'] = ['Results']
            attrsman = self.get_attrsman()
            attrsman.add(cm.ObjConf(resultobj, **kwargs))
            setattr(self, resultobj.get_ident(), resultobj)

        # for resultobj in self.get_attrsman().get_group_attrs('Results').values():
        #    print '  check resultobject',resultobj.get_ident_abs()

    # def import_xml(self, edgedatapath=None, edgenoisepath=None, edgeemissionspath = None, routesdatapath=None, tripdatapath=None):

    # def get_path(self, datapath):
    #    #edgedatapath=None, edgenoisepath=None, edgeemissionspath = None, routesdatapath=None, tripdatapath=None

    def import_xml(self, sumo, **datapaths):
        print 'Simresults.import_xml', self.get_ident_abs()
# print '  datapaths',datapaths
        # import first all edge oriented results for the whole net
        if datapaths.has_key('edgedatapath'):
            print 'import edge data'
            self.edgeresults.import_edgedata(sumo, datapaths['edgedatapath'])

        if datapaths.has_key('edgenoisepath'):
            print 'import edge noise'
            self.edgeresults.import_edgenoise(sumo, datapaths['edgenoisepath'])

        if datapaths.has_key('edgeemissionspath'):
            print 'import edge emissons'
            self.edgeresults.import_edgeemissions(sumo, datapaths['edgeemissionspath'])

        # import all other resultobjects
        for resultobj in self.get_attrsman().get_group_attrs('Results').values():
            print '  import other resultobject', resultobj.ident
            resultobj.import_xml(sumo, datapaths)

        if datapaths.has_key('routesdatapath'):
            print 'import route results'
            self.routeresults.import_xml(sumo, datapaths)
            print 'importedge flows'
            self.edgeresults.import_edgeflows(sumo, datapaths['edgedatapath'])
            print 'import connection flows'
            self.connectionresults.evaluate_results(sumo, datapaths)

    # def process(self, process = None):
    #    print 'Simresults.process'
    #    for demandobj in self.parent.demand.get_demandobjects():
    #        print '  process_results',demandobj
    #        demandobj.process_results(self, process)

    def get_tripresults(self):
        return self.get_attrsman().get_group_attrs('Trip results').values()

    # def import_routesdata(self, routesdatapath):
    #    for tripresult in self.get_tripresults():
    #        tripresult.import_routesdata(routesdatapath)

    # def import_tripdata(self, tripdatapath):
    #    for tripresult in self.get_tripresults():
    #        tripresult.import_tripdata(tripdatapath)

    def save(self, filepath=None, is_not_save_parent=True):
        if filepath is None:
            self.get_scenario().get_rootfilepath()+'.res.obj'
        cm.save_obj(self, filepath, is_not_save_parent=is_not_save_parent)

    def get_scenario(self):
        return self.parent.parent


class MarouterLoadReader(handler.ContentHandler):
    """
    Reads edge based load results of Marouter.

    """

    def __init__(self, marouter, attrsconfigs_cumulative, attrsconfigs_average):
        self._marouter = marouter
        self._attrconfigs = attrconfigs
        self._element = 'edge'
        self._time_begin = None
        self._time_inter_begin = 0.0
        self._time_inter_end = 0.0
        self._time_tot = 0.0
        self._n_inter = 0
        self._values = {}
        self._ids = []

    def startElement(self, name, attrs):
        # <edge id="-120634989" traveltime="6.74" speed="13.89" entered="0.00" flowCapacityRatio="0.00"/>
        # if attrs.has_key('id'):
        # print 'startElement',name,name == self._element,self._element
        if name == 'interval':
            time_inter_begin = float(attrs['begin'])
            # print 79*'-'
            # print 'startElement  interval',self.is_inter_valid,' time_inter_begin',time_inter_begin,'is_valid',time_inter_begin >= self._sumo.time_warmup
            self.is_inter_valid = True
            time_inter_end = float(attrs['end'])

            self._time_inter = int(time_inter_end)-int(time_inter_begin)
            # here we just take the start and end time ofthe whole
            # measurement period
            if self._time_begin is None:  # take very first time only
                self._time_begin = int(time_inter_begin)
            self._time_end = int(time_inter_end)
            self._time_tot += self._time_inter
            self._n_inter += 1

        if name == self._element:
            if not self.is_inter_valid:
                return  # no interval initialized

            id_elem = attrs.get('id', None)
            # print 'startElement  ---id_elem',id_elem
            if id_elem is None:
                return  # id did not fit requested trip type

            if id_elem not in self._ids:
                self._ids.append(id_elem)

            # no arrival data availlable if trip has not been finished!!
            for attrsconfig in self._attrsconfigs_cumulative:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname

                if attrs.has_key(xmltag):
                    # print '  attrname cum',attrname,attrs.has_key(attrname),'*'+attrs[attrname]+'*'
                    a = attrs[xmltag]

                    if a.strip() != '':
                        if self._values[attrname].has_key(id_elem):
                            self._values[attrname][id_elem] += float(a)
                            # print '    added val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)
                        else:
                            self._values[attrname][id_elem] = float(a)
                            # print '    set val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)

                    # if (id in ('1/0to1/1','1/0to2/0')) & (attrname == 'entered'):
                    #    self.n_test+=int(attrs[attrname])
                    #    print '  -read ',id,attrname,attrs[attrname],self.n_test,self._values[attrname][id]
                    #
                    # if (id in ('0/0to1/0')) & (attrname == 'left'):
                    #    self.n_test2+=int(attrs[attrname])
                    #    print '  +read ',id,attrname,attrs[attrname],self.n_test2,self._values[attrname][id]

            for attrsconfig in self._attrsconfigs_average:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname
                if attrs.has_key(xmltag):
                    # print '  attrname',attrname,attrs.has_key(attrname),'*'+attrs[attrname]+'*'
                    # n=float(self.n_inter)
                    a = attrs[xmltag]
                    if a.strip() != '':
                        if self._values[attrname].has_key(id_elem):
                            valcum, n = self._values[attrname][id_elem]
                            valcum += float(a)
                            n += 1
                            #self._values[attrname][id_elem] = ( (n-1)*self._values[attrname][id_elem] + float(a))/n
                            #self._values[attrname][id] += float(a)/self._time_inter
                            #self._n_values[attrname][id] += 1
                        else:
                            valcum = float(a)
                            n = 1
                            #self._values[attrname][id_elem] = float(a)
                            #self._values[attrname][id_elem] = float(a)/self._time_inter
                            #self._n_values[attrname][id] = 1
                        # print '    added cumval',xmltag,attrname,valcum,'val',float(a)
                        self._values[attrname][id_elem] = (valcum, n)


class IntervalAvReader2(handler.ContentHandler):
    """
    Reads edge or lane based intervals
    and returns time averaged values for each attribute name.

    """

    def __init__(self, element, sumo,  attrsconfigs_cumulative, attrsconfigs_average):
        """
        element is "lane" or "edge" or "tripinfo"
        attrnames is a list of attribute names to read.
        """
        print 'IntervalAvReader2', element
        # print '  attrsconfigs_cumulative'
        # for attrconfig in attrsconfigs_cumulative: print '    ',attrconfig.attrname

        # print '  attrsconfigs_average'
        # for attrconfig in attrsconfigs_average: print '    ',attrconfig.attrname
        self._element = element
        self._sumo = sumo  # the sumo process generating the data
        self._time_start_recording = sumo.simtime_start + sumo.time_warmup
        self._attrsconfigs_cumulative = attrsconfigs_cumulative
        self._attrsconfigs_average = attrsconfigs_average
        self._time_begin = None
        self._time_end = None
        self._values = {}
        self._ids = []
        #self._n_values= {}
        self.is_inter_valid = False
        self.n_inter = 0
        self.n_test = 0
        self.n_test2 = 0

        # TODO: if we knew here all ids then we
        # could create a numeric array per attribute
        # idea: pass ids as input arg
        for attrsconfig in attrsconfigs_cumulative+attrsconfigs_average:
            self._values[attrsconfig.attrname] = {}
            # print '  init',attrsconfig.attrname
            #self._n_values= {}

    def get_id_elem(self, attrs):
        """
        Returns the trip ID as integer if the desired trip type has been read.
        Otherwise -1 is returned. 
        """
        return attrs['id']

    def startElement(self, name, attrs):
        # if attrs.has_key('id'):
        #    print '  parse',name,self._element,name == self._element,self.is_inter_valid, 'id=',attrs.get('id','-')
        # print 'startElement',name,name == self._element,self._element

        # if name == 'timestep':

        if name == 'interval':
            time_inter_begin = float(attrs['begin'])
            # print 79*'-'
            # print 'startElement  interval',self.is_inter_valid,' time_inter_begin',time_inter_begin,'is_valid',time_inter_begin >= self._sumo.time_warmup
            if time_inter_begin >= self._sumo.time_warmup:
                self.is_inter_valid = True
                time_inter_end = float(attrs['end'])

                self._time_inter = int(time_inter_end)-int(time_inter_begin)
                # here we just take the start and end time ofthe whole
                # measurement period
                if self._time_begin is None:  # take very first time only
                    self._time_begin = int(time_inter_begin)
                self._time_end = int(time_inter_end)
                self.n_inter += 1
            else:
                self.is_inter_valid = False

        if name == self._element:
            if not self.is_inter_valid:
                return  # no interval initialized

            id_elem = self.get_id_elem(attrs)
            # print 'startElement  ---id_elem',id_elem
            if id_elem == -1:
                return  # id did not fit requested trip type

            if id_elem not in self._ids:
                self._ids.append(id_elem)

            # no arrival data availlable if trip has not been finished!!
            for attrsconfig in self._attrsconfigs_cumulative:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname

                if attrs.has_key(xmltag):
                    # print '  attrname cum',attrname,attrs.has_key(attrname),'*'+attrs[attrname]+'*'
                    a = attrs[xmltag]

                    if a.strip() != '':
                        if self._values[attrname].has_key(id_elem):
                            self._values[attrname][id_elem] += float(a)
                            # print '    added val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)
                        else:
                            self._values[attrname][id_elem] = float(a)
                            # print '    set val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)

                    # if (id in ('1/0to1/1','1/0to2/0')) & (attrname == 'entered'):
                    #    self.n_test+=int(attrs[attrname])
                    #    print '  -read ',id,attrname,attrs[attrname],self.n_test,self._values[attrname][id]
                    #
                    # if (id in ('0/0to1/0')) & (attrname == 'left'):
                    #    self.n_test2+=int(attrs[attrname])
                    #    print '  +read ',id,attrname,attrs[attrname],self.n_test2,self._values[attrname][id]

            for attrsconfig in self._attrsconfigs_average:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname
                if attrs.has_key(xmltag):
                    # print '  attrname',attrname,attrs.has_key(attrname),'*'+attrs[attrname]+'*'
                    # n=float(self.n_inter)
                    a = attrs[xmltag]
                    if a.strip() != '':
                        if self._values[attrname].has_key(id_elem):
                            valcum, n = self._values[attrname][id_elem]
                            valcum += float(a)
                            n += 1
                            #self._values[attrname][id_elem] = ( (n-1)*self._values[attrname][id_elem] + float(a))/n
                            #self._values[attrname][id] += float(a)/self._time_inter
                            #self._n_values[attrname][id] += 1
                        else:
                            valcum = float(a)
                            n = 1
                            #self._values[attrname][id_elem] = float(a)
                            #self._values[attrname][id_elem] = float(a)/self._time_inter
                            #self._n_values[attrname][id] = 1
                        # print '    added cumval',xmltag,attrname,valcum,'val',float(a)
                        self._values[attrname][id_elem] = (valcum, n)

    def get_data(self):
        return self._values

    def get_interval(self):
        # returns time interval between first and last measurement
        return (self._time_begin, self._time_end)

    def get_ids(self):
        return self._ids


class ElectricalEnergyReader(IntervalAvReader2):
    """
    Reads intervals of the electrcal energy file and cumulates data.
    """

    def __init__(self, element, sumo,  attrsconfigs_cumulative, attrsconfigs_average):
        """
        element is "lane" or "edge" or "tripinfo"
        attrnames is a list of attribute names to read.
        """
        print 'ElectricalEnergyReader', element
        # print '  attrsconfigs_cumulative',attrsconfigs_cumulative
        # print '  attrsconfigs_average',attrsconfigs_average
        IntervalAvReader2.__init__(self, element, sumo,  attrsconfigs_cumulative, attrsconfigs_average)
        self.time_lastint = -1.0

        self.times = []
        self.energies = []
        self._energy_step = 0.0  # energy os all vehicles during last step
        self._consumptionsname = "consumptions"

    def startElement(self, name, attrs):
        # print 'startElement',name,name == self._element,'time_lastint', self.time_lastint,'is_inter_valid',self.is_inter_valid,'e=',self._energy_step

        if name == 'timestep':
            if self.time_lastint < 0:
                # print '  set time_lastint to ',float(attrs['time'])
                self.time_lastint = float(attrs['time'])
                self.is_inter_valid = False
            else:

                time_inter_begin = self.time_lastint
                # print 79*'-'
                # print '   time_inter_begin',time_inter_begin,'is_valid',time_inter_begin >= self._sumo.time_warmup
                if time_inter_begin >= self._sumo.time_warmup:
                    self.is_inter_valid = True
                    time_inter_end = float(attrs['time'])
                    self.times.append(time_inter_end)
                    self.energies.append(self._energy_step)
                    self._energy_step = 0.0

                    self._time_inter = int(time_inter_end)-int(time_inter_begin)
                    # here we just take the start and end time ofthe whole
                    # measurement period
                    if self._time_begin is None:  # take very first time only
                        self._time_begin = int(time_inter_begin)
                    self._time_end = int(time_inter_end)
                    self.n_inter += 1
                else:
                    self.is_inter_valid = False

        if name == self._element:
            if not self.is_inter_valid:
                return  # no interval initialized

            id_elem = self.get_id_elem(attrs)
            # print '  ---id_veh_sumo',id_elem
            if id_elem == -1:
                return  # id did not fit requested trip type

            if id_elem not in self._ids:
                self._ids.append(id_elem)

            for attrsconfig in self._attrsconfigs_cumulative:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname
                # print '    attrname (cum)',attrname,xmltag,attrs.has_key(xmltag)
                if attrs.has_key(xmltag):

                    a = attrs[xmltag]

                    if a.strip() != '':
                        a = float(a)
                        if self._values[attrname].has_key(id_elem):
                            self._values[attrname][id_elem] += a
                            # print '      added val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)
                        else:
                            self._values[attrname][id_elem] = a
                            # print '    set val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)

                        if attrname == self._consumptionsname:
                            self._energy_step += a

            for attrsconfig in self._attrsconfigs_average:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname
                # print '    attrname (av)',attrname,xmltag,attrs.has_key(xmltag)
                if attrs.has_key(xmltag):

                    a = attrs[xmltag]
                    if a.strip() != '':
                        a = float(a)
                        if self._values[attrname].has_key(id_elem):
                            valcum, n = self._values[attrname][id_elem]
                            # print '    add val', float(a),'to',valcum
                            valcum += a
                            n += 1
                        else:
                            valcum = a
                            n = 1
                            # print '    set val',valcum
                        # print '      added cumval',xmltag,attrname,valcum,'val',float(a)
                        self._values[attrname][id_elem] = (valcum, n)

    def get_times(self):
        return np.array(self.times, dtype=np.float32)

    def get_energies(self):
        return np.array(self.energies, dtype=np.float32)


class TripresultReader(IntervalAvReader2):
    def __init__(self, trips, element, sumo, attrsconfigs_cumulative, attrsconfigs_average):
        """
        element is "lane" or "edge" or "tripinfo" or "personinfo"
        attrnames is a list of attribute names to read.

        """
        self._trips = trips
        #IntervalAvReader2.__init__(self, element,sumo,  attrsconfigs_cumulative, attrsconfigs_average)
        self._element = element
        self._sumo = sumo  # the sumo process generating the data
        self._attrsconfigs_cumulative = attrsconfigs_cumulative
        self._attrsconfigs_average = attrsconfigs_average
        #self._time_begin = None
        #self._time_end = None
        self._values = {}
        self._ids = []
        #self._n_values= {}
        #self.is_inter_valid = False
        #self.n_inter = 0
        #self.n_test = 0
        #self.n_test2 = 0

        # TODO: if we knew here all ids then we
        # could create a numeric array per attribute
        # idea: pass ids as input arg
        for attrsconfig in attrsconfigs_cumulative+attrsconfigs_average:
            self._values[attrsconfig.attrname] = {}
            # print '  init',attrsconfig.attrname
            #self._n_values= {}

    def get_id_elem(self, attrs):
        """
        Returns the trip ID as integer if the desired trip type has been read.
        Otherwise -1 is returned. 
        """
        # print 'get_id_elem check id=',attrs['id'],self._trips,id(self._trips)
        return self._trips.get_id_from_id_sumo(attrs['id'])

    def startElement(self, name, attrs):
        # if attrs.has_key('id'):
        #    print '  parse',name,self._element,name == self._element,'id=',attrs.get('id','-')

        if name == self._element:

            id_elem = self.get_id_elem(attrs)
            # print '  ---id_elem',id_elem
            if id_elem == -1:
                return  # id did not fit requested trip type

            if id_elem not in self._ids:
                self._ids.append(id_elem)

            # no arrival data availlable if trip has not been finished!!
            for attrsconfig in self._attrsconfigs_cumulative:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname

                if attrs.has_key(xmltag):
                    # print '  attrname',attrname,attrs.has_key(attrname)
                    if attrs.has_key(attrname):

                        # print '   val *'+attrs[xmltag]+'*'
                        a = attrs[xmltag]

                        if a.strip() != '':
                            if self._values[attrname].has_key(id_elem):
                                self._values[attrname][id_elem] += float(a)
                            else:
                                self._values[attrname][id_elem] = float(a)

                    # if (id in ('1/0to1/1','1/0to2/0')) & (attrname == 'entered'):
                    #    self.n_test+=int(attrs[attrname])
                    #    print '  -read ',id,attrname,attrs[attrname],self.n_test,self._values[attrname][id]
                    #
                    # if (id in ('0/0to1/0')) & (attrname == 'left'):
                    #    self.n_test2+=int(attrs[attrname])
                    #    print '  +read ',id,attrname,attrs[attrname],self.n_test2,self._values[attrname][id]

            for attrsconfig in self._attrsconfigs_average:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname
                if attrs.has_key(xmltag):
                    # n=float(self.n_inter)
                    a = attrs[xmltag]
                    if a.strip() != '':
                        if self._values[attrname].has_key(id_elem):
                            valcum, n = self._values[attrname][id_elem]
                            valcum += float(a)
                            n += 1
                            #self._values[attrname][id_elem] = ( (n-1)*self._values[attrname][id_elem] + float(a))/n
                            #self._values[attrname][id] += float(a)/self._time_inter
                            #self._n_values[attrname][id] += 1
                        else:
                            valcum = float(a)
                            n = 1
                            #self._values[attrname][id_elem] = float(a)
                            #self._values[attrname][id_elem] = float(a)/self._time_inter
                            #self._n_values[attrname][id] = 1
                        self._values[attrname][id_elem] = (valcum, n)

    def get_data(self):
        return self._values

    def get_ids(self):
        return self._ids


def read_electrical_energy(filepath, sumo,  element, attrsconfigs):
    print 'read_electrical_energy'
    attrsconfigs_cumulative = []
    attrsconfigs_average = []
    for attrsconfig in attrsconfigs:
        # print '  check',attrsconfig.attrname
        if hasattr(attrsconfig, 'is_average'):
            if attrsconfig.is_average:
                attrsconfigs_average.append(attrsconfig)
            else:
                attrsconfigs_cumulative.append(attrsconfig)
        else:
            attrsconfigs_cumulative.append(attrsconfig)

    reader = ElectricalEnergyReader(element, sumo,  attrsconfigs_cumulative, attrsconfigs_average)
    parse(filepath, reader)
    return reader.get_ids(), reader.get_data(), reader.get_interval(), reader.get_times(), reader.get_energies()


class ElectricalEnergyReader(IntervalAvReader2):
    """
    Reads intervals of the electrcal energy file and cumulates data.
    """

    def __init__(self, element, sumo,  attrsconfigs_cumulative, attrsconfigs_average):
        """
        element is "lane" or "edge" or "tripinfo"
        attrnames is a list of attribute names to read.
        """
        print 'ElectricalEnergyReader', element
        # print '  attrsconfigs_cumulative',attrsconfigs_cumulative
        # print '  attrsconfigs_average',attrsconfigs_average
        IntervalAvReader2.__init__(self, element, sumo,  attrsconfigs_cumulative, attrsconfigs_average)
        self.time_lastint = -1.0

        self.times = []
        self.energies = []
        self._energy_step = 0.0  # energy os all vehicles during last step
        self._consumptionsname = "consumptions"

    def startElement(self, name, attrs):
        # print 'startElement',name,name == self._element,'time_lastint', self.time_lastint,'is_inter_valid',self.is_inter_valid,'e=',self._energy_step

        if name == 'timestep':
            if self.time_lastint < 0:
                # print '  set time_lastint to ',float(attrs['time'])
                self.time_lastint = float(attrs['time'])
                self.is_inter_valid = False
            else:

                time_inter_begin = self.time_lastint
                # print 79*'-'
                # print '   time_inter_begin',time_inter_begin,'is_valid',time_inter_begin >= self._sumo.time_warmup
                if time_inter_begin >= self._sumo.time_warmup:
                    self.is_inter_valid = True
                    time_inter_end = float(attrs['time'])
                    self.times.append(time_inter_end)
                    self.energies.append(self._energy_step)
                    self._energy_step = 0.0

                    self._time_inter = int(time_inter_end)-int(time_inter_begin)
                    # here we just take the start and end time ofthe whole
                    # measurement period
                    if self._time_begin is None:  # take very first time only
                        self._time_begin = int(time_inter_begin)
                    self._time_end = int(time_inter_end)
                    self.n_inter += 1
                else:
                    self.is_inter_valid = False

        if name == self._element:
            if not self.is_inter_valid:
                return  # no interval initialized

            id_elem = self.get_id_elem(attrs)
            # print '  ---id_veh_sumo',id_elem
            if id_elem == -1:
                return  # id did not fit requested trip type

            if id_elem not in self._ids:
                self._ids.append(id_elem)

            for attrsconfig in self._attrsconfigs_cumulative:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname
                # print '    attrname (cum)',attrname,xmltag,attrs.has_key(xmltag)
                if attrs.has_key(xmltag):

                    a = attrs[xmltag]

                    if a.strip() != '':
                        a = float(a)
                        if self._values[attrname].has_key(id_elem):
                            self._values[attrname][id_elem] += a
                            # print '      added val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)
                        else:
                            self._values[attrname][id_elem] = a
                            # print '    set val',xmltag,attrname,self._values[attrname][id_elem],'val',float(a)

                        if attrname == self._consumptionsname:
                            self._energy_step += a

            for attrsconfig in self._attrsconfigs_average:
                xmltag = attrsconfig.xmltag
                attrname = attrsconfig.attrname
                # print '    attrname (av)',attrname,xmltag,attrs.has_key(xmltag)
                if attrs.has_key(xmltag):

                    a = attrs[xmltag]
                    if a.strip() != '':
                        a = float(a)
                        if self._values[attrname].has_key(id_elem):
                            valcum, n = self._values[attrname][id_elem]
                            # print '    add val', float(a),'to',valcum
                            valcum += a
                            n += 1
                        else:
                            valcum = a
                            n = 1
                            # print '    set val',valcum
                        # print '      added cumval',xmltag,attrname,valcum,'val',float(a)
                        self._values[attrname][id_elem] = (valcum, n)

    def get_times(self):
        return np.array(self.times, dtype=np.float32)

    def get_energies(self):
        return np.array(self.energies, dtype=np.float32)


class TrajectoryReader(IntervalAvReader2):
    """
    Read trajectories, angles and speeds from a FCD XLM file

    """

    def __init__(self, element, sumo):
        """
        element is "lane" or "edge" or "tripinfo"
        attrnames is a list of attribute names to read.
        """
        print 'TrajectoryReader', element
        # print '  attrsconfigs_cumulative',attrsconfigs_cumulative
        # print '  attrsconfigs_average',attrsconfigs_average
        IntervalAvReader2.__init__(self, element, sumo,  [], [])
        #self.time_lastint = -1.0
        self.n_inter = 0
        self.times = []
        self.trajectories = {}
        self.angles = {}
        self.speeds = {}

        #self._attrnames = ()
        self._xname = "x"
        self._yname = "y"
        self._anglename = "angle"
        self._speedname = "speed"

    def startElement(self, name, attrs):
        # print 'startElement',name,name == self._element,'is_inter',self.n_inter

        # <vehicle id="<VEHICLE_ID>" x="<VEHICLE_POS_X>" y="<VEHICLE_POS_Y>" angle="<VEHICLE_ANGLE>" type="<VEHICLE_TYPE>" speed="<VEHICLE_SPEED>"/>
        if name == 'timestep':
            # if self.time_lastint < 0:
            #    #print '  set time_lastint to ',float(attrs['time'])
            #    self.time_lastint = float(attrs['time'])
            #    self.is_inter_valid = False
            # else:

            #time_inter_begin = self.time_lastint
            # print 79*'-'
            # print '   time_inter_begin',time_inter_begin,'is_valid',time_inter_begin >= self._sumo.time_warmup
            # if time_inter_begin >= self._sumo.time_warmup:
            #self.is_inter_valid = True
            time_inter_end = float(attrs['time'])
            self.times.append(time_inter_end)
            # self.energies.append(self._energy_step)
            #self._energy_step = 0.0

            #self._time_inter = int(time_inter_end)-int(time_inter_begin)
            # here we just take the start and end time ofthe whole
            # measurement period
            # if self._time_begin is None:# take very first time only
            #    self._time_begin = int(time_inter_begin)
            #self._time_end = int(time_inter_end)
            self.n_inter += 1
            # else:
            #   self.is_inter_valid = False

        if name == self._element:
            # if not self.is_inter_valid: return # no interval initialized

            id_elem = self.get_id_elem(attrs)  # id_sumo
            # print '  ---id_veh_sumo',id_elem
            if id_elem == -1:
                return  # id did not fit requested trip type

            if id_elem not in self._ids:
                self._ids.append(id_elem)

                self.trajectories[id_elem] = (self.n_inter-1)*[(np.nan, np.nan)]
                self.angles[id_elem] = (self.n_inter-1)*[np.nan]
                self.speeds[id_elem] = (self.n_inter-1)*[np.nan]

            self.trajectories[id_elem].append([float(attrs[self._xname].strip()), float(attrs[self._yname].strip())])
            self.angles[id_elem].append(float(attrs[self._anglename].strip()))
            self.speeds[id_elem].append(float(attrs[self._speedname].strip()))
            # print '    trajectories',[float(attrs[self._xname].strip()),float(attrs[self._yname].strip())]
            # print '    id_sumo_veh',id_elem,'n_inter',self.n_inter,'len(angles)',len(self.angles[id_elem]),'len(traj)',len(self.trajectories[id_elem])

    def endElement(self, name):
        if name == 'timestep':
            # print 'endElement',self.times[-1],len(self.times)
            for id_sumo_veh in self._ids:
                # print '    id_sumo_veh',id_sumo_veh,'n_inter',self.n_inter,'len(angles)',len(self.angles[id_sumo_veh]),'len(traj)',len(self.trajectories[id_sumo_veh])
                if len(self.trajectories[id_sumo_veh]) < self.n_inter:
                    self.trajectories[id_sumo_veh].append((np.nan, np.nan))
                if len(self.angles[id_sumo_veh]) < self.n_inter:
                    self.angles[id_sumo_veh].append(np.nan)
                if len(self.speeds[id_sumo_veh]) < self.n_inter:
                    self.speeds[id_sumo_veh].append(np.nan)


def read_trajectories(filepath, sumo,  element):
    print 'read_trajectories', element

    reader = TrajectoryReader(element, sumo)
    parse(filepath, reader)
    ids_sumo = reader.get_ids()
    times = np.array(reader.times, dtype=np.float32)
    n_veh = len(ids_sumo)
    trajectories = np.ones(n_veh, dtype=np.object)
    angles = np.ones(n_veh, dtype=np.object)
    speeds = np.ones(n_veh, dtype=np.object)

    for i, id_sumo in zip(xrange(n_veh), ids_sumo):
        trajectories[i] = reader.trajectories[id_sumo]
        angles[i] = reader.angles[id_sumo]
        speeds[i] = reader.speeds[id_sumo]

    return ids_sumo, times, trajectories, angles, speeds


def read_interval2(filepath, sumo,  element, attrsconfigs):
    # print 'read_interval2'
    attrsconfigs_cumulative = []
    attrsconfigs_average = []
    for attrsconfig in attrsconfigs:
        # print '  check',attrsconfig.attrname
        if hasattr(attrsconfig, 'is_average'):
            if attrsconfig.is_average:
                attrsconfigs_average.append(attrsconfig)
            else:
                attrsconfigs_cumulative.append(attrsconfig)
        else:
            attrsconfigs_cumulative.append(attrsconfig)

    reader = IntervalAvReader2(element, sumo,  attrsconfigs_cumulative, attrsconfigs_average)
    #parser = make_parser()
    # parser.setContentHandler(reader)
    #fn = '"'+filepath+'"'
    # print 'read_interval >'+fn+'<'
    # print '     >'+filepath+'<'
    # parser.parse(filepath)
    parse(filepath, reader)
    return reader.get_ids(), reader.get_data(), reader.get_interval()


def read_tripresult(filepath, sumo,  trips, element, attrsconfigs):
    print 'read_tripresult', filepath, trips.ident, 'element', element
    attrsconfigs_cumulative = []
    attrsconfigs_average = []
    for attrsconfig in attrsconfigs:
        # print '  check',attrsconfig.attrname
        if hasattr(attrsconfig, 'is_average'):
            if attrsconfig.is_average:
                attrsconfigs_average.append(attrsconfig)
            else:
                attrsconfigs_cumulative.append(attrsconfig)
        else:
            attrsconfigs_cumulative.append(attrsconfig)

    reader = TripresultReader(trips, element, sumo,  attrsconfigs_cumulative, attrsconfigs_average)
    #parser = make_parser()
    # parser.setContentHandler(reader)
    #fn = '"'+filepath+'"'
    # print 'read_interval >'+fn+'<'
    # print '    start parse >'+filepath+'<'
    # parser.parse(filepath)
    parse(filepath, reader)
    # print '  end',len(reader._ids)
    return reader.get_ids(), reader.get_data()


def read_routeresult(filepath, sumo,  trips, element, attrsconfigs):
    # print 'read_tripresult',filepath,trips.ident
    attrsconfigs_cumulative = []
    attrsconfigs_average = []
    for attrsconfig in attrsconfigs:
        # print '  check',attrsconfig.attrname
        if hasattr(attrsconfig, 'is_average'):
            if attrsconfig.is_average:
                attrsconfigs_average.append(attrsconfig)
            else:
                attrsconfigs_cumulative.append(attrsconfig)
        else:
            attrsconfigs_cumulative.append(attrsconfig)

    counter = RouteCounter()
    parse(filepath, counter)
    reader = RouteReader(trips, counter)  # parser = make_parser()
    # parser.setContentHandler(reader)
    #fn = '"'+filepath+'"'
    # print 'read_interval >'+fn+'<'
    # print '    start parse >'+filepath+'<'
    # parser.parse(filepath)
    parse(filepath, reader)
    # print '  end',len(reader._ids)
    return reader._get_kwargs()
