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

# @file    origin_to_destination.py
# @author  Joerg Schweizer
# @date   2012


import numpy as np
from numpy import random
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
#from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from coremodules.network import routing
from agilepy.lib_base.processes import Process, CmlMixin

import demand as dm
import demandbase as db
from agilepy.lib_base.geometry import is_polyline_intersect_polygon


class OdFlowTable(am.ArrayObjman):
    def __init__(self, parent, modes, zones, activitytypes=None, **kwargs):
        self._init_objman(ident='odflowtab', parent=parent,
                          name='OD flows',
                          info='Table with intervals, modes, OD and respective number of trips.',
                          #xmltag = ('odtrips','odtrip',None),
                          **kwargs)

        self.add_col(am.ArrayConf('times_start', 0,
                                  groupnames=['parameters'],
                                  perm='r',
                                  name='Start time',
                                  unit='s',
                                  info='Start time of interval in seconds (no fractional seconds).',
                                  xmltag='t_start',
                                  ))

        self.add_col(am.ArrayConf('times_end', 3600,
                                  groupnames=['parameters'],
                                  perm='r',
                                  name='End time',
                                  unit='s',
                                  info='End time of interval in seconds (no fractional seconds).',
                                  xmltag='t_end',
                                  ))

        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['parameters'],
                                     perm='r',
                                     #choices = MODES,
                                     name='ID mode',
                                     xmltag='vClass',
                                     info='ID of transport mode.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_orig', zones,
                                     groupnames=['parameters'],
                                     name='Orig.',
                                     perm='r',
                                     #choices =  zones.ids_sumo.get_indexmap(),
                                     info='traffic assignment zone of origin of trip.',
                                     xmltag='id_orig',
                                     ))

        self.add_col(am.IdsArrayConf('ids_dest', zones,
                                     groupnames=['parameters'],
                                     name='Dest.',
                                     perm='r',
                                     #choices =  zones.ids_sumo.get_indexmap(),
                                     info='ID of traffic assignment zone of destination of trip.',
                                     xmltag='id_dest',
                                     ))

        self.add_col(am.ArrayConf('tripnumbers', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Trips',
                                  info='Number of trips from zone with ID Orig to zone with ID Dest.',
                                  xmltag='tripnumber',
                                  ))

        if activitytypes is not None:
            self.add_col(am.IdsArrayConf('ids_activitytype_orig', activitytypes,
                                         groupnames=['parameters'],
                                         perm='rw',
                                         #choices = activitytypes.names.get_indexmap(),
                                         name='Activity type at orig.',
                                         symbol='Act. orig.',
                                         info='Type of activity performed at origin, before the trip.',
                                         ))

            self.add_col(am.IdsArrayConf('ids_activitytype_dest', activitytypes,
                                         groupnames=['parameters'],
                                         perm='rw',
                                         #choices = activitytypes.names.get_indexmap(),
                                         name='Activity type at dest.',
                                         symbol='Act. dest.',
                                         info='Type of activity performed at destination, after the trip.',
                                         ))
        #self.add( cm.ObjConf( zones, is_child = False,groups = ['_private']))

    def add_flows(self,  time_start,
                  time_end,
                  id_mode,
                  ids_orig,
                  ids_dest,
                  tripnumbers,
                  id_activitytype_orig=1,
                  id_activitytype_dest=1,
                  ):
        n = len(tripnumbers)
        self.add_rows(n=n,
                      times_start=time_start*np.ones(n),
                      times_end=time_end*np.ones(n),
                      ids_mode=id_mode*np.ones(n),
                      ids_orig=ids_orig,
                      ids_dest=ids_dest,
                      tripnumbers=tripnumbers,
                      ids_activitytype_orig=id_activitytype_orig*np.ones(n),
                      ids_activitytype_dest=id_activitytype_dest*np.ones(n),
                      )


