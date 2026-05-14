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

# @file    matsim_demand.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012


from xml.sax import saxutils, parse, handler
import os
import sys
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.processes import Process
from agilepy.lib_base.geometry import is_polyline_intersect_polygon
from collections import Counter
import coremodules.demand.demandbase as db
from coremodules.network.routing import get_mincostroute_edge2edge, edgedijkstra
from .matsim_base import *
import json
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import gzip

pathsep = os.path.sep

MATSIM_SUMO_MODEMAP = {'car': 4, 'pt': 5, 'walk': 1, 'bike': 2, 'motorbike': 3,
                       'walk_transit': 5, }
START_FIRST_ACTIVITY_BEFORE_MIDNIGHT = 3*3600  # this is arbitrary, but does not have any effect on the simuation

# helper functions


class PlanParser(ParserMixin, handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file and puts all plans 
    in the self.population attribute which is a dictionary
    where id_person is key and the entire plan is value.
    """

    def __init__(self, _matsim,
                 projparams_matsim='',
                 projectionmode='',
                 hour_begin=0, hour_end=24,
                 is_both_activities_inside=True,
                 is_one_activity_inside=False,
                 is_no_activity_inside=False,
                 is_geofilter=True,
                 timeformat='hh:mm:ss',
                 routesep=' '):
        print('PlanParser.__init__ timeformat', timeformat)
        self._matsim = _matsim
        self._scenario = _matsim.get_scenario()
        self._ids_edge_matsim = _matsim.net.edges.ids_matsim
        self.ids_tonode = _matsim.net.edges.ids_tonode
        self._nodecoords = _matsim.net.nodes.coords
        self.id_person = None
        self.n_person_tot = 0
        self.n_person = 0
        self.n_activity = 0
        self.is_both_activities_inside = is_both_activities_inside
        self.is_one_activity_inside = is_one_activity_inside
        self.is_no_activity_inside = is_no_activity_inside
        self._id_timeformat = TIMEFORMATMAP[timeformat]

        self._time_begin_first_trip = hour_begin*3600
        self._time_end_last_trip = hour_end*3600
        self._routesep = routesep

        self._init_projection(projparams_matsim, projectionmode)

        # determine whether a node is inside the area for all nodes
        ids_node = _matsim.net.nodes.get_ids()
        self._are_node_in_area = np.zeros(np.max(ids_node)+1, dtype=bool)
        self._are_node_in_area[ids_node] = self._in_boundaries(self._nodecoords[ids_node], x_border=0.0, y_border=0.0)

        self._init_plan()
        self._is_geofilter = is_geofilter

        self.population = {}
        self.modenames = []
        self.activitynames = []

        self.counter_activitytypes = Counter()
        self.counter_modes = Counter()

    def _init_plan(self):
        # init plan, reset in end plan
        self._xvec = []
        self._yvec = []
        self._activitytypes = []
        self._ids_mode = []
        self._traveldistances = []
        self._traveltimes = []
        self._times_begin = []
        self._times_end = []
        self._is_route_valid = False
        self._routes = []

        self._is_plan_valid = False
        self._time_end_laststage = -np.inf

    def get_ids_mode(self):
        ids_mode_used = []
        for modename in self.modenames:
            ids_mode_used.append(MATSIM_SUMO_MODEMAP.get(modename, -1))
        return ids_mode_used

    def record_activity(self, attrs):
        self._times_begin.append(self._time_end_laststage)

        if 'end_time' in attrs:
            time_end = self.get_seconds_from_string(attrs['end_time'])
            self._times_end.append(time_end)
            self._time_end_laststage = time_end
        else:
            # duration of last activity is not given , just put a constant START_FIRST_ACTIVITY_BEFORE_MIDNIGHT
            self._times_end.append(np.inf)  # this will signal the last activity

        activityname = attrs['type']
        self.counter_activitytypes.update([activityname])
        if activityname not in self.activitynames:
            self.activitynames.append(activityname)

        self._xvec.append(float(attrs['x']))
        self._yvec.append(float(attrs['y']))

        self._activitytypes.append(MATSIM_ACTIVITYMAP.get(activityname, -1))

        return True  # returning False can invalidate plan and stop recording

    def characters(self, chars):

        if self._is_route_valid:
            # print ('  appends SUMOPy MATSIM edge IDs',chars.split(self._routesep),self._ids_edge_matsim.get_ids_from_indices_save(chars.split(self._routesep)))

            self._chars += chars.strip()  # self._routesep +
            # print    ('chars',self._chars,len(self._chars))

    def startElement(self, name, attrs):
        # print ('startElement',name,self._is_plan_valid)
        if name == 'person':
            self.id_person = attrs['id']
            # print (70*'-')
            # print ('new person ID',self.id_person)

        elif name == 'plan':
            # must be a selected plan and there should not be an already recorded plan
            # of this person, we record only one plan per person
            self._is_plan_valid = (attrs['selected'] == 'yes') & (self.id_person != "")  # & (not self._is_record_plan)
            # self._is_record_plan = True

            # if attrs['selected']== 'yes':
            #    self._is_plan_new = True
            #    self._is_plan_valid = True
            # else:
            #    self._is_plan_valid = False

        elif name == 'activity':

            if self._is_plan_valid:  # plan is selected
                self.record_activity(attrs)

        elif name == 'leg':
            # leg only recorded for valid plans
            # print ('  mode',attrs['mode'],'valid',self._is_plan_valid)
            if self._is_plan_valid:  # & self._is_record_plan:
                modename = attrs['mode']
                if modename not in self.modenames:
                    self.modenames.append(modename)

                self.counter_modes.update([modename])

                # convert mode in SUMO mode ID
                # if no conversion is available, put -1
                # this will prevent the trip to ve generated
                self._ids_mode.append(MATSIM_SUMO_MODEMAP.get(modename, -1))

                time_travel = self.get_seconds_from_string(attrs['trav_time'])
                self._traveltimes.append(time_travel)
                self._time_end_laststage += time_travel
                # init character collection for route
                self._chars = ''

        elif name == 'route':

            if self._is_plan_valid:  # & self._is_record_plan:
                if len(self._ids_mode) > 0:
                    self._traveldistances.append(attrs['distance'])
                    self._is_route_valid = True
                # print ('  routeattrs',attrs.keys(),attrs.values())

    def endElement(self, name):
        # print ('endElement',name,self._is_plan_valid,'len(ids_mode)',len(self._ids_mode),'len(routes)',len(self._routes))
        if name == 'plan':
            self._is_plan_valid = False

        elif name == 'leg':
            # leg only recorded for valid plans

            if self._is_plan_valid:
                if self._chars != "":
                    self._routes.append(self._ids_edge_matsim.get_ids_from_indices_save(
                        self._chars.split(self._routesep)))
                    # print ('    inserted route',len(self._routes),self._routes[-1])
                # make sure that there is a route
                else:
                    # insert empty one if necessary
                    # here we must put something, otherwise trouble with array conversion below
                    self._routes.append([-1])
                    # print ('    inserted emptyroute',len(self._routes))
                self._is_route_valid = False

        elif name == 'person':
            self.n_person_tot += 1
            # attention, here we conclude a person, assuming
            # that exactly one plan has been selected (and recorded).
            # If several plans are parsed (also the unselected)
            # then the each plan must be recorded separately
            # when the plan has been concluded
            # However, this requires a more complex datastructure

            # check here if plan is valid, dependent on the set criteria
            # all or at least 1 activities in set time interval?
            # - all inside net boundaries?
            # - at least one activity within boundaries?
            # - activity locations are crossing net boundaries?

            # if valid then add person with plan to population
            # and reset temporary plan data

            # DEBUG
            # if self.id_person == '1421':
            #    print ('\nid_person',self.id_person,self._activitytypes)
            n_activity = len(self._activitytypes)
            if (n_activity >= 2) & (len(self._routes) == n_activity-1):

                coords_matsim = np.zeros((n_activity, 3), dtype=np.float32)
                coords_matsim[:, 0], coords_matsim[:, 1] = (self._xvec, self._yvec)

                # check if activities are inside (boundaries or zones)
                are_inside = self._in_boundaries_matsim(coords_matsim,
                                                        x_border=0.0, y_border=0.0)
                # DEBUG
                # if self.id_person == '1421':
                #    print ('  coords_matsim',coords_matsim)
                #    print ('  are_inside',are_inside)
                #    print ('  matsim boundaries',self._boundaries)
                inds_valid_leg = np.zeros((n_activity-1), dtype=bool)
                inds_valid_act = np.zeros((n_activity), dtype=bool)
                i = 0
                routes_valid = []
                for ind, atype1, t_end1, is_inside1, atype2, t_begin2, is_inside2, route, id_mode in zip(list(range(1, n_activity)), self._activitytypes[:-1], self._times_end[:-1], are_inside[:-1], self._activitytypes[1:], self._times_begin[1:], are_inside[1:], self._routes, self._ids_mode):
                    route_valid = np.array(route, dtype=np.int32)
                    if (id_mode != -1) & (t_end1 >= self._time_begin_first_trip)\
                        & (t_begin2 < self._time_end_last_trip)\
                            & (atype1 != -1) & (atype2 != -1):
                        # check now if the route traverses the area
                        if route != [-1]:
                            inds_inside = self._are_node_in_area[self.ids_tonode[route]]

                            # here we cut the route that crosses the area
                            route_valid = route_valid[inds_inside]

                            if len(route_valid) == 0:
                                route_valid = np.array([-1], dtype=np.int32)

                        if self.is_both_activities_inside:
                            inds_valid_leg[i] = is_inside1 & is_inside2

                        if self.is_one_activity_inside:
                            inds_valid_leg[i] |= (is_inside1 != is_inside2)

                        if self.is_no_activity_inside & (route_valid[0] != -1):
                            # check how often route enters and exits area
                            inds_valid_leg[i] |= np.sum(np.abs(np.diff(inds_inside))) <= 2

                    # validate activity before and after leg
                    inds_valid_act[i] |= inds_valid_leg[i]
                    inds_valid_act[i+1] = inds_valid_leg[i]
                    routes_valid.append(list(route_valid))
                    i += 1

                # print ('  inds_valid_leg',inds_valid_leg)
                # print ('  inds_valid_act',inds_valid_act)
                n_activity_valid = np.sum(inds_valid_act)

                if n_activity_valid > 0:  # any valid activity
                    # register person

                    # print ('    self._routes',self._routes)
                    # print ('    route array',np.array(self._routes, dtype = object).shape,np.array(self._routes, dtype = object))
                    self.population[self.id_person] = (np.array(self._activitytypes, dtype=np.int32)[inds_valid_act],
                                                       coords_matsim[inds_valid_act, :],
                                                       np.array(self._times_begin, dtype=np.float32)[inds_valid_act],
                                                       np.array(self._times_end, dtype=np.float32)[inds_valid_act],
                                                       are_inside[inds_valid_act],
                                                       # information on the leg made after the activity
                                                       np.array(self._ids_mode, dtype=np.int32)[inds_valid_leg],
                                                       np.array(self._traveldistances, dtype=np.float32)[
                        inds_valid_leg],
                        np.array(self._traveltimes, dtype=np.float32)[inds_valid_leg],
                        np.array(routes_valid, dtype=object)[inds_valid_leg],
                    )
                    self.n_person += 1
                    self.n_activity += n_activity_valid

                    print('registered person', self.id_person, self.population[self.id_person])

            self._init_plan()


class TripImporter(ParserMixin, MatsimProcessMixin, Process):
    def __init__(self,  matsim, planfilepath='', ids_mode=None,
                 logger=None, **kwargs):
        print('TripImporter.__init__', matsim)
        self._init_common('tripimporter', name='MATSIM trip importer',
                          logger=logger,
                          info='Imports MATSIM plan XML file file and generates trips.',
                          )
        self._matsim = matsim
        self._scenario = matsim.get_scenario()
        self._net = self._scenario.net
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()

        attrsman = self.set_attrsman(cm.Attrsman(self))
        self._init_importer()
        if planfilepath is None:
            planfilepath = os.path.join(rootdirpath, rootname+'.plan.xml')

        self.planfilepath = attrsman.add(cm.AttrConf('planfilepath', kwargs.get('planfilepath', planfilepath),
                                                     groupnames=['options'],
                                                     name='Matsim Plan file',
                                                     wildcards='Plan XML files (*.xml)|*.xml*',
                                                     metatype='filepath',
                                                     info='MATSIM plan file to be imported.',
                                                     ))

        self.id_zone_target = attrsman.add(cm.AttrConf('id_zone_target', kwargs.get('id_zone_target', -1),
                                                       groupnames=['options'],
                                                       name='ID target zone',
                                                       info='Specifies ID of a target zone. -1 means no target zone specified.',
                                                       ))

        self.hour_begin = attrsman.add(cm.AttrConf('hour_begin', kwargs.get('hour_begin', 0),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   name='Hour begin',
                                                   unit='h',
                                                   info='Hour of the beginning of a trip of a person.',
                                                   ))

        self.hour_end = attrsman.add(cm.AttrConf('hour_end', kwargs.get('hour_end', 10),
                                                 groupnames=['options'],
                                                 name='Hour end',
                                                 unit='h',
                                                 info='Hour of the end of a trip of a person.',
                                                 ))

        self.is_both_activities_inside = attrsman.add(cm.AttrConf('is_both_activities_inside', kwargs.get('is_both_activities_inside', False),
                                                                  groupnames=['options'],
                                                                  perm='rw',
                                                                  name='Activity pair inside area',
                                                                  info='Any pair of consecutive activities must be located in the specified area, which is by default the network bounding box',
                                                                  ))

        self.is_one_activity_inside = attrsman.add(cm.AttrConf('is_one_activity_inside', kwargs.get('is_one_activity_inside', True),
                                                               groupnames=['options'],
                                                               perm='rw',
                                                               name='One activity inside area',
                                                               info='For any pair of consecutive activities, exactly one activity must be located in the specified area, which is by default the network bounding box.',
                                                               ))

        self.is_no_activity_inside = attrsman.add(cm.AttrConf('is_no_activity_inside', kwargs.get('is_no_activity_inside', True),
                                                              groupnames=['options'],
                                                              perm='rw',
                                                              name='No activity inside area',
                                                              info='For any pair of consecutive activities, no activity must be located in the specified area, which is by default the network bounding box.',
                                                              ))

        self.scaling_factor = attrsman.add(cm.AttrConf('scaling_factor', kwargs.get('scaling_factor', 1),
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Scaling factor for trip import',
                                                       info='Scaling factor for trip import',
                                                       ))

        modechoices = self._net.modes.names.get_indexmap()

        if (ids_mode is None):
            # no info given, ...
            ids_mode_default = [modechoices['passenger'], modechoices['bicycle'],
                                modechoices['pedestrian'], modechoices['motorcycle'], modechoices['taxi']]

        elif ids_mode is not None:
            ids_mode_default = ids_mode

        # print ('  ids_mode_default',ids_mode_default)
        self.ids_mode = attrsman.add(cm.ListConf('ids_mode', ids_mode_default,
                                                 groupnames=['options'],
                                                 choices=modechoices,
                                                 perm='rw',
                                                 name='Modes',
                                                 info='Transport modes to be imported from plans.',
                                                 ))

        self.fringepriority_min = attrsman.add(cm.AttrConf('fringepriority_min', kwargs.get('fringepriority_min', 12),
                                                           groupnames=['options'],
                                                           name='Min. Fringe edge priority',
                                                           info='Minimum priority for being a fringe edge, an edge on the border of the given area.',
                                                           ))
        self.fringenumlanes_min = attrsman.add(cm.AttrConf('fringenumlanes_min', kwargs.get('fringenumlanes_min', 1),
                                                           groupnames=['options'],
                                                           name='Min. Fringe number of lanes',
                                                           info='Minimum number of lanes for being a fringe edge, an edge on the border of the given area.',
                                                           ))

        self.timeformat = attrsman.add(cm.AttrConf('timeformat', kwargs.get('timeformat', 'hh:mm:ss'),
                                                   groupnames=['options'],
                                                   choices=list(TIMEFORMATMAP.keys()),
                                                   name='Time Format',
                                                   info='Time Format',
                                                   ))

        self.init_proj_attributes(projparams_matsim=matsim.net.projparams,
                                  projectionmode='custom projection')

    def do(self):
        edges = self._net.edges
        nodes = self._net.nodes
        matsimedges = self._matsim.net.edges
        matsimnodes = self._matsim.net.nodes

        _vtypes = self._scenario.demand.vtypes
        _trips = self._scenario.demand.trips
        # parse population file
        parser = PlanParser(self._matsim,
                            projparams_matsim=self.projparams_matsim,
                            projectionmode=self.projectionmode,
                            hour_begin=self.hour_begin,
                            hour_end=self.hour_end,
                            is_both_activities_inside=self.is_both_activities_inside,
                            is_one_activity_inside=self.is_one_activity_inside,
                            is_no_activity_inside=self.is_no_activity_inside,
                            timeformat=self.timeformat,
                            is_geofilter=False
                            )

        parse(self.planfilepath, parser)

        # dictionary with all persons and plans
        population = parser.population
        print(70*'=')
        print('finished parsing, filtered population size', parser.n_person, 'of', parser.n_person_tot)
        print('    used modenames', parser.modenames)
        print('    used activitynames', parser.activitynames)

        ids_mode_used = parser.get_ids_mode()

        # here we would build a dictionary with mode as key
        # and access levels as value

        for id_mode in ids_mode_used:
            if id_mode != -1:  # unknown mode
                self.make_fringenodes(priority_min=self.fringepriority_min,
                                      numlanes_min=self.fringenumlanes_min, id_mode=id_mode)

        # id_mode_car = self._net.modes.get_id_mode('passenger')
        # accesslevels_car = edges.get_accesslevels(id_mode_car)
        # ids_edges_internal_car =  ids_edge[accesslevels_car[ids_edge] >= 0]

        # generate
        ids_user = []
        ids_vtype = []
        times_depart = []
        ids_edge_depart = []
        ids_edge_arrival = []
        for id_person, (activitytypes, coords_matsim, times_begin,  times_end, are_inside, ids_mode, traveldistances, traveltimes, routes) in population.items():
            n_activity = len(activitytypes)
            # print ('  id_person',id_person,'n_activity',n_activity)
            coords = np.zeros((n_activity, 3), dtype=np.float32)
            coords[:, 0], coords[:, 1] = parser.get_coord_sumo(coords_matsim[:, 0], coords_matsim[:, 1])

            # print ('  id_person',id_person,'n_act',len(activitytypes),len(coords),len(times_begin),len(times_end),len(are_inside),len(ids_mode),len(routes))

            for ind_trip, id_mode, coord1, is_inside1,  atype1, times_end1, coord2, is_inside2, atype2, t_begin2, route in zip(list(range(len(ids_mode))), ids_mode, coords[:-1], are_inside[:-1], activitytypes[:-1], times_end[:-1], coords[1:], are_inside[1:], activitytypes[1:], times_begin[1:], routes):

                # print ('   mode',id_mode,'from',coord1,is_inside1,'te1',times_end1,'to',coord2,is_inside1, 'tb2', t_begin2)
                # print ('     route',route)

                if id_mode in self.ids_mode:
                    # id_activitytype = matsim_sumo_activitymap[activitytype]
                    # print ('      check Either origin or destination is in the area',is_inside1 != is_inside2)
                    # print ('      check both origin and destination are outside the area',(not is_inside1) & (not is_inside2))
                    # print ('      check Origin and destination inside',( is_inside1) & ( is_inside2))

                    if route[0] == -1:  # no route provided
                        # take activity coordinates
                        coord_from = coord1
                        coord_to = coord2
                    else:
                        # take coords from route geometry
                        coord_from = matsimnodes.coords[matsimedges.ids_fromnode[route[0]]]
                        coord_to = matsimnodes.coords[matsimedges.ids_tonode[route[-1]]]

                    if is_inside1 != is_inside2:

                        if is_inside1:
                            # origin in in the area
                            # trip goes from edge within to a fringe edge
                            ids_edge_from, dists = edges.get_closest_edge(
                                coord_from,  n_best=10, accesslevels=edges.get_accesslevels(id_mode))
                            id_edge_from = ids_edge_from[0]
                            # print( '    found internal',id_edge_from,'with dist',dists[0])

                            id_node_to, dist = self.get_id_fringenode(coord_to, id_mode)
                            id_edge_to = nodes.ids_incoming[id_node_to][0]
                            # print ('    found fringenode',id_node_to,'id_edge_to',id_edge_to,'with dist',dist)

                        elif is_inside2:
                            # trip goes from a fringe edge to an edge inside the area
                            id_node_from, dist = self.get_id_fringenode(coord_from, id_mode)
                            id_edge_from = nodes.ids_outgoing[id_node_from][0]
                            # print ('    found fringenode',id_node_from,'id_edge_from',id_edge_from,'with dist',dist)

                            ids_edge_to, dists = edges.get_closest_edge(
                                coord_to,  n_best=10, accesslevels=edges.get_accesslevels(id_mode))
                            id_edge_to = ids_edge_to[0]
                           # print( '    found internal',id_edge_to,'with dist',dists[0])

                    elif (not is_inside1) & (not is_inside2):
                        # both are outside, trip goes from fringe edge
                        # to fringe edge

                        id_node_from, dist = self.get_id_fringenode(coord_from, id_mode)
                        id_edge_from = nodes.ids_outgoing[id_node_from][0]
                        # print ('    found from fringenode',id_node_from,'id_edge_from',id_edge_from,'with dist',dist)

                        id_node_to, dist = self.get_id_fringenode(coord_to, id_mode)
                        id_edge_to = nodes.ids_incoming[id_node_to][0]
                        # print ('    to from fringenode',id_node_to,'id_edge_to',id_edge_to,'with dist',dist)

                    else:
                        # print ('  Origin and destination inside',( is_inside1) & ( is_inside2))
                        ids_edge_from, dists = edges.get_closest_edge(
                            coord_from,  n_best=10, accesslevels=edges.get_accesslevels(id_mode))
                        id_edge_from = ids_edge_from[0]
                       # print( '    found internal',id_edge_from,'with dist',dists[0])

                        ids_edge_to, dists = edges.get_closest_edge(
                            coord_to,  n_best=10, accesslevels=edges.get_accesslevels(id_mode))
                        id_edge_to = ids_edge_to[0]
                        # print( '    found internal',id_edge_to,'with dist',dists[0])

                    for i in range(1, int(self.scaling_factor)):
                        if i == 1:
                            ids_user.append(id_person+'00'+str(ind_trip)+'00'+str(i))
                            ids_vtype.append(_vtypes.get_vtype_for_mode(id_mode))
                            times_depart.append(times_end1)  # time end of previous activity
                            ids_edge_depart.append(id_edge_from)
                            ids_edge_arrival.append(id_edge_to)
                        else:
                            ids_user.append(id_person+'00'+str(ind_trip)+'00'+str(i))
                            ids_vtype.append(_vtypes.get_vtype_for_mode(id_mode))
                            times_depart.append(max(0, times_end1+(-20*60+np.random.randint(40*60)))
                                                )  # time end of previous activity
                            ids_edge_depart.append(id_edge_from)
                            ids_edge_arrival.append(id_edge_to)
        # print ('     ids_edge_depart',type(ids_edge_depart),ids_edge_depart)
        # print ('     ids_user',type(ids_user),ids_user)

        n_trips = len(ids_user)
        _trips.make_trips(ids_vtype, is_generate_ids=False,
                          ids_sumo=ids_user,
                          times_depart=times_depart,
                          ids_edge_depart=ids_edge_depart,
                          ids_edge_arrival=ids_edge_arrival,
                          inds_lane_depart=np.ones(n_trips, dtype=np.int32)*db.OPTIONMAP_LANE_DEPART["best"],
                          positions_depart=np.ones(n_trips, dtype=np.int32)*db.OPTIONMAP_POS_DEPARTURE["base"],
                          speeds_depart=np.ones(n_trips, dtype=np.int32)*db.OPTIONMAP_SPEED_DEPARTURE["max"],
                          inds_lane_arrival=np.ones(n_trips, dtype=np.int32)*db.OPTIONMAP_LANE_ARRIVAL["current"],
                          positions_arrival=np.ones(n_trips, dtype=np.int32)*db.OPTIONMAP_POS_ARRIVAL["random"],
                          speeds_arrival=np.ones(n_trips, dtype=np.int32)*db.OPTIONMAP_SPEED_ARRIVAL["current"],
                          )

        return True


class PopulationImporter(ParserMixin, MatsimProcessMixin, Process):
    def __init__(self,  matsim, planfilepath='', ids_mode=None,
                 logger=None, **kwargs):
        print('PopulationImporter.__init__', matsim)
        self._init_common('populationimporter', name='MATSIM population importer',
                          logger=logger,
                          info='Imports MATSIM plan XML file and creates a virtual population, including plans.',
                          )
        self._matsim = matsim
        self._scenario = matsim.get_scenario()
        self._net = self._scenario.net
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()

        attrsman = self.set_attrsman(cm.Attrsman(self))
        self._init_importer()
        if planfilepath is None:
            planfilepath = os.path.join(rootdirpath, rootname+'.plan.xml')

        self.planfilepath = attrsman.add(cm.AttrConf('planfilepath', kwargs.get('planfilepath', planfilepath),
                                                     groupnames=['options'],
                                                     name='Matsim Plan file',
                                                     wildcards='Plan XML files (*.xml)|*.xml*',
                                                     metatype='filepath',
                                                     info='MATSIM plan file to be imported.',
                                                     ))

        self.id_zone_target = attrsman.add(cm.AttrConf('id_zone_target', kwargs.get('id_zone_target', -1),
                                                       groupnames=['options'],
                                                       name='ID target zone',
                                                       info='Specifies ID of a target zone. -1 means no target zone specified.',
                                                       ))

        self.hour_begin = attrsman.add(cm.AttrConf('hour_begin', kwargs.get('hour_begin', 0),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   name='Hour begin',
                                                   unit='h',
                                                   info='Hour of the beginning of a trip of a person.',
                                                   ))

        self.hour_end = attrsman.add(cm.AttrConf('hour_end', kwargs.get('hour_end', 10),
                                                 groupnames=['options'],
                                                 name='Hour end',
                                                 unit='h',
                                                 info='Hour of the end of a trip of a person.',
                                                 ))

        self.timeformat = attrsman.add(cm.AttrConf('timeformat', kwargs.get('timeformat', 'hh:mm:ss'),
                                                   groupnames=['options'],
                                                   choices=list(TIMEFORMATMAP.keys()),
                                                   name='Time Format',
                                                   info='Time Format',
                                                   ))

        self.init_proj_attributes(projparams_matsim=kwargs.get('projparams_matsim', matsim.net.projparams),
                                  projectionmode=kwargs.get('projectionmode', 'custom projection')
                                  )

    def do(self):
        edges = self._net.edges
        nodes = self._net.nodes
        _vtypes = self._scenario.demand.vtypes

        _strategies = self._scenario.demand.virtualpop.get_strategies()
        _virtualpop = self._scenario.demand.virtualpop
        _activities = _virtualpop.activities.get_value()
        _activitytypes = self._scenario.demand.activitytypes

        ids_build = self._scenario.landuse.facilities.get_ids_building()
        centroids_facs = self._scenario.landuse.facilities.centroids[ids_build]
        ids_landusetype_fac = self._scenario.landuse.facilities.ids_landusetype[ids_build]

        # parse population file
        parser = PlanParser(self._matsim,
                            projparams_matsim=self.projparams_matsim,
                            projectionmode=self.projectionmode,
                            hour_begin=self.hour_begin,
                            hour_end=self.hour_end,
                            is_both_activities_inside=True,
                            is_one_activity_inside=False,
                            is_no_activity_inside=False,
                            timeformat=self.timeformat,
                            is_geofilter=False
                            )

        parse(self.planfilepath, parser)

        # dictionary with all persons and plans
        population = parser.population
        print(70*'=')
        print('finished parsing, filtered population size', len(population))
        print('    used modenames', parser.modenames)
        print('    used activitynames', parser.activitynames)

        # generate
        ids_person_vp = _virtualpop.add_rows(n=len(population), names=list(population.keys()))
        i = 0
        for id_person, (activitytypes, coords_matsim, times_begin,  times_end, are_inside, ids_mode, traveldistances, traveltimes, routes) in population.items():
            n_activity = len(activitytypes)
            print('  id_person', id_person, 'n_activity', n_activity)
            coords = np.zeros((n_activity, 3), dtype=np.float32)
            coords[:, 0], coords[:, 1] = parser.get_coord_sumo(coords_matsim[:, 0], coords_matsim[:, 1])

            ids_facility_person = []
            ids_activitytype_person = []
            hours_begin = []
            durations = []
            time_end_previous = -START_FIRST_ACTIVITY_BEFORE_MIDNIGHT  # 3h before midnight

            for ind_act, coord1, is_inside1,  id_activitytype, time_begin, time_end in zip(list(range(len(activitytypes))), coords, are_inside, activitytypes, times_begin, times_end):

                print('act at', coord1, is_inside1, 'time_begin', time_begin,
                      'time_end', time_end, 'id_activitytype', id_activitytype)

                # if id_mode in self.ids_mode:
                id_person_vp = ids_person_vp[i]

                diff_coords = centroids_facs - coord1
                dists2 = diff_coords[:, 0]**2 + diff_coords[:, 1]**2
                id_facility = ids_build[np.argmin(dists2)]

                # this would find a facility with a landusetype in accordance with the activity
                # ids_landusetype = myactivitytypes.ids_landusetypes[id_activitytype] # get landuse types eligible for this activity
                # n = 10
                # is_wrong_landuse = True
                # while (n>0) & (is_wrong_landuse):
                #    ind_mindist = np.argmin(dists2)
                #    id_facility = ids_build[ind_mindist]
                #    id_landuse = ids_landusetype_fac[ind_mindist]
                #    is_wrong_landuse = id_landuse not in ids_landusetype
                #    dists2[ind_mindist] = np.inf
                #    n -= 1

                hours_begin.append(float(time_end_previous)/3600)
                # not quite correct because excludes time for transport
                durations.append(float(time_end - time_end_previous)/3600)
                time_end_previous = time_end

                ids_facility_person.append(id_facility)
                ids_activitytype_person.append(id_activitytype)

                time_end_previous = time_end

            ids_activity = _activities.add_activities(ids_activitytype_person,
                                                      ids_facility_person,
                                                      hours_begin_earliest=np.array(hours_begin, dtype=np.float32)-0.1,
                                                      hours_begin_latest=np.array(hours_begin, dtype=np.float32)+0.1,
                                                      durations_min=np.array(durations, dtype=np.float32)-0.1,
                                                      durations_max=np.array(durations, dtype=np.float32)+0.1)

            # identify the preferred mode as the most used mode
            id_mode_preferred = np.argmax(np.bincount(ids_mode))

            _virtualpop.activitypatterns[id_person_vp] = ids_activity
            _virtualpop.ids_mode_preferred[id_person_vp] = id_mode_preferred
            _virtualpop.identifications[id_person_vp] = 'MS.'+id_person
            print('  id_person', id_person, 'id_mode_preferred', id_mode_preferred,
                  'hours_begin', hours_begin, 'durations', durations)
            i += 1

        return True


class TripImporterbasedonMapMatching(ParserMixin, MatsimProcessMixin, Process):
    def __init__(self,  matsim, planfilepath='', ids_mode=None,
                 logger=None, **kwargs):
        print('TripImporter.__init__', matsim)
        self._init_common('tripimporter', name='MATSIM trip importer',
                          logger=logger,
                          info='Imports MATSIM plan XML file file and generates trips.',
                          )
        self._matsim = matsim
        self._scenario = matsim.get_scenario()
        self._net = self._scenario.net
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()

        attrsman = self.set_attrsman(cm.Attrsman(self))
        self._init_importer()
        if planfilepath is None:
            planfilepath = os.path.join(rootdirpath, rootname + '.xml')

        self.planfilepath = attrsman.add(cm.AttrConf('planfilepath', kwargs.get('planfilepath', planfilepath),
                                                     groupnames=['options'],
                                                     name='Matsim Plan file',
                                                     wildcards='Plan XML files (*.xml)|*.xml*',
                                                     metatype='filepath',
                                                     info='extracted MATSIM plan file to be imported.',
                                                     ))

        self.eventfilepath = attrsman.add(cm.AttrConf('eventfilepath', kwargs.get('planfilepath', planfilepath),
                                                      groupnames=['options'],
                                                      name='Matsim Events file',
                                                      wildcards='Plan XML files (*.xml)|*.xml*',
                                                      metatype='filepath',
                                                      info='extracted MATSIM event file to be imported.',
                                                      ))

        self.hour_begin = attrsman.add(cm.AttrConf('hour_begin', kwargs.get('hour_begin', 0),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   name='Hour begin',
                                                   unit='h',
                                                   info='Hour of the beginning of a trip of a person.',
                                                   ))

        self.hour_end = attrsman.add(cm.AttrConf('hour_end', kwargs.get('hour_end', 10),
                                                 groupnames=['options'],
                                                 name='Hour end',
                                                 unit='h',
                                                 info='Hour of the end of a trip of a person.',
                                                 ))

        self.scaling_factor = attrsman.add(cm.AttrConf('scaling_factor', kwargs.get('scaling_factor', 1),
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Scaling factor for trip import',
                                                       info='Scaling factor for trip import',
                                                       ))

        modechoices = self._net.modes.names.get_indexmap()

        if (ids_mode is None):
            # no info given, ...
            ids_mode_default = [modechoices['passenger'], modechoices['bicycle'],
                                modechoices['pedestrian'], modechoices['motorcycle'], modechoices['taxi']]

        elif ids_mode is not None:
            ids_mode_default = ids_mode

        self.timeformat = attrsman.add(cm.AttrConf('timeformat', kwargs.get('timeformat', 'hh:mm:ss'),
                                                   groupnames=['options'],
                                                   choices=list(TIMEFORMATMAP.keys()),
                                                   name='Time Format',
                                                   info='Time Format',
                                                   ))

        self.use_map_matching = attrsman.add(cm.AttrConf('use_map_matching', False,
                                                         groupnames=['options'],
                                                         name='use_map_matching',
                                                         info='Use MapMatching (True) or MapUpdater (False)',
                                                         ))
        self.multi_modal_import = attrsman.add(cm.AttrConf('multi_modal_import', False,
                                                           groupnames=['options'],
                                                           name='multi_modal_import',
                                                           info='Import Bike and Walk as well',
                                                           ))

        self.init_proj_attributes(projparams_matsim=matsim.net.projparams,
                                  projectionmode='custom projection')

    def do(self):
        # edges = self._net.edges
        # nodes = self._net.nodes
        # matsimedges = self._matsim.net.edges
        # matsimnodes = self._matsim.net.nodes
        # scenariorootname = 'C:/TEST_SCHWABING_MATSIM_SUMO/SUMOPY_Scenario/21092023_base_scenario_det'
        #
        #######

        hour_offset = 0
        hour_begin_ext = hour_offset
        hour_end_ext = hour_offset+24
        myedges = self._net.edges
        myvtypes = self._scenario.demand.vtypes

        matsim_sumo_modemap = {u'car': 4, u'pt': 5, u'walk': 1, u'bike': 2, u'motorbike': 3}
        matsim_sumo_activitymap = {u'work': 3, u'home': 2, u'education': 4, u'other': 6, u'shopping': 5}

        # OPTIONMAP_POS_DEPARTURE =      {"random": -1, "free": -2, "random_free": -3, "base": -4, "last": -5, "first": -6}
        # OPTIONMAP_POS_ARRIVAL =        {"random": -1, "max": -2}
        # OPTIONMAP_SPEED_DEPARTURE =    {"random": -1, "max": -2}
        # OPTIONMAP_SPEED_ARRIVAL =      {"current": -1}
        # OPTIONMAP_LANE_DEPART =        {"random": -1, "free": -2,"allowed": -3, "best": -4, "first": -5}
        # OPTIONMAP_LANE_ARRIVAL =       {"current": -1}
        ids_user = []
        ids_vtype = []  # np.zeros(n_trips, dtype = np.int32)
        times_depart = []  # np.zeros(n_trips, dtype = np.int32)
        ids_edge_depart = []  # np.zeros(n_trips, dtype = np.int32)
        ids_edge_arrival = []  # np.zeros(n_trips, dtype = np.int32)
        ids_routes = []
        arrival_speeds = []

        walk_ids_user = []
        walk_ids_vtype = []  # np.zeros(n_trips, dtype = np.int32)
        walk_times_depart = []  # np.zeros(n_trips, dtype = np.int32)
        walk_ids_edge_depart = []  # np.zeros(n_trips, dtype = np.int32)
        walk_ids_edge_arrival = []  # np.zeros(n_trips, dtype = np.int32)
        walk_ids_routes = []
        walk_arrival_speeds = []

        n_trips = 0
        n_trips_abs = 0

        print('Trips Through Schawbing')
        print('... extracting matsim trips')

        # parse plan file

        parser = MatsimPlanParser(self._scenario, self.use_map_matching, self.multi_modal_import)
        if self.planfilepath.endswith('.gz'):
            # Open GZIP file
            gzfile = gzip.open(self.planfilepath, 'rb')
            parse(gzfile, parser)
        else:
            parse(self.planfilepath, parser)
        externaltrips = parser.population

        # parse event file
        print('... parse events')
        relevant_persons = externaltrips.keys()
        parser = MatsimEventParser(self._scenario, relevant_persons)
        if self.eventfilepath.endswith('.gz'):
            # Open GZIP file
            gzfile = gzip.open(self.eventfilepath, 'rb')
            parse(gzfile, parser)
        else:
            parse(self.eventfilepath, parser)

        entry_times = parser.entry_times

        all_sumo_ids = myedges.get_ids()
        modename_last = ''

        ######################################################################################################################################################################################################
        # get routing graphs
        id_mode = self._scenario.demand.vtypes.ids_mode[myvtypes.get_vtype_for_mode(matsim_sumo_modemap['walk'])]

        weights_walk = self._net.edges.get_times_cached(id_mode=id_mode,
                                                        speed_max=self._scenario.demand.vtypes.speeds_max[myvtypes.get_vtype_for_mode(
                                                            matsim_sumo_modemap['walk'])],
                                                        is_check_lanes=True,
                                                        modeconst_excl=-10.0, modeconst_mix=-5.0,
                                                        )
        fstar_walk = self._net.edges.get_fstar_cached(id_mode=id_mode)

        id_mode = self._scenario.demand.vtypes.ids_mode[myvtypes.get_vtype_for_mode(matsim_sumo_modemap['bike'])]

        weights_bike = self._net.edges.get_times_cached(id_mode=id_mode,
                                                        speed_max=self._scenario.demand.vtypes.speeds_max[myvtypes.get_vtype_for_mode(
                                                            matsim_sumo_modemap['bike'])],
                                                        is_check_lanes=True,
                                                        modeconst_excl=-10.0, modeconst_mix=-5.0,
                                                        )
        fstar_bike = self._net.edges.get_fstar_cached(id_mode=id_mode)

        id_mode = self._scenario.demand.vtypes.ids_mode[myvtypes.get_vtype_for_mode(matsim_sumo_modemap['car'])]

        weights_car = self._net.edges.get_times_cached(id_mode=id_mode,
                                                       speed_max=self._scenario.demand.vtypes.speeds_max[myvtypes.get_vtype_for_mode(
                                                           matsim_sumo_modemap['car'])],
                                                       is_check_lanes=True,
                                                       modeconst_excl=-10.0, modeconst_mix=-5.0,
                                                       )
        fstar_car = self._net.edges.get_fstar_cached(id_mode=id_mode)
        ######################################################################################################################################################################################################

        all_routes = []
        ids_vtype_all = []
        ids_trip_all = []
        is_add_all = []
        error_list = []
        error_list_2 = []
        for id_user_matsim in externaltrips:

            # Iterate through persons

            modenames, triptimes, travtimes, ids_edge_sumo_start, ids_edge_sumo_end, routes, start_x_list, start_y_list, end_x_list, end_y_list, reached_dest = externaltrips[
                id_user_matsim]

            # Iterate through trips of person

            for modename, triptime, travtime, id_edge_sumo_start, id_edge_sumo_end, route, start_x, start_y, end_x, end_y, reached_dest_bool in zip(modenames, triptimes, travtimes, ids_edge_sumo_start, ids_edge_sumo_end, routes, start_x_list, start_y_list, end_x_list, end_y_list, reached_dest):

                # Relevanz wird bereits von dem matSim Parser geprüft! Es ist hier nicht mehr notwendig!
                if True:  # (id_edge_sumo_start in all_sumo_ids) & (id_edge_sumo_end in all_sumo_ids):

                    hh, mm, ss = triptime.split(':')

                    h, m, s = int(hh), int(mm), int(ss)

                    starttime_in_sek = 3600*h + 60*m + s

                    ######################################################################################################################################################################################################

                    hh, mm, ss = travtime.split(':')

                    h, m, s = int(hh), int(mm), int(ss)

                    endtime_in_sek = starttime_in_sek + 3600*h + 60*m + s

                    ######################################################################################################################################################################################################
                    # Time - correction
                    if modename == 'car':

                        if str(id_user_matsim)+'-'+str(id_edge_sumo_start) in entry_times:

                            result = entry_times[str(id_user_matsim)+'-'+str(id_edge_sumo_start)]

                        else:

                            result = []

                        counter = 0

                        result_found = False

                        while counter < len(result) and not result_found:

                            arival_time_in_sumo = result[counter]
                            counter += 1

                            if arival_time_in_sumo >= starttime_in_sek and arival_time_in_sumo <= endtime_in_sek:
                                result_found = True

                        if not result_found:
                            arival_time_in_sumo = starttime_in_sek
                    else:

                        arival_time_in_sumo = starttime_in_sek

                    ##################################################################################################################################################
                    # Jetzt geht es ans routing!
                    if (h >= hour_begin_ext) & (h < hour_end_ext):

                        if modename == 'walk':

                            ids_edge_from, dists = self._net.edges.get_closest_edge(
                                [start_x, start_y],  n_best=10, accesslevels=self._net.edges.get_accesslevels(MATSIM_SUMO_MODEMAP[modename]))
                            # id_edge_from = ids_edge_from[0]
                            # # hier werden teilweise mehrere edges zurück gegeben!

                            ids_edge_to, dists = self._net.edges.get_closest_edge(
                                [end_x, end_y],  n_best=10, accesslevels=self._net.edges.get_accesslevels(MATSIM_SUMO_MODEMAP[modename]))

                            id_edge_sumo_start = ids_edge_from[0]
                            id_edge_sumo_end = ids_edge_to[0]
                            # no route for walking
                            route_sumo = []

                        if modename == 'bike':

                            ids_edge_from, dists = self._net.edges.get_closest_edge(
                                [start_x, start_y],  n_best=10, accesslevels=self._net.edges.get_accesslevels(MATSIM_SUMO_MODEMAP[modename]))
                            # id_edge_from = ids_edge_from[0]
                            # # hier werden teilweise mehrere edges zurück gegeben!

                            ids_edge_to, dists = self._net.edges.get_closest_edge(
                                [end_x, end_y],  n_best=10, accesslevels=self._net.edges.get_accesslevels(MATSIM_SUMO_MODEMAP[modename]))

                            # id_edge_to= ids_edge_to[0]
                            # # Nutze hybridPY Standard Funktionen -> deutlich performanter !
                            route_sumo = []
                            route_sumo_temp = []
                            counter = 0
                            counter_1 = 0
                            counter_2 = 0
                            while len(route_sumo_temp) == 0 and counter <= 5:

                                id_edge_sumo_start = ids_edge_from[0]
                                id_edge_sumo_end = ids_edge_to[counter_2]

                                cost, route_sumo_temp = get_mincostroute_edge2edge(id_edge_sumo_start,
                                                                                   id_edge_sumo_end,
                                                                                   weights=weights_bike,
                                                                                   fstar=fstar_bike)
                                if counter % 2 == 0:
                                    counter_2 += 1
                                else:
                                    counter_2 += 1

                                counter += 1

                            if len(route_sumo_temp) > 0:

                                route_sumo.extend(route_sumo_temp)
                            else:
                                print('No route found for biking!')

                        if modename == 'car':

                            # Hier muss noch mit den "genauen Aktivitätsorten gerechnet werden"

                            id_start_temp = id_edge_sumo_start

                            route_sumo = []
                            # for matsim_edge in route:
                            pos = 0
                            destination_reached = False

                            if id_edge_sumo_start == id_edge_sumo_end:

                                destination_reached = True
                                route_sumo.extend([id_edge_sumo_start])

                            while pos < len(route) and not destination_reached:

                                matsim_edge = route[pos]
                                if self.use_map_matching:
                                    # aktuell: MATSim ID incoming (using map-matching)
                                    id_mapmatch = self._scenario.demand.matsim.mapmatch.ids_matsim.get_id_from_index(
                                        matsim_edge)
                                    sumo_value = self._scenario.demand.matsim.mapmatch.get_row(id_mapmatch)['ids_sumo']
                                else:
                                    # (using )
                                    sumo_value = matsim_edge

                                # aktuelle Matching Logik: 1 SUmo Edge -> 1 MATSim Edge -> okay!
                                # if type(sumo_value) == int:
                                temp_dest = sumo_value
                                # else:
                                #    temp_dest  = sumo_value[-1]

                                if not (id_start_temp == temp_dest):

                                    cost, route_sumo_temp = get_mincostroute_edge2edge(id_start_temp,
                                                                                       temp_dest,
                                                                                       weights=weights_car,
                                                                                       fstar=fstar_car)
                                    if len(route_sumo_temp) > 5 and (pos+1) < len(route):

                                        # handelt es sich um ein miss-matching?
                                        # will ich falsch abbiegen oder bin ich falsch abgebogen? || Definition pos: pos = nächste Edge ; pos + 1 = übernächste Egde; pos - 1 = aktuelle Position; pos - 2 = vorherige Position
                                        route_sumo_check_prev = []
                                        route_sumo_check = []

                                        matsim_edge = route[pos+1]
                                        # id_mapmatch = self._scenario.demand.matsim.mapmatch.ids_matsim.get_id_from_index(matsim_edge)
                                        # sumo_value_check = self._scenario.demand.matsim.mapmatch.get_row(id_mapmatch)['ids_sumo']

                                        if self.use_map_matching:
                                            # aktuell: MATSim ID incoming (using map-matching)
                                            id_mapmatch = self._scenario.demand.matsim.mapmatch.ids_matsim.get_id_from_index(
                                                matsim_edge)
                                            sumo_value_check = self._scenario.demand.matsim.mapmatch.get_row(id_mapmatch)[
                                                'ids_sumo']
                                        else:
                                            # (using )
                                            sumo_value_check = matsim_edge

                                        cost, route_sumo_check = get_mincostroute_edge2edge(id_start_temp,
                                                                                            sumo_value_check,
                                                                                            weights=weights_car,
                                                                                            fstar=fstar_car)
                                        ignore_previous = False

                                        if pos-2 >= 0:

                                            matsim_edge = route[pos-2]

                                            if self.use_map_matching:
                                                # aktuell: MATSim ID incoming (using map-matching)
                                                id_mapmatch = self._scenario.demand.matsim.mapmatch.ids_matsim.get_id_from_index(
                                                    matsim_edge)
                                                sumo_value_check_prev = self._scenario.demand.matsim.mapmatch.get_row(id_mapmatch)[
                                                    'ids_sumo']
                                            else:
                                                # (using )
                                                sumo_value_check_prev = matsim_edge

                                            # id_mapmatch = self._scenario.demand.matsim.mapmatch.ids_matsim.get_id_from_index(matsim_edge)
                                            # sumo_value_check_prev = self._scenario.demand.matsim.mapmatch.get_row(id_mapmatch)['ids_sumo']

                                            cost, route_sumo_check_prev = get_mincostroute_edge2edge(sumo_value_check_prev,
                                                                                                     temp_dest,
                                                                                                     weights=weights_car,
                                                                                                     fstar=fstar_car)
                                        else:
                                            ignore_previous = True

                                        if len(route_sumo_check) >= len(route_sumo_temp) and route_sumo_check[:len(route_sumo_temp)] == route_sumo_temp and (len(route_sumo_check_prev) >= len(route_sumo_temp) or ignore_previous):
                                            pass
                                        # len muss größer null sein!
                                        # sumo_value_check_prev muss ungleich temp_dest sein!
                                        elif (len(route_sumo_check_prev) < len(route_sumo_temp)) and (ignore_previous != False) and len(route_sumo_check_prev) > 0:
                                            # Ich bin falsch abgebogen!
                                            # print("Matching falsch für "+ str(id_start_temp))
                                            pos_start = route_sumo.index(route_sumo_check_prev[0])
                                            route_sumo = route_sumo[:pos_start]
                                            route_sumo_temp = route_sumo_check_prev
                                            error_list_2.append(id_start_temp)

                                        elif len(route_sumo_check) > 0:
                                            # Mein Ziel ist Falsch
                                            print(str(temp_dest))
                                            error_list.append(temp_dest)
                                            route_sumo_temp = route_sumo_check
                                            pos += 1

                                    if len(route_sumo_temp) > 0:
                                        id_start_temp = route_sumo_temp[-1]
                                        if (id_edge_sumo_end in route_sumo_temp):
                                            index = route_sumo_temp.index(id_edge_sumo_end)
                                            route_sumo.extend(route_sumo_temp[:index + 1])
                                            # Wenn er bereits am Ziel vorbei fährt, dann stoppe die Route
                                            destination_reached = True
                                        else:
                                            route_sumo.extend(route_sumo_temp[:-1])

                                pos += 1
                        #########################################################################################################################################################################################################################
                        # Überprüfen der Routen notwendig! -> schneide Route, wenn Agent bereits auf der Gegespur vorbei gefahren ist!

                        if modename == "car" or modename == "bike":
                            if len(route_sumo) > 1:
                                edge_end = route_sumo[-1]
                                # opposite = set(self._scenario.net.edges.select_ids((self._scenario.net.edges.ids_tonode.value == self._scenario.net.edges.get_row(edge_end)['ids_fromnode']) and (self._scenario.net.edges.ids_fromnode.value == self._scenario.net.edges.get_row(edge_end)['ids_tonode'])))
                                A = self._scenario.net.edges.select_ids(
                                    (self._scenario.net.edges.ids_tonode.value == self._scenario.net.edges.get_row(edge_end)['ids_fromnode']))
                                B = self._scenario.net.edges.select_ids(
                                    (self._scenario.net.edges.ids_fromnode.value == self._scenario.net.edges.get_row(edge_end)['ids_tonode']))
                                opposite = A[np.in1d(A, B)]

                                if len(opposite) == 1:
                                    if (opposite[0] in route_sumo):
                                        index = route_sumo.index(edge_end)
                                        route_sumo = route_sumo[:index]

                            # node_prev ------> node -----> node_next
                            if reached_dest_bool:
                                arrival_speed = 0
                            else:
                                arrival_speed = db.OPTIONMAP_SPEED_ARRIVAL["current"]

                        u_turn = False

                        # if modename == "car":
                        # #check for u-turns!
                        #     if len(route_sumo) >1:
                        #         counter = 0
                        #         for edge in route_sumo:
                        #             if counter == 0:
                        #                 counter += 1
                        #                 node_prev = self._scenario.net.edges.get_row(edge)['ids_fromnode']
                        #                 #node = self._scenario.net.edges.get_row(edge_end)['ids_tonode']
                        #             else:
                        #                 if node_prev == self._scenario.net.edges.get_row(edge)['ids_tonode']:
                        #                     print("U turn")
                        #                     u_turn = True
                        #                     node_prev = self._scenario.net.edges.get_row(edge)['ids_fromnode']
                        #                     #node = self._scenario.net.edges.get_row(edge_end)['ids_tonode']
                        #                 else:
                        #                     node_prev = self._scenario.net.edges.get_row(edge)['ids_fromnode']
                        #                     #node = self._scenario.net.edges.get_row(edge_end)['ids_tonode']

                        # Es gibt Routen, die nur aus einem Edge bestehen -> Fehleranfällig!
                        if (len(route_sumo) > 1 and u_turn == False) and (modename == "bike" or modename == "car"):

                            for i in range(1, int(self.scaling_factor)+1):
                                if i == 1:
                                    ids_user.append(str(id_user_matsim)+'00'+str(n_trips)+'00'+str(i))
                                    ids_vtype.append(myvtypes.get_vtype_for_mode(matsim_sumo_modemap[modename]))
                                    times_depart.append(arival_time_in_sumo)  # time end of previous activity
                                    ids_edge_depart.append(id_edge_sumo_start)
                                    ids_edge_arrival.append(id_edge_sumo_end)
                                    arrival_speeds.append(arrival_speed)
                                else:
                                    ids_user.append(str(id_user_matsim)+'00'+str(n_trips)+'00'+str(i))
                                    ids_vtype.append(myvtypes.get_vtype_for_mode(matsim_sumo_modemap[modename]))
                                    # time end of previous activity
                                    times_depart.append(max(0, arival_time_in_sumo + (-20*60+np.random.randint(40*60))))
                                    ids_edge_depart.append(id_edge_sumo_start)
                                    ids_edge_arrival.append(id_edge_sumo_end)
                                    arrival_speeds.append(arrival_speed)

                                n_trips_abs += 1
                                all_routes.append(route_sumo)
                                ids_vtype_all.append(myvtypes.get_vtype_for_mode(matsim_sumo_modemap[modename]))
                                ids_trip_all.append(n_trips_abs)
                                # is_add_all.append(False)

                            print("Added " + modename + " route")
                            n_trips += 1

                        if modename == "walk":

                            for i in range(1, int(self.scaling_factor)+1):
                                if i == 1:
                                    walk_ids_user.append(str(id_user_matsim)+'00'+str(n_trips)+'00'+str(i))
                                    walk_ids_vtype.append(myvtypes.get_vtype_for_mode(matsim_sumo_modemap[modename]))
                                    walk_times_depart.append(arival_time_in_sumo)  # time end of previous activity
                                    walk_ids_edge_depart.append(id_edge_sumo_start)
                                    walk_ids_edge_arrival.append(id_edge_sumo_end)
                                else:
                                    walk_ids_user.append(str(id_user_matsim)+'00'+str(n_trips)+'00'+str(i))
                                    walk_ids_vtype.append(myvtypes.get_vtype_for_mode(matsim_sumo_modemap[modename]))
                                    # time end of previous activity
                                    walk_times_depart.append(
                                        max(0, arival_time_in_sumo + (-20*60+np.random.randint(40*60))))
                                    walk_ids_edge_depart.append(id_edge_sumo_start)
                                    walk_ids_edge_arrival.append(id_edge_sumo_end)

                            print("Added " + modename + " route")
                            n_trips += 1
                else:
                    print('WARNING: user', id_user_matsim, 'uses unknown SUMO edge', id_edge_sumo_start, id_edge_sumo_end)

        print("Matching errors")
        print(set(error_list))
        print("Matching errors")
        print(set(error_list_2))
        # print 'ids_edge_depart=',ids_edge_depart
        self._scenario.demand.trips.clear_trips()

        self._scenario.demand.trips.make_trips(ids_vtype, is_generate_ids=False,
                                               ids_sumo=ids_user,
                                               times_depart=times_depart,
                                               ids_edge_depart=ids_edge_depart,
                                               ids_edge_arrival=ids_edge_arrival,
                                               inds_lane_depart=np.ones(n_trips_abs, dtype=np.int32) *
                                               db.OPTIONMAP_LANE_DEPART["best"],
                                               positions_depart=np.ones(n_trips_abs, dtype=np.int32) *
                                               db.OPTIONMAP_POS_DEPARTURE["base"],
                                               speeds_depart=np.ones(n_trips_abs, dtype=np.int32) *
                                               db.OPTIONMAP_SPEED_DEPARTURE["max"],
                                               inds_lane_arrival=np.ones(
                                                   n_trips_abs, dtype=np.int32)*db.OPTIONMAP_LANE_ARRIVAL["current"],
                                               positions_arrival=np.ones(
                                                   n_trips_abs, dtype=np.int32)*db.OPTIONMAP_POS_ARRIVAL["random"],
                                               speeds_arrival=arrival_speeds,
                                               )
        self._scenario.demand.trips.make_routes(np.asarray(ids_vtype_all),
                                                routes=np.asarray(all_routes),
                                                ids_trip=np.asarray(ids_trip_all),
                                                is_add=False,
                                                )

        self._scenario.demand.trips.make_trips(walk_ids_vtype, is_generate_ids=False,
                                               ids_user=walk_ids_user,
                                               times_depart=walk_times_depart,
                                               ids_edge_depart=walk_ids_edge_depart,
                                               ids_edge_arrival=walk_ids_edge_arrival,
                                               inds_lane_depart=np.ones(
                                                   len(walk_ids_user), dtype=np.int32)*db.OPTIONMAP_LANE_DEPART["best"],
                                               positions_depart=np.ones(
                                                   len(walk_ids_user), dtype=np.int32)*db.OPTIONMAP_POS_DEPARTURE["base"],
                                               speeds_depart=np.ones(len(walk_ids_user), dtype=np.int32) *
                                               db.OPTIONMAP_SPEED_DEPARTURE["max"],
                                               inds_lane_arrival=np.ones(
                                                   len(walk_ids_user), dtype=np.int32)*db.OPTIONMAP_LANE_ARRIVAL["current"],
                                               positions_arrival=np.ones(
                                                   len(walk_ids_user), dtype=np.int32)*db.OPTIONMAP_POS_ARRIVAL["random"],
                                               speeds_arrival=np.ones(len(walk_ids_user), dtype=np.int32) *
                                               db.OPTIONMAP_SPEED_ARRIVAL["current"],
                                               )
        return True

    def plot_bike_routing_problem(self, line_start, line_end):
        # Debugging function!

        x_min, y_min, x_max, y_max = self._net.get_boundaries(is_netboundaries=True)
        # Calculate the width and height of the rectangle
        rect_width = x_max - x_min
        rect_height = y_max - y_min

        # Create a new figure and axis
        fig, ax = plt.subplots()

        # Plot the rectangle
        rectangle = patches.Rectangle([x_min, y_min], rect_width, rect_height,
                                      linewidth=2, edgecolor='blue', facecolor='none')
        ax.add_patch(rectangle)

        # Plot the line
        plt.plot([line_start[0], line_end[0]], [line_start[1], line_end[1]], color='red', linewidth=2)

        # Set the limits of the plot
        ax.set_xlim(min(x_min, line_start[0]) - 1, max(x_max, line_end[0]) + 1)
        ax.set_ylim(min(y_min, line_start[1]) - 1, max(y_max, line_end[1]) + 1)

        # Add labels and title
        plt.xlabel('X coordinate')
        plt.ylabel('Y coordinate')
        plt.title('Rectangle and Line Plot')

        # Show the plot
        plt.grid(True)
        plt.show()


class MatsimPlanParser(handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self, scenario, use_map_matching=False, multi_modal_import=False):
        self.population = {}
        self.id_person = None
        self.modes = []
        self.dep_times = []
        self.trav_times = []
        self.reached_dest = []
        self.scenario = scenario
        self._selected_plan = False
        self._is_route = False
        self.multi_modal_import = multi_modal_import
        # Parameters for routed ways
        self.routed_leg = False
        self._new_plan = True
        self.entrance_points = []
        self.exit_points = []
        self.route = []
        self.start_x_list = []
        self.start_y_list = []
        self.end_x_list = []
        self.end_y_list = []

        # parameters for unrouted routes
        self.unrouted_leg = False
        self.record_coords = False
        self.start_x = 0
        self.start_y = 0
        self.end_x = 0
        self.end_y = 0
        self.find_in_sumo = 0
        self.key_dict_matsim = {}
        self.key_dict_matsim_sumo = {}

        self._in_boundaries = scenario.net.in_boundaries
        self._offset = scenario.net.get_offset()
        self._projparams_matsim = 'EPSG:31468'
        self._id_projectionmode = 1
        self._projparams_sumo = scenario.net._projparams
        coord_matsim = pyproj.CRS(self._projparams_matsim)
        coord_sumo = pyproj.CRS(self._projparams_sumo)
        self.transformer_I = pyproj.Transformer.from_crs(coord_matsim, coord_sumo, always_xy=True)

        self.edges = scenario.net.edges
        x_min, y_min, x_max, y_max = scenario.net.get_boundaries(is_netboundaries=True)
        x_min_matsim, y_min_matsim = self.get_coord_matsim(x_min, y_min)
        x_max_matsim, y_max_matsim = self.get_coord_matsim(x_max, y_max)
        self._boundaries = np.array([x_min_matsim, y_min_matsim, x_max_matsim, y_max_matsim])

        # TODO: Parametrisierung -> welches Mapping soll verwendet werden?
        if use_map_matching:
            for id in self.scenario.demand.matsim.mapmatch.ids_matsim:
                external = self.scenario.demand.matsim.net.edges.get_row(id)['ids_matsim']
                self.key_dict_matsim[external] = id
                id_mapmatch = self.scenario.demand.matsim.mapmatch.ids_matsim.get_id_from_index(id)
                sumo_value = self.scenario.demand.matsim.mapmatch.get_row(id_mapmatch)['ids_sumo']
                self.key_dict_matsim_sumo[id] = sumo_value
        else:
            for id in self.scenario.net.edges.ids_sumo:
                # id =
                # external = self.scenario.net.edges.get_row(id)['ids_sumo']
                ind = self.scenario.net.edges.ids_sumo.get_id_from_index(id)
                self.key_dict_matsim[id] = ind
                self.key_dict_matsim_sumo[ind] = ind

    def startElement(self, name, attrs):

        if name == 'person':
            self.id_person = attrs['id']

        elif name == 'activity':

            if self.unrouted_leg and self._selected_plan:
                # handle unrouted
                self.end_x = float(attrs['x'])
                self.end_y = float(attrs['y'])

                # correct it later!
                if self.multi_modal_import:

                    if (self._leg_mode == 'walk' or self._leg_mode == 'bike') and self._new_plan == False:

                        # Berechnung des schnittpunkts ist aufwändig => erst relevanz abschätzung
                        traveled_distance = np.linalg.norm(np.array([float(self.end_x), float(
                            self.end_y)])-np.array([float(self.start_x), float(self.start_y)])) * 2
                        x_border = traveled_distance
                        y_border = traveled_distance
                        relevant_pre_test_1 = False
                        if ((self._boundaries[0]-x_border <= self.start_x) & (self._boundaries[2]+x_border >= self.start_x) & (self._boundaries[1]-y_border <= self.start_y) & (self._boundaries[3]+y_border >= self.start_y)):
                            relevant_pre_test_1 = True
                        relevant_pre_test_2 = False
                        if ((self._boundaries[0]-x_border <= self.end_x) & (self._boundaries[2]+x_border >= self.end_x) & (self._boundaries[1]-y_border <= self.end_y) & (self._boundaries[3]+y_border >= self.end_y)):
                            relevant_pre_test_2 = True

                        if relevant_pre_test_1 and relevant_pre_test_2:

                            # working with MATSim coords for simplicity
                            is_relevant, start_coord, end_coord = self.line_rect_intersection(np.array([float(self.start_x), float(self.start_y)]), np.array([float(
                                self.end_x), float(self.end_y)]), np.array([self._boundaries[0], self._boundaries[1]]), np.array([self._boundaries[2], self._boundaries[3]]))

                            if is_relevant:
                                x_temp, y_temp = self.get_coord_sumo(start_coord[0], start_coord[1])
                                # transfer to SUMO neccessary
                                self.start_x_list.append(x_temp)
                                self.start_y_list.append(y_temp)

                                x_temp, y_temp = self.get_coord_sumo(end_coord[0], end_coord[1])

                                self.end_x_list.append(x_temp)
                                self.end_y_list.append(y_temp)

                                self.entrance_points.append('')
                                self.exit_points.append('')
                                self.modes.append(self._leg_mode)
                                self.dep_times.append(self._dep_time)
                                self.route.append([])
                                self.trav_times.append(self._trav_time)
                                self.reached_dest.append('')

                self.unrouted_leg = False
                self.start_x = float(attrs['x'])
                self.start_y = float(attrs['y'])

            else:
                if self._new_plan == False:

                    self.end_x = float(attrs['x'])
                    self.end_y = float(attrs['y'])

                    if self.record_coords == True:

                        x_temp, y_temp = self.get_coord_sumo(self.start_x, self.start_y)
                        # transfer to SUMO neccessary
                        self.start_x_list.append(x_temp)
                        self.start_y_list.append(y_temp)

                        x_temp, y_temp = self.get_coord_sumo(self.end_x, self.end_y)

                        self.end_x_list.append(x_temp)
                        self.end_y_list.append(y_temp)

                        # self.start_x_list.append(self.start_x)
                        # self.start_y_list.append(self.start_y)
                        # self.end_x_list.append(self.end_x)
                        # self.end_y_list.append(self.end_y)

                self._new_plan = False
                self.start_x = float(attrs['x'])
                self.start_y = float(attrs['y'])

        elif name == 'plan' and attrs['selected'] == 'yes':
            self._selected_plan = True
            self._new_plan = True

        if self._selected_plan:
            if name == 'leg':
                self._leg_mode = attrs['mode']
                self._dep_time = attrs['dep_time']
                # new matsim version
                if 'trav_time' in attrs:
                    self._trav_time = attrs['trav_time']
                self.record_coords = False

            elif name == 'route':
                if attrs['type'] == 'links':
                    self.unrouted_leg = False
                else:
                    self.unrouted_leg = True

                if 'trav_time' in attrs:
                    self._trav_time = attrs['trav_time']

                self._is_route = True
                self._route = ''
                self._entry = False
                self._exit = False
                self._prev_edge = ''

    def endElement(self, name):
        if name == 'person':
            # save entries
            if self.id_person is not None:
                self.population[self.id_person] = (self.modes, self.dep_times, self.trav_times, self.entrance_points, self.exit_points,
                                                   self.route, self.start_x_list, self.start_y_list, self.end_x_list, self.end_y_list, self.reached_dest)

            # reset variables
            self.entrance_points = []
            self.exit_points = []
            self.modes = []
            self.dep_times = []
            self.trav_times = []
            self.route = []
            self.start_x_list = []
            self.start_y_list = []
            self.end_x_list = []
            self.end_y_list = []
            self.reached_dest = []

        elif name == 'plan':
            if self._selected_plan and len(self.entrance_points) == 0 and len(self.exit_points) == 0:
                self.id_person = None
            # reset variables
            self._selected_plan = False

        elif self._selected_plan:
            if name == 'route':
                # reset variables
                self._is_route = False

    def characters(self, content):
        if self._selected_plan and self._is_route:
            route_temp = []
            self._route = content.split(' ')
            for index, edge in enumerate(self._route):

                # Diese Stelle ist sehr sehr langsam! -> Besser

                if edge in self.key_dict_matsim:
                    route_temp.append(self.key_dict_matsim[edge])

            if self._route[-1] in self.key_dict_matsim:
                reached_dest = True
            else:
                reached_dest = False

            if not (route_temp == []):
                temp = 0
                if not isinstance(self.key_dict_matsim_sumo[route_temp[0]], list):
                    temp = self.key_dict_matsim_sumo[route_temp[0]]
                else:
                    temp = self.key_dict_matsim_sumo[route_temp[0]][0]

                self.entrance_points.append(temp)
                temp = 0
                if not isinstance(self.key_dict_matsim_sumo[route_temp[-1]], list):
                    temp = self.key_dict_matsim_sumo[route_temp[-1]]
                else:
                    temp = self.key_dict_matsim_sumo[route_temp[-1]][-1]

                self.exit_points.append(temp)
                self.trav_times.append(self._trav_time)
                self.route.append(route_temp)
                self.modes.append(self._leg_mode)
                self.dep_times.append(self._dep_time)
                self.reached_dest.append(reached_dest)
                # trip inside area
                self.record_coords = True
            else:
                # trip outside area
                self.record_coords = False

    def get_coord_matsim(self, x_sumo, y_sumo):

        coord_matsim = pyproj.CRS(self._projparams_matsim)
        coord_sumo = pyproj.CRS(self._projparams_sumo)
        transformer_I = pyproj.Transformer.from_crs(coord_sumo, coord_matsim, always_xy=True)
        x2, y2 = transformer_I.transform(x_sumo-self._offset[0], y_sumo-self._offset[1])
        return x2, y2

    def get_coord_sumo(self, x_matsim, y_matsim):
        x2, y2 = self.transformer_I.transform(x_matsim, y_matsim)
        return x2 + self._offset[0], y2 + self._offset[1]

    # def check_relevanz(self,coords_1, coords_2, boundaries):
    #     traveled_distance = np.linalg.norm(np.array(coords_2)-np.array(coords_1)) *2
    #     x_border = traveled_distance
    #     y_border = traveled_distance
    #     relevant_1 = False
    #     if ((boundaries[0]-x_border <= coords_1[0]) & (boundaries[2]+x_border >= coords_1[0])&(boundaries[1]-y_border <= coords_1[1]) & (boundaries[3]+y_border >= coords_1[1])):
    #         relevant_1 = True
    #     relevant_2 = False
    #     if ((boundaries[0]-x_border <= coords_2[0]) & (boundaries[2]+x_border >= coords_2[0])&(boundaries[1]-y_border <= coords_2[1]) & (boundaries[3]+y_border >= coords_2[1])):
    #         relevant_2 = True

    #     if relevant_1 == True and relevant_2 == True:
    #         return True
    #     else:
    #         return False

    def prf_schnittp(self, line1_start, line1_end, line2_start, line2_end):
        dir_vec1 = line1_end - line1_start
        dir_vec2 = line2_end - line2_start

        # Berechne die Richtungsdeterminanten
        det = np.cross(dir_vec1, dir_vec2)
        if det != 0:
            # Berechne die Differenz zwischen den Startpunkten der Linien
            start_diff = line2_start - line1_start

            # Berechne die Parameter für den Schnittpunkt der Linien
            t1 = np.cross(start_diff, dir_vec2) / det

            # Berechne den Schnittpunkt der Linien
            intersection_point = line1_start + t1 * dir_vec1

            if self.check_schnittpunkt(line1_start, line1_end, intersection_point) and self.check_schnittpunkt(line2_start, line2_end, intersection_point):
                return True, intersection_point
            else:
                return False, None
        else:
            return False, None

    def check_schnittpunkt(self, start, end, intersection):
        a = intersection-start
        b = end - start
        dot_product = np.dot(a, b)
        norm_a = np.linalg.norm(a)
        norm_b = np.linalg.norm(b)
        similarity = dot_product / (norm_a * norm_b)
        if norm_a < norm_b and similarity > 0.95:
            return True
        else:
            return False

    def line_rect_intersection(self, start, end, rect_p1, rect_p2):
        rect_l_u = rect_p1
        rect_r_o = rect_p2
        rect_l_o = np.array([rect_p1[0], rect_p2[1]])
        rect_r_u = np.array([rect_p2[0], rect_p1[1]])

        min_x = rect_p1[0]
        max_x = rect_p2[0]
        min_y = rect_p1[1]
        max_y = rect_p2[1]

        intersection = []
        return_point_start = []
        return_point_end = []

        if min_x <= start[0] <= max_x and min_y <= start[1] <= max_y:
            # Startpunkt liegt innerhalb
            return_point_start = start
        if min_x <= end[0] <= max_x and min_y <= end[1] <= max_y:
            # Startpunkt liegt innerhalb
            return_point_end = end

        if return_point_start == [] or return_point_end == []:
            intersection = []
            ckeck, buffer = self.prf_schnittp(start, end, rect_l_u, rect_l_o)
            if ckeck:
                intersection.append(buffer)
                buffer = None
            ckeck, buffer = self.prf_schnittp(start, end, rect_l_o, rect_r_o)
            if ckeck:
                intersection.append(buffer)
                buffer = None
            ckeck, buffer = self.prf_schnittp(start, end, rect_r_o, rect_r_u)
            if ckeck:
                intersection.append(buffer)
                buffer = None
            ckeck, buffer = self.prf_schnittp(start, end, rect_r_u, rect_l_u)
            if ckeck:
                intersection.append(buffer)
                buffer = None

        if len(return_point_start) > 0 and len(return_point_end) > 0:
            # Linie innerhalb
            return True, return_point_start, return_point_end
        elif len(return_point_start) > 0 and len(intersection) == 1:
            # Ein Punkt innerhalb
            return True, return_point_start, intersection[0]
        elif len(return_point_end) > 0 and len(intersection) == 1:
            # Ein Punkt innerhalb
            return True, intersection[0], return_point_end
        elif len(intersection) == 2:
            if np.linalg.norm(intersection[0]-start) < np.linalg.norm(intersection[1]-start):
                return True, intersection[0], intersection[1]
            else:
                return True, intersection[1], intersection[0]
        else:
            return False, None, None


class MatsimEventParser(handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self, scenario, populationids):
        self.scenario = scenario
        self.populationids = populationids
        self.entry_times = {}
        self.key_dict_matsim = {}
        self.key_dict_matsim_sumo = {}

        for id in self.scenario.demand.matsim.mapmatch.ids_matsim:
            external = self.scenario.demand.matsim.net.edges.get_row(id)['ids_matsim']
            self.key_dict_matsim[external] = id
            id_mapmatch = self.scenario.demand.matsim.mapmatch.ids_matsim.get_id_from_index(id)
            sumo_value = self.scenario.demand.matsim.mapmatch.get_row(id_mapmatch)['ids_sumo']
            self.key_dict_matsim_sumo[id] = sumo_value

    def startElement(self, name, attrs):

        if name == 'event':
            if attrs['type'] == 'entered link':
                if attrs['link'] in self.key_dict_matsim and str(attrs['vehicle']) in self.populationids:
                    edge_id_hybridPY = self.key_dict_matsim[attrs['link']]

                    if not isinstance(self.key_dict_matsim_sumo[edge_id_hybridPY], list):
                        temp = self.key_dict_matsim_sumo[edge_id_hybridPY]
                    else:
                        temp = self.key_dict_matsim_sumo[edge_id_hybridPY][0]

                    if str(attrs['vehicle']) + '-' + str(temp) in self.entry_times:

                        self.entry_times[str(attrs['vehicle']) + '-' + str(temp)].append(int(float(attrs['time'])))
                    else:
                        self.entry_times[str(attrs['vehicle']) + '-' + str(temp)] = [int(float(attrs['time']))]
