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

# @file    demandbase.py
# @author  Joerg Schweizer
# @date   2012


import numpy as np
from xml.sax import saxutils, parse, handler
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import random_choice, get_inversemap, dict_to_str

OPTIONMAP_POS_DEPARTURE = {"random": -1, "free": -2,
                           "random_free": -3, "base": -4, "last": -5, "first": -6}
OPTIONMAP_POS_ARRIVAL = {"random": -1, "max": -2}
OPTIONMAP_SPEED_DEPARTURE = {"random": -1, "max": -2}
OPTIONMAP_SPEED_ARRIVAL = {"current": -1}

OPTIONMAP_LANE_DEPART = {"random": -1, "free": -2,
                         "allowed": -3, "best": -4, "first": -5}

OPTIONMAP_LANE_ARRIVAL = {"current": -1}


class TripoptionMixin:
    """
    Class mixin wich defines some trip options.
    To be used with different processes.
    """

    def add_posoptions(self):
        attrsman = self.get_attrsman()

        self.add_option('pos_depart', OPTIONMAP_POS_DEPARTURE['base'],
                        groupnames=['options'],
                        cml='--departpos',
                        name='Departure position',
                        unit='m',
                        info="Departure position of vehicle.\n\nSpecial values:\n"
                        + dict_to_str(OPTIONMAP_POS_DEPARTURE, intend=2),
                        cmlvaluemap=get_inversemap(OPTIONMAP_POS_DEPARTURE),
                        )

        self.add_option('pos_arrival', OPTIONMAP_POS_ARRIVAL['max'],
                        groupnames=['options'],
                        cml='--arrivalpos',
                        name='Arival position',
                        unit='m',
                        info="Arival position of vehicle.\n\nSpecial values:\n"
                        + dict_to_str(OPTIONMAP_POS_ARRIVAL, intend=2),
                        cmlvaluemap=get_inversemap(OPTIONMAP_POS_ARRIVAL),
                        )

    def add_speedoptions(self):
        attrsman = self.get_attrsman()

        self.add_option('speed_depart', OPTIONMAP_SPEED_DEPARTURE['max'],
                        groupnames=['options'],
                        cml='--departspeed',
                        name='Departure speed',
                        unit='m/s',
                        info="Departure speed of vehicle.\n\nSpecial values:\n"
                        + dict_to_str(OPTIONMAP_SPEED_DEPARTURE, intend=2),
                        cmlvaluemap=get_inversemap(OPTIONMAP_SPEED_DEPARTURE),
                        )

        self.add_option('speed_arrival', OPTIONMAP_SPEED_ARRIVAL['current'],
                        groupnames=['options'],
                        cml='--arrivalspeed',
                        name='Arival speed',
                        unit='m/s',
                        info="Arival speed of vehicle.\n\nSpecial values:\n"
                        + dict_to_str(OPTIONMAP_SPEED_ARRIVAL, intend=2),
                        cmlvaluemap=get_inversemap(OPTIONMAP_SPEED_ARRIVAL),
                        )

    def add_laneoptions(self):
        attrsman = self.get_attrsman()

        self.add_option('ind_lane_depart', OPTIONMAP_LANE_DEPART['best'],
                        groupnames=['options'],
                        cml='--departlane',
                        name='Depart lane',
                        info="Departure lane index. 0 is rightmost lane or sidewalk, if existant.\n\nSpecial values:\n"
                        + dict_to_str(OPTIONMAP_LANE_DEPART, intend=2),
                        cmlvaluemap=get_inversemap(OPTIONMAP_LANE_DEPART),
                        )

        self.add_option('ind_lane_arrival', OPTIONMAP_LANE_ARRIVAL['current'],
                        groupnames=['options'],
                        cml='--arrivallane',
                        name='Arrival lane',
                        info="Arrival lane index. 0 is rightmost lane or sidewalk, if existant.\n\nSpecial values:\n"
                        + dict_to_str(OPTIONMAP_LANE_ARRIVAL, intend=2),
                        cmlvaluemap=get_inversemap(OPTIONMAP_LANE_ARRIVAL),
                        )