class OdTrips(am.ArrayObjman):
    def __init__(self, ident, parent, zones, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='OD trips',
                          info='Contains the number of trips between an origin and a destination zone.',
                          version=0.2,
                          xmltag=('odtrips', 'odtrip', None), **kwargs)

        self._init_attributes(zones)
        self._init_constants()

    def _init_attributes(self, zones=None):
        # print '_init_attributes',self.ident
        if not self.has_attrname('zones'):
            self.add(cm.ObjConf(
                zones, is_child=False, groups=['_private']))
        else:
            # zones is already an attribute
            zones = self.zones.get_value()

        if self.get_version() < 0.1:
            # update attrs from previous
            # IdsArrayConf not yet modifiable interactively, despite perm = 'rw',!!!
            self.ids_orig.set_perm('rw')
            self.ids_dest.set_perm('rw')

        if hasattr(self, 'func_delete_row'):
            self.func_make_row._is_returnval = False
            self.func_delete_row._is_returnval = False

        self.add_col(am.IdsArrayConf('ids_orig', zones,
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Orig.',
                                     #choices =  zones.ids_sumo.get_indexmap(),
                                     info='traffic assignment zone of origin of trip.',
                                     xmltag='id_orig',
                                     ))

        self.add_col(am.IdsArrayConf('ids_dest', zones,
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Dest.',
                                     #choices =  zones.ids_sumo.get_indexmap(),
                                     info='ID of traffic assignment zone of destination of trip.',
                                     xmltag='id_dest',
                                     ))

        self.add_col(am.ArrayConf('tripnumbers', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Trips',
                                  info='Number of trips from zone with ID Orig to zone with ID Dest.',
                                  xmltag='tripnumber',
                                  ))

        # print '  pre add func_make_row'
        self.add(cm.FuncConf('func_make_row', 'on_add_row', None,
                             groupnames=['rowfunctions', '_private'],
                             name='New OD flow.',
                             info='Add a new OD flow.',
                             is_returnval=False,
                             ))
        # print '  post add func_make_row'
        self.add(cm.FuncConf('func_delete_row', 'on_del_row', None,
                             groupnames=['rowfunctions', '_private'],
                             name='Del OD flow',
                             info='Delete OD flow.',
                             is_returnval=False,
                             ))

        # print '  _init_attributes done',self.ident

    def _init_constants(self):
        # print 'OdTrips._init_constants!!'
        #self.edgeweights_orig = None
        #self.edgeweights_dest = None
        self._mode_to_edgeinfo = {}
        self._fstars = {}
        self._times = {}
        attrsman = self.get_attrsman()
        attrsman.do_not_save_attrs(['_mode_to_edgeinfo', '_fstars', '_times'])

    def _get_fstar(self, id_mode,  is_return_arrays=True, is_ignor_connections=False):

        if not self._fstars.has_key(id_mode):
            self._fstars[id_mode] = self.get_edges().get_fstar(id_mode,
                                                               is_ignor_connections=is_ignor_connections,
                                                               is_return_arrays=is_return_arrays,)
        return self._fstars[id_mode]

    def _get_times(self, id_mode,  is_check_lanes=False):

        if not self._times.has_key(id_mode):
            self._times[id_mode] = self.get_edges().get_times(id_mode=id_mode,
                                                              is_check_lanes=is_check_lanes,
                                                              )
        return self._times[id_mode]

    def get_zone_edgeinfo(self, id_mode, id_zone,
                          n_edges_min_length, n_edges_max_length,
                          priority_max, speed_max,
                          id_mode_fallback=None):

        if not self._mode_to_edgeinfo.has_key(id_mode):
            self._mode_to_edgeinfo[id_mode] = {}

        if not self._mode_to_edgeinfo[id_mode].has_key(id_zone):
            zones = self.get_zones()
            ids_edge = zones.get_zoneedges_by_mode_fast(id_zone, id_mode,
                                                        weights=self._get_times(id_mode),
                                                        fstar=self._get_fstar(id_mode),
                                                        )
            n_edges = len(ids_edge)
            if n_edges > 0:
                #weights = self._get_edgeweights(ids_edge, n_edges_min_length, n_edges_max_length, priority_max, speed_max)
                #self._mode_to_edgeinfo[id_mode][id_zone] = (ids_edge, weights / np.sum(weights))
                # store un-normlized weights
                self._mode_to_edgeinfo[id_mode][id_zone] = (ids_edge,
                                                            self._get_edgeweights(
                                                                ids_edge, n_edges_min_length, n_edges_max_length, priority_max, speed_max),
                                                            )
            else:
                self._mode_to_edgeinfo[id_mode][id_zone] = ([], [])

        return self._mode_to_edgeinfo[id_mode][id_zone]

    def _get_edgeweights(self, ids_edge, n_edges_min_length, n_edges_max_length, priority_max, speed_max):
        # print 'get_edgeweights ids_edge',ids_edge
        edges = self.get_edges()
        n_edges = len(ids_edge)
        if (n_edges > n_edges_min_length) & (n_edges < n_edges_max_length):
            return edges.lengths[ids_edge]*((edges.priorities[ids_edge] < priority_max) & (edges.speeds_max[ids_edge] < speed_max))
        else:
            return np.ones(n_edges, dtype=np.float32)*((edges.priorities[ids_edge] < priority_max) & (edges.speeds_max[ids_edge] < speed_max))

    def on_del_row(self, id_row=None):
        if id_row is not None:
            # print 'on_del_row', id_row
            self.del_row(id_row)

    def on_add_row(self, id_row=None):
        print 'on_add_row'
        if len(self) > 0:

            # copy previous
            od_last = self.get_row(self.get_ids()[-1])
            #id_orig = self.odtab.ids_orig.get(id_last)
            #id_dest = self.odtab.ids_dest.get(id_last)
            #id = self.suggest_id()
            self.add_row(**od_last)
        else:
            self.add_row(self.suggest_id())

    def generate_odflows(self, odflowtab,  time_start, time_end, id_mode, **kwargs):
        """
        Insert all od flows in odflowtab.
        """
        # for id_od in self.get_ids():
        odflowtab.add_flows(time_start,
                            time_end,
                            id_mode,
                            self.ids_orig.get_value(),
                            self.ids_dest.get_value(),
                            self.tripnumbers.get_value(),
                            **kwargs
                            )

    def generate_trips(self, demand, time_start, time_end, id_mode_primary,
                       id_mode_fallback=-1,
                       pos_depart_default=db.OPTIONMAP_POS_DEPARTURE['random_free'],
                       #pos_arrival_default = db.OPTIONMAP_POS_ARRIVAL['max'],
                       pos_arrival_default=db.OPTIONMAP_POS_ARRIVAL['random'],
                       speed_depart_default=0.0,
                       speed_arrival_default=0.0,
                       # pedestrians always depart on lane 0
                       ind_lane_depart_default=db.OPTIONMAP_LANE_DEPART['allowed'],
                       # pedestrians always arrive on lane 0
                       ind_lane_arrival_default=db.OPTIONMAP_LANE_ARRIVAL['current'],
                       n_trials_connect=5,
                       is_make_route=True,
                       priority_max=10,
                       speed_max=14.0,
                       n_edges_min_length=1,
                       n_edges_max_length=500,
                       ):
        """
        Generates trips in demand.trip table.
        """
        print 'generate_trips', time_start, time_end, 'id_mode_primary', id_mode_primary, 'id_mode_fallback', id_mode_fallback
        id_mode_ped = MODES['pedestrian']
        zones = self.get_zones()
        edges = self.get_edges()
        # TODO: a bid crazy to pass demand as an argument here
        trips = demand.trips

        # How to deal with fallback mode? calculate both wights separately, normalize commonly and assign separately

        # for id_zone in zones.get_ids():
        #    print '  calculate edgeprops for id_zone',id_zone
        #    ids_edge, edgeprobs = self.get_zone_edgeinfo(id_mode_primary, id_zone, n_edges_min_length, n_edges_max_length, priority_max, speed_max)
        #    for id_edge, edgeprob in zip(ids_edge, edgeprobs):
        #        print '  id_edge',id_edge,'edgeprob',edgeprob

        # return True

        #OPTIONMAP_POS_DEPARTURE = { -1:"random",-2:"free",-3:"random_free",-4:"base"}
        #OPTIONMAP_POS_ARRIVAL = { -1:"random",-2:"max"}
        #OPTIONMAP_SPEED_DEPARTURE = { -1:"random",-2:"max"}
        #OPTIONMAP_SPEED_ARRIVAL = { -1:"current"}
        #OPTIONMAP_LANE_DEPART = {-1:"random",-2:"free",-3:"departlane"}
        #OPTIONMAP_LANE_ARRIVAL = { -1:"current"}

        # define primary and secondary mode, if appropriate
        # in case there is a secondary mode, the secondary mode is chosen
        ids_vtype_mode_primary, prob_vtype_mode_primary = demand.vtypes.select_by_mode(
            id_mode_primary, is_share=True)
        print '  ids_vtype_mode_primary', ids_vtype_mode_primary, prob_vtype_mode_primary
        n_vtypes_primary = len(ids_vtype_mode_primary)

        n_vtypes_fallback = 0
        if (id_mode_primary == MODES['passenger']) & (id_mode_fallback not in [-1, MODES['passenger']]):
            ids_vtype_mode_fallback, prob_vtype_mode_fallback = demand.vtypes.select_by_mode(
                id_mode_fallback, is_share=True)
            # print '  ids_vtype_mode_fallback', ids_vtype_mode_fallback
            n_vtypes_fallback = len(ids_vtype_mode_fallback)
            is_fallback = True
        else:
            ids_vtype_mode_fallback = []
            id_mode_fallback = -1
            is_fallback = False
        # print '  n_vtypes_primary',n_vtypes_primary,'n_vtypes_fallback',n_vtypes_fallback,'is_fallback',is_fallback
        if n_vtypes_primary == 0:
            print 'WARNING: no vehicle types for this mode with ID', id_mode_primary
            return False

        ids_od = self.get_ids()
        n_trips_generated = 0
        for id_od, id_orig, id_dest,  tripnumber in \
            zip(ids_od, self.ids_orig[ids_od], self.ids_dest[ids_od],
                np.array(self.tripnumbers[ids_od]+random.rand(len(ids_od)), dtype=np.int32),
                ):

            print '  generate', tripnumber, ' trips from id_orig', id_orig, 'to id_dest', id_dest

            ids_edge_orig, weights_orig = self.get_zone_edgeinfo(
                id_mode_primary, id_orig, n_edges_min_length, n_edges_max_length, priority_max, speed_max)
            ids_edge_dest, weights_dest = self.get_zone_edgeinfo(
                id_mode_primary, id_dest, n_edges_min_length, n_edges_max_length, priority_max, speed_max)

            n_edges_orig = len(ids_edge_orig)
            n_edges_dest = len(ids_edge_dest)
            # print '  n_edges_orig',n_edges_orig,len(weights_orig),'n_edges_dest',n_edges_dest,len(weights_dest)

            if 1:  # (n_edges_orig > 0) & (n_edges_dest > 0):
                for i_trip in xrange(tripnumber):

                    time_depart = random.uniform(time_start, time_end)

                    id_vtype_primary = ids_vtype_mode_primary[np.argmax(
                        random.rand(n_vtypes_primary)*prob_vtype_mode_primary)]

                    id_trip = trips.make_trip(id_vtype=id_vtype_primary,
                                              time_depart=time_depart,
                                              id_edge_depart=ids_edge_orig[np.argmax(
                                                  random.rand(n_edges_orig)*weights_orig)],
                                              id_edge_arrival=ids_edge_dest[np.argmax(
                                                  random.rand(n_edges_dest)*weights_dest)],
                                              ind_lane_depart=ind_lane_depart_default,
                                              ind_lane_arrival=ind_lane_arrival_default,
                                              position_depart=pos_depart_default,
                                              position_arrival=pos_arrival_default,
                                              speed_depart=speed_depart_default,
                                              speed_arrival=speed_arrival_default,
                                              # route=route,
                                              )

                    n_trips_generated += 1

        print '  -----'
        print '  n_trips_generated', n_trips_generated
        print '  n_trips_failed', np.sum(self.tripnumbers[ids_od])-n_trips_generated
        return True

    def add_od_trips(self, scale, names_orig, names_dest, tripnumbers,
                     ids_zone_orig_filter=[], ids_zone_dest_filter=[], ids_zone_cross_filter=[],
                     dist_min=-1.0, dist_max=-1.0):

        is_filter_orig = len(ids_zone_orig_filter) > 0
        is_filter_dest = len(ids_zone_dest_filter) > 0
        is_filter_cross = len(ids_zone_cross_filter) > 0
        is_dist_min = dist_min > 0
        is_dist_max = dist_max > 0
        print 'OdTrips.add_od_trips', is_filter_orig, is_filter_dest, is_filter_cross
        # print '  filter',ids_zone_orig_filter,ids_zone_dest_filter
        # print '  scale, names_orig, names_dest, tripnumbers',scale, names_orig, names_dest, tripnumbers,len(tripnumbers)
        zones = self.get_zones()

        for name_orig, name_dest, tripnumber in zip(names_orig, names_dest, tripnumbers):
            is_add = (zones.ids_sumo.has_index(name_orig)) & (zones.ids_sumo.has_index(name_dest))
            # print '  check',name_orig, name_dest, tripnumber,is_add

            if is_add:

                id_zone_orig = zones.ids_sumo.get_id_from_index(name_orig)
                id_zone_dest = zones.ids_sumo.get_id_from_index(name_dest)
                # print '    id_zone_orig',id_zone_orig, id_zone_orig in ids_zone_orig_filter,'id_zone_dest',id_zone_dest, id_zone_dest in ids_zone_dest_filter

                if is_filter_orig:
                    is_add = (id_zone_orig in ids_zone_orig_filter)

                if is_add & is_filter_dest:
                    is_add = (id_zone_dest in ids_zone_dest_filter)

                if is_add & is_filter_cross:
                    is_add = False
                    line = (zones.coords[id_zone_orig], zones.coords[id_zone_dest])
                    # print '      line',line
                    for shape in zones.shapes[ids_zone_cross_filter]:
                        # print '        intersect',shape,is_polyline_intersect_polygon(line, shape, is_lineinterpolate = True)
                        if is_polyline_intersect_polygon(line, shape, is_lineinterpolate=True):
                            is_add = True
                            break

                if is_add & is_dist_max:
                    is_add = np.sqrt(np.sum((zones.coords[id_zone_orig] - zones.coords[id_zone_dest])**2)) < dist_max

                if is_add & is_dist_min:
                    is_add = np.sqrt(np.sum((zones.coords[id_zone_orig] - zones.coords[id_zone_dest])**2)) > dist_min

            if is_add:

                # -----
                n_trips_scale = float(scale * tripnumber)
                n_trips_scale_int = int(n_trips_scale)
                n_trips_scale_dec = float(n_trips_scale - n_trips_scale_int)
                n_random = random.random()
                if n_random > n_trips_scale_dec:
                    n_trips_scale_fin = int(n_trips_scale_int)
                else:
                    n_trips_scale_fin = int(n_trips_scale_int + 1)
                # -----
                print '      add from', name_orig, 'to', name_dest, 'tripnumber', n_trips_scale_fin

                self.add_row(ids_orig=id_zone_orig,
                             ids_dest=id_zone_dest,
                             tripnumbers=n_trips_scale_fin)  # prima c'era (tripnumbers = scale*tripnumber)
            else:
                if not ((zones.ids_sumo.has_index(name_orig)) & (zones.ids_sumo.has_index(name_dest))):
                    print '  WARNING: zone named %s or %s not known' % (
                        name_orig, name_dest)
                    # print '  zones indexmap', zones.get_indexmap()
                    # print '  ids_sumo', zones.ids_sumo.get_value()
                    # print '  ids_sumo._index_to_id', zones.ids_sumo._index_to_id

    def get_zones(self):
        return self.ids_dest.get_linktab()

    def get_edges(self):
        return self.get_zones().parent.parent.net.edges


