# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
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

# @file    publictransportservices.py
# @author  Joerg Schweizer
# @date   2012

import os
import sys
import string
from xml.sax import saxutils, parse, handler


import numpy as np
from numpy import random
from collections import OrderedDict
from coremodules.modules_common import *
from coremodules.network.routing import get_mincostroute_edge2edge
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.geometry import *
from agilepy.lib_base.processes import Process, CmlMixin
from agilepy.lib_base.misc import get_inversemap

from coremodules.simulation import results as res
from coremodules.demand.demandbase import DemandobjMixin
#from coremodules.network.network import SumoIdsConf, MODES

# These are all possible linktypes


# example
# https://github.com/planetsumo/sumo/blob/master/tests/sumo/extended/busses/three_busses/input_additional.add.xml


class PtLines(DemandobjMixin, am.ArrayObjman):
    def __init__(self, ident, demand, **kwargs):
        self._init_objman(ident=ident,
                          parent=demand,
                          name='Public transport lines',
                          info='Object, containing information on public transport line services.',
                          #xmltag = ('flows','flow','id'),
                          version=0.2,
                          **kwargs)

        self._init_attributes()

    # def set_version(self, verion = 0.1):
    #    self._version = 0.1

    def _init_attributes(self):
        # attrsman = self # = self.get_attrsman()
        demand = self.parent
        net = self.get_net()

        self.add(cm.ObjConf(PtLinks('ptlinks', self)))
        self.add_col(am.ArrayConf('linenames', default='',
                                  dtype='object',
                                  perm='rw',
                                  name='Line name',
                                  info='This is the official name or number of the line. Note that the same line may have several line services for different service periods.',
                                  xmltag='line',
                                  ))

        self.add_col(am.ArrayConf('times_begin', 0,
                                  name='Begin time',
                                  unit='s',
                                  perm='rw',
                                  info='Time when service begins.',
                                  xmltag='begin',
                                  ))

        self.add_col(am.ArrayConf('times_end', 0,
                                  name='End time',
                                  perm='rw',
                                  unit='s',
                                  info='Time when service ends.',
                                  xmltag='end',
                                  ))

        self.add_col(am.ArrayConf('periods', 0,
                                  name='Interval',
                                  perm='rw',
                                  unit='s',
                                  info='Time interval between consecutive vehicles.',
                                  xmltag='period',
                                  ))

        self.add_col(am.ArrayConf('times_dwell', 20,
                                  groupnames=['options'],
                                  perm='rw',
                                  name='Dwell time',
                                  untit='s',
                                  info='Dwell time in a stop while passengers are boarding/alighting.',
                                  xmltag='duration',
                                  ))

        self.add_col(am.IdlistsArrayConf('ids_stops', net.ptstops,
                                         groupnames=['parameters'],
                                         name='PT stop IDs',
                                         info='Sequence of IDs of stops or stations of a public transort line.',
                                         ))

        self.add_col(am.IdlistsArrayConf('ids_edges', net.edges,
                                         name='Edge IDs',
                                         info='Sequence of edge IDs constituting this public transport line.',
                                         xmltag='edges',
                                         ))

        self.add_col(am.IdsArrayConf('ids_vtype', demand.vtypes,
                                     id_default=0,
                                     groupnames=['state'],
                                     name='Veh. type ID',
                                     info='Vehicle type used to derve this line.',
                                     xmltag='type',
                                     ))

        self.add(cm.FuncConf('func_delete_row', 'on_del_row', None,
                             groupnames=['rowfunctions', '_private'],
                             name='Del line',
                             info='Delete line service.',
                             is_returnval=False,
                             ))

        if self.get_version() < 0.2:
            if hasattr(self, 'period'):
                self.delete('period')
            self.linenames.set_xmltag('line')
            self.ids_vtype.set_xmltag('type')
            self.times_dwell.set_xmltag('duration')
            self.periods.set_xmltag('period')
            self.times_end.set_xmltag('end')
            self.times_begin.set_xmltag('begin')

    def check_initial_stops(self):
        """
        Check whether the initial stop of each line is on the first 
        edge of the route. If not, connect the berth edge with the 
        first edge of the route. This method is used as work around for a bug
        in the PT line creation from GTFS.
        """
        stops = self.get_ptstops()
        net = self.get_net()
        # get edge travel times for each PT mode
        get_times = net.edges.get_times
        map_mode_to_times = {}
        fstar = net.edges.get_fstar()

        vtypes = self.parent.vtypes

        ids_line = self.get_ids()

        for id_line, ids_stop, id_mode in zip(ids_line,
                                              self.ids_stops[ids_line],
                                              vtypes.ids_mode[self.ids_vtype[ids_line]]):

            if len(ids_stop) >= 2:
                id_stopedge = net.lanes.ids_edge[stops.ids_lane[ids_stop[0]]]
                ids_edge = self.ids_edges[id_line]

                if ids_edge is not None:
                    if len(ids_edge) > 0:
                        if id_stopedge not in ids_edge:
                            id_edge = ids_edge[0]
                            print '  initial stop edge %d of line %d disconnected from route edge %d' % (id_stopedge, id_line, id_edge)
                            # print '    before:',ids_edge
                            if id_mode not in map_mode_to_times:
                                map_mode_to_times[id_mode] = get_times(id_mode=id_mode,
                                                                       is_check_lanes=True)

                            time, ids_edges_connect = get_mincostroute_edge2edge(
                                id_stopedge,
                                id_edge,
                                weights=map_mode_to_times[id_mode],
                                fstar=fstar)

                            if len(ids_edges_connect) > 0:
                                if ids_edges_connect[-1] == id_edge:
                                    print '      prepend connection', ids_edges_connect
                                    self.ids_edges[id_line] = ids_edges_connect[:-1] + ids_edge
                                    # print '    after:',self.ids_edges[id_line]

    def format_ids(self, ids):
        return ','.join(self.linenames[ids])

    def get_id_from_formatted(self, idstr):
        return self.linenames.get_id_from_index(idstr)

    def get_ids_from_formatted(self, idstrs):
        return self.linenames.get_ids_from_indices_save(idstrs.split(','))

    def get_net(self):
        return self.parent.get_net()

    def get_ptstops(self):
        return self.parent.get_net().ptstops

    def get_ptlinks(self):
        return self.ptlinks.get_value()

    def make(self, **kwargs):
        return self.add_row(linenames=kwargs.get('linename', None),
                            times_begin=kwargs.get('time_begin', None),
                            times_end=kwargs.get('time_end', None),
                            periods=kwargs.get('period', None),
                            times_dwell=kwargs.get('time_dwell', None),
                            ids_stops=kwargs.get('ids_stop', None),
                            ids_edges=kwargs.get('ids_edge', None),
                            ids_vtype=kwargs.get('id_vtype', None),
                            )

    def on_del_row(self, id_row=None):
        if id_row is not None:
            # print 'on_del_row', id_row
            self.del_row(id_row)

    def get_time_depart_first(self):
        if len(self) > 0:
            return float(np.min(self.times_begin.get_value()))
        else:
            return np.inf

    def get_time_depart_last(self):
        if len(self) > 0:
            return float(np.max(self.times_end.get_value()))
        else:
            return 0.0

    def guess_routes(self, is_keep_existing=False, ids_line=None):
        """
        Guess sequence of edges between stops if not previously specified 
        using shortest path routing.
        """
        # print 'guess_routes'
        if ids_line is None:
            ids_line = self.get_ids()
        vtypes = self.ids_vtype.get_linktab()
        ptstops = self.get_ptstops()
        net = self.get_net()
        #edges = net.edges
        lanes = net.lanes
        #'ids_lane', net.lanes,
        #ids_stopedge = ptstops.ids_lane
        ids_laneedge = net.lanes.ids_edge
        ids_stoplane = ptstops.ids_lane

        # make forward star for transport net
        fstar = net.edges.get_fstar()

        # get edge travel times for each PT mode
        get_times = net.edges.get_times
        map_mode_to_times = {}
        ids_mode = vtypes.ids_mode[self.ids_vtype[ids_line]]
        for id_mode in set(ids_mode):
            map_mode_to_times[id_mode] = get_times(id_mode=id_mode,
                                                   is_check_lanes=True)

        # complete routes between all pairs of stops of all lines
        for id_line, ids_stop, id_mode in zip(ids_line,
                                              self.ids_stops[ids_line],
                                              ids_mode
                                              ):
            # print '  id_line, ids_stop',id_line, ids_stop
            ids_stopedge = ids_laneedge[ids_stoplane[ids_stop]]
            # print '  ids_stopedge',ids_stopedge
            ids_edge = self.ids_edges[id_line]
            # print '  ids_edge',ids_edge
            if (ids_edge in [None, []]) | (not is_keep_existing):
                # complete route between stops
                ids_edge = []
                duration = 0
                for i in xrange(1, len(ids_stop)):
                    # print '    route',ids_stopedge[i-1],ids_stopedge[i]
                    time, ids_edges_current = get_mincostroute_edge2edge(
                        ids_stopedge[i-1],
                        ids_stopedge[i],
                        weights=map_mode_to_times[id_mode],
                        fstar=fstar)

                    # print '    ids_edges_current',ids_edges_current
                    if len(ids_edges_current) == 0:
                        # no connections found between stops
                        ids_edges = []
                        break
                    else:
                        duration += time
                        if i == 1:
                            ids_edge += ids_edges_current
                        else:
                            # avoid edge overlaps
                            ids_edge += ids_edges_current[1:]

                # print '  ids_edge',ids_edge
                self.ids_edges[id_line] = ids_edge

    def get_vtypes(self):
        return set(self.ids_vtype.get_value())

    def get_writexmlinfo(self, is_route=False, is_plain=False, **kwargs):
        """
        Returns three array where the first array is the 
        begin time of the first vehicle and the second array is the
        write function to be called for the respectice vehicle and
        the third array contains the vehicle ids

        Method used to sort trips when exporting to route or trip xml file
        """
        # print 'PtLines.get_writexmlinfo is_route, is_plain',is_route, is_plain,len(self)
        n = len(self)
        ids = self.get_ids()
        #
        if is_plain:
            ids_run = []
            times_begin_run = []
            # print '  ids',ids,
            # print '  times_begin',self.times_begin[ids]
            # print '  times_end',self.times_end[ids]
            # print '  periods',self.periods[ids]
            for id_line, time_begin, time_end, period in zip(ids, self.times_begin[ids], self.times_end[ids], self.periods[ids]):
                # print '    id_line, time_begin,time_end, period',id_line, time_begin,time_end, period
                i_run = 0
                for t_begin in np.arange(time_begin, time_end, period):
                    # print '  append (id_line,i_run)',(id_line,i_run),'t_begin',t_begin
                    ids_run.append((id_line, i_run))
                    times_begin_run.append(t_begin)
                    i_run += 1

            return times_begin_run, len(ids_run)*[self.write_run_xml], ids_run

        else:
            return self.times_begin[ids], n*[self.write_flow_xml], ids

    def get_trips(self):
        # returns trip object, method common to all demand objects
        return self

    def write_flow_xml(self, fd, id_line, time_begin, indent=0):
        #_idents = self.get_keys()
        #_inds = self.get_inds_from_keys(_idents)
        #_ids_egdes = self.cols.ids_edge[_inds]

        # for  _ind, _id_line, _ids_egde in zip(_inds, _idents, _ids_egdes):
        #vtype = self.cols.vtype[id_line]

        # write vehicle flow data
        fd.write(xm.start('flow id="ptline.%s"' % id_line, indent))
        for attrconfig in [self.ids_vtype,
                           self.linenames,
                           self.times_begin,
                           self.times_end,
                           self.periods,
                           ]:
            # print '    attrconfig',attrconfig.attrname
            attrconfig.write_xml(fd, id_line)

        #fd.write(xm.num('begin', '%d'%self.times_begin[id_line]))
        #fd.write(xm.num('end', '%d'%self.times_end[id_line]))
        #fd.write(xm.num('period', '%d'%self.periods[id_line]))
        #fd.write(xm.num('line', self.linenames[id_line]))
        #fd.write(xm.num('type', self.ids_vtype[id_line]))

        fd.write(xm.stop())

        # write route
        #ids_edge, duration = self.route(_id_line, vtype)
        ids_edge = self.ids_edges[id_line]
        if len(ids_edge) > 0:
            fd.write(xm.start('route', indent+2))
            self.ids_edges.write_xml(fd, id_line)
            # fd.write(xm.arr('edges',ids_egde,indent+4))
            #fd.write(xm.num('departPos', pos_depart))

            # depart lane is 1 , 0 would be on the side-walk)
            #fd.write(xm.num('departLane', laneind_parking))
            fd.write(xm.stopit())

        # write stops

        ids_stop = self.ids_stops[id_line]

        if len(ids_stop) > 0:
            stopnames = self.ids_stops.get_linktab().stopnames[ids_stop]
            time_dwell = self.times_dwell[id_line]
            for stopname in stopnames:
                fd.write(xm.start('stop', indent+2))
                fd.write(xm.num('busStop', stopname))
                fd.write(xm.num('duration', time_dwell))
                fd.write(xm.stopit())
        fd.write(xm.end('flow', indent))

    def write_run_xml(self, fd, id_run, time_begin, indent=0):
        """
        Writes PT runs to xml instead of flows
        """
        id_line, i_run = id_run
        #_idents = self.get_keys()
        #_inds = self.get_inds_from_keys(_idents)
        #_ids_egdes = self.cols.ids_edge[_inds]

        # for  _ind, _id_line, _ids_egde in zip(_inds, _idents, _ids_egdes):
        #vtype = self.cols.vtype[id_line]

        ids_edge_sumo = self.ids_edges.get_linktab().ids_sumo
        ids_edge = self.ids_edges[id_line]
        if len(ids_edge) > 0:

            # write vehicle flow data
            fd.write(xm.start('trip id="ptline.%s.%d"' % (id_line, i_run), indent))

            fd.write(xm.num('depart', '%d' % time_begin))
            self.ids_vtype.write_xml(fd, id_line)
            fd.write(xm.num('from', ids_edge_sumo[ids_edge[0]]))
            fd.write(xm.num('to', ids_edge_sumo[ids_edge[-1]]))
            fd.write(xm.stop())

            fd.write(xm.end('trip', indent))
            i_run += 1

    # def prepare_sim(self, process):
    #    return []

    def get_id_from_id_sumo(self, id_veh_sumo):
        # print 'ptservices.get_id_from_id_sumo',id_veh_sumo,id_veh_sumo.split('.')
        if len(id_veh_sumo.split('.')) == 3:
            prefix, id_veh, ind_run = id_veh_sumo.split('.')
            if prefix == 'ptline':
                return int(id_veh)
            else:
                return -1
        return -1

    def config_results(self, results):
        tripresults = res.Tripresults('publiclineresults', results,
                                      self,
                                      self.get_net().edges,
                                      name='Public line results',
                                      info='Table with simulation results for each public transport line. The results refer to all trips made by all vehicles of a public transport line during the entire simulation period.',
                                      )

        results.add_resultobj(tripresults, groupnames=['Trip results'])

    def process_results(self, results, process=None):
        pass