class ModeShares(am.ArrayObjman):
    """
    Utility table with some default mode shares.
    """

    def __init__(self, ident, parent, modes, **kwargs):

        self._init_objman(ident, parent=parent, name='Mode shares',
                          version=0.0,
                          **kwargs)

        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['parameters'],
                                     name='Mode ID',
                                     info='Transport Mode ID.',
                                     ))

        self.add_col(am.ArrayConf('shares', '',
                                  dtype=np.float32,
                                  is_index=True,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Share',
                                  info='Mode share.',
                                  ))

        # self.add_col(am.ArrayConf( 'speeds_max', 50.0/3.6,
        #                            dtype = np.float32,
        #                            groupnames = ['parameters'],
        #                            perm='rw',
        #                            name = 'Max. Speed',
        #                            unit = 'm/s',
        #                            info = 'Maximum possible speed for this mode. Speed is used to estimate free flow link travel times, mainly for routig purposes. Note that speeds are usully limited by the lane speed attribute',
        #                            ))
        self._init_attributes()
        self.add_default()

    def _init_attributes(self, landuse=None):
        # self.add_col(SumoIdsConf('Activitytypes'))
        pass

    def add_share(self, mode, share):
        modes = self.ids_mode.get_linktab()
        return self.add_row(ids_mode=modes.get_id_from_formatted(mode),
                            shares=share)

    def add_default(self):
        """
        Sets the default maximum possible speed for certain modes.
        """
        self.add_share("pedestrian", 0.1)
        self.add_share("bicycle", 0.1)
        self.add_share("motorcycle", 0.1)
        self.add_share("passenger", 0.5)
        self.add_share("bus", 0.2)

    def get_modes_random(self, n):
        """
        Return a vector with mode IDs of length n.
        """
        ids = self.get_ids()
        ids_modes_all = self.ids_mode[ids]
        return ids_modes_all[random_choice(n, self.shares[ids])]


class ActivityTypes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions
    def __init__(self, ident, demand, **kwargs):

        self._init_objman(ident, parent=demand, name='Activity Types',
                          version=0.0,
                          xmltag=('actTypes', 'actType', 'names'),
                          **kwargs)

        self._init_attributes()
        self.add_default()

    def _init_attributes(self, landuse=None):
        # self.add_col(SumoIdsConf('Activitytypes'))

        self.add_col(am.ArrayConf('names', '',
                                  dtype=np.object,
                                  is_index=True,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Type name',
                                  info='Human readable name of activity type.',
                                  ))

        self.add_col(am.ArrayConf('symbols', '',
                                  dtype=np.object,
                                  perm='rw',
                                  is_index=True,
                                  name='Type symbol',
                                  info='Symbol of activity type name. Used to represent activity sequences.',
                                  ))

        self.add_col(am.ArrayConf('descriptions', '',
                                  dtype=np.object,
                                  perm='rw',
                                  name='Description',
                                  info='Description of activity.',
                                  ))

        # this works only for first init
        # if landuse is not None:
        self.add_col(am.IdlistsArrayConf('ids_landusetypes', self.parent.get_scenario().landuse.landusetypes,
                                         name='Landuse types',
                                         info="Landuse type IDs, eher this activity type can take place.",
                                         ))

        self.add_col(am.ArrayConf('hours_begin_earliest', 0.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Earliest hour begin',
                                  unit='h',
                                  info='Default value for earliest hour when this activity can begin.',
                                  ))

        self.add_col(am.ArrayConf('hours_begin_latest', 1.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Latest begin hour',
                                  unit='h',
                                  info='Default value for latest hour when this activity can begin.',
                                  ))

        self.add_col(am.ArrayConf('durations_min', 6.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Min. Duration',
                                  unit='h',
                                  info='Default value for minimum activity duration for a person within a day.',
                                  ))

        self.add_col(am.ArrayConf('durations_max', 8.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Max. Duration',
                                  unit='h',
                                  info='Default value for maximum activity duration for a person within a day.',
                                  ))

    def format_ids(self, ids):
        return ', '.join(self.names[ids])

    def get_id_from_formatted(self, idstr):
        return self.names.get_id_from_index(idstr)

    def get_ids_from_formatted(self, idstrs):
        return self.names.get_ids_from_indices_save(idstrs.split(','))

    def get_id_from_name(self, activitytypename):
        return self.names.get_id_from_index(activitytypename)

    def get_id_from_symbol(self, activitytypesymbol):
        return self.symbols.get_id_from_index(activitytypesymbol)

    def add_default(self):
        """
        Sets the default maximum possible speed for certain modes.
        """
        landusetypekeys = self.parent.get_scenario().landuse.landusetypes.typekeys
        self.add_row(names='none',
                     descriptions='None activity type. Will be skipped when planning.',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices([]),
                     symbols='n',
                     hours_begin_earliest=0.0,
                     hours_begin_latest=0.0,
                     durations_min=0.0,
                     durations_max=0.0,
                     )

        self.add_row(names='home',
                     descriptions='General home activity, like sleeping, eating, watching TV, etc.',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['residential', 'mixed']),
                     symbols='h',
                     hours_begin_earliest=-1.0,
                     hours_begin_latest=-1.0,
                     durations_min=7.0,
                     durations_max=8.0,
                     )

        self.add_row(names='work',
                     descriptions="""Work activity, for example work in
                     industry, offices or as employee at
                     educational facilities.""",
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['industrial', 'commercial', 'education', 'mixed']),
                     symbols='w',
                     hours_begin_earliest=8.5,
                     hours_begin_latest=9.0,
                     durations_min=6.0,
                     durations_max=9.0,
                     )

        self.add_row(names='education',
                     descriptions='Education activity, for example visiting courses at schools or at universities.',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['education', ]),
                     symbols='e',
                     hours_begin_earliest=8.0,
                     hours_begin_latest=10.0,
                     durations_min=4.0,
                     durations_max=6.0,
                     )

        self.add_row(names='shopping',
                     descriptions='Shopping activity',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['commercial', 'mixed']),
                     symbols='s',
                     hours_begin_earliest=16.0,
                     hours_begin_latest=19.0,
                     durations_min=0.2,
                     durations_max=2.0,
                     )

        self.add_row(names='leisure',
                     descriptions='Leisure activity',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['leisure', 'mixed']),
                     symbols='l',
                     hours_begin_earliest=12.0,
                     hours_begin_latest=15.0,
                     durations_min=1.0,
                     durations_max=3.0,
                     )