class OdModes(am.ArrayObjman):
    def __init__(self, ident, parent, modes, zones, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='Mode OD tables',
                          info='Contains for each transport mode an OD trip table.',
                          xmltag=('modesods', 'modeods', 'ids_mode'), **kwargs)

        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['state'],
                                     choices=MODES,
                                     name='ID mode',
                                     xmltag='vClass',
                                     info='ID of transport mode.',
                                     ))

        self.add_col(cm.ObjsConf('odtrips',
                                 groupnames=['state'],
                                 is_save=True,
                                 name='OD matrix',
                                 info='Matrix with trips from origin to destintion for a specific mode.',
                                 ))

        self.add(cm.ObjConf(zones, is_child=False, groups=['_private']))

    def generate_trips(self, demand, time_start, time_end, **kwargs):
        for id_od_mode in self.get_ids():
            self.odtrips[id_od_mode].generate_trips(
                demand, time_start, time_end, self.ids_mode[id_od_mode], **kwargs)

    def generate_odflows(self, odflowtab, time_start, time_end, **kwargs):
        for id_od_mode in self.get_ids():
            self.odtrips[id_od_mode].generate_odflows(
                odflowtab, time_start, time_end, self.ids_mode[id_od_mode], **kwargs)

    def add_od_trips(self, id_mode, scale, names_orig, names_dest, tripnumbers, **kwargs):
        # print 'OdModes.add_od_trips',id_mode, scale, names_orig, names_dest, tripnumbers
        ids_mode = self.select_ids(self.ids_mode.get_value() == id_mode)
        if len(ids_mode) == 0:
            id_od_modes = self.add_row(ids_mode=id_mode)
            # print '  create',id_od_modes
            odtrips = OdTrips((self.odtrips.attrname, id_od_modes),
                              self, self.zones.get_value())
            self.odtrips[id_od_modes] = odtrips
            odtrips.add_od_trips(scale, names_orig, names_dest, tripnumbers, **kwargs)
            return odtrips
        else:
            id_od_modes = ids_mode[0]  # modes are unique
            # print '  use',id_od_modes
            self.odtrips[id_od_modes].add_od_trips(
                scale, names_orig, names_dest, tripnumbers, **kwargs)
            return self.odtrips[id_od_modes]