class PtLinks(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions
    def __init__(self, ident, ptlines, **kwargs):
        self._init_objman(ident=ident, parent=ptlines,
                          name='PT links',
                          #xmltag = ('edges','edge','ids_sumo'),
                          info='Public transport stop-to-stop links',
                          version=0.1,
                          **kwargs)

        self._init_attributes()

    def _init_attributes(self):
        LINKTYPES = {'none': 0,
                     'enter': 1,
                     'transit': 2,
                     'board': 3,
                     'alight': 4,
                     'transfer': 5,
                     'walk': 6,
                     'exit': 7,
                     }
        ptlines = self.parent
        net = self.parent.parent.get_net()

        self.add_col(am.ArrayConf('types', LINKTYPES['none'],
                                  dtype=np.int32,
                                  perm='rw',
                                  choices=LINKTYPES,
                                  name='Type',
                                  info='Type of PT link. Walking is needed to tranfer between lines.',
                                  # xmltag = 'type', # should not be exported?
                                  ))

        self.add_col(am.IdsArrayConf('ids_fromstop', net.ptstops,
                                     groupnames=['state'],
                                     name='ID stop from',
                                     info='ID of stop where the link starts.',
                                     xmltag='from',
                                     ))

        self.add_col(am.IdsArrayConf('ids_tostop', net.ptstops,
                                     groupnames=['state'],
                                     name='ID stop to',
                                     info='ID of stop where the link ends.',
                                     xmltag='to',
                                     ))

        # Attention, when building Fstar, we need to take
        # into account the travel time, as lines
        # are defined over time intervals
        self.add_col(am.IdsArrayConf('ids_line', ptlines,
                                     groupnames=['state'],
                                     name='Line ID',
                                     info='ID of public transport line. -1 means no line, in case of walking.',
                                     ))

        self.add_col(am.NumArrayConf('lengths', 0.0,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='r',
                                     name='Length',
                                     unit='m',
                                     info='Edge length.',
                                     ))

        self.add_col(am.NumArrayConf('durations', 0.0,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Duration',
                                     unit='s',
                                     info='Time duration of link, including dwell time.',
                                     ))

        self.add_col(am.IdlistsArrayConf('ids_links_forward', self,
                                         groupnames=['parameters'],
                                         name='Forward link IDs',
                                         info='Forward link IDs.',
                                         ))
        if self.get_version() < 0.1:
            self.types.choices = LINKTYPES
            self.delete('speeds_line')

    def print_link(self, id_link, ident=2, is_edges=True, is_link_forward=False):
        linktype = get_inversemap(self.types.choices)[self.types[id_link]]
        if linktype == 'transit':
            id_line = self.ids_line[id_link]
            time_interval = self.parent.periods[id_line]

            print ident*' ', 'PtLink:', id_link, 'line', id_line, '(%s)' % (self.parent.linenames[id_line]), 'from stop', self.ids_fromstop[id_link], 'to stop', self.ids_tostop[id_link], 'T=%ds I=%ds' % (self.durations[id_link], time_interval)
            #id_link_board = self.get_link_board(id_link)
            # print ident*' ',        'check interval: I=%d, I_board=%d, id_link_board=%d'%(time_interval, self.durations[id_link_board], id_link_board),'ids_link_forward',self.ids_links_forward[id_link_board]

            if is_edges:
                print ident*' ', '      ids_edge=', self.get_ids_edge(id_link)

        elif linktype == 'board':
            id_line = self.ids_line[id_link]
            print ident*' ', 'PtLink:', id_link, 'type', linktype, 'T=%ds' % (self.durations[id_link]), 'line', id_line, '(%s)' % (self.parent.linenames[id_line]), 'at stop', self.ids_fromstop[id_link]

        elif linktype == 'walk':
            print ident*' ', 'PtLink:', id_link, 'type', linktype, 'T=%ds' % (self.durations[id_link]), 'from stop', self.ids_fromstop[id_link], 'to stop', self.ids_tostop[id_link]

        else:

            print ident*' ', 'PtLink:', id_link, 'type', linktype, 'T=%ds' % (self.durations[id_link]), 'at stop', self.ids_fromstop[id_link]

        if is_link_forward:
            # print ident*' ','      ids_link_forward=',self.ids_links_forward[id_link]
            for id_link_forward in self.ids_links_forward[id_link]:
                self.print_link(id_link_forward, ident+4, is_edges=False, is_link_forward=False)

    def get_ids_edge(self, id_link):
        """
        Returns the network edge ids used by id_link in case of a transit link.
        Otherwirse returns an empty list.
        """
        if self.types.choices['transit'] == self.types[id_link]:
            id_line = self.ids_line[id_link]
            ptlines = self.parent
            lanes = ptlines.get_net().lanes
            ptstops = ptlines.get_ptstops()
            id_edge_from = lanes.ids_edge[ptstops.ids_lane[self.ids_fromstop[id_link]]]
            id_edge_to = lanes.ids_edge[ptstops.ids_lane[self.ids_tostop[id_link]]]
            ids_edge = ptlines.ids_edges[self.ids_line[id_link]]
            if id_edge_from in ids_edge:
                ind_from = ids_edge.index(id_edge_from)
            else:
                return []
            if id_edge_to in ids_edge:
                ind_to = ids_edge.index(id_edge_to)
                return ids_edge[ind_from:ind_to]  # this excludes last edge
            else:
                return []
        else:
            return []

    def get_ptstops(self):

        ids_stop = ptlines.ids_stop

    def build(self, dist_walk_los=150.0, speed_walk_los=0.5,
              duration_transfer=10, duration_alight=10,
              duration_exit=5, duration_enter=5):
        """
        Bulid PT links from PT lines and PT stops.

        dist_walk_los is the line-of-sight walking distance acceptable between
        two stops for transfer.

        speed_walk_los is the assumed line of sight walking speed
        between two stops
        """
        print 'build', self.ident, dist_walk_los, speed_walk_los

        id_stop_debug = 459

        self.clear()
        ptlines = self.parent
        net = self.parent.get_net()

        ptlinktypes = self.types.choices
        type_enter = ptlinktypes['enter']
        type_transit = ptlinktypes['transit']
        type_board = ptlinktypes['board']
        type_alight = ptlinktypes['alight']
        type_transfer = ptlinktypes['transfer']
        type_walk = ptlinktypes['walk']
        type_exit = ptlinktypes['exit']

        edgelengths = net.edges.lengths
        edgespeeds = net.edges.speeds_max
        demand = ptlines.parent
        ids_line = ptlines.get_ids()

        ids_stoplane = net.ptstops.ids_lane
        ids_laneedge = net.lanes.ids_edge
        stoppositions = net.ptstops.positions_to

        # dictionary with is_sto as key and a dicionary with
        # links as value
        stoplinks = {}
        for id_stop in net.ptstops.get_ids():
            stoplinks[id_stop] = {'ids_transit_out': [],
                                  'ids_transit_in': [],
                                  'ids_board': [],
                                  'ids_alight': [],
                                  'id_transfer': -1,
                                  'ids_walk': [],
                                  'id_exit': -1
                                  }

        # first create links between stops of each line
        ids_ptlines_failed = []
        for id_line,  id_vtype, ids_stop, ids_edge, linename in zip(
            ids_line,
            ptlines.ids_vtype[ids_line],
            ptlines.ids_stops[ids_line],
            ptlines.ids_edges[ids_line],
            ptlines.linenames[ids_line],
        ):
            n_edge = len(ids_edge)
            n_stop = len(ids_stop)
            print 79*'-'
            print 'Build links of line', linename, 'id_line', id_line, 'n_edge', n_edge, 'n_stop', n_stop
            if (len(ids_edge) > 1) & (len(ids_stop) > 2):

                # check if all stop edges are on route and in the correct order:
                ind_edge = 0

                id_stopedge_next = ids_laneedge[ids_stoplane[ids_stop[1]]]
                for ind_stop in xrange(1, n_stop):
                    id_fromstop = ids_stop[ind_stop-1]
                    id_tostop = ids_stop[ind_stop]
                    if id_fromstop != id_tostop:
                        is_cont = True
                        is_mismatch = False
                        while is_cont:
                            if id_stopedge_next == ids_edge[ind_edge]:
                                is_cont = False

                            elif ind_edge == n_edge-1:
                                is_mismatch = True
                                is_cont = False
                            else:
                                ind_edge += 1

                            #    print '      ind_edge,id_stopedge_next,ids_edge[ind_edge]',ind_edge,id_stopedge_next,ids_edge[ind_edge],len(ids_edge)

                    #is_mismatch =   (ind_edge == (n_edge-1)) & (ind_stop != (n_stop-1))
                    print '    ind_stop', ind_stop, 'ind_edge', ind_edge, is_mismatch
                    if is_mismatch:
                        break

                    if id_tostop != ids_stop[-1]:
                        id_stopedge_next = ids_laneedge[ids_stoplane[ids_stop[ind_stop+1]]]

                # if set(ids_edge).issuperset(ids_laneedge[ids_stoplane[ids_stop[1:]]]):
                if not is_mismatch:

                    length = edgelengths[ids_edge[0]] - stoppositions[ids_stop[0]]

                    # TODO: limit speed by vtype
                    duration = length/edgespeeds[ids_edge[0]]
                    #length_laststop = length_current
                    id_stopedge_next = ids_laneedge[ids_stoplane[ids_stop[1]]]

                    ids_link = []
                    ind_edge = 0
                    for ind_stop in xrange(1, len(ids_stop)):
                        id_fromstop = ids_stop[ind_stop-1]
                        id_tostop = ids_stop[ind_stop]

                        # if id_fromstop == id_stop_debug:
                        print '    ind_stop', ind_stop, 'id_fromstop,id_tostop', id_fromstop, id_tostop, 'ind_edge', ind_edge

                        # this prevents error in case two successive stops have
                        # (by editing error) the same ID
                        if id_fromstop != id_tostop:

                            # compute length and time between  fromstop and tostop
                            while id_stopedge_next != ids_edge[ind_edge]:
                                # if id_fromstop == id_stop_debug:
                                print '      ind_edge', ind_edge, ',id_stopedge_next,ids_edge[ind_edge]', id_stopedge_next, ids_edge[ind_edge]
                                ind_edge += 1
                                length_edge = edgelengths[ids_edge[ind_edge]]
                                length += length_edge
                                # TODO: limit speed by vtype
                                duration += length_edge/edgespeeds[ids_edge[ind_edge]]

                            # adjust length and time measurement on last edge
                            length_delta = edgelengths[id_stopedge_next] - stoppositions[id_tostop]
                            length -= length_delta
                            duration -= length_delta/edgespeeds[id_stopedge_next]

                            # add dwell time
                            duration += ptlines.times_dwell[id_line]

                            id_link = self.add_row(types=type_transit,
                                                   ids_fromstop=id_fromstop,
                                                   ids_tostop=id_tostop,
                                                   ids_line=id_line,
                                                   lengths=length,
                                                   durations=duration,
                                                   )
                            ids_link.append(id_link)

                            # debugging
                            if id_fromstop == id_stop_debug:
                                print '    append transit link', id_link

                            stoplinks[id_fromstop]['ids_transit_out'].append(id_link)
                            stoplinks[id_tostop]['ids_transit_in'].append(id_link)
                            #  is id_tostop the last stop?
                            if id_tostop != ids_stop[-1]:
                                # prepare lengthe and duration mesurement
                                id_stopedge_next = ids_laneedge[ids_stoplane[ids_stop[ind_stop+1]]]
                                length = length_delta
                                duration = 0.0

                    # create forward links for this line
                    for i in xrange(1, len(ids_link)):
                        self.ids_links_forward[ids_link[i-1]] = [ids_link[i]]
                    # put empty link list to line end-stop
                    self.ids_links_forward[ids_link[i]] = []

                else:
                    print 'WARNING in line', linename, 'id_line', id_line
                    print '         stop edges not on route, line will not build.'
                    ids_ptlines_failed.append(id_line)

        # complete stoplink database

        #ids_link_transit = self.get_ids().copy()
        #ids_fromstop = self.ids_fromstop[ids_link_transit].copy()
        #ids_tostop = self.ids_tostop[ids_link_transit].copy()
        #ids_lines_transit = self.ids_line[ids_link_transit].copy()
        periods = ptlines.periods
        #periods_transit = ptlines.periods[ids_lines_transit]

        # get for each stop a list of close stops with distances
        ids_stops_prox = net.ptstops.get_stops_proximity(dist_walk_los=dist_walk_los)
        # print '  ids_stops_prox', ids_stops_prox

        for id_stop in net.ptstops.get_ids():

            # walk links
            dists_stop_prox, ids_stop_prox = ids_stops_prox[id_stop]
            n_stop_prox = len(ids_stop_prox)
            unitvec = np.ones(n_stop_prox, dtype=np.int32)
            ids_walk = self.add_rows(types=type_walk*unitvec,  # access
                                     ids_fromstop=id_stop*unitvec,
                                     ids_tostop=ids_stop_prox,
                                     lengths=dists_stop_prox,
                                     durations=dists_stop_prox/speed_walk_los,
                                     ids_links_forward=[None, ]*n_stop_prox,  # later
                                     )

            stoplinks[id_stop]['ids_walk'] = ids_walk

            # boarding links
            # print '    ids_transit_out',stoplinks[id_stop]['ids_transit_out']
            ids_transit = np.array(stoplinks[id_stop]['ids_transit_out'], dtype=np.int32)
            # print '    ids_transit',ids_transit
            n_transit = len(ids_transit)
            unitvec = np.ones(n_transit, dtype=np.int32)
            # print '    type_board*unitvec',type_board*unitvec
            ids_board = self.add_rows(n=n_transit,
                                      types=type_board*unitvec,  # access
                                      ids_fromstop=id_stop*unitvec,
                                      ids_tostop=id_stop*unitvec,
                                      ids_line=self.ids_line[ids_transit],
                                      lengths=1.0*unitvec,
                                      durations=0.5*periods[self.ids_line[ids_transit]],
                                      ids_links_forward=ids_transit.reshape(-1, 1).tolist(),
                                      )

            stoplinks[id_stop]['ids_board'] = ids_board

            # transfer link
            id_transfer = self.add_row(types=type_transfer,  # access
                                       ids_fromstop=id_stop,
                                       ids_tostop=id_stop,
                                       lengths=1.0,
                                       speeds_line=1.0,
                                       durations=duration_transfer,
                                       ids_links_forward=ids_board.tolist()+ids_walk.tolist(),  # completed below
                                       )

            stoplinks[id_stop]['id_transfer'] = id_transfer

            # exit link
            id_exit = self.add_row(types=type_exit,  # access
                                   ids_fromstop=id_stop,
                                   ids_tostop=id_stop,
                                   lengths=0.0,
                                   durations=duration_exit,
                                   ids_links_forward=[],  # dead end
                                   )
            stoplinks[id_stop]['id_exit'] = id_exit

            # enter link
            id_enter = self.add_row(types=type_enter,  # access
                                    ids_fromstop=id_stop,
                                    ids_tostop=id_stop,
                                    lengths=0.0,
                                    durations=duration_enter,
                                    ids_links_forward=[id_transfer, id_exit],
                                    )

            stoplinks[id_stop]['id_enter'] = id_enter

            #  alight links
            ids_transit = np.array(stoplinks[id_stop]['ids_transit_in'], dtype=np.int32)
            n_transit = len(ids_transit)
            unitvec = np.ones(n_transit, dtype=np.int32)
            ids_alight = self.add_rows(n=n_transit,
                                       types=type_alight*unitvec,  # access
                                       ids_fromstop=id_stop*unitvec,
                                       ids_tostop=id_stop*unitvec,
                                       ids_line=self.ids_line[ids_transit],
                                       lengths=0.0*unitvec,
                                       durations=duration_alight*unitvec,
                                       ids_links_forward=[[id_transfer, id_exit, ]]*n_transit,
                                       )
            # print '    ids_links_forward[ids_transit]',self.ids_links_forward[ids_transit]
            # print '    ids_alight.reshape(-1,1).tolist()',ids_alight.reshape(-1,1).tolist()
            for id_transit, id_alight in zip(ids_transit, ids_alight):
                self.ids_links_forward[id_transit].append(id_alight)

            stoplinks[id_stop]['ids_alight'] = ids_alight

            # debugging
            if id_stop == id_stop_debug:
                print '    Parameters of id_stop', id_stop
                for key, val in stoplinks[id_stop].iteritems():
                    if key in ['ids_transit_out', 'ids_transit_in', 'ids_board', 'ids_alight']:
                        print '    ', key, ':'
                        for id_link in val:
                            self.print_link(id_link, ident=6)

                    else:
                        print '    ', key, ':', val

        # connect walk links from one stop to board and transfer
        for id_stop in net.ptstops.get_ids():
            ids_walk = stoplinks[id_stop]['ids_walk']
            for id_walk, id_tostop in zip(ids_walk, self.ids_tostop[ids_walk]):
                self.ids_links_forward[id_walk] = [stoplinks[id_tostop]['id_enter']]
        if 0:
            # debugging
            print 79*'='
            ids_link = self.get_ids()
            for id_link, linktype, id_fromstop, id_tostop, id_line, duration, ids_link_forward in zip(
                    ids_link, self.types[ids_link], self.ids_fromstop[ids_link], self.ids_tostop[ids_link], self.ids_line[ids_link], self.durations[ids_link], self.ids_links_forward[ids_link]):
                if id_fromstop == id_stop_debug:
                    print '  FROM', id_stop_debug, 'TO', id_tostop
                    self.print_link(id_link, ident=4)
                    print '      ids_link_forward=', ids_link_forward

            print 79*'='
            for id_link, linktype, id_fromstop, id_tostop, id_line, duration, ids_link_forward in zip(
                    ids_link, self.types[ids_link], self.ids_fromstop[ids_link], self.ids_tostop[ids_link], self.ids_line[ids_link], self.durations[ids_link], self.ids_links_forward[ids_link]):

                if id_tostop == id_stop_debug:
                    print '  FROM', id_fromstop, 'TO', id_stop_debug
                    self.print_link(id_link, ident=4)
                    print '      ids_link_forward=', ids_link_forward

        # debug
        print 'Number of failed Ptlines', len(ids_ptlines_failed), 'of', len(ptlines)
        for id_line, linename, period in zip(
            ids_ptlines_failed,
            ptlines.linenames[ids_ptlines_failed],
            ptlines.periods[ids_ptlines_failed],
        ):
            print '    Failed to build line', linename, 'id_line', id_line, 'period', period, 's'

        return ids_ptlines_failed

    def get_map_stop_to_ptlinks(self, is_fromstop=True, is_tostop=False, linktype=2):
        """
        Returns a dictionary, mapping id stop to a list of PT links, where
        is_fromstop and is_tostop indicates whether the stop should be
        at the start or at the end of the link.

        linktype specifies the link type ID , default is 2 ("transit")
        """
        # print 'get_map_stop_to_link'
        ptlines = self.parent
        #net = self.parent.get_net()

        #ptlinktypes = self.types.choices
        #type_transit = ptlinktypes['transit']
        #type_transfer = ptlinktypes['transfer']
        #type_transfer = ptlinktypes['transfer']
        map_id_stop_to_ids_link = {}
        ids = self.get_ids()
        if is_fromstop:
            for id_link, linktype_stop, id_stop in zip(ids, self.types[ids], self.ids_fromstop[ids]):
                if linktype == linktype_stop:
                    if map_id_stop_to_ids_link.has_key(id_stop):
                        # print '  append id_transitlink',id_link,linktype,'to id_stop',id_stop
                        map_id_stop_to_ids_link[id_stop].append(id_link)
                    else:
                        map_id_stop_to_ids_link[id_stop] = [id_link]
                        # print '  add id_transitlink',id_link,linktype,'to id_stop',id_stop
        if is_tostop:
            for id_link, linktype_stop, id_stop in zip(ids, self.types[ids], self.ids_tostop[ids]):
                if linktype == linktype_stop:
                    if map_id_stop_to_ids_link.has_key(id_stop):
                        # print '  append id_transitlink',id_link,linktype,'to id_stop',id_stop
                        map_id_stop_to_ids_link[id_stop].append(id_link)
                    else:
                        map_id_stop_to_ids_link[id_stop] = [id_link]
                        # print '  add id_transitlink',id_link,linktype,'to id_stop',id_stop
        return map_id_stop_to_ids_link

    def get_link_board(self, id_link):
        """
        Returns boarding link for given transit type link.
        """
        ptlines = self.parent
        #net = self.parent.get_net()

        type_board = self.types.choices['board']

        ids = self.select_ids(self.types.get_value() == type_board)
        for id_link_board, id_link_forward in zip(ids, self.ids_links_forward[ids]):
            if id_link_forward == id_link:
                break
        return id_link_board

    def get_fstar(self, is_array=False):
        """
        Returns the forward star graph of the public  network as dictionary:
            fstar[id_fromedge] = set([id_toedge1, id_toedge2,...])
        """

        ids_link = self.get_ids()
        n = len(ids_link)
        if n == 0:
            # no links built
            if is_array:
                return []
            else:
                return {}
        # algo with dictionnary
        #fstar = {}
        # for id_link, ids_link_forward in zip(ids_link, self.link_forward[ids_link]):
        #    fstar[id_link] = ids_link_forward

        # algo with numarray as lookup
        fstar = np.array(np.zeros(np.max(ids_link)+1, np.object))
        fstar[ids_link] = self.ids_links_forward[ids_link]
        if is_array:
            return fstar
        else:
            return dict(np.concatenate((ids_link.reshape((-1, 1)), fstar[ids_link].reshape((-1, 1))), 1))

    def get_times(self, id_mode=0, is_randomwait=False):
        """
        Returns freeflow travel times for all PT links.
        The returned array represents the speed and the index corresponds to
        PT link IDs.

        TODO: mode selection
        """
        # print 'get_times id_mode,is_check_lanes,speed_max',id_mode,is_check_lanes,speed_max

        ids_link = self.get_ids()
        times = np.array(np.zeros(np.max(ids_link)+1, np.float32))
        times[ids_link] = self.durations[ids_link]

        if is_randomwait:
            ids_link_board = self.select_ids(self.types.get_value() == self.types.choices['board'])

            for id_link, time_board_max in\
                    zip(ids_link_board, 2*self.durations[ids_link_board]):
                times[id_link] = random.uniform(0.0, time_board_max)

        return times

    def get_stops_to_enter_exit(self):
        ptlinktypes = self.types.choices

        ids = self.select_ids(self.types.get_value() == ptlinktypes['enter'])
        ids_stops = self.ids_tostop[ids]
        stops_to_enter = np.array(np.zeros(np.max(ids_stops)+1, np.int32))
        stops_to_enter[ids_stops] = ids

        ids = self.select_ids(self.types.get_value() == ptlinktypes['exit'])
        ids_stops = self.ids_fromstop[ids]
        stops_to_exit = np.array(np.zeros(np.max(ids_stops)+1, np.int32))
        stops_to_exit[ids_stops] = ids
        return stops_to_enter, stops_to_exit

    def print_route(self, ids_link):
        typemap = get_inversemap(self.types.choices)
        for id_link, id_type, id_line, id_fromstop, id_tostop in\
                zip(ids_link,
                    self.types[ids_link],
                    self.ids_line[ids_link],
                    self.ids_fromstop[ids_link],
                    self.ids_tostop[ids_link]
                    ):
            if id_line >= 0:
                line = self.parent.linenames[id_line]
            else:
                line = 'X'
            print '%4d %06s  fromstop=%3d tostop=%3d %06s' % (id_link, line, id_fromstop, id_tostop, typemap[id_type])

    def route(self, id_fromstop, id_tostop,
              stops_to_enter=None, stops_to_exit=None,
              times=None, fstar=None):
        """
        Routes public transit from fromstop to tostop.
        Returned are the following arrays, one entry per stage:
            ids_line        : line IDs , with negative id for walking
            linktypes       : type of link
            ids_fromstop    : IDs of stops where each stage  starts
            ids_tostop      : IDs of stops where each stage  ends
            durations       : Duration of each stage in secs
        """
        print 'route id_fromstop, id_tostop', id_fromstop, id_tostop
        if times is None:
            times = self.get_times()

        if fstar is None:
            fstar = self.get_fstar()

        if stops_to_enter is None:
            stops_to_enter, stops_to_exit = self.get_stops_to_enter_exit()

        ptlinktypes = self.types.choices
        #type_enter = ptlinktypes['enter']
        type_transit = ptlinktypes['transit']
        type_board = ptlinktypes['board']
        type_alight = ptlinktypes['alight']
        #type_transfer = ptlinktypes['transfer']
        type_walk = ptlinktypes['walk']
        #type_exit = ptlinktypes['exit']

        # trick: pick a link that points to the fromstop
        # and one that starts with tostop
        # later remove first an last link and respective duration

        id_fromlink = stops_to_enter[id_fromstop]
        id_tolink = stops_to_exit[id_tostop]

        # print '  route id_fromstop, id_tostop',id_fromstop, id_tostop
        # print '  route id_fromlink, id_tolink',id_fromlink, id_tolink

        routeduration, route = get_mincostroute_edge2edge(
            id_fromlink, id_tolink,
            weights=times, fstar=fstar
        )

        # self.print_route(route)

        # print '  len(route)',len(route)
        if len(route) == 0:
            return [], [], [], [], []

        # unite links on the same line and determine durations
        # for each stage
        ids_line = []
        ids_fromstop = []
        ids_tostop = []
        durations = []
        linktypes = []
        id_line_last = -1
        duration_accum = 0.0
        # print '  self.types[route]',self.types[route],len(self.types[route])
        # print '  self.types[route]',self.types[route],len(self.types[route])
        # for linktype,id_line,duration in zip(self.types[route],self.ids_line[route],):
        #    print '    linktype',linktype,linktype == type_transit,linktype == type_walk
        #    print '    id_line',id_line
        #    #print '    ',

        for linktype, id_line, duration, id_fromstop, id_tostop in\
            zip(self.types[route],
                self.ids_line[route],
                self.durations[route],
                self.ids_fromstop[route], self.ids_tostop[route]):

            # print '    check linktype',linktype,'id_line',id_line,'duration',duration
            if linktype == type_transit:
                # check if this link is yet another stop of te same line
                if id_line_last == -1:
                    # no previous line, so it is
                    # the first transit link in a transit
                    # init first stage
                    ids_line.append(id_line)
                    ids_fromstop.append(id_fromstop)
                    ids_tostop.append(id_tostop)  # will be updated
                    linktypes.append(linktype)
                    durations.append(duration+duration_accum)  # will be updated
                    id_line_last = id_line
                    duration_accum = 0.0

                else:  # successive stop(s) in a transit
                    durations[-1] += duration
                    ids_tostop[-1] = id_tostop

            elif linktype == type_walk:
                ids_line.append(id_line)
                ids_fromstop.append(id_fromstop)
                ids_tostop.append(id_tostop)
                linktypes.append(linktype)
                durations.append(duration+duration_accum)
                id_line_last = -1
                duration_accum = 0.0
            else:
                # for all other link types simply accumulate duration
                # which will be added to a successive walk or transit stage
                duration_accum += duration
                id_line_last = -1

            # print '      ids_line',ids_line
            # print '      linktypes',linktypes
            # print '      durations',durations
            # print '      ids_fromstop',ids_fromstop
            # print '      ids_tostop',ids_tostop

        return durations, linktypes, ids_line, ids_fromstop, ids_tostop


class PtLinefilter(Process):
    def __init__(self, ident='ptlinefilter', ptlines=None,  logger=None, **kwargs):
        print 'PtLinefilter.__init__'

        # TODO: let this be independent, link to it or child??

        self._init_common(ident,
                          parent=ptlines,
                          name='Public Transport line filter',
                          logger=logger,
                          info='Removes undesired PT lines with specific characteristics.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.period_max = attrsman.add(cm.AttrConf('period_max', kwargs.get('period_max', 1200),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   name='Max. duration of period',
                                                   info="""Max. duration of period""",
                                                   ))

        self.n_stops_min = attrsman.add(cm.AttrConf('n_stops_min', kwargs.get('n_stops_min', 5),
                                                    groupnames=['options'],
                                                    perm='rw',
                                                    name='Min. number of stops',
                                                    info="""Min. number of stops.""",
                                                    ))

    def do(self):
        print 'PtLinefilter.do'
        # links
        ptlines = self.parent

        stops = ptlines.get_ptstops()
        #net = ptlines.get_net()
        #vtypes = ptlines.parent.vtypes

        ids_line = ptlines.get_ids()

        ids_remove = []
        for id_line, ids_stop, period in zip(ids_line,
                                             ptlines.ids_stops[ids_line],
                                             ptlines.periods[ids_line]
                                             ):
            if len(ids_stop) < self.n_stops_min:
                ids_remove.append(id_line)

            elif period > self.period_max:
                ids_remove.append(id_line)

        print '  Eliminated %d lines:' % (len(ids_remove))

        for id_line, name, ids_stop, period in zip(ids_remove,
                                                   ptlines.linenames[ids_remove],
                                                   ptlines.ids_stops[ids_remove],
                                                   ptlines.periods[ids_remove]
                                                   ):
            print '    id_line', id_line, 'name', name, 'period', period, 'n_stops', len(ids_stop)

        ptlines.del_rows(ids_remove)

        return True