class DemandobjMixin:
    def export_trips_xml(self, filepath=None, encoding='UTF-8',
                         ids_vtype_exclude=[], **kwargs):
        """
        Export trips to SUMO xml file.
        Method takes care of sorting trips by departure time.
        """
        return False

    def get_writexmlinfo(self, is_route=False, **kwargs):
        """
        Returns three array where the first array is the
        begin time of the first vehicle and the second array is the
        write function to be called for the respectice vehicle and
        the third array contains the vehicle ids

        Method used to sort trips when exporting to route or trip xml file
        """
        return [], [], []

    def config_results(self, results):
        # tripresults = res.Tripresults(          'tripresults', results,
        #                                        self,
        #                                        self.get_net().edges
        #                                        )
        #
        #
        #results.add_resultobj(tripresults, groupnames = ['Trip results'])
        pass

    def process_results(self, results, process=None):
        pass

    def get_time_depart_first(self):
        return np.inf

    def get_time_depart_last(self):
        return 0.0

    def import_routes_xml(self, routefilepath, **kwargs):
        """
        Import routes from filepath an store them somwhere.
        Demand object specific.
        """

        pass


class TripCounter(handler.ContentHandler):
    """Parses a SUMO route XML file and counts trips."""

    def __init__(self):
        self.n_trip = 0

    def startElement(self, name, attrs):
        # print 'startElement',name,self.n_trip
        if name == 'trip':
            self.n_trip += 1