class OdIntervals(am.ArrayObjman):
    def __init__(self, ident='odintervals',  parent=None, net=None, zones=None, **kwargs):
        self._init_objman(ident, parent=parent,  # = demand
                          name='OD Demand',
                          info='Contains origin-to-destination zone transport demand for different time intervals.',
                          xmltag=('odintervals', 'odinteval', None), **kwargs)

        self.add_col(am.ArrayConf('times_start', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Start time',
                                  unit='s',
                                  info='Start time of interval in seconds (no fractional seconds).',
                                  xmltag='t_start',
                                  ))

        self.add_col(am.ArrayConf('times_end', 3600,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='End time',
                                  unit='s',
                                  info='End time of interval in seconds (no fractional seconds).',
                                  xmltag='t_end',
                                  ))

        activitytypes = self.parent.activitytypes
        self.add_col(am.IdsArrayConf('ids_activitytype_orig', activitytypes,
                                     groupnames=['parameters'],
                                     perm='rw',
                                     #choices = activitytypes.names.get_indexmap(),
                                     name='Activity type at orig.',
                                     symbol='Act. orig.',
                                     info='Type of activity performed at origin, before the trip.',
                                     #xmltag = 'actType',
                                     #xmlmap = get_inversemap( activitytypes.names.get_indexmap()),
                                     ))

        self.add_col(am.IdsArrayConf('ids_activitytype_dest', activitytypes,
                                     groupnames=['parameters'],
                                     perm='rw',
                                     #choices = activitytypes.names.get_indexmap(),
                                     name='Activity type at dest.',
                                     symbol='Act. dest.',
                                     info='Type of activity performed at destination, after the trip.',
                                     #xmltag = 'actType',
                                     #xmlmap = get_inversemap( activitytypes.names.get_indexmap()),
                                     ))

        self.add_col(cm.ObjsConf('odmodes',
                                 groupnames=['state'],
                                 is_save=True,
                                 name='OD modes',
                                 info='OD transport demand for all transport modes within the respective time interval.',
                                 ))
        self.add(cm.ObjConf(net, is_child=False, groups=['_private']))
        self.add(cm.ObjConf(zones, is_child=False, groups=['_private']))
        # print 'OdIntervals.__init__',self,dir(self)

    def generate_trips(self, is_refresh_zoneedges=True, **kwargs):
        """
        Generates trips in trip table.
        """
        if is_refresh_zoneedges:
            # make sure zone edges are up to date
            self.get_zones().refresh_zoneedges()
        demand = self.parent
        for id_inter in self.get_ids():
            self.odmodes[id_inter].generate_trips(demand,   self.times_start[id_inter],
                                                  self.times_end[id_inter],
                                                  **kwargs)

    def generate_odflows(self, **kwargs):
        """
        Generates a flat table with all OD flows.
        """
        odflowtab = OdFlowTable(self, self.get_modes(),
                                self.get_zones(), self.get_activitytypes())
        for id_inter in self.get_ids():
            self.odmodes[id_inter].generate_odflows(odflowtab,
                                                    self.times_start[id_inter],
                                                    self.times_end[id_inter],
                                                    id_activitytype_orig=self.ids_activitytype_orig[id_inter],
                                                    id_activitytype_dest=self.ids_activitytype_dest[id_inter],
                                                    **kwargs)
        return odflowtab

    def write_xml(self, fd=None, indent=0):
        ft = self.generate_odflows()
        scenario = self.parent.parent
        ids_zone_sumo = scenario.landuse.zones.ids_sumo
        get_vtype_for_mode = scenario.demand.vtypes.get_vtype_for_mode
        ids = ft.get_ids()
        fd.write(xm.begin('trips', indent))
        self.parent.vtypes.write_xml(fd, indent=indent,
                                     #ids = ids_vtype_selected,
                                     is_print_begin_end=False)

        for id_flow, time_start, time_end, id_mode, id_orig_sumo, id_dest_sumo, tripnumber in zip(
                ids, ft.times_start[ids], ft.times_end[ids], ft.ids_mode[ids],
                ids_zone_sumo[ft.ids_orig[ids]], ids_zone_sumo[ft.ids_dest[ids]],
                ft.tripnumbers[ids]):

            # <flow id="f" begin="0" end="100" number="23" fromTaz="taz1" toTaz="taz2"/>

            fd.write(xm.start('flow', indent + 2))
            fd.write(xm.num('id', id_flow))
            fd.write(xm.num('begin', time_start))
            fd.write(xm.num('end', time_end))
            fd.write(xm.num('number', tripnumber))

            fd.write(xm.num('fromTaz', id_orig_sumo))
            fd.write(xm.num('toTaz', id_dest_sumo))
            fd.write(xm.num('type', get_vtype_for_mode(id_mode=id_mode, is_sumoid=True)))

            fd.write(xm.stopit())

        fd.write(xm.end('trips', indent))

    def get_flowfilepath(self):
        return self.parent.parent.get_rootfilepath()+'.flow.xml'

    def get_amitranfilepath(self):
        return self.parent.parent.get_rootfilepath()+'.ami.xml'

    def export_amitranxml(self, filepath=None, encoding='UTF-8'):
        """
        Export flows to Amitran format that defines the demand per OD pair in time slices for every vehicle type.
        """
        print 'export_amitranxml', filepath, len(self)

        if len(self) == 0:
            return None

        if filepath is None:
            filepath = self.get_amitranfilepath()

        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_sumoxml: could not open', filepath
            return False

        indent = 0

        ft = self.generate_odflows()
        scenario = self.parent.parent
        ids_zone_sumo = scenario.landuse.zones.ids_sumo
        get_vtype_for_mode = scenario.demand.vtypes.get_vtype_for_mode
        ids = ft.get_ids()
        fd.write(xm.begin('demand', indent))
        # self.parent.vtypes.write_xml(   fd, indent=indent,
        #                                #ids = ids_vtype_selected,
        #                                is_print_begin_end = False)

        # <demand>
        #   <actorConfig id="0">
        #       <timeSlice duration="86400000" startTime="0">
        #           <odPair amount="100" destination="2" origin="1"/>
        #       </timeSlice>
        #   </actorConfig>
        # </demand>

        for id_flow, time_start, time_end, id_mode, id_orig_sumo, id_dest_sumo, tripnumber in zip(
                ids, ft.times_start[ids], ft.times_end[ids], ft.ids_mode[ids],
                ids_zone_sumo[ft.ids_orig[ids]], ids_zone_sumo[ft.ids_dest[ids]],
                ft.tripnumbers[ids]):

            fd.write(xm.start('actorConfig', indent + 2))
            fd.write(xm.num('id', get_vtype_for_mode(id_mode=id_mode, is_sumoid=True)))
            fd.write(xm.stop())

            fd.write(xm.start('timeSlice', indent + 4))
            fd.write(xm.num('duration', int(time_end-time_start)))
            fd.write(xm.num('startTime', int(time_start)))
            fd.write(xm.stop())

            fd.write(xm.start('odPair', indent + 6))
            fd.write(xm.num('origin', id_orig_sumo))
            fd.write(xm.num('destination', id_dest_sumo))
            fd.write(xm.num('amount', int(tripnumber)))
            fd.write(xm.stopit())

            fd.write(xm.end('timeSlice', indent + 4))

            fd.write(xm.end('actorConfig', indent + 2))

        fd.write(xm.end('demand', indent))

        fd.close()
        return filepath

    def export_sumoxml(self, filepath=None, encoding='UTF-8'):
        """
        Export flows to SUMO xml file formate.
        """
        print 'export_sumoxml', filepath, len(self)
        if len(self) == 0:
            return None

        if filepath is None:
            filepath = self.get_flowfilepath()

        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_sumoxml: could not open', filepath
            return False
        #xmltag, xmltag_item, attrname_id = self.xmltag
        #fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
        #fd.write('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
        indent = 0
        #fd.write(xm.begin('routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"',indent))

        self.write_xml(fd, indent=0)

        fd.close()
        return filepath

    def clear_od_trips(self):
        self.clear()

    def add_od_flows(self, t_start, t_end, id_mode,
                     id_activitytype_orig, id_activitytype_dest,
                     scale, names_orig, names_dest, tripnumbers,
                     ids_zone_orig_filter=[], ids_zone_dest_filter=[], ids_zone_cross_filter=[],
                     dist_min=-1.0, dist_max=-1.0):

        # print 'OdIntervals.add_od_flows',t_start, t_end, id_mode, scale
        ids_inter = self.select_ids(
            (self.times_start.get_value() == t_start)
            & (self.times_end.get_value() == t_end)
            & (self.ids_activitytype_orig.get_value() == id_activitytype_orig)
            & (self.ids_activitytype_dest.get_value() == id_activitytype_dest)
        )

        if len(ids_inter) == 0:
            # no existing interval found. Create a new one
            id_inter = self.add_row(times_start=t_start, times_end=t_end,
                                    ids_activitytype_orig=id_activitytype_orig,
                                    ids_activitytype_dest=id_activitytype_dest,
                                    )
            # print '  create new',id_inter
            #odintervals.add_rows(2, t_start=[0,3600], t_end=[3600, 7200])
            odmodes = OdModes((self.odmodes.attrname, id_inter), parent=self,
                              modes=self.get_net().modes, zones=self.get_zones())
            # NO!! odmodes = OdModes( ('ODMs for modes', id_inter), parent = self, modes = self.get_net().modes, zones = self.get_zones())
            self.odmodes[id_inter] = odmodes

            odtrips = odmodes.add_od_trips(
                id_mode, scale, names_orig, names_dest, tripnumbers,
                ids_zone_orig_filter=ids_zone_orig_filter, ids_zone_dest_filter=ids_zone_dest_filter,
                ids_zone_cross_filter=ids_zone_cross_filter,
                dist_min=dist_min, dist_max=dist_max)
            return odtrips
        else:

            # there should be only one demand table found for a certain interval
            id_inter = ids_inter[0]
            # print '  use',id_inter
            odtrips = self.odmodes[id_inter].add_od_trips(
                id_mode, scale, names_orig, names_dest, tripnumbers,
                ids_zone_orig_filter=ids_zone_orig_filter, ids_zone_dest_filter=ids_zone_dest_filter,
                ids_zone_cross_filter=ids_zone_cross_filter,
                dist_min=dist_min, dist_max=dist_max)
            return odtrips

    def add_od_flow(self, t_start, t_end, id_mode,
                    id_activitytype_orig, id_activitytype_dest,
                    scale,
                    name_orig, name_dest, tripnumber):

        # print 'OdIntervals.add_od_flow',t_start, t_end, id_mode, scale, name_orig,name_dest,tripnumber
        odtrips = self.add_od_flows(t_start, t_end, id_mode,
                                    id_activitytype_orig, id_activitytype_dest,
                                    scale,
                                    [name_orig], [name_dest], [tripnumber])

        return odtrips

    def get_net(self):
        return self.net.get_value()

    def get_zones(self):
        return self.zones.get_value()

    def get_edges(self):
        return self.get_zones().ids_edges_inside.get_linktab()

    def get_od_matrix(self, id_interval, id_odmode, ids_zone):
        od_matrix = np.zeros((len(ids_zone), len(ids_zone)))