class PtNetbuilder(Process):
    def __init__(self, ident='ptnetbuilder', ptlinks=None,  logger=None, **kwargs):
        print 'PtNetbuilder.__init__'

        # TODO: let this be independent, link to it or child??

        self._init_common(ident,
                          parent=ptlinks,
                          name='Public Transport Net builder',
                          logger=logger,
                          info='Builds Ptlinks of the Public Transport Net.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        # make for each possible pattern a field for prob

        self.dist_walk_los = attrsman.add(cm.AttrConf('dist_walk_los', kwargs.get('dist_walk_los', 150),
                                                      groupnames=['options'],
                                                      perm='rw',
                                                      name='Max. station internal walk dist.',
                                                      info="""Maximum station internal walking diststance specifies the maximum line of sight distance
                            between 2 stops such that a transfers between the stops can be considered.
                            """,
                                                      ))

        self.speed_walk_los = attrsman.add(cm.AttrConf('speed_walk_los', kwargs.get('speed_walk_los', 0.5),
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Walk speed between stops',
                                                       info="""The line of sight walk speed between stops is used to estimate the walking time between stops.""",
                                                       ))

        self.duration_transfer = attrsman.add(cm.AttrConf('duration_transfer', kwargs.get('duration_transfer', 10),
                                                          groupnames=['options'],
                                                          perm='rw',
                                                          name='Duration of transfer',
                                                          info="""Duration of transfer accounts for the time spend on the platform of the same stop during a transfer.
                                    More specifically, this is the time after alighting and before exiting or walking to another stop.
                                    """,
                                                          ))

        self.duration_alight = attrsman.add(cm.AttrConf('duration_alight', kwargs.get('duration_alight', 10),
                                                        groupnames=['options'],
                                                        perm='rw',
                                                        name='Duration of alighing',
                                                        info="""Duration of alighing accounts for the assumed alighing process from a PT vehicle. This is only a logistic duration, no change in position takes place.
                                    """,
                                                        ))
        self.duration_enter = attrsman.add(cm.AttrConf('duration_enter', kwargs.get('duration_enter', 5),
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Duration of stop entering',
                                                       info="""Duration of entering a stop or station. This is only a logistic duration, no change in position takes place.
                                    """,
                                                       ))
        self.duration_exit = attrsman.add(cm.AttrConf('duration_exit', kwargs.get('duration_exit', 5),
                                                      groupnames=['options'],
                                                      perm='rw',
                                                      name='Duration of stop exiting',
                                                      info="""Duration of exiting a stop or station. This is only a logistic duration, no change in position takes place.
                                    """,
                                                      ))

    def do(self):
        print 'VehicleProvider.do'
        # links

        #virtualpop = self.parent
        logger = self.get_logger()
        logger.w('Build PT network...')

        ids_ptlines_failed = self.parent.build(dist_walk_los=self.dist_walk_los, speed_walk_los=self.speed_walk_los,
                                               duration_transfer=self.duration_transfer, duration_alight=self.duration_alight,
                                               duration_exit=self.duration_exit, duration_enter=self.duration_enter)

        return True


class LineReader(handler.ContentHandler):
    """Reads pt lines from xml file into ptlines structure"""

    def __init__(self, ptlines):
        self._ptlines = ptlines

    def startElement(self, name, attrs):
        # print 'startElement',name
        if name == 'line':
            self._ptlines.add_xml(**attrs)