class TripReader(handler.ContentHandler):
    """Reads trips from trip or route file into trip table"""

    def __init__(self, trips,  n_trip, vtype_default=None):
        # print 'RouteReader.__init__',demand.ident
        self._trips = trips
        demand = trips.parent

        net = demand.get_scenario().net

        self._ids_vtype_sumo = demand.vtypes.ids_sumo
        self._modemap = net.modes.names.get_indexmap()
        self._get_vtype_for_mode = demand.vtypes.get_vtype_for_mode

        if id_vtype_default is None:
            self._id_vtype_default = self.get_ids()[0]
        else:
            self._id_vtype_default = id_vtype_default

        self._ids_edge_sumo = net.edges.ids_sumo

        self.ids_sumo = np.zeros(n_trip, np.object)
        self.ids_vtype = np.zeros(n_trip, np.int32)
        self.times_depart = np.zeros(n_trip, np.int32)
        self.ids_edge_depart = np.zeros(n_trip, np.int32)
        self.ids_edge_arrival = np.zeros(n_trip, np.int32)
        self.inds_lane_depart = np.zeros(n_trip, np.int32)
        self.positions_depart = np.zeros(n_trip, np.float32)
        self.speeds_depart = np.zeros(n_trip, np.float32)
        self.inds_lane_arrival = np.zeros(n_trip, np.int32)
        self.positions_arrival = np.zeros(n_trip, np.float32)
        self.speeds_arrival = np.zeros(n_trip, np.float32)
        self.routes = np.zeros(n_trip, np.object)

        self._ind_trip = -1

        self._has_routes = False
        self._ids_sumoedge_current = []
        self._id_sumoveh_current = None
        #self._time_depart = 0
        #self._attrs = {}
        #self._is_generate_ids = is_generate_ids
        self._intervals_current = ''

    # def _init_reader(self):

    def _get_id_vtype(self, attrs):
        vtype = str(attrs['type'])
        if self._ids_vtype_sumo.has_index(vtype):
            # vtype is known
            return self._ids_vtype_sumo.get_id_from_index(vtype)

        # vtype is not known, so check out whether vtype
        # is actually a mode
        elif vtype in self._modemap:
            # pick id_vtype according to its probability with
            # all vtypes of this mode
            id_vtype = self._get_vtype_for_mode(self._modemap[vtype])

            if id_vtype >= 0:
                return id_vtype
            else:
                return self._id_vtype_default

        else:
            # no valid vtype, get a default
            return self._id_vtype_default

    def startElement(self, name, attrs):
        # <vehicle id="3_21" type="bus" depart="2520.00">
        # <route edges="bottom1to1/0 1/0to0/0 0/0tobottom0"/>
        # </vehicle>
        # print 'startElement',name
        if name == 'trip':
            # print '  startElement',attrs['id'],attrs['depart']
            self._ind_trip += 1

            self._id_sumoveh_current = attrs['id']
            self.ids_sumo[self._ind_trip] = self._id_sumoveh_current
            # print 'startElement ids_vtype',attrs['type'], self._ids_vtype_sumo.get_id_from_index(str(attrs['type']))

            self.ids_vtype[self._ind_trip] = self._get_id_vtype(attrs)
            self.times_depart[self._ind_trip] = int(float(attrs['depart']))

            if attrs.has_key('from'):
                self.ids_edge_depart[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['from']))
            if attrs.has_key('to'):
                self.ids_edge_arrival[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['to']))

            ind_lane_depart_raw = attrs.get('departLane', 'free')
            if OPTIONMAP_LANE_DEPART.has_key(ind_lane_depart_raw):
                self.inds_lane_depart[self._ind_trip] = OPTIONMAP_LANE_DEPART[ind_lane_depart_raw]
            else:
                self.inds_lane_depart[self._ind_trip] = int(ind_lane_depart_raw)

            positions_depart_raw = attrs.get('departPos', 'base')
            if OPTIONMAP_POS_DEPARTURE.has_key(positions_depart_raw):
                self.positions_depart[self._ind_trip] = OPTIONMAP_POS_DEPARTURE[positions_depart_raw]
            else:
                self.positions_depart[self._ind_trip] = float(positions_depart_raw)

            self.speeds_depart[self._ind_trip] = attrs.get('departSpeed', 0.0)

            ind_lane_arrival_raw = attrs.get('arrivalLane', 'current')
            if OPTIONMAP_LANE_ARRIVAL.has_key(ind_lane_arrival_raw):
                self.inds_lane_arrival[self._ind_trip] = OPTIONMAP_LANE_ARRIVAL[ind_lane_arrival_raw]
            else:
                self.inds_lane_arrival[self._ind_trip] = int(ind_lane_arrival_raw)

            positions_arrival_raw = attrs.get('arrivalPos', 'max')
            if OPTIONMAP_POS_ARRIVAL.has_key(positions_arrival_raw):
                self.positions_arrival[self._ind_trip] = OPTIONMAP_POS_ARRIVAL[positions_arrival_raw]
            else:
                self.positions_arrival[self._ind_trip] = float(positions_arrival_raw)

            self.speeds_arrival[self._ind_trip] = attrs.get('arrivalSpeed', 0.0)

    def _get_kwargs(self):
        return {'ids_sumo': self.ids_sumo,
                'times_depart': self.times_depart,
                'ids_edge_depart': self.ids_edge_depart,
                'ids_edge_arrival': self.ids_edge_arrival,
                'inds_lane_depart': self.inds_lane_depart,
                'positions_depart': self.positions_depart,
                'speeds_depart': self.speeds_depart,
                'inds_lane_arrival': self.inds_lane_arrival,
                'positions_arrival': self.positions_arrival,
                'speeds_arrival': self.speeds_arrival,
                }

    def insert_trips(self, is_generate_ids=True):

        # print 'TripReader.insert_trips self.ids_vtype',self.ids_vtype
        kwargs = self._get_kwargs()
        ids_trips = self._trips.make_trips(self.ids_vtype,
                                           is_generate_ids=is_generate_ids,
                                           **kwargs)

        return ids_trips