##        id_odmode = self.odmodes[id_interval].ids_mode.get_id_from_index(odmode)
        od_matrix_ids = self.odmodes[id_interval].odtrips[id_odmode].get_ids()
        i = 0
        j = 0
        for id_zonei in ids_zone:
            for id_zonej in ids_zone:
                for od_matrix_id in od_matrix_ids:
                    od_trip = self.odmodes[id_interval].odtrips[id_odmode].get_row(od_matrix_id)
                    id_dest = od_trip['ids_dest']
                    id_orig = od_trip['ids_orig']

                    if id_orig == id_zonei and id_dest == id_zonej:
                        tripnumber = od_trip['tripnumbers']
                        od_matrix[i, j] += tripnumber
                        break
                j += 1
                # print 'j', j
            j = 0
            i += 1
            # print 'i', i
    ##        od_matrix = self.odmodes[id_interval].odtrips[id_odmode]
        # print od_matrix
        return od_matrix

    def get_modes(self):
        return self.net.get_value().modes

    def get_activitytypes(self):
        return self.parent.activitytypes


class OdTripgenerator(db.TripoptionMixin, CmlMixin, Process):
    def __init__(self, odintervals, logger=None, **kwargs):

        self._init_common('odtripgenerator', name='OD Trip generator',
                          parent=odintervals,
                          logger=logger,
                          info='Generates trips from OD flows.',
                          )

        self.init_cml(' ')  # pass  no commad to generate options only

        attrsman = self.get_attrsman()

        self.is_clear_trips = attrsman.add(am.AttrConf('is_clear_trips', True,
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Clear trips',
                                                       info='Clear all trips in current trips database before routing.',
                                                       ))

        self.is_refresh_zoneedges = attrsman.add(am.AttrConf('is_refresh_zoneedges', True,
                                                             groupnames=['options'],
                                                             perm='rw',
                                                             name='Refresh zone edges',
                                                             info="""Identify all edges in all zones before generating the trips. 
                                              Dependent on the  will take some time.""",
                                                             ))

        self.is_make_route = attrsman.add(am.AttrConf('is_make_route', True,
                                                      groupnames=['options'],
                                                      perm='rw',
                                                      name='Make also routes',
                                                      info="""Perform also a shortes distance routing between edge of origin and edge of destination.""",
                                                      ))

        self.n_trials_connect = attrsman.add(am.AttrConf('n_trials_connect', 5,
                                                         groupnames=['options'],
                                                         perm='rw',
                                                         name='Connect trials',
                                                         info="""Number of triels to connect randomly chosen  
                                              origin- and destination edges with valid routes.""",
                                                         ))

        modechoices = odintervals.parent.parent.net.modes.names.get_indexmap()
        modechoices['No fallback'] = -1
        # print '  modechoices',modechoices
        self.id_mode_fallback = attrsman.add(am.AttrConf('id_mode_fallback',  modechoices['No fallback'],
                                                         groupnames=['options'],
                                                         choices=modechoices,
                                                         name='Fallback Mode',
                                                         info="""Transport mode to be used instead of "passenger" mode
                                         in case the origin and destination cannot be connected by a route. 
                                         This is typically the case with origins or destinations 
                                         in traffic restricted zones. 
                                         Coose for example "taxi" to get access to traffic restricted Zones.
                                         """,
                                                         ))

        # priority_max.get_value()) & (edges.speeds_max[ids_edge] < self.speed_max.get_value()))
        self.priority_max = attrsman.add(cm.AttrConf('priority_max', 8,
                                                     groupnames=['options'],
                                                     name='Max. edge priority',
                                                     perm='rw',
                                                     info="Maximum edge priority for which edges in a zone are considered departure or arrival edges.",
                                                     ))

        self.speed_max = attrsman.add(cm.AttrConf('speed_max', 14.0,
                                                  groupnames=['options'],
                                                  name='Max. edge speed',
                                                  perm='rw',
                                                  unit='m/s',
                                                  info="Maximum edge speed for which edges in a zone are considered departure or arrival edges.",
                                                  ))

        self.n_edges_min_length = attrsman.add(cm.AttrConf('n_edges_min_length', 1,
                                                           groupnames=['options'],
                                                           name='Min. edge number length prob.',
                                                           perm='rw',
                                                           info="Edge arrival/departure probability is proportional to the edge length if the number of zone edges is above this minimum number zone edges. Otherwise probability is proportional to the number of zone edges.",
                                                           ))

        self.n_edges_max_length = attrsman.add(cm.AttrConf('n_edges_max_length', 500,
                                                           groupnames=['options'],
                                                           name='Max. edge number length prob.',
                                                           perm='rw',
                                                           info="Edge arrival/departure probability is proportional to the edge length if the number of zone edges is below this maximum number zone edges. Otherwise probability is proportional to the number of zone edges.",
                                                           ))

        self.add_posoptions()
        self.add_laneoptions()
        self.add_speedoptions()

    def do(self):

        if self.is_clear_trips:
            self.parent.parent.trips.clear_trips()

        cml = self.get_cml()

        self.parent.generate_trips(
            id_mode_fallback=self.id_mode_fallback,
            is_refresh_zoneedges=self.is_refresh_zoneedges,
            pos_depart_default=self.pos_depart,
            pos_arrival_default=self.pos_arrival,
            speed_depart_default=self.speed_depart,
            speed_arrival_default=self.speed_arrival,
            ind_lane_depart_default=self.ind_lane_depart,
            ind_lane_arrival_default=self.ind_lane_arrival,
            n_trials_connect=self.n_trials_connect,
            is_make_route=self.is_make_route,
            priority_max=self.priority_max,
            speed_max=self.speed_max,
            n_edges_min_length=self.n_edges_min_length,
            n_edges_max_length=self.n_edges_max_length,
        )

        return True