class RouteCounter(handler.ContentHandler):
    """Parses a SUMO route XML file and counts trips."""

    def __init__(self):
        self.n_veh = 0
        self.n_pers = 0
        #self.n_rou = 0

    def startElement(self, name, attrs):
        # print 'startElement',name,self.n_trip
        if name == 'vehicle':
            self.n_veh += 1
        elif name == 'person':
            self.n_pers += 1
        # elif name == 'route':
        #    if attrs.has_key('id'):
        #        self.n_rou += 1


class RouteReader(TripReader):
    """Reads trips from trip or route file into trip table"""

    def __init__(self, trips,  counter):
        # print 'RouteReader.__init__',demand.ident
        self._trips = trips
        n_veh = counter.n_veh
        n_per = counter.n_pers
        #n_rou = counter.n_rou
        n_trip = n_veh+n_per
        demand = trips.parent

        net = demand.get_scenario().net
        self._modemap = net.modes.names.get_indexmap()
        self._get_vtype_for_mode = demand.vtypes.get_vtype_for_mode

        self._ids_vtype_sumo = demand.vtypes.ids_sumo
        self._ids_edge_sumo = net.edges.ids_sumo

        self.ids_sumo = np.zeros(n_trip, np.object)
        self.ids_sumo[:] = ''
        self.ids_vtype = np.zeros(n_trip, np.int32)
        self.times_depart = np.zeros(n_trip, np.int32)
        self.times_arrival = np.zeros(n_trip, np.int32)
        self.type = np.zeros(n_trip, np.object)
        self.ids_edge_depart = np.zeros(n_trip, np.int32)
        self.ids_edge_arrival = np.zeros(n_trip, np.int32)
        self.inds_lane_depart = np.zeros(n_trip, np.int32)
        self.positions_depart = np.zeros(n_trip, np.float32)
        self.speeds_depart = np.zeros(n_trip, np.float32)
        self.inds_lane_arrival = np.zeros(n_trip, np.int32)
        self.positions_arrival = np.zeros(n_trip, np.float32)
        self.speeds_arrival = np.zeros(n_trip, np.float32)
        self.routes = np.zeros(n_trip, np.object)

        self._ind_trip = -1

        self._has_routes = False
        self._ids_sumoedge_current = []
        self._id_sumoveh_current = None
        #self._time_depart = 0
        #self._attrs = {}
        #self._is_generate_ids = is_generate_ids
        self._intervals_current = ''

    def startElement(self, name, attrs):
        # <vehicle id="3_21" type="bus" depart="2520.00">
        # <route edges="bottom1to1/0 1/0to0/0 0/0tobottom0"/>
        # </vehicle>
        # print 'startElement',name
        if name == 'vehicle':
            # print '  startElement',attrs['id'],attrs['depart']
            self._ind_trip += 1

            self._id_sumoveh_current = attrs['id']
            self.ids_sumo[self._ind_trip] = self._id_sumoveh_current
            # print 'startElement ids_vtype',attrs['type'], self._ids_vtype_sumo.get_id_from_index(str(attrs['type']))

            self.ids_vtype[self._ind_trip] = self.ids_vtype[self._ind_trip] = self._get_id_vtype(attrs)

            self.times_depart[self._ind_trip] = int(float(attrs['depart']))
            if attrs.has_key('arrival'):
                self.times_arrival[self._ind_trip] = int(float(attrs['arrival']))
            else:
                # duarouter is not calculating arrival time in results!
                self.times_arrival[self._ind_trip] = 0.0

            self.type[self._ind_trip] = attrs['type']
            if attrs.has_key('from'):
                self.ids_edge_depart[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['from']))
            if attrs.has_key('to'):
                self.ids_edge_arrival[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['to']))

            ind_lane_depart_raw = attrs.get('departLane', 'free')
            if OPTIONMAP_LANE_DEPART.has_key(ind_lane_depart_raw):
                self.inds_lane_depart[self._ind_trip] = OPTIONMAP_LANE_DEPART[ind_lane_depart_raw]
            else:
                self.inds_lane_depart[self._ind_trip] = int(ind_lane_depart_raw)

            positions_depart_raw = attrs.get('departPos', 'base')
            if OPTIONMAP_POS_DEPARTURE.has_key(positions_depart_raw):
                self.positions_depart[self._ind_trip] = OPTIONMAP_POS_DEPARTURE[positions_depart_raw]
            else:
                self.positions_depart[self._ind_trip] = float(positions_depart_raw)

            speed_depart_raw = attrs.get('departSpeed', 'max')
            if OPTIONMAP_SPEED_DEPARTURE.has_key(speed_depart_raw):
                self.speeds_depart[self._ind_trip] = OPTIONMAP_SPEED_DEPARTURE[speed_depart_raw]
            else:
                self.speeds_depart[self._ind_trip] = float(speed_depart_raw)

            ind_lane_arrival_raw = attrs.get('arrivalLane', 'current')
            if OPTIONMAP_LANE_ARRIVAL.has_key(ind_lane_arrival_raw):
                self.inds_lane_arrival[self._ind_trip] = OPTIONMAP_LANE_ARRIVAL[ind_lane_arrival_raw]
            else:
                self.inds_lane_arrival[self._ind_trip] = int(ind_lane_arrival_raw)

            positions_arrival_raw = attrs.get('arrivalPos', 'max')
            if OPTIONMAP_POS_ARRIVAL.has_key(positions_arrival_raw):
                self.positions_arrival[self._ind_trip] = OPTIONMAP_POS_ARRIVAL[positions_arrival_raw]
            else:
                self.positions_arrival[self._ind_trip] = float(positions_arrival_raw)

            speed_arrival_raw = attrs.get('arrivalSpeed', 'current')
            if OPTIONMAP_SPEED_ARRIVAL.has_key(speed_arrival_raw):
                self.speeds_arrival[self._ind_trip] = OPTIONMAP_SPEED_ARRIVAL[speed_arrival_raw]
            else:
                self.speeds_arrival[self._ind_trip] = float(speed_arrival_raw)

        if name == 'route':
            self._has_routes = True
            # print ' ',attrs.get('edges', '')
            self._ids_sumoedge_current = attrs.get('edges', '')
            self._intervals_current = attrs.get('intervals', '')

    # def characters(self, content):
    #    if (len(self._route_current)>0)&(self._intervals_current!=''):
    #        self._intervals_current = self._intervals_current + content

    def endElement(self, name):

        if name == 'vehicle':
            # print 'endElement',name,self._id_current,len(self._intervals_current)
            if (self._id_sumoveh_current is not None):
                ids_edge = []
                for id_sumoedge in self._ids_sumoedge_current.split(' '):
                    if not id_sumoedge in ('', ' ', ','):
                        if self._ids_edge_sumo.has_index(id_sumoedge):
                            ids_edge.append(self._ids_edge_sumo.get_id_from_index(id_sumoedge.strip()))
                self.routes[self._ind_trip] = ids_edge

                if len(ids_edge) >= 1:
                    self.ids_edge_depart[self._ind_trip] = ids_edge[0]
                    self.ids_edge_arrival[self._ind_trip] = ids_edge[-1]

                self._id_sumoveh_current = None
                #self._attrs = {}
                self._ids_sumoedge_current = []

        # elif name in ['routes','trips']:
        #    self.make_trips()

    def process_intervals(self):
        interval = []
        es = self._intervals_current.rstrip().split(" ")
        for e in es:
            p = e.split(",")
            interval.append((float(p[0]), float(p[1])))
        self._intervals_current = ''
        return interval

    def _get_kwargs(self, inds=None):
        if inds is None:
            inds = np.arange(len(self.ids_sumo))
        return {'ids_sumo': self.ids_sumo[inds],
                'times_depart': self.times_depart[inds],
                'times_arrival': self.times_arrival[inds],
                'type': self.type[inds],
                'ids_edge_depart': self.ids_edge_depart[inds],
                'ids_edge_arrival': self.ids_edge_arrival[inds],
                'inds_lane_depart': self.inds_lane_depart[inds],
                'positions_depart': self.positions_depart[inds],
                'speeds_depart': self.speeds_depart[inds],
                'inds_lane_arrival': self.inds_lane_arrival[inds],
                'positions_arrival': self.positions_arrival[inds],
                'speeds_arrival': self.speeds_arrival[inds],
                'ids_edges': self.routes[inds]
                }

    def insert_routes(self, is_generate_ids=True, is_add=False, is_overwrite_only=False):
        print 'TripReader.make_routes is_generate_ids', is_generate_ids, 'is_add', is_add, 'is_overwrite_only', is_overwrite_only

        # self._trips is scenario trip database
        # self.ids_sumo is a list of SUMO IDs read from xml file

        if is_overwrite_only & (not is_add):
            is_generate_ids = False
            is_add = False
            # get trip ids from xml file
            # ony import routes ids from existing sumo ids

            # this is index of self.ids_sumo to be overwritten
            inds = np.flatnonzero(np.array(self._trips.ids_sumo.has_indices(self.ids_sumo)))
            # print '  overwrite trip ids_sumo',self.ids_sumo[inds]

            ids_trip = np.array(self._trips.ids_sumo.get_ids_from_indices(self.ids_sumo[inds]), dtype=np.int32)
            # print '  ids_trip',ids_trip
            # print 'n_trips',len(ids_trip)

            ids_routes, ids_trip = self._trips.make_routes(self.ids_vtype[inds],
                                                           # is_generate_ids = is_generate_ids,# depricated
                                                           routes=self.routes[inds],
                                                           ids_trip=ids_trip,
                                                           is_add=is_add,
                                                           **self._get_kwargs(inds=inds)
                                                           )
            # print '  ids_routes',ids_routes

        else:
            if is_add:
                #is_generate_ids = False
                # get trip ids from xml file

                inds = np.flatnonzero(self.ids_sumo != '')  # ony import routes from specified sumo ids
                # print '  self.ids_sumo',self.ids_sumo[inds]
                # print '  inds',inds
                ids_trip = np.array(self._trips.ids_sumo.get_ids_from_indices_save(self.ids_sumo[inds]), dtype=np.int32)
                inds_valid = np.array(ids_trip, dtype=np.int32) > -1

                ids_routes, ids_trip = self._trips.make_routes(self.ids_vtype[inds[inds_valid]],
                                                               # is_generate_ids = is_generate_ids,# depricated
                                                               routes=self.routes[inds[inds_valid]],
                                                               ids_trip=ids_trip[inds_valid],
                                                               is_add=is_add,
                                                               **self._get_kwargs(inds=inds[inds_valid])
                                                               )

            if (not is_add) & is_generate_ids:
                inds = np.arange(len(self.routes))
                ids_trip = None

                # print '  ids_trip',ids_trip
                ids_routes, ids_trip = self._trips.make_routes(self.ids_vtype[inds],
                                                               is_generate_ids=is_generate_ids,
                                                               routes=self.routes[inds],
                                                               ids_trip=ids_trip,
                                                               is_add=is_add,
                                                               **self._get_kwargs(inds=inds)
                                                               )

        return ids_routes, ids_trip

# def get_data(self):
# values = {'ids_sumo': {}, 'times_depart': {},'ids_edge_depart': {},'ids_edge_arrival': {},\
# 'inds_lane_depart': {},'positions_depart': {},'speeds_depart': {},'inds_lane_arrival': {},\
# 'positions_arrival': {},'speeds_arrival' : {}, 'ids_edges' : {}}
# attrnames = ['ids_sumo', 'times_depart','ids_edge_depart','ids_edge_arrival',\
# 'inds_lane_depart','positions_depart','speeds_depart','inds_lane_arrival',\
# 'positions_arrival','speeds_arrival', 'ids_edges' ]
# valuess = [self.ids_sumo,
# self.times_depart,
# self.ids_edge_depart,
# self.ids_edge_arrival,
# self.inds_lane_depart,
# self.positions_depart,
# self.speeds_depart,
# self.inds_lane_arrival,
# self.positions_arrival,
# self.speeds_arrival,
# self.routes
# ]
# for attrname, value in zip(attrnames, valuess):
# print attrname
# print values
# for id_elem , val in zip(self.get_ids(), value):
##                values[attrname][id_elem] = (val,1)
# return values
##

# def get_ids(self):
# return range(len(self.ids_sumo[(self.ids_sumo!='')])+1)[1:]
