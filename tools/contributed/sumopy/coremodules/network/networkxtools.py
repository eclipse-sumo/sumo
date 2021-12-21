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

# @file    networkxtools.py
# @author  Joerg Schweizer
# @date   2012

from coremodules.network.network import MODES, ID_MODE_PED, ID_MODE_BIKE, ID_MODE_CAR
from coremodules.network.netconvert import *
from agilepy.lib_base.geometry import get_length_polypoints, get_dist_point_to_segs, get_diff_angle_clockwise
import pickle
import os
import json
from coremodules.misc.shapeformat import guess_utm_from_coord
from agilepy.lib_base.misc import filepathstring_to_filepathlist, filepathlist_to_filepathstring
from agilepy.lib_base.processes import Process
import agilepy.lib_base.xmlman as xm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.classman as cm
import numpy as np
from collections import OrderedDict
from copy import deepcopy
import sys
reload(sys)
sys.setdefaultencoding('utf8')

try:
    import networkx as nx
    IS_NX = True
except:
    IS_NX = False


try:
    import pyproj
except:
    from mpl_toolkits.basemap import pyproj


LANEATTRS_DEFAULT = {'is_rightside': False, 'is_leftside': False, 'n_lane': 0,
                     'ids_modes_disallow': [], 'ids_modes_allow': [],
                     'is_sidewalk': False,
                     }

LANEATTRS = ('lane', 'track')

YES_OR_DESIGNATED = ('yes', 'designated')


def load_objfile(filepath):
    f = open(filepath, 'rb')
    try:
        f = open(filepath, 'rb')
    except:
        print 'WARNING in load_obj: could not open', filepath
        return None

    # try:
    # print '  pickle.load...'
    obj = pickle.load(f)
    f.close()
    return obj


def print_attrs(attrs):
    for key, val in attrs.iteritems():
        print '    %s=\t%s' % (key, val)
    print


def get_loop(digraph, ids_node, ids_nodepairs=[], n_node_max=4):
    print 'get_loop ids_node', ids_node
    if (ids_node[-1] == ids_node[0]):
        print '    success.'
        return ids_nodepairs
    elif (len(ids_node) == n_node_max):
        print '    no loop reached.'
        return []
    else:
        id_node = ids_node[-1]
        node = digraph.node[id_node]
        node_to_dirind = node['node_to_dirind']
        id_nbnode = ids_node[-2]
        # print '    node_to_dirind.keys()',node_to_dirind.keys(),node_to_dirind.has_key(id_nbnode)

        if node_to_dirind.has_key(id_nbnode):
            ind = node['node_to_dirind'][id_nbnode]
        else:
            ind = node['node_to_dirind'][-id_nbnode]

        if ind+1 == len(node['node_to_dirind']):
            ind = 0
        else:
            ind += 1

        id_nbnode_new = node['dirind_to_node'][ind]
        if id_nbnode == id_nbnode_new:
            print '  simple bidir return link'
            return []

        elif abs(id_nbnode_new) in ids_node[1:]:
            print '  complex bidir return link'
            return []

        if id_nbnode_new < 0:
            # print '    append',-id_nbnode_new
            # entering neighbour
            ids_node.append(-id_nbnode_new)
            ids_nodepairs.append((-id_nbnode_new, id_node))
        else:
            # exiting neighbour
            # print '    append',id_nbnode_new
            ids_node.append(id_nbnode_new)
            ids_nodepairs.append((id_node, id_nbnode_new))

        return get_loop(digraph, ids_node, ids_nodepairs, n_node_max)


class Road:
    def __init__(self, element, parent):
        # edges_osm[id_way]= {}#{'nodes':(element['nodes'][0],element['nodes'][1])}# not needed {'nodes': element['nodes']}

        self.parent = parent

        self._osmattrs = element.get('tags', {})
        self.ids_osmnode = element.get('nodes', [])
        self.highway = self._osmattrs.get('highway', 'road')
        # if element.has_key('tags'):
        #    self._attrs.update(element['tags'])

        # is_one is a scpecial one way detection
        #self._is_oneway = self.is_oneway_osm()
        # self.is_oneway_osm()

        #self._id_osm = id_osm
        #osmattrs = self._osmattrs
        # if not self._is_oneway:
        #    self._id_osm_opp = -id_osm
        # else:
        #    self._id_osm_opp = -1

        self._lanes = []
        self._lanes_opp = []
        self.make_oneway()
        self.config()
        self.make_lanestructure()
        self.make_sumoattrs()
        self.make_road()
        # get from parent
        # self._width_bikelane = 1.5 #m
        # self.parent.width_sidewalk = 1.0 #m
        #self._modes_reserved_lane = ['bus', 'taxi','emergency']

    def config(self):
        """
        Make defaults to be overridden
        """
        print 'config'
        # pass here either road type specific defaults
        # or global defaults from parent process

        self.n_lane = self.parent.n_lane
        self.id_mode = ID_MODE_CAR
        self.lanewidth = self.parent.lanewidth
        self.lanewidth_bike = self.parent.lanewidth_bike
        self.lanewidth_bus = self.parent.lanewidth_bus
        self.width_sidewalk = self.parent.width_sidewalk
        #self.width_footpath = self.parent.width_footpath

        self.ids_modes_pt_shared = self.parent.ids_modes_pt_shared
        self.ids_modes_bike = self.parent.ids_modes_bike
        self.ids_modes_moped = self.parent.ids_modes_moped
        self.ids_modes_allow = []
        self.ids_modes_disallow = []

        self.make_speed(self.parent.speed_max)
        self.speed_max_bus = self.parent.speed_max_bus
        self.speed_max_bike = self.parent.speed_max_bike
        self.speed_max_ped = self.parent.speed_max_ped

    def make_laneattrs_main(self):
        self._laneattrs_main = {'speed_max': self.speed_max,
                                'width': self.lanewidth,
                                'ids_modes_allow': self.ids_modes_allow,
                                'ids_modes_disallow': self.ids_modes_disallow,
                                # 'ids_mode': MODES["passenger"],
                                'is_rightside': True,
                                'is_leftside': False,
                                'width_rightside': self.lanewidth,
                                'is_sidewalk': False,
                                }

    def make_laneattrs_main_accessrestrict(self):
        """Adds access restrictions to main lanes attributes
        if specified in OSM attributes.

        Often residential roads have mode restrictions
        on main lane.
        """
        osmattrs = self._osmattrs
        # give access to particular vehicle on main lane(s)
        if osmattrs.get('bicycle', 'no') in YES_OR_DESIGNATED:
            # if segregated, a separate bikeway will be created
            # in make_bikewayattrs
            if not osmattrs.get('segregated', '') == 'yes':
                # add bikeaccess
                ids_modes_allow = self._laneattrs_main['ids_modes_allow']

                ids_allowed_set = deepcopy(self.ids_modes_bike)
                for id_mode in ids_modes_allow:
                    if id_mode not in ids_allowed_set:
                        ids_allowed_set.append(id_mode)

                self._laneattrs_main['ids_modes_allow'] = ids_allowed_set
                self._laneattrs_main['ids_modes_disallow'] = []

        if osmattrs.get('psv', 'no') in YES_OR_DESIGNATED:
            # add public transport access
            ids_modes_allow = self._laneattrs_main['ids_modes_allow']
            ids_allowed_set = deepcopy(self.ids_modes_pt_shared)
            for id_mode in ids_modes_allow:
                if id_mode not in ids_allowed_set:
                    ids_allowed_set.append(id_mode)

            self._laneattrs_main['ids_modes_allow'] = ids_allowed_set
            self._laneattrs_main['ids_modes_disallow'] = []

        if osmattrs.get('moped', 'no') in YES_OR_DESIGNATED:
            # add public transport access
            ids_modes_allow = self._laneattrs_main['ids_modes_allow']
            ids_allowed_set = deepcopy(self.ids_modes_moped)
            for id_mode in ids_modes_allow:
                if id_mode not in ids_allowed_set:
                    ids_allowed_set.append(id_mode)

            self._laneattrs_main['ids_modes_allow'] = ids_allowed_set
            self._laneattrs_main['ids_modes_disallow'] = []

    def get_osmattr(self, key, is_int=False, is_float=False, default=None):
        valstr = self._osmattrs.get(key, '')
        if valstr.find(',') > -1:
            v = valstr.split(',')[0]
        elif valstr.find(';') > -1:
            v = valstr.split(';')[0]
        else:
            v = valstr
        if is_int:
            try:
                return int(v)
            except:
                return default
        if is_float:
            try:
                return float(v)
            except:
                return default

        else:
            return v

    def make_sumoattrs(self):

        osmattrs = self._osmattrs
        print 'make_sumoattrs'
        print_attrs(osmattrs)
        #self._highway = osmattrs.get('highway','road')

        self.make_laneattrs_main()
        self.make_sidewalkattrs()
        self.make_buswayattrs()
        self.make_bikewayattrs()

    def make_lanestructure(self):
        osmattrs = self._osmattrs

        # use the one way declarations by osm
        # for interpreting the correct number of lanes
        is_oneway_osm = (osmattrs.get('oneway', 'no') == 'yes') | (osmattrs.get('junction', '') == 'roundabout')

        n_lane_forward_osm = -1
        n_lane_backward_osm = -1
        is_lanes_forward_rigid = False
        is_lanes_backward_rigid = False
        if osmattrs.has_key('lanes'):
            # print '  total number of lanes provided n_lane_osm_str',osmattrs['lanes']
            n_lane_osm_str = osmattrs['lanes']
            # if type(n_lane_osm_str) in cm.STRINGTYPES:
            if n_lane_osm_str.count(';') > 0:
                n_lane_osm = 1
                for n_lane_osm_elem in n_lane_osm_str.split(';'):
                    if int(n_lane_osm_elem) > n_lane_osm:
                        n_lane_osm = int(n_lane_osm_elem)
            elif n_lane_osm_str.count(',') > 0:
                n_lane_osm = 1
                for n_lane_osm_elem in n_lane_osm_str.split(','):
                    if int(n_lane_osm_elem) > n_lane_osm:
                        n_lane_osm = int(n_lane_osm_elem)
            else:
                n_lane_osm = int(osmattrs['lanes'])

            if is_oneway_osm:
                # in cas of oneway
                n_lane_backward_osm = 0
                n_lane_forward_osm = n_lane_osm
                is_lanes_forward_rigid = True

            else:
                # in case of bidir
                if osmattrs.has_key('lanes:forward'):
                    n_lane_forward_osm = int(osmattrs['lanes:forward'])
                    is_lanes_forward_rigid = True
                    if osmattrs.has_key('lanes:backward'):
                        n_lane_backward_osm = int(osmattrs['lanes:backward'])
                        is_lanes_backward_rigid = True
                    else:
                        n_lane_backward_osm = n_lane_osm-n_lane_forward_osm
                        # correct if necessary
                        if n_lane_backward_osm <= 0:
                            n_lane_backward_osm = 1
                        n_lane_osm = n_lane_forward_osm+n_lane_backward_osm

                elif osmattrs.has_key('lanes:backward'):
                    n_lane_backward_osm = int(osmattrs['lanes:backward'])
                    n_lane_forward_osm = n_lane_osm-n_lane_backward_osm
                    is_lanes_backward_rigid = True

                    # correct if necessary
                    if n_lane_forward_osm <= 0:
                        n_lane_forward_osm = 1
                    n_lane_osm = n_lane_forward_osm+n_lane_backward_osm

                else:
                    # symmetric dist
                    if n_lane_osm < 2:
                        n_lane_osm = 2

                    n_lane_forward_osm = int(0.5*n_lane_osm+0.5)
                    n_lane_backward_osm = n_lane_forward_osm
        else:
            # no information on total of lanes
            if is_oneway_osm:
                # in cas of (declared) oneway
                n_lane_backward_osm = 0
                if osmattrs.has_key('lanes:forward'):
                    n_lane_forward_osm = int(osmattrs['lanes:forward'])
                    is_lanes_forward_rigid = True
                else:
                    n_lane_forward_osm = self.n_lane  # default
            else:
                # bidir
                if osmattrs.has_key('lanes:forward'):
                    n_lane_forward_osm = int(osmattrs['lanes:forward'])
                    is_lanes_forward_rigid = True
                    if osmattrs.has_key('lanes:backward'):
                        n_lane_backward_osm = int(osmattrs['lanes:backward'])
                        is_lanes_backward_rigid = True
                    else:
                        n_lane_backward_osm = self.n_lane  # default

                elif osmattrs.has_key('lanes:backward'):
                    n_lane_backward_osm = int(osmattrs['lanes:backward'])
                    is_lanes_backward_rigid = True
                    n_lane_forward_osm = self.n_lane  # default

                else:
                    # no lane information
                    n_lane_forward_osm = self.n_lane  # default
                    n_lane_backward_osm = self.n_lane  # default

            n_lane_osm = n_lane_forward_osm+n_lane_backward_osm

        self._is_lanes_backward_rigid = is_lanes_backward_rigid
        self._is_lanes_forward_rigid = is_lanes_forward_rigid

        self._n_lane_forward_osm = n_lane_forward_osm
        self._n_lane_backward_osm = n_lane_backward_osm
        self._n_lane_osm = n_lane_osm
        print '  lane numbers: n_lane_forward_osm=%d, n_lane_backward_osm=%d n_default=%d' % (n_lane_forward_osm, n_lane_backward_osm, self.n_lane), 'rigid fb', is_lanes_forward_rigid, is_lanes_backward_rigid

    def _get_access(self, access_str):
        access_data = np.array(access_str.split('|'), dtype=np.object)[::-1]
        return access_data == 'designated', access_data == 'no'

    def get_laneaccess(self, vname, is_opp=False):
        if is_opp:
            if self._is_lanes_backward_rigid:
                osmattrs = self._osmattrs
                access_str = osmattrs.get(vname+':lanes', '')
                if access_str != '':
                    allowed, disallowed = self._get_access(access_str)
                    return allowed[:self._n_lane_backward_osm], disallowed[:self._n_lane_backward_osm]

                else:
                    access_str = osmattrs.get(vname+':lanes:backward', '')
                    if access_str != '':
                        allowed, disallowed = self._get_access(access_str)
                        return allowed, disallowed

                    else:
                        return [], []

            else:
                return [], []

        else:
            if self._is_lanes_forward_rigid:
                osmattrs = self._osmattrs
                access_str = osmattrs.get(vname+':lanes', '')
                if access_str != '':
                    allowed, disallowed = self._get_access(access_str)
                    return allowed[self._n_lane_backward_osm:], disallowed[self._n_lane_backward_osm:]

                else:
                    access_str = osmattrs.get(vname+':lanes:forward', '')
                    if access_str != '':
                        allowed, disallowed = self._get_access(access_str)
                        return allowed, disallowed

                    else:
                        return [], []

            else:
                return [], []

    def get_lanes(self, is_opp=False):
        if is_opp:
            return self._lanes
        else:
            return self._lanes_opp

    def make_road(self):
        """
        Makes road lanes in both directions
        """

        #lanes = []
        #lanes_opp = []
        print '  realoneway', self.is_oneway()

        #self._lanes = []
        #self._lanes_opp = []

        print '  Main Dir ^^^^^^^^^^^', self._is_lanes_forward_rigid, self._is_lanes_backward_rigid
        if self._is_lanes_forward_rigid:
            self.make_lanes_rigid(is_opp=False, n_lane_osm=self._n_lane_forward_osm)
        else:
            self.make_lanes(is_opp=False, n_lane_osm=self._n_lane_forward_osm)

        if not self.is_oneway():

            print '  Opp  Dir vvvvvvvvvvv'
            if self._is_lanes_backward_rigid:
                self.make_lanes_rigid(is_opp=True, n_lane_osm=self._n_lane_backward_osm)
            else:
                self.make_lanes(is_opp=True, n_lane_osm=self._n_lane_backward_osm)

        print '  id', id(self)
        print '  len(self._lanes)', len(self._lanes)
        print '  len(self._lanes_opp)', len(self._lanes_opp)

    def _get_speedinfo(self, speed_max_str):
        speed_max_data = speed_max_str.split(' ')
        if len(speed_max_data) == 1:
            #speed_max_data = speed_max_data[0].split(';')
            # TODO: here we could do a per lane assignment
            return float(speed_max_data[0])/3.6
        else:

            # todo
            # maxspeed=60
            # maxspeed=50 mph
            # maxspeed=10 knots
            unit = speed_max_data[1]
            if unit == 'mph':
                return float(speed_max_data[0])/3.6*1.609344

            elif unit == 'knots':
                return float(speed_max_data[0])/3.6*1.852

    def make_speed(self, speed_max_default):
        # estimate speed max in m/s
        if self._osmattrs.has_key('maxspeed'):
            speed_max_str = self._osmattrs['maxspeed']
            # print 'make_speed speed_max_str',speed_max_str
            if speed_max_str.count(';') > 0:
                speed_max = np.inf
                for v_str in speed_max_str.split(';'):
                    v = self._get_speedinfo(v_str)
                    if v < speed_max:
                        speed_max = v

            else:
                if speed_max_str.isdigit():
                    speed_max = self._get_speedinfo(speed_max_str)
                else:
                    speed_max = speed_max_default
        else:
            speed_max = speed_max_default

        self.speed_max = speed_max

    def _is_opposite(self, osmattrs, tag):
        if osmattrs.has_key(tag):
            elems = osmattrs[tag].split('_')
            return elems[0] == 'opposite'
        else:
            return False

    def is_oneway(self):
        return self._is_oneway

    def make_oneway(self):
        # print 'make_oneway'
        osmattrs = self._osmattrs

        if osmattrs.get('junction', '') == 'roundabout':
            self._is_oneway = True

        elif osmattrs.has_key('oneway'):
            if osmattrs['oneway'] == 'no':
                self._is_oneway = False

            else:
                if self._is_opposite(osmattrs, 'busway'):
                    self._is_oneway = False

                elif self._is_opposite(osmattrs, 'busway:right'):
                    self._is_oneway = False

                elif osmattrs.has_key('lanes:bus:backward'):
                    self._is_oneway = False

                elif osmattrs.has_key('trolley_wire:both'):
                    self._is_oneway = False

                elif osmattrs.has_key('lanes:psv:backward'):
                    self._is_oneway = False

                elif self._is_opposite(osmattrs, 'cycleway'):
                    self._is_oneway = False

                elif self._is_opposite(osmattrs, 'busway'):
                    self._is_oneway = False

                elif osmattrs.get('cycleway', '') == 'both':
                    self._is_oneway = False

                # elif osmattrs.get('busway','') == 'both':
                #    self._is_oneway = False

                # sidewalks on both sides go in the same direction
                # elif osmattrs.get('sidewalk','') == 'both':
                #    self._is_oneway = False

                elif osmattrs.has_key('oneway:bicycle'):
                    if osmattrs['oneway:bicycle'] == 'no':
                        self._is_oneway = False

                    else:
                        self._is_oneway = True

                else:
                    self._is_oneway = True

        else:
            self._is_oneway = False

    # def remove_sidewalk(self, is_opp = True):
    #    """
    #    Not in use!
    #    """
    #    if is_opp:
    #        ind = 0
    #    else:
    #        ind = 1
    #    print 'remove_sidewalk',is_opp,self._sidewalkattrs[ind]['n_lane'],self._sidewalkattrs[ind]['n_lane']==0
    #    if  self._sidewalkattrs[ind]['n_lane']>0:
    #        self._sidewalkattrs[ind]['n_lane']=0
    #        self.make_road()

    def has_sidewalks(self, is_opp=True):
        if is_opp:
            ind = 0
        else:
            ind = 1
        return self._sidewalkattrs[ind]['n_lane'] > 0

    def add_sidewalk(self, road, is_opp=False):
        """
        Method to explicitely add sidewalks with the given road attributes
        """
        osmattrs = self._osmattrs

        if is_opp:
            ind = 0
        else:
            ind = 1

        print 'add_sidewalk', is_opp, self._sidewalkattrs[ind]['n_lane'], self._sidewalkattrs[ind]['n_lane'] == 0
        if self._sidewalkattrs[ind]['n_lane'] == 0:
            self.make_sidewalk(is_opp=is_opp, is_rightside=True, width=road.lanewidth)
            self.make_road()

        # if self._is_oneway:
        #    # generate sidewalks in both directions
        #    ind = 1
        #    if self._sidewalkattrs[ind]['n_lane'] == 1:
        #        self.make_sidewalk(is_opp = False, is_rightside = False, width = road.lanewidth)

    def make_sidewalk(self, is_opp=False, is_rightside=True,
                      ids_modes_allow=[], width=1.0, n_lane=1):

        if is_opp:
            ind = 0
        else:
            ind = 1
        #  !!!!foot=use_sidepath may be applied on the street
        #  to mark that sidewalks are mapped separately.
        # Also, sidewalk=separate

        print 'make_sidewalk', ind, is_rightside
        # needed to merge access?
        self._sidewalkattrs[ind]['is_share'] = len(ids_modes_allow) > 0

        ids_allowed_set = [ID_MODE_PED]
        for id_mode in ids_modes_allow:
            if id_mode not in ids_allowed_set:
                ids_allowed_set.append(id_mode)

        self._sidewalkattrs[ind]['is_sidewalk'] = True
        self._sidewalkattrs[ind]['ids_modes_allow'] = list(ids_allowed_set)
        self._sidewalkattrs[ind]['ids_modes_disallow'] = []
        if is_rightside:
            self._sidewalkattrs[ind]['is_rightside'] = True
            self._sidewalkattrs[ind]['width_rightside'] = width
            self._sidewalkattrs[ind]['n_lane'] += n_lane
            # self._sidewalkattrs[ind]['is_leftside'] = False # no!?
        else:
            self._sidewalkattrs[ind]['is_leftside'] = True
            self._sidewalkattrs[ind]['width_leftside'] = width
            self._sidewalkattrs[ind]['n_lane'] += n_lane
            # self._sidewalkattrs[ind]['is_rightside'] = False # no!?

        self._sidewalkattrs[ind]['speed_max'] = self.speed_max_ped
        # print '  sidewalkattr=',self._sidewalkattrs[ind]
        print_attrs(self._sidewalkattrs[ind])

    def make_sidewalkattrs(self):
        print 'make_sidewalkattrs'

        if self.get_priority() < 7:
            return self.make_sidewalkattrs_smallroads()
        else:
            return self.make_sidewalkattrs_largeroads()

    def make_sidewalkattrs_largeroads(self):
        print 'make_sidewalkattrs_largeroads'

        osmattrs = self._osmattrs
        self._sidewalkattrs = (deepcopy(LANEATTRS_DEFAULT),
                               deepcopy(LANEATTRS_DEFAULT))

        self._sidewalkattrs[0]['is_sidewalk'] = True
        self._sidewalkattrs[1]['is_sidewalk'] = True
        sidewalk = osmattrs.get('sidewalk', 'auto')

        if sidewalk not in ['no', 'none', 'auto']:
            # there is a sidewalk, make sure there is at least one

            if osmattrs.has_key('sidewalk:width'):
                width_left = float(osmattrs['sidewalk:width'])
                width_right = width_left

            elif osmattrs.has_key('sidewalk:both:width'):
                width_left = float(osmattrs['sidewalk:both:width'])
                width_right = width_left
            else:
                width_left = float(osmattrs.get('sidewalk:left:width', self.width_sidewalk))
                width_right = float(osmattrs.get('sidewalk:right:width', self.width_sidewalk))

            if osmattrs.get('oneway', 'no') == 'no':
                # bidirecional

                if sidewalk == 'both':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)
                    self.make_sidewalk(is_opp=True, is_rightside=True, width=width_left)

                elif sidewalk == 'left':
                    self.make_sidewalk(is_opp=True, is_rightside=True, width=width_left)

                elif sidewalk == 'right':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)

                # elif osmattrs.get('junction','') == 'roundabout':
                #    self.make_sidewalk(is_opp = False, is_rightside = True, width = width_right)

                else:
                    # no indication means both
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)
                    self.make_sidewalk(is_opp=True, is_rightside=True, width=width_left)

            else:
                # oneway
                if sidewalk == 'both':
                    # both sidewalks go in the same direction
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)
                    self.make_sidewalk(is_opp=False, is_rightside=False, width=width_left)

                elif sidewalk == 'left':
                    self.make_sidewalk(is_opp=False, is_rightside=False, width=width_left)

                elif sidewalk == 'right':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)

                # no indication, no sidewalks
                # else:
                #    # no indication means both
                #    self.make_sidewalk(is_opp = False, is_rightside = True, width = width_right)
                #    self.make_sidewalk(is_opp = False, is_rightside = False, width = width_left)

        else:
            # sidewalk = 'auto'
            # no indication, no sidewalks
            pass

        # if no siedewalk width_sidewalk = -1
        #self.width_sidewalk = max(widths)
        # print '  sidewalkattr='#,self._sidewalkattrs
        # for attrs in self._sidewalkattrs:
        #    if attrs['n_lane']>0:
        #        print_attrs(attrs)

    def make_sidewalkattrs_smallroads(self):
        print 'make_sidewalkattrs_smallroads'
        osmattrs = self._osmattrs
        self._sidewalkattrs = (deepcopy(LANEATTRS_DEFAULT),
                               deepcopy(LANEATTRS_DEFAULT))

        self._sidewalkattrs[0]['is_sidewalk'] = True
        self._sidewalkattrs[1]['is_sidewalk'] = True
        sidewalk = osmattrs.get('sidewalk', 'auto')

        if sidewalk not in ['no', 'none', 'auto']:
            # there is a sidewalk, make sure there is at least one

            if osmattrs.has_key('sidewalk:width'):
                width_left = float(osmattrs['sidewalk:width'])
                width_right = width_left

            elif osmattrs.has_key('sidewalk:both:width'):
                width_left = float(osmattrs['sidewalk:both:width'])
                width_right = width_left
            else:
                width_left = float(osmattrs.get('sidewalk:left:width', self.width_sidewalk))
                width_right = float(osmattrs.get('sidewalk:right:width', self.width_sidewalk))

            if osmattrs.get('oneway', 'no') == 'no':
                # bidirecional

                if sidewalk == 'both':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)
                    self.make_sidewalk(is_opp=True, is_rightside=True, width=width_left)

                elif sidewalk == 'left':
                    self.make_sidewalk(is_opp=True, is_rightside=True, width=width_left)

                elif sidewalk == 'right':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)

                else:
                    # no indication means both
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)
                    self.make_sidewalk(is_opp=True, is_rightside=True, width=width_left)

            else:
                # oneway
                if sidewalk == 'both':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)
                    self.make_sidewalk(is_opp=False, is_rightside=False, width=width_left)

                elif sidewalk == 'left':
                    self.make_sidewalk(is_opp=False, is_rightside=False, width=width_left)

                elif sidewalk == 'right':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)

                elif osmattrs.get('junction', '') == 'roundabout':
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)

                else:
                    # no indication means both
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=width_right)
                    self.make_sidewalk(is_opp=False, is_rightside=False, width=width_left)

        elif sidewalk == 'auto':
            # automatic sidewalk generation for residential roads
            if self.is_oneway():

                if osmattrs.get('junction', '') == 'roundabout':
                    print '  put a sidewalk around roundabouts, not inside'
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=self.width_sidewalk)
                else:
                    print '  put a sidewalk on both sides of the oneway'
                    self.make_sidewalk(is_opp=False, is_rightside=True, width=self.width_sidewalk)
                    self.make_sidewalk(is_opp=False, is_rightside=False, width=self.width_sidewalk)
            else:
                print '    put a sidewalk on both sides of the road'
                self.make_sidewalk(is_opp=False, is_rightside=True, width=self.width_sidewalk)
                self.make_sidewalk(is_opp=True, is_rightside=True, width=self.width_sidewalk)

    def make_bikelane(self, is_opp=False, is_rightside=True,
                      ids_modes_allow=[], width=1.5, n_lane=1):
        print 'make_bikelane', is_opp, is_rightside, ids_modes_allow
        if is_opp:
            ind = 0
        else:
            ind = 1

        # needed to merge access?
        self._bikewayattrs[ind]['is_share'] = len(ids_modes_allow) > 0

        ids_allowed_set = deepcopy(self.ids_modes_bike)
        for id_mode in ids_modes_allow:
            if id_mode not in ids_allowed_set:
                ids_allowed_set.append(id_mode)

        self._bikewayattrs[ind]['ids_modes_allow'] = list(ids_allowed_set)
        self._bikewayattrs[ind]['ids_modes_disallow'] = []
        if is_rightside:
            self._bikewayattrs[ind]['is_rightside'] = True
            self._bikewayattrs[ind]['width_rightside'] = width
            self._bikewayattrs[ind]['n_lane'] += n_lane
        else:
            self._bikewayattrs[ind]['is_leftside'] = True
            self._bikewayattrs[ind]['width_leftside'] = width
            self._bikewayattrs[ind]['n_lane'] += n_lane

        self._bikewayattrs[ind]['speed_max'] = self.speed_max_bike

        print_attrs(self._bikewayattrs[ind])

    def make_bikewayattrs(self):
        """ Returns a tuple with attribute dictionaries for bikeways 
            on left and right side of the road.  
                Dictionary with the following keys: 
                widths:  is a tuple with sidewalk widths of left and right sidewalk.
                if values are less than 0 means no sidewalk.
        """
        print 'make_bikewayattrs'
        self._bikewayattrs = (deepcopy(LANEATTRS_DEFAULT),
                              deepcopy(LANEATTRS_DEFAULT))

        osmattrs = self._osmattrs
        cycleway = osmattrs.get('cycleway', '')
        cycleway_left = osmattrs.get('cycleway:left', '')
        cycleway_right = osmattrs.get('cycleway:right', '')

        if osmattrs.has_key('cycleway:width'):
            width_left = float(osmattrs['cycleway:width'])
            width_right = width_left

        elif osmattrs.has_key('cycleway:both:width'):
            width_left = float(osmattrs['cycleway:both:width'])
            width_right = width_left

        else:
            width_left = float(osmattrs.get('cycleway:left:width', self.lanewidth_bike))
            width_right = float(osmattrs.get('cycleway:right:width', self.lanewidth_bike))

        if osmattrs.get('oneway', 'no') == 'no':
            # cycle lanes on bidir road

            if cycleway in LANEATTRS:
                self.make_bikelane(is_opp=False, width=width_right)
                self.make_bikelane(is_opp=True, width=width_left)
                return

            elif cycleway in OSMKEYS_SHARED:
                self.make_bikelane(is_opp=False, ids_modes_allow=self.ids_modes_pt_shared, width=width_right)
                self.make_bikelane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared, width=width_left)
                return

            elif cycleway in OSMKEYS_SHARED_OPP:
                self.make_bikelane(is_opp=False, width=width_right)
                self.make_bikelane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared, width=width_left)
                return

            cycleway = osmattrs.get('cycleway:both', '')
            if cycleway in LANEATTRS:
                self.make_bikelane(is_opp=False, width=width_right)
                self.make_bikelane(is_opp=True, width=width_left)
                return

            elif cycleway in OSMKEYS_SHARED:
                self.make_bikelane(is_opp=False, ids_modes_allow=self.ids_modes_pt_shared, width=width_right)
                self.make_bikelane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared, width=width_right)
                return

            #cycleway = osmattrs.get('cycleway:left','')
            if (cycleway_left != '') & (cycleway_right != ''):
                # also both sides
                if cycleway_left in LANEATTRS:
                    self.make_bikelane(is_opp=True, is_rightside=False, width=width_left)
                elif cycleway_left in OSMKEYS_SHARED:
                    self.make_bikelane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared)

                if cycleway_right in LANEATTRS:
                    self.make_bikelane(is_opp=False, is_rightside=False, width=width_left)
                elif cycleway_right in OSMKEYS_SHARED:
                    self.make_bikelane(is_opp=False, ids_modes_allow=self.ids_modes_pt_shared)

            # check left only
            elif cycleway_left in LANEATTRS:
                self.make_bikelane(is_opp=True, is_rightside=False, width=width_left)
                return

            elif cycleway_left in OSMKEYS_SHARED:
                self.make_bikelane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared)
                # if osmattrs.get('bicycle','no') == 'yes':
                #    self.make_bikelane(is_opp = False, is_rightside = True,width = width_right)

            # check right only
            elif cycleway_right in LANEATTRS:
                self.make_bikelane(is_opp=False, is_rightside=True)

            elif cycleway_right == 'opposite_share_busway':
                self.make_bikelane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared, width=width_left)
                # if osmattrs.get('bicycle','no') == 'yes':
                #    self.make_bikelane(is_opp = False, is_rightside = True,width = width_right)

            # general bicycle tag if all from abive fails but share with pedestrians
            # Moved to main lane attr except if segragated
            elif (osmattrs.get('bicycle', 'no') in YES_OR_DESIGNATED) & (osmattrs.get('segregated', '') == 'yes'):
                print '  check if there are cycle lanes already', self._bikewayattrs[0]['n_lane'] == 0, self._bikewayattrs[1]['n_lane'] == 0

                if (self._bikewayattrs[0]['n_lane'] == 0) & (self._bikewayattrs[1]['n_lane'] == 0):
                    # no bikelanes in both dir
                    self.make_bikelane(is_opp=False, is_rightside=True, width=width_right)
                    self.make_bikelane(is_opp=True, is_rightside=True, width=width_left)

        else:
            # cycle lanes on oneway road

            if cycleway in LANEATTRS:

                self.make_bikelane(is_opp=False)

                if osmattrs.get('oneway:bicycle', 'yes') in YES_OR_DESIGNATED:
                    self.make_bikelane(is_opp=True, width=width_left)
                # else:
                #    self.make_bikelane(is_opp = False, width = width_right)
                return

            elif cycleway == 'opposite':
                self.make_bikelane(is_opp=True, is_rightside=True, width=width_left)
                # if osmattrs.get('bicycle','no') == 'yes':
                #    self.make_bikelane(is_opp = False, is_rightside = True, width = width_right)
                return

            elif cycleway == 'opposite_share_busway':
                self.make_bikelane(is_opp=True, is_rightside=True,
                                   ids_modes_allow=self.ids_modes_pt_shared, width=width_right)
                # if osmattrs.get('bicycle','no') == 'yes':
                #    self.make_bikelane(is_opp = False, is_rightside = True,width = width_right)
                return

            if cycleway == 'both':
                # attention with  this key a cycle lane in the opposite direction
                # will be created as it makes no sense to have bikelanes
                # in the smae direction on both sides
                self.make_bikelane(is_opp=False, is_rightside=True, width=width_right)
                self.make_bikelane(is_opp=True, is_rightside=True, width=width_left)

            elif cycleway_right in LANEATTRS:
                self.make_bikelane(is_opp=False, width=width_right)

            elif cycleway_left in LANEATTRS:
                self.make_bikelane(is_opp=False, is_rightside=False, width=width_right)

            elif cycleway_left == 'opposite_lane':
                self.make_bikelane(is_opp=True, is_rightside=True, width=width_left)
                # if osmattrs.get('bicycle','no') == 'yes':
                #    self.make_bikelane(is_opp = False, is_rightside = True,width = width_right)

            elif osmattrs.get('oneway:bicycle', '') == 'no':
                self.make_bikelane(is_opp=True, is_rightside=False, width=width_right)
                # if (osmattrs.get('bicycle','no') == 'yes'):
                #    # check if there are cycle lanes already
                #    if (self._bikewayattrs[1]['n_lane'] == 0):
                #        self.make_bikelane(is_opp = False, is_rightside = True, width = width_right)

            # Moved to main lane attr except segregated
            elif (osmattrs.get('bicycle', 'no') in YES_OR_DESIGNATED) & (osmattrs.get('segregated', '') == 'yes'):

                print '  check if there are cycle lanes already', self._bikewayattrs[1]['n_lane'] == 0
                if (self._bikewayattrs[1]['n_lane'] == 0):
                    self.make_bikelane(is_opp=False, is_rightside=True, width=width_left)

        # print '  bikewayattrs='#,self._bikewayattrs
        # for attrs in self._bikewayattrs:
        #    if attrs['n_lane']>0:
        #        print_attrs(attrs)

    def make_buslane(self, is_opp=False, is_rightside=True,
                     ids_modes_allow=[], width=3.5, n_lane=1):
        print 'make_buslane', is_opp, is_rightside, width
        if is_opp:
            ind = 0
        else:
            ind = 1

        #self._buswayattrs[ind]['is_share'] = len(ids_modes_allow)>0

        ids_allowed_set = deepcopy(self.ids_modes_pt_shared)
        for id_mode in ids_modes_allow:
            if id_mode not in ids_allowed_set:
                ids_allowed_set.append(id_mode)

        self._buswayattrs[ind]['ids_modes_allow'] = list(ids_allowed_set)
        self._buswayattrs[ind]['ids_modes_disallow'] = []

        if is_rightside:
            self._buswayattrs[ind]['is_rightside'] = True
            self._buswayattrs[ind]['width_rightside'] = width
            self._buswayattrs[ind]['n_lane'] += n_lane
        else:
            self._buswayattrs[ind]['is_leftside'] = True
            self._buswayattrs[ind]['width_leftside'] = width
            self._buswayattrs[ind]['n_lane'] += n_lane
        # print '  buswayattr',self._buswayattrs[ind]
        self._buswayattrs[ind]['speed_max'] = self.speed_max_bus
        print_attrs(self._buswayattrs[ind])

    def _get_psv_from_str(self, psv_str):
        if psv_str.isdigit():
            return int(psv_str)
        else:
            if psv_str == 'yes':
                return 1
            else:
                return 0

    def make_buswayattrs(self):
        print 'make_buswayattrs'
        self._buswayattrs = (deepcopy(LANEATTRS_DEFAULT),
                             deepcopy(LANEATTRS_DEFAULT))

        osmattrs = self._osmattrs

        if osmattrs.has_key('busway:width'):
            width_left = float(osmattrs['busway:width'])
            width_right = width_left

        elif osmattrs.has_key('busway:both:width'):
            width_left = float(osmattrs['busway:both:width'])
            width_right = width_left
        else:
            width_left = float(osmattrs.get('busway:left:width', self.lanewidth_bus))
            width_right = float(osmattrs.get('busway:right:width', self.lanewidth_bus))

        busway = osmattrs.get('busway', '')

        if busway is not "":
            # busway scheme
            if osmattrs.get('oneway', 'no') == 'no':
                # bidir
                print '  buslane bidir', busway
                if busway == 'lane':
                    self.make_buslane(is_opp=False)
                    self.make_buslane(is_opp=True)
                    return

                elif busway in OSMKEYS_SHARED:
                    self.make_buslane(is_opp=False, ids_modes_allow=self.ids_modes_pt_shared, width=width_right)
                    self.make_buslane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared, width=width_left)
                    return

                busway = osmattrs.get('busway:both', '')
                if busway == 'lane':
                    self.make_buslane(is_opp=False)
                    self.make_buslane(is_opp=True)
                    return

                elif busway in OSMKEYS_SHARED:
                    self.make_buslane(is_opp=False, ids_modes_allow=self.ids_modes_pt_shared, width=width_right)
                    self.make_buslane(is_opp=True, ids_modes_allow=self.ids_modes_pt_shared, width=width_left)

                # elif busway in OSMKEYS_SHARED_OPP :
                #    self.make_buslane(is_opp = True, ids_modes_allow = self.ids_modes_pt_shared)

                busway = osmattrs.get('busway:left', '')
                if busway == 'lane':
                    self.make_buslane(is_opp=True, is_rightside=False, width=width_left)

                elif busway in OSMKEYS_SHARED:
                    self.make_buslane(is_opp=True,  is_rightside=False,
                                      ids_modes_allow=self.ids_modes_pt_shared, width=width_left)

                elif busway in OSMKEYS_SHARED_OPP:
                    self.make_buslane(is_opp=True,  is_rightside=False,
                                      ids_modes_allow=self.ids_modes_pt_shared, width=width_left)

                busway = osmattrs.get('busway:right', '')
                if busway == 'lane':
                    self.make_buslane(is_opp=False)

                elif busway in OSMKEYS_SHARED:
                    self.make_buslane(is_opp=False, ids_modes_allow=self.ids_modes_pt_shared, width=width_right)

                elif busway in OSMKEYS_SHARED_OPP:
                    self.make_buslane(is_opp=True,  ids_modes_allow=self.ids_modes_pt_shared, width=width_left)

                return

            else:
                # cycle lanes on oneway road
                print '  buslane oneway', busway, busway in ('opposite', 'opposite_lane')
                if busway == 'lane':
                    self.make_buslane(is_opp=False, width=width_right)

                    if osmattrs.get('oneway:bus', 'yes') == 'no':
                        self.make_buslane(is_opp=True, width=width_left)
                    return

                elif busway in ('opposite', 'opposite_lane'):
                    self.make_buslane(is_opp=True, is_rightside=False, width=width_left)

                elif busway == 'opposite_share_busway':
                    self.make_buslane(is_opp=True, is_rightside=False,
                                      ids_modes_allow=self.ids_modes_pt_shared, width=width_left)

                elif self._osmattrs.get('busway:right', '') == 'lane':
                    self.make_buslane(is_opp=False, width=width_right)

                elif self._osmattrs.get('busway:left', '') == 'lane':
                    self.make_buslane(is_opp=False, is_rightside=False, width=width_right)

                elif self._osmattrs.get('busway:left', '') == 'opposite_lane':
                    self.make_buslane(is_opp=True, width=width_left)

                return

        elif osmattrs.has_key('lanes:psv'):
            # lanes:psv=* scheme
            if osmattrs.get('oneway', 'no') == 'no':
                # bidir
                psv = self._get_psv_from_str(osmattrs['lanes:psv'])
                if psv/2 > 1:
                    self.make_buslane(is_opp=True, n_lane=psv/2, width=width_right)
                    self.make_buslane(is_opp=False, n_lane=psv/2, width=width_left)
                else:
                    self.make_buslane(is_opp=False, n_lane=1, width=width_right)

            if osmattrs['lanes:psv'] == 'backward':
                self.make_buslane(is_opp=True, n_lane=1, width=width_left)
            elif osmattrs['lanes:psv'] == 'forward':
                self.make_buslane(is_opp=False, n_lane=1, width=width_right)

            else:
                # oneway
                psv = self._get_psv_from_str(osmattrs['lanes:psv'])
                self.make_buslane(is_opp=False, n_lane=psv, width=width_right)
            return

        elif osmattrs.has_key('lanes:bus'):
            # lanes:psv=* scheme
            if osmattrs.get('oneway', 'no') == 'no':
                # bidir
                psv = self._get_psv_from_str(osmattrs['lanes:bus'])
                if psv/2 > 1:
                    self.make_buslane(is_opp=True, n_lane=psv/2, width=width_right)
                    self.make_buslane(is_opp=False, n_lane=psv/2, width=width_left)
                else:
                    self.make_buslane(is_opp=False, n_lane=1, width=width_right)
            else:
                # oneway
                psv = self._get_psv_from_str(osmattrs['lanes:bus'])
                self.make_buslane(is_opp=False, n_lane=psv, width=width_right)
            return

        if osmattrs.has_key('lanes:psv:forward'):
            psv = self._get_psv_from_str(osmattrs['lanes:psv:forward'])
            self.make_buslane(is_opp=False, n_lane=psv, width=width_right)

        if osmattrs.has_key('lanes:psv:backward'):
            psv = self._get_psv_from_str(osmattrs['lanes:psv:backward'])
            self.make_buslane(is_opp=True, n_lane=psv, width=width_left)

        if osmattrs.has_key('lanes:bus:forward'):
            n_lane = self._get_psv_from_str(osmattrs['lanes:bus:forward'])
            self.make_buslane(is_opp=False, n_lane=n_lane, width=width_right)

        if osmattrs.has_key('lanes:bus:backward'):
            n_lane = self._get_psv_from_str(osmattrs['lanes:bus:backward'])
            self.make_buslane(is_opp=True, n_lane=n_lane, width=width_left)

        if osmattrs.get('oneway', 'no') == 'yes':
            # special check of opposite bus lane
            if osmattrs.has_key('trolley_wire:both') | osmattrs.has_key('trolley_wire:backward'):
                # if way is oneway withot reserved access,
                # but there are wires in both ways,
                # then there is probably a reserved bus lane in opposite
                # direction
                if self._buswayattrs[0]['n_lane'] == 0:
                    self.make_buslane(is_opp=True, width=width_right)

        # NO! Moved to main lane attr
        # if  osmattrs.get('psv','no') in YES_OR_DESIGNATED:
        #    if osmattrs.get('oneway','no')=='no':
        #        self.make_buslane(is_opp = False, n_lane = 1,width = width_right)
        #    else:
        #        self.make_buslane(is_opp = False, n_lane = 1,width = width_right)
        #        self.make_buslane(is_opp = True,width = width_right)
        # print '  buswayattrs=',self._buswayattrs
        # for attrs in self._buswayattrs:
        #    if attrs['n_lane']>0:
        #        print_attrs(attrs)

    def get_priority(self, is_opp=False):
        """
        Returns priority of road.

        To be overridden.
        """
        if is_opp:
            lanes = self._lanes_opp
        else:
            lanes = self._lanes

        speed_max = self.get_speed_max()
        n_lane = len(lanes)
        # is_residential =...
        if n_lane == 0:
            return 0

        if n_lane == 1:
            if speed_max < 11.0/3.6:
                return 1
            elif speed_max <= 31.0/3.6:
                return 2
            elif speed_max <= 51.0/3.6:
                return 3
            else:
                return 4

        elif n_lane == 2:
            if speed_max < 11.0/3.6:
                return 4
            elif speed_max <= 31.0/3.6:
                return 5
            elif speed_max <= 51.0/3.6:
                return 6
            else:
                return 7

        elif n_lane == 3:
            if speed_max < 11.0/3.6:
                return 5
            elif speed_max <= 31.0/3.6:
                return 6
            elif speed_max <= 51.0/3.6:
                return 7
            elif speed_max <= 111.0/3.6:
                return 8
            else:
                return 9

        elif n_lane > 3:
            if speed_max < 11.0/3.6:
                return 6
            elif speed_max <= 51.0/3.6:
                return 7
            elif speed_max <= 91.0/3.6:
                return 8
            elif speed_max <= 111.0/3.6:
                return 9
            else:
                return 10

    def merge_laneattrs(self, laneattrs_dest, laneattrs_merge, is_rightside=False, is_leftside=False):
        print 'merge_laneattrs'
        # print '  laneattrs_dest',laneattrs_dest
        # print '  laneattrs_merge',laneattrs_merge
        #self._buswayattrs[ind]['ids_modes_allow'] = ids_allowed
        #self._buswayattrs[ind]['width'] = width
        #self._buswayattrs[ind]['is_rightside'] = is_rightside
        # if laneattrs_merge['width']>laneattrs_dest['width']:
        width_dest = 0.0
        if laneattrs_dest.has_key('width'):
            width_dest = laneattrs_dest['width']
        else:
            if is_rightside:
                width_dest = laneattrs_dest['width_rightside']
            else:
                width_dest = laneattrs_dest['width_leftside']

        if laneattrs_merge.has_key('width'):
            width_merge = laneattrs_merge['width']
        else:
            if is_rightside:
                width_merge = laneattrs_merge['width_rightside']
            else:
                width_merge = laneattrs_merge['width_leftside']

        # if width_dest<width_merge:
        #    laneattrs_dest['width'] = width_merge
        # else:
        laneattrs_dest['width'] = width_dest

        if laneattrs_dest['speed_max'] > laneattrs_merge['speed_max']:
            laneattrs_dest['speed_max'] = laneattrs_merge['speed_max']

        self.disallow_on_lane(laneattrs_dest, laneattrs_merge['ids_modes_disallow'])
        self.allow_on_lane(laneattrs_dest, laneattrs_merge['ids_modes_allow'])

    def append_lane(self, lanes, laneattrs, is_rightside=False, is_leftside=False):
        if is_rightside:
            lanes.insert(0, deepcopy(laneattrs))
            lanes[0]['width'] = laneattrs['width_rightside']

        elif is_leftside:
            lanes.append(deepcopy(laneattrs))
            lanes[-1]['width'] = laneattrs['width_leftside']

    def set_lane(self, lanes, laneattrs, ind):
        lanes[ind] = deepcopy(laneattrs)
        if not laneattrs.has_key('width'):
            if laneattrs.has_key('width_rightside'):
                lanes[ind]['width'] = laneattrs['width_rightside']
            else:
                lanes[ind]['width'] = laneattrs['width_rightside']

    def disallow_on_lane(self, laneattrs, ids_disallow):
        # print 'disallow_on_lane ids_disallow',ids_disallow
        for id_disallow in ids_disallow:
            if id_disallow in laneattrs['ids_modes_allow']:
                laneattrs['ids_modes_allow'].remove(id_disallow)

        if len(laneattrs['ids_modes_allow']) == 0:
            for id_disallow in ids_disallow:
                if id_disallow not in laneattrs['ids_modes_disallow']:
                    laneattrs['ids_modes_disallow'].append(id_disallow)
        else:
            laneattrs['ids_modes_disallow'] = []

    def allow_on_lane(self, laneattrs, ids_allow):
        laneattrs['ids_modes_disallow'] = []

        for id_allow in ids_allow:
            if id_allow not in laneattrs['ids_modes_allow']:
                laneattrs['ids_modes_allow'].append(id_allow)

    def make_lanes_rigid(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes_rigid', is_opp, n_lane_osm
        osmattrs = self._osmattrs

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        # if is_lanes_rigid:
        #    # respect given lane numbers for main modes, except footpath
        #    pass
        # else:

        # do busways
        attrs = self._buswayattrs[ind]

        if attrs['n_lane'] > 0:
            print '  busways n_lane', attrs['n_lane']

            n_lane = len(lanes)

            allowed, disallowed = self.get_laneaccess('bus', is_opp=is_opp)
            if len(allowed) == 0:
                allowed, disallowed = self.get_laneaccess('psv', is_opp=is_opp)

            n_allow = len(allowed)
            if n_allow > 0:
                for i, a, d in zip(xrange(n_allow), allowed, disallowed):
                    if ind < n_lane:
                        if a & (i < n_lane):
                            self.set_lane(lanes, attrs, i)
                        if d:
                            self.disallow_on_lane(lanes[i], attrs['ids_modes_disallow'])

            else:
                n_lane_assig = attrs['n_lane']
                for i in range(n_lane):
                    if n_lane_assig > 0:
                        if attrs['is_rightside']:
                            self.merge_laneattrs(lanes[i], attrs, is_rightside=True)
                            n_lane_assig -= 1

                        if attrs['is_leftside']:
                            self.merge_laneattrs(lanes[n_lane-i-1], attrs, is_leftside=True)
                            n_lane_assig -= 1

        # do bikeways
        attrs = self._bikewayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  bikeways n_lane', attrs['n_lane']

            n_lane = len(lanes)

            allowed, disallowed = self.get_laneaccess('bicycle', is_opp=is_opp)

            n_allow = len(allowed)
            if n_allow > 0:
                for i, a, d in zip(xrange(n_allow), allowed, disallowed):
                    if i < n_lane:
                        if a & i < n_lane:
                            self.set_lane(lanes, attrs, i)
                        if d:
                            self.disallow_on_lane(lanes[i], attrs['ids_modes_disallow'])

            else:
                n_lane_assig = attrs['n_lane']
                for i in range(n_lane):
                    if n_lane_assig > 0:
                        if attrs['is_rightside']:
                            self.merge_laneattrs(lanes[i], attrs, is_rightside=True)
                            n_lane_assig -= 1

                        if attrs['is_leftside']:
                            self.merge_laneattrs(lanes[n_lane-i-1], attrs, is_leftside=True)
                            n_lane_assig -= 1

        # do sidewalks
        attrs = self._sidewalkattrs[ind]
        if attrs['n_lane'] > 0:
            print '  sidewalks n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)
            # sidewalks are not considered lanes in osm
            # so we append/prepend them
            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

            # disallow pedestrians on other lanes
            #
            for laneattrs in lanes:
                if ID_MODE_PED not in laneattrs['ids_modes_allow']:
                    self.disallow_on_lane(laneattrs, [ID_MODE_PED])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True

    def make_lanes(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes', is_opp, n_lane_osm
        osmattrs = self._osmattrs
        is_lanes_rigid = False

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        # do busways
        attrs = self._buswayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  busways n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)

            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

        # do bikeways
        attrs = self._bikewayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  bikeways n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)

            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1

                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

        # do sidewalks
        attrs = self._sidewalkattrs[ind]

        if attrs['n_lane'] > 0:
            print '  sidewalks n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)
            # sidewalks are not considered lanes in osm
            # so we append/prepend them
            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

            # disallow pedestrians on other lanes
            #
            for laneattrs in lanes:
                if ID_MODE_PED not in laneattrs['ids_modes_allow']:
                    self.disallow_on_lane(laneattrs, [ID_MODE_PED])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True

    def get_speed_max(self, is_opp=False):
        # TODO: go through lanes and get speed max
        return self.speed_max

    def is_roundabout(self):
        return self._osmattrs.get('junction', '') == 'roundabout'

    def configure_edge(self, id_edge, net, is_opp=False, is_remove_sidewalk=0):
        # print '  id',id(self)
        # print '  len(self._lanes)',len(self._lanes)
        # print '  len(self._lanes_opp)',len(self._lanes_opp)

        osmattrs = self._osmattrs
        edges = net.edges
        if is_opp:
            ind = 0
            lanes = np.array(self._lanes_opp)
        else:
            ind = 1
            lanes = np.array(self._lanes)

        print 'configure_edge', id_edge, net.edges.ids_sumo[id_edge], self.highway, 'is_opp', is_opp, 'n_lanes', len(lanes), 'is_remove_sidewalk', is_remove_sidewalk

        if self._is_oneway:  # & (not self.is_roundabout()):
            type_spread = 1  # centered
        else:
            type_spread = 0  # right spread

        wayattrs = self._sidewalkattrs[ind]
        if (wayattrs['n_lane'] > 0) & (not is_remove_sidewalk):
            width_sidewalk = max(wayattrs.get('width_leftside', -1), wayattrs.get('width_rightside', -1))
        else:
            width_sidewalk = -1.0

        # get max lane width
        lanewidth = 0.0

        # 'road': Road,
        #        'path': Footpath,
        #        'pedestrian': Footpath,
        #        'footway': Footpath,
        #        'stairs':Footpath,
        #        'steps':Footpath,
        #        'platform':Footpath,

        if self.highway not in ['residential', 'living_street', 'cycleway']:
            inds_valid = []
            ind = 0
            ind_lastlane = len(lanes)-1
            for laneattrs in lanes:
                # &lanes[-1]['is_sidewalk']:
                if (ind == 0) & laneattrs['is_sidewalk'] & (is_remove_sidewalk == 2) & (ind_lastlane > 0):
                    print '  sidewalk removed on right side'
                    pass
                # &lanes[0]['is_sidewalk']:
                elif (ind == ind_lastlane) & laneattrs['is_sidewalk'] & (is_remove_sidewalk == 1) & (ind_lastlane > 0):
                    print '  sidewalk removed on left side'
                    pass
                else:
                    inds_valid.append(ind)
                    if laneattrs['width'] > lanewidth:
                        lanewidth = laneattrs['width']

                ind += 1
        else:
            inds_valid = np.arange(len(lanes))

        n_lane = len(inds_valid)
        edges.types[id_edge] = self.highway
        edges.nums_lanes[id_edge] = n_lane
        edges.types_spread[id_edge] = type_spread
        edges.speeds_max[id_edge] = self.get_speed_max(is_opp)
        edges.priorities[id_edge] = self.get_priority(is_opp)
        edges.names[id_edge] = osmattrs.get('name', '')
        edges.widths_lanes_default[id_edge] = lanewidth
        edges.widths_sidewalk[id_edge] = width_sidewalk
        # edges.[id_edge] =

        # net.edges.set_row(  id_edge,
        #                types_spread = type_spread,
        #                speeds_max = self.get_speed_max(is_opp),
        #                priorities = self.get_priority(is_opp),
        #                #'widths'=# will be done during update
        #                names = osmattrs.get('name',''),
        #                widths_lanes_default = lanewidth,
        #                widths_sidewalk = width_sidewalk,
        #                )

        # add lanes

        if n_lane == 0:
            print 'WARNING: no lane for this direction!!'
        else:
            print '    inds_valid', inds_valid
        ids_lane = net.lanes.add_rows(n_lane)

        for id_lane, ind_lane, laneattrs in zip(ids_lane, xrange(n_lane), lanes[inds_valid]):
            if len(laneattrs['ids_modes_allow']) == 0:
                id_mode_main = self.id_mode
            else:
                id_mode_main = laneattrs['ids_modes_allow'][0]

            net.lanes.set_row(id_lane,
                              indexes=ind_lane,
                              widths=laneattrs['width'],
                              speeds_max=laneattrs['speed_max'],
                              #offsets_end = kwargs['offsets_end'],
                              #ids_modes_allow = laneattrs['ids_modes_allow'],
                              #ids_modes_disallow = laneattrs['ids_modes_disallow'],
                              ids_mode=id_mode_main,
                              ids_edge=id_edge,
                              )
            # must be done seperately!!
            net.lanes.ids_modes_allow[id_lane] = laneattrs['ids_modes_allow']
            net.lanes.ids_modes_disallow[id_lane] = laneattrs['ids_modes_disallow']

        edges.ids_lanes[id_edge] = list(ids_lane)


class Primary(Road):

    def config(self):
        """
        Make defaults to be overridden
        """
        Road.config(self)
        self.highway = self._osmattrs.get('highway', 'primary')
        # put 50km/h default speed limit
        self.make_speed(50.0/3.6)

        self.n_lane = 2
        self.id_mode = ID_MODE_CAR
        self.lanewidth = 3.5

        self.ids_modes_pt_shared = self.parent.ids_modes_pt_shared
        self.ids_modes_bike = self.parent.ids_modes_bike
        self.ids_modes_moped = self.parent.ids_modes_moped

    def get_priority(self, is_opp=False):
        """
        Returns priority of road.

        To be overridden.
        """
        if is_opp:
            lanes = self._lanes_opp
        else:
            lanes = self._lanes

        speed_max = self.get_speed_max()
        n_lane = len(lanes)
        # is_residential =...
        if n_lane == 0:
            return 0

        elif n_lane <= 2:
            if speed_max <= 51.0/3.6:
                return 7
            elif speed_max <= 111.0/3.6:
                return 8
            else:
                return 9

        elif n_lane == 3:
            if speed_max <= 51.0/3.6:
                return 7
            elif speed_max <= 91.0/3.6:
                return 8
            elif speed_max <= 111.0/3.6:
                return 9
            else:
                return 10

        elif n_lane > 3:
            return 10

    def make_lanes_rigid(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes_rigid', is_opp, n_lane_osm
        osmattrs = self._osmattrs

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        # if is_lanes_rigid:
        #    # respect given lane numbers for main modes, except footpath
        #    pass
        # else:

        # do busways
        attrs = self._buswayattrs[ind]

        if attrs['n_lane'] > 0:
            print '  busways n_lane', attrs['n_lane']

            n_lane = len(lanes)

            allowed, disallowed = self.get_laneaccess('bus', is_opp=is_opp)
            if len(allowed) == 0:
                allowed, disallowed = self.get_laneaccess('psv', is_opp=is_opp)

            n_allow = len(allowed)
            if n_allow > 0:
                for i, a, d in zip(xrange(n_allow), allowed, disallowed):
                    if ind < n_lane:
                        if a & (i < n_lane):
                            self.set_lane(lanes, attrs, i)
                        if d:
                            self.disallow_on_lane(lanes[i], attrs['ids_modes_disallow'])

            else:
                n_lane_assig = attrs['n_lane']
                for i in range(n_lane):
                    if n_lane_assig > 0:
                        if attrs['is_rightside']:
                            self.merge_laneattrs(lanes[i], attrs, is_rightside=True)
                            n_lane_assig -= 1

                        if attrs['is_leftside']:
                            self.merge_laneattrs(lanes[n_lane-i-1], attrs, is_leftside=True)
                            n_lane_assig -= 1

        # do bikeways
        attrs = self._bikewayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  bikeways n_lane', attrs['n_lane']

            n_lane = len(lanes)

            allowed, disallowed = self.get_laneaccess('bicycle', is_opp=is_opp)

            n_allow = len(allowed)
            if n_allow > 0:
                for i, a, d in zip(xrange(n_allow), allowed, disallowed):
                    if i < n_lane:
                        if a & i < n_lane:
                            self.set_lane(lanes, attrs, i)
                        if d:
                            self.disallow_on_lane(lanes[i], attrs['ids_modes_disallow'])

            else:
                n_lane_assig = attrs['n_lane']
                for i in range(n_lane):
                    if n_lane_assig > 0:
                        if attrs['is_rightside']:
                            if is_opp:
                                self.merge_laneattrs(lanes[i], attrs, is_rightside=True)
                            else:
                                # in a primary link require 2 lanes for cars
                                if n_lane_osm > 2:
                                    self.merge_laneattrs(lanes[i], attrs, is_rightside=True)
                                else:
                                    self.append_lane(lanes, attrs, is_rightside=True)
                            n_lane_assig -= 1

                        if attrs['is_leftside']:
                            self.merge_laneattrs(lanes[n_lane-i-1], attrs, is_leftside=True)
                            n_lane_assig -= 1

        # do sidewalks
        attrs = self._sidewalkattrs[ind]
        if attrs['n_lane'] > 0:
            print '  sidewalks n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)
            # sidewalks are not considered lanes in osm
            # so we append/prepend them
            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1

                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

            # disallow pedestrians on other lanes
            #
            for laneattrs in lanes:
                if ID_MODE_PED not in laneattrs['ids_modes_allow']:
                    self.disallow_on_lane(laneattrs, [ID_MODE_PED])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True

    def make_lanes(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes primary', is_opp, n_lane_osm
        osmattrs = self._osmattrs
        #is_lanes_rigid = False

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        # do busways
        attrs = self._buswayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  busways n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)

            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

        # do bikeways
        attrs = self._bikewayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  bikeways n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)

            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if is_opp:
                        if attrs['is_share'] & (len(lanes) > 0):
                            self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                        else:
                            self.append_lane(lanes, attrs, is_rightside=True)
                    else:
                        # primary link specific: only merge if more than 2 lanes
                        if attrs['is_share'] & (len(lanes) > 2):
                            self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                        else:
                            self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1

                if attrs['is_leftside']:
                    # primary link specific: only merge if more than 2 lanes
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

        # do sidewalks
        attrs = self._sidewalkattrs[ind]
        if attrs['n_lane'] > 0:
            print '  sidewalks n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)
            # sidewalks are not considered lanes in osm
            # so we append/prepend them
            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

            # disallow pedestrians on other lanes
            #
            for laneattrs in lanes:
                if ID_MODE_PED not in laneattrs['ids_modes_allow']:
                    self.disallow_on_lane(laneattrs, [ID_MODE_PED])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True


class Motorway(Road):

    def config(self):
        """
        Make defaults to be overridden
        """
        Road.config(self)
        self.highway = self._osmattrs.get('highway', 'motorway')
        # put 50km/h default speed limit
        self.make_speed(130.0/3.6)

        self.n_lane = 2
        self.id_mode = ID_MODE_CAR
        self.lanewidth = 3.5

        self.ids_modes_pt_shared = self.parent.ids_modes_pt_shared
        self.ids_modes_bike = self.parent.ids_modes_bike
        self.ids_modes_moped = self.parent.ids_modes_moped

    def get_priority(self, is_opp=False):
        """
        Returns priority of road.

        To be overridden.
        """
        if is_opp:
            lanes = self._lanes_opp
        else:
            lanes = self._lanes

        speed_max = self.get_speed_max()
        n_lane = len(lanes)
        # is_residential =...
        if n_lane == 0:
            return 0

        elif n_lane == 1:
            return 9
        else:
            return 10

    def make_lanes_rigid(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes_rigid', is_opp, n_lane_osm
        osmattrs = self._osmattrs

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        # disallow pedestrians and bikes on all lanes
        for laneattrs in lanes:
            self.disallow_on_lane(laneattrs, [ID_MODE_PED, ID_MODE_BIKE])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True

    def make_lanes(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes primary', is_opp, n_lane_osm
        osmattrs = self._osmattrs
        #is_lanes_rigid = False

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        for laneattrs in lanes:
            self.disallow_on_lane(laneattrs, [ID_MODE_PED, ID_MODE_BIKE])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True


class Residential(Road):

    def config(self):
        """
        Make defaults to be overridden
        """
        Road.config(self)
        # put 30km/h default speed limit
        self.make_speed(30.0/3.6)

        self.ids_modes_pt_shared = self.parent.ids_modes_pt_shared
        self.ids_modes_bike = self.parent.ids_modes_bike
        self.ids_modes_moped = self.parent.ids_modes_moped

    def make_laneattrs_main(self):
        Road.make_laneattrs_main(self)
        self.make_laneattrs_main_accessrestrict()

    def get_priority(self, is_opp=False):
        """
        Returns priority of road.

        To be overridden.
        """
        if is_opp:
            lanes = self._lanes_opp
        else:
            lanes = self._lanes

        speed_max = self.get_speed_max()
        n_lane = len(lanes)
        return 3

    def make_sidewalkattrs(self):
        print 'make_sidewalkattrs'
        self.make_sidewalkattrs_smallroads()


class Footpath(Road):
    def config(self):
        """
        Make defaults to be overridden
        """
        # print 'config'
        # pass here either road type specific defaults
        # or global defaults from parent process
        self.id_mode = ID_MODE_PED
        # highway=footway|path
        self.highway = self._osmattrs.get('highway', 'footway')

        # Use footway=crossing for places where the footway crosses a street.
        # Use footway=sidewalk

        self.footway = self._osmattrs.get('footway', '')
        self.n_lane = 1

        if self.footway == 'sidewalk':
            lanewidth = self.parent.width_sidewalk
        else:
            lanewidth = self.parent.width_footpath

        # half lanewidth if segregated
        if self._osmattrs.get('segregated', '') == 'yes':
            lanewidth /= 2

        self.lanewidth = lanewidth
        self.lanewidth_bike = lanewidth
        self.lanewidth_bus = self.parent.lanewidth_bus
        #self.width_sidewalk = self.parent.width_sidewalk
        #self.width_footpath = self.parent.width_footpath

        #self.ids_modes_pt_shared = self.parent.ids_modes_pt_shared
        self.ids_modes_bike = self.parent.ids_modes_bike
        self.ids_modes_pt_shared = self.parent.ids_modes_pt_shared
        self.ids_modes_moped = self.parent.ids_modes_moped

        self.ids_modes_allow = [ID_MODE_PED]
        self.ids_modes_disallow = []
        self.make_speed(self.parent.speed_max_ped)

        self.speed_max_bus = self.parent.speed_max_bus
        self.speed_max_bike = self.parent.speed_max_bike
        #self.speed_max_ped = self.parent.speed_max_ped

    def make_laneattrs_main(self):
        Road.make_laneattrs_main(self)
        self.make_laneattrs_main_accessrestrict()
        self._laneattrs_main['is_sidewalk'] = True

    def get_priority(self, is_opp=False):
        """
        Returns priority of road.

        To be overridden.
        """
        if is_opp:
            lanes = self._lanes_opp
        else:
            lanes = self._lanes

        speed_max = self.get_speed_max()
        n_lane = len(lanes)
        return 1

    def make_oneway(self):
        # print 'make_oneway'
        osmattrs = self._osmattrs

        # default for footpath is oneway = yes
        if osmattrs.get('oneway', 'yes') == 'no':
            self._is_oneway = False

        else:
            # check if really oneway
            # we make oneway if only pedestrians use this edge
            if (osmattrs.get('bicycle', '') in YES_OR_DESIGNATED)\
               | (osmattrs.get('route', '') == 'bicycle')\
               | (osmattrs.get('psv', 'no') in YES_OR_DESIGNATED)\
               | (osmattrs.get('moped', 'no') in YES_OR_DESIGNATED):
                self._is_oneway = False
            else:
                self._is_oneway = True

    def make_sidewalkattrs(self):
        self._sidewalkattrs = (deepcopy(LANEATTRS_DEFAULT),
                               deepcopy(LANEATTRS_DEFAULT))
        # pass
        self._sidewalkattrs[0]['is_sidewalk'] = True
        self._sidewalkattrs[1]['is_sidewalk'] = True
        # but there will be no sidewalk of sidewalk

    def make_bikewayattrs(self):
        print 'Footpath.make_bikewayattrs'
        osmattrs = self._osmattrs
        self._bikewayattrs = (deepcopy(LANEATTRS_DEFAULT),
                              deepcopy(LANEATTRS_DEFAULT))

        width_left = self.lanewidth_bike
        width_right = self.lanewidth_bike

        if osmattrs.get('oneway', 'no') == 'no':
            # cycle lanes on bidir footpath (normal)
            if (osmattrs.get('bicycle', '') in YES_OR_DESIGNATED) | (osmattrs.get('route', '') == 'bicycle'):
                if osmattrs.get('segregated', '') == 'yes':
                    # bike lanes to the left of the footpath
                    self.make_bikelane(is_opp=False, is_rightside=False, width=width_right)
                    self.make_bikelane(is_opp=True, is_rightside=False, width=width_left)
                # else:
                #
                #    # bike lanes will be merged with the footpath
                #    self.make_bikelane(is_opp = False, width = width_right, ids_modes_allow =[ID_MODE_PED])
                #    self.make_bikelane(is_opp = True, width = width_left, ids_modes_allow =[ID_MODE_PED])
        else:
            if (osmattrs.get('bicycle', '') in YES_OR_DESIGNATED) | (osmattrs.get('route', '') == 'bicycle'):
                if osmattrs.get('segregated', '') == 'yes':
                    # bike lanes to the left of the footpath
                    self.make_bikelane(is_opp=False, is_rightside=False, width=width_right)

    def make_lanes_rigid(self, is_opp=False, n_lane_osm=0):
        print 'Footpath.make_lanes_rigid ped', is_opp, n_lane_osm
        osmattrs = self._osmattrs

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []

        n_other = self._bikewayattrs[ind]['n_lane']+self._buswayattrs[ind]['n_lane']
        if (not is_opp) | (n_other == 0):
            # main footpath ony in main direction
            n_lane_osm = max(n_lane_osm, 1)  # guarantee that one lane is created
            for i in range(n_lane_osm):
                lanes.append(deepcopy(self._laneattrs_main))

        # if is_lanes_rigid:
        #    # respect given lane numbers for main modes, except footpath
        #    pass
        # else:

        # do busways
        attrs = self._buswayattrs[ind]

        if attrs['n_lane'] > 0:
            print '  busways n_lane', attrs['n_lane']

            n_lane = len(lanes)

            allowed, disallowed = self.get_laneaccess('bus', is_opp=is_opp)
            if len(allowed) == 0:
                allowed, disallowed = self.get_laneaccess('psv', is_opp=is_opp)

            n_allow = len(allowed)
            if n_allow > 0:
                for i, a, d in zip(xrange(n_allow), allowed, disallowed):
                    if ind < n_lane:
                        if a & (i < n_lane):
                            self.set_lane(lanes, attrs, i)
                        if d:
                            self.disallow_on_lane(lanes[i], attrs['ids_modes_disallow'])

            else:
                n_lane_assig = attrs['n_lane']
                for i in range(n_lane):
                    if n_lane_assig > 0:
                        if attrs['is_rightside']:
                            self.merge_laneattrs(lanes[i], attrs, is_rightside=True)
                            n_lane_assig -= 1

                        if attrs['is_leftside']:
                            self.merge_laneattrs(lanes[n_lane-i-1], attrs, is_leftside=True)
                            n_lane_assig -= 1

        # do bikeways
        attrs = self._bikewayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  bikeways n_lane', attrs['n_lane']

            n_lane = len(lanes)

            allowed, disallowed = self.get_laneaccess('bicycle', is_opp=is_opp)

            n_allow = len(allowed)
            if n_allow > 0:
                for i, a, d in zip(xrange(n_allow), allowed, disallowed):
                    if i < n_lane:
                        if a & i < n_lane:
                            self.set_lane(lanes, attrs, i)
                        if d:
                            self.disallow_on_lane(lanes[i], attrs['ids_modes_disallow'])

            else:
                n_lane_assig = attrs['n_lane']
                for i in range(n_lane):
                    if n_lane_assig > 0:
                        if attrs['is_rightside']:
                            self.merge_laneattrs(lanes[i], attrs, is_rightside=True)
                            n_lane_assig -= 1

                        if attrs['is_leftside']:
                            self.merge_laneattrs(lanes[n_lane-i-1], attrs, is_leftside=True)
                            n_lane_assig -= 1

    def make_lanes(self, is_opp=False, n_lane_osm=0):
        print 'Footpath.make_lanes', is_opp, n_lane_osm, 'n_bikelane', len(self._bikewayattrs)
        osmattrs = self._osmattrs

        print '  main lane attrs'
        print_attrs(self._laneattrs_main)

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        n_other = self._bikewayattrs[ind]['n_lane']+self._buswayattrs[ind]['n_lane']
        if (not is_opp) | (n_other == 0):
            n_lane_osm = max(n_lane_osm, 1)  # guarantee that one lane is created
            for i in range(n_lane_osm):
                lanes.append(deepcopy(self._laneattrs_main))

        # do busways
        attrs = self._buswayattrs[ind]
        if attrs['n_lane'] > 0:
            print '  busways n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)

            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

        # do bikeways

        print '  bikewayattrs:'
        attrs = self._bikewayattrs[ind]
        print_attrs(attrs)
        if attrs['n_lane'] > 0:
            print '  bikeways n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)

            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1

                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes

        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True


class Cycleway(Road):

    def config(self):
        """
        Make defaults to be overridden
        """
        print 'config cycleway'
        # pass here either road type specific defaults
        # or global defaults from parent process

        self.highway = self._osmattrs.get('highway', 'cycleway')

        self.id_mode = ID_MODE_BIKE
        self.n_lane = 1

        # print '  width',self._osmattrs.get('width',self.parent.lanewidth_bike)
        if self.is_oneway():
            width_default = self.parent.lanewidth_bike
        else:
            width_default = 2*self.parent.lanewidth_bike

        self.lanewidth = self.get_osmattr('width', is_float=True, default=width_default)

        # half lane width if bidir
        if not self.is_oneway():
            self.lanewidth *= 0.5

        #self.lanewidth_bike = self.parent.lanewidth_bike
        #self.lanewidth_bus = self.parent.lanewidth_bus
        self.width_sidewalk = self.parent.width_sidewalk
        self.width_footpath = self.parent.width_footpath

        #self.ids_modes_pt_shared = self.parent.ids_modes_pt_shared
        #self.ids_modes_bike = self.parent.ids_modes_bike
        self.ids_modes_allow = self.parent.ids_modes_bike
        self.ids_modes_disallow = []
        self.make_speed(self.parent.speed_max_bike)

        print '  ids_modes_allow', self.ids_modes_allow
        #self.speed_max_bus = self.parent.speed_max_bus
        #self.speed_max_bike = self.parent.speed_max_bike
        self.speed_max_ped = self.parent.speed_max_ped

    def make_laneattrs_main(self):
        Road.make_laneattrs_main(self)
        # self.make_laneattrs_main_accessrestrict()

    def make_oneway(self):
        # print 'make_oneway'
        osmattrs = self._osmattrs

        # default for bikeway is oneway = no
        if osmattrs.get('oneway', 'no') == 'no':
            self._is_oneway = False

        else:
            self._is_oneway = True

    def make_buswayattrs(self):
        # print 'make_buswayattrs'
        # self._buswayattrs = (LANEATTRS_DEFAULT.copy(),
        #                        LANEATTRS_DEFAULT.copy())
        pass

    # def make_sidewalkattrs(self):
    #    self._sidewalkattrs = (deepcopy(LANEATTRS_DEFAULT),
    #                            deepcopy(LANEATTRS_DEFAULT))
    #    #pass

    def make_bikewayattrs(self):
        pass

    def make_sidewalkattrs(self):
        print 'make_sidewalkattrs'
        # put sidewalks only if sidewalk  attributes are given
        return self.make_sidewalkattrs_largeroads()

    def make_lanes_rigid(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes_rigid cycle', is_opp, n_lane_osm
        osmattrs = self._osmattrs

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        # do sidewalks
        attrs = self._sidewalkattrs[ind]
        if attrs['n_lane'] > 0:
            print '  sidewalks n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)
            # sidewalks are not considered lanes in osm
            # so we append/prepend them
            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

            # disallow pedestrians on other lanes
            #
            for laneattrs in lanes:
                if ID_MODE_PED not in laneattrs['ids_modes_allow']:
                    self.disallow_on_lane(laneattrs, [ID_MODE_PED])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True

    def make_lanes(self, is_opp=False, n_lane_osm=0):
        print 'make_lanes cycle', is_opp, n_lane_osm
        osmattrs = self._osmattrs
        is_lanes_rigid = False

        if is_opp:
            ind = 0
            if self._is_oneway:
                self._lanes_opp = []
                return False
        else:
            ind = 1

        lanes = []
        for i in range(n_lane_osm):
            lanes.append(deepcopy(self._laneattrs_main))

        # do sidewalks
        attrs = self._sidewalkattrs[ind]
        if attrs['n_lane'] > 0:
            print '  sidewalks n_lane', attrs['n_lane']
            n_lane_assig = attrs['n_lane']
            n_lane = len(lanes)
            # sidewalks are not considered lanes in osm
            # so we append/prepend them
            while n_lane_assig > 0:
                if attrs['is_rightside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[0], attrs, is_rightside=True)
                    else:
                        self.append_lane(lanes, attrs, is_rightside=True)
                    n_lane_assig -= 1
                if attrs['is_leftside']:
                    if attrs['is_share'] & (len(lanes) > 0):
                        self.merge_laneattrs(lanes[-1], attrs, is_leftside=True)
                    else:
                        self.append_lane(lanes, attrs, is_leftside=True)
                    n_lane_assig -= 1

            # disallow pedestrians on other lanes
            #
            for laneattrs in lanes:
                if ID_MODE_PED not in laneattrs['ids_modes_allow']:
                    self.disallow_on_lane(laneattrs, [ID_MODE_PED])

        if is_opp:
            self._lanes_opp = lanes
        else:
            self._lanes = lanes
        print '  created %d lanes' % len(lanes)
        # print '  lanes', lanes
        for laneattrs in lanes:
            print_attrs(laneattrs)
        return True

    def get_priority(self, is_opp=False):
        """
        Returns priority of road.

        To be overridden.
        """
        if is_opp:
            lanes = self._lanes_opp
        else:
            lanes = self._lanes

        speed_max = self.get_speed_max()
        n_lane = len(lanes)
        return 2


ROADCLASSES = {'road': Road,
               'path': Footpath,
               'pedestrian': Footpath,
               'footway': Footpath,
               'stairs': Footpath,
               'steps': Footpath,
               'platform': Footpath,
               'cycleway': Cycleway,
               'residential': Residential,
               'living_street': Residential,
               'primary': Primary,
               'primary_link': Primary,
               'motorway': Motorway,
               'trunk': Motorway,
               'motorway_link': Motorway,
               'trunk_link': Motorway,
               'primary_link': Primary,
               }
OSMKEYS_SHARED = ('shared', 'share_lane', 'share_busway', 'share_cycleway')
OSMKEYS_SHARED_OPP = ('opposite_shared', 'opposite_share_lane', 'opposite_share_busway', 'opposite_share_cycleway')


class OxImporter(Process):
    def __init__(self,  scenario,
                 ident='oximporter',
                 name='OSMnx importer',
                 info='Import of network imported with the help of osmnx.',
                 logger=None, **kwargs):

        print 'OxImporter.__init__'

        self._init_common(ident,
                          parent=scenario,
                          name=name,
                          logger=logger,
                          info=info,
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        # self.id_mode = attrsman.add(am.AttrConf('id_mode',  modechoices['bicycle'],
        #                                groupnames = ['options'],
        #                                choices = modechoices,
        #                                name = 'Mode',
        #                                info = 'Transport mode to be matched.',
        #                                ))

        self.osmdatafilepaths = attrsman.add(
            cm.AttrConf('osmdatafilepaths', kwargs.get('osmdatafilepaths', scenario.get_rootfilepath()),
                        groupnames=['options'],
                        perm='rw',
                        name='OSM data file(s)',
                        wildcards='Jason file (*.json)|*.json',
                        metatype='filepaths',
                        info="""One or several jason filepaths holding OSM related information.""",
                        ))

        self.nxnetworkpaths = attrsman.add(
            cm.AttrConf('nxnetworkpaths', kwargs.get('nxnetworkpaths', scenario.get_rootfilepath()),
                        groupnames=['options'],
                        perm='rw',
                        name='Networkx file(s)',
                        wildcards='NetworkX file (*.obj)|*.obj',
                        metatype='filepaths',
                        info="""One or several network X filepaths containing topological information of simplified network.""",
                        ))

        self.n_lane = attrsman.add(cm.AttrConf('n_lane', 1,
                                               groupnames=['options', 'osm'],
                                               name='Default lane number',
                                               info="Default lane number.",
                                               ))

        self.speed_max = attrsman.add(cm.AttrConf('speed_max', 50.0/3.6,
                                                  groupnames=['options', 'osm'],
                                                  name='Default Max. speed',
                                                  info="General default maximum speed.",
                                                  unit='m/s',
                                                  ))

        self.speed_max_bus = attrsman.add(cm.AttrConf('speed_max_bus', 50.0/3.6,
                                                      groupnames=['options', 'osm'],
                                                      name='Default Max. bus speed',
                                                      info="Default maximum speed on busways.",
                                                      unit='m/s',
                                                      ))

        self.speed_max_bike = attrsman.add(cm.AttrConf('speed_max_bike', 20.0/3.6,
                                                       groupnames=['options', 'osm'],
                                                       name='Default Max. bike speed',
                                                       info="Default maximum speed on bikeways.",
                                                       unit='m/s',
                                                       ))

        self.speed_max_ped = attrsman.add(cm.AttrConf('speed_max_ped', 3.0/3.6,
                                                      groupnames=['options', 'osm'],
                                                      name='Default Max. ped. speed',
                                                      info="Default maximum speed on footpath or sidewalks.",
                                                      unit='m/s',
                                                      ))

        self.lanewidth = attrsman.add(cm.AttrConf('lanewidth', 3.25,
                                                  groupnames=['options', 'osm'],
                                                  name='Default lane',
                                                  info="Default lane width for standard lane.",
                                                  unit='m',
                                                  ))

        self.lanewidth_bike = attrsman.add(cm.AttrConf('lanewidth_bike', 1.5,
                                                       groupnames=['options', 'osm'],
                                                       name='Default bikeway width',
                                                       info="Default bikeway width.",
                                                       unit='m',
                                                       ))

        self.lanewidth_bus = attrsman.add(cm.AttrConf('lanewidth_bus', 3.5,
                                                      groupnames=['options', 'osm'],
                                                      name='Default busway width',
                                                      info="Default busway width.",
                                                      unit='m',
                                                      ))

        self.width_footpath = attrsman.add(cm.AttrConf('width_footpath', 1.5,
                                                       groupnames=['options', 'osm'],
                                                       name='Default footpath width',
                                                       info="Default footpath width.",
                                                       unit='m',
                                                       ))

        self.width_sidewalk = attrsman.add(cm.AttrConf('width_sidewalk', 1.0,
                                                       groupnames=['options', 'osm'],
                                                       name='Default sidewalk width',
                                                       info="Default sidewalk width.",
                                                       unit='m',
                                                       ))

        self.modes_pt_shared = attrsman.add(cm.AttrConf('modes_pt_shared', ['bus', 'taxi', 'emergency', 'bicycle'],
                                                        groupnames=['options', 'osm'],
                                                        name='Modes on PT reserved lanes',
                                                        info="Modes that are allowed on reserved lanes for public transport.",
                                                        perm='r',
                                                        ))

        self.modes_bike = attrsman.add(cm.AttrConf('modes_bike', ['bicycle', ],
                                                   groupnames=['options', 'osm'],
                                                   name='Modes on dedicated bike lanes',
                                                   info="Modes that are allowed on dedicated bike lanes.",
                                                   perm='r',
                                                   ))

        self.modes_moped = attrsman.add(cm.AttrConf('modes_moped', ['moped', ],
                                                    groupnames=['options', 'osm'],
                                                    name='Modes allowed on dedicated moped acess',
                                                    info="Modes that are allowed on lanes with dedicated moped access.",
                                                    perm='r',
                                                    ))

        self.is_simplify_footpath = attrsman.add(cm.AttrConf('is_simplify_footpath', True,
                                                             groupnames=['options', 'osm'],
                                                             name='Simplify footpath',
                                                             info="Simplify footpath by removing footpath parallel to roads and giving a sidewalk to roads instead. (Unsafe option)",
                                                             ))

        self.n_node_sidewalkremove = attrsman.add(cm.AttrConf('n_node_sidewalkremove', 8,
                                                              groupnames=['options', 'osm'],
                                                              name='Node number sidewalk remove',
                                                              info="Maximum number of nodes in a ring where inner sidewalks are removed. Used only with option 'Simplify footpath'",
                                                              unit='m',
                                                              ))

        self.ids_modes_moped = []
        for mode in self.modes_moped:
            self.ids_modes_moped.append(MODES[mode])

        self.ids_modes_bike = []
        for mode in self.modes_bike:
            self.ids_modes_bike.append(MODES[mode])

        self.ids_modes_pt_shared = []
        for mode in self.modes_pt_shared:
            self.ids_modes_pt_shared.append(MODES[mode])

        # def import_xnet(net, nodeshapefilepath, edgeshapefilepath, polyshapefilepath):
        #import_nodeshapes(net, nodeshapefilepath)

    def get_scenario(self):
        return self.parent

    def do(self):
        print self.ident+'.do'

        print '   osmdatafilepaths', self.osmdatafilepaths
        print '   nxnetworkpaths', self.nxnetworkpaths
        net = self.get_scenario().net
        self._net = net
        self._map_id_edge_sumo_to_doubles = {}
        self.edges_osm = {}
        self.nodes_osm = {}

        projparams_target = net.get_projparams()
        if projparams_target in ("", "!"):
            projparams_target = None

        for filepath in self.osmdatafilepaths.split(','):
            self.import_osmdata(json.loads(open(filepath).read()))

        for filepath in self.nxnetworkpaths.split(','):  # filepathstring_to_filepathlist(self.nxnetworkpaths):
            graphx = load_objfile(filepath.strip())
            self.import_nx(graphx, projparams=projparams_target)

        net.edges.update(is_update_lanes=True)

        return True

    def import_osmdata(self, osmdata):
        edges_osm = self.edges_osm
        nodes_osm = self.nodes_osm
        for element in osmdata['elements']:
            # {u'nodes': [5340543333L, 5340543332L], u'type': u'way', u'id': 553233159, u'tags': {u'highway': u'service', u'oneway': u'no', u'service': u'driveway', u'surface': u'asphalt', u'incline': u'up'}}
            # {u'lat': 47.6038005, u'type': u'node', u'lon': 7.6006465, u'id': 453208, u'tags': {u'ref': u'69', u'name': u'Weil am Rhein / H\xfcningen', u'highway': u'motorway_junction'}}
            # print '  parse element',element['id'],element['type']
            if element['type'] == 'way':
                if element.has_key('tags'):
                    highway = element['tags'].get('highway', 'road')
                else:
                    highway = 'road'
                print '  '+70*'-'
                print '  way', element['id'], type(element['id']), highway
                edges_osm[str(element['id'])] = ROADCLASSES.get(highway, Road)(element, self)
                print '  done with way', element['id'], id(self.edges_osm[str(element['id'])])

            if element['type'] == 'node':
                nodes_osm[element['id']] = element.get('tags', {})

        print '   import_osmdata:done'

    # def get_id_edge_sumo(self, osmid):
    #    idmap = self._map_id_edge_sumo_to_doubles
    #    if idmap.has_key(osmid):
    #        idmap[osmid] +=1
    #    else:
    #        idmap[osmid] = 0
    #    return (osmid, idmap[osmid])
    #    #return str(osmid)+'#'+str(idmap[osmid])

    def get_edgenumber(self, osmid_dir):
        idmap = self._map_id_edge_sumo_to_doubles
        if idmap.has_key(osmid_dir):
            idmap[osmid_dir] += 1
        else:
            idmap[osmid_dir] = 0
        return idmap[osmid_dir]

    def get_id_edge_sumo(self, osmid_dir):
        edgenumber = self.get_edgenumber(osmid_dir)
        return osmid_dir+'#'+str(edgenumber)

    def get_id_edge_osm_from_id_sumo(self, id_sumo):
        """
        Returns from id_sumo:
            (id_osm, is_opp, num)
        """
        data = id_sumo.split('#')
        if data[0][0] == '-':
            return data[0][1:], True, int(data[1])
        else:
            return data[0], False, int(data[1])

    def is_edge_eligible(self, id_osm, id_fromnode_sumo, id_tonode_sumo):

        # print 'is_edge_eligible',id_osm,type(id_osm),id_fromnode_sumo, id_tonode_sumo,self.edges_osm.has_key(id_osm),self._edgelookup.has_key(id_osm)
        if not self.edges_osm.has_key(id_osm):
            # for some reason, nx network has edges that are not in jason
            return False

        edgelookup = self._edgelookup

        if edgelookup.has_key(id_osm):
            # is reverse already in database
            # print '  check',edgelookup[id_osm], (id_tonode_sumo, id_fromnode_sumo) in edgelookup[id_osm]
            edgelookup[id_osm].add((id_fromnode_sumo, id_tonode_sumo))
            if (id_tonode_sumo, id_fromnode_sumo) in edgelookup[id_osm]:
                return False
            else:
                return True

        else:
            edgelookup[id_osm] = set([(id_fromnode_sumo, id_tonode_sumo)])
            return True

    def make_segment_edge_map(self):
        """
        Generates a vertex matrix with line segments of all edges
        and a map that maps each line segment to edge index.
        """
        polylines = self._edgeshapes
        n_edges = len(polylines)
        inds = np.arange(n_edges)
        #ids = inds+1
        print 'make_linevertices', n_edges

        linevertices = np.zeros((0, 2, 3), np.float32)
        vertexinds = np.zeros((0, 2), np.int32)
        polyinds = []

        lineinds = []
        linebeginstyles = []
        lineendstyles = []

        i = 0
        ind_line = 0

        for ind in inds:

            # if is_laneshapes:
            #    polyline = laneshapes[ids_lanes[ind][0]]
            # else:
            polyline = polylines[ind]

            n_seg = len(polyline)
            # print '  =======',n_seg#,polyline

            if n_seg > 1:
                polyvinds = range(n_seg)
                # print '  polyvinds\n',polyvinds
                vi = np.zeros((2*n_seg-2), np.int32)
                vi[0] = polyvinds[0]
                vi[-1] = polyvinds[-1]

                # Important type conversion!!
                v = np.zeros((2*n_seg-2, 3), np.float32)
                v[0] = polyline[0]
                v[-1] = polyline[-1]
                if len(v) > 2:

                    v[1:-1] = np.repeat(polyline[1:-1], 2, 0)
                    vi[1:-1] = np.repeat(polyvinds[1:-1], 2)

                n_lines = len(v)/2
                # print '  v\n',v
                polyinds += n_lines*[ind]
                lineinds.append(np.arange(ind_line, ind_line+n_lines))
                ind_line += n_lines

            else:
                # empty polygon treatment
                v = np.zeros((0, 3), np.float32)
                vi = np.zeros((0), np.int32)

            linevertices = np.concatenate((linevertices, v.reshape((-1, 2, 3))))
            vertexinds = np.concatenate((vertexinds, vi.reshape((-1, 2))))
            # print '  linevertex\n',linevertices
            i += 1

        #self._segvertices = linevertices
        self._edgeinds = np.array(polyinds, np.int32)
        self._seginds = lineinds
        self._segvertexinds = np.array(vertexinds, np.int32)

        self._x1 = linevertices[:, 0, 0]
        self._y1 = linevertices[:, 0, 1]

        self._x2 = linevertices[:, 1, 0]
        self._y2 = linevertices[:, 1, 1]

    def get_closest_edge_dist(self, p, ind_shape, n_best=0, d_min=0.5, alpha_min=1.0*np.pi/180, d_max=None):
        """
        Returns edge id which is closest to point p.
        Requires execution of make_segment_edge_map
        """
        # print '  x1', x1
        # print '  x2', x2
        #halfwidths = 0.5*self.get_widths_array()[self._polyinds]
        x1, y1, x2, y2 = self._x1, self._y1, self._x2, self._y2
        d2 = get_dist_point_to_segs(p[0:2], x1, y1, x2, y2, is_ending=True)
        # print '  min(d2)=',np.min(d2),'argmin=',np.argmin(d2),self.get_ids(self._edgeinds[np.argmin(d2)])

        d2[self._seginds[ind_shape]] = np.inf
        #d2[d2<0.5] = np.inf

        # check side
        v1 = np.concatenate(((x2-x1).reshape(1, -1), (y2-y1).reshape(1, -1)), 0)
        v2 = np.concatenate(((p[0]-x1).reshape(1, -1), (p[1]-y1).reshape(1, -1)), 0)
        angles = get_diff_angle_clockwise(v1, v2)
        #d2[angles<alpha_min] = np.inf

        if n_best == 0:
            ind = np.argmin(d2)
            return self._edgeinds[ind], np.sqrt(d2[ind])

        elif d_max is None:
            dists2_min = np.zeros(n_best, dtype=np.float32)
            inds_min = np.zeros(n_best, dtype=np.int)
            for i in xrange(n_best):
                ind = np.argmin(d2)
                inds_min[i] = ind
                dists2_min[i] = d2[ind]
                d2[ind] = np.inf

            return self._edgeinds[inds_min], np.sqrt(dists2_min), angles[inds_min]

        else:
            ind = np.argmin(d2)
            dist2_min = d2[ind]
            dists2_min = [dist2_min]
            inds_min = [ind]
            while (dist2_min < d_max**2) & (len(dists2_min) < n_best):
                d2[ind] = np.inf
                ind = np.argmin(d2)
                dist2_min = d2[ind]
                dists2_min.append(dist2_min)
                inds_min.append(ind)

            return self._edgeinds[inds_min], np.sqrt(dists2_min), angles[inds_min]

    def configure_sidewalks(self):
        print 'configure_sidewalks'
        nodes = self._net.nodes

        #are_remove_sidewalk = np.zeros(len(self._are_edge_valid), dtype = np.int32)
        are_remove_sidewalk = self.are_remove_sidewalk
        edges_osm = self.edges_osm
        ids_edge_osm = self._ids_edge_osm
        ids_edge_sumo = self._ids_edge_sumo
        ids_tonode = self._ids_tonode
        ids_fromnode = self._ids_fromnode
        shapes = self._edgeshapes

        alpha_crit = 90.0*np.pi/180.0

        #graph = nx.Graph()
        digraph = nx.DiGraph()

        for ind, id_edge_osm, shape, id_fromnode, id_tonode in zip(np.arange(len(ids_edge_osm)), ids_edge_osm, self._edgeshapes, ids_fromnode, ids_tonode):
            road = edges_osm[id_edge_osm]
            # if road.is_roundabout():# & (len(shape)>2):
            print '  check edge id_edge_osm', ids_edge_sumo[ind], 'no cycleped', (road.highway not in ['cycleway', 'path', 'pedestrian', 'stairs', 'steps', 'platform']), 'has sidewalk', (road.has_sidewalks(False) | road.has_sidewalks(True))
            # if (road.has_sidewalks(False)|road.has_sidewalks(True))\
            #    &(road.highway not in ['cycleway','path','pedestrian','stairs','steps','platform']):
            if 1:  # (road.highway not in ['path','pedestrian','stairs','steps','platform','footway']):
                print '   add_edge', ids_edge_sumo[ind], 'id_fromnode', id_fromnode, 'id_tonode', id_tonode, 'ind', ind

                shapearray = np.array(shape, dtype=np.float32)
                delta_out = shapearray[1]-shapearray[0]
                delta_in = shapearray[-2]-shapearray[-1]
                digraph.add_edge(id_fromnode, id_tonode,
                                 id_edge_osm=id_edge_osm,
                                 id_edge_sumo=ids_edge_sumo[ind],
                                 ind=ind,
                                 angle_out=np.arctan2(delta_out[0], delta_out[1]),
                                 angle_in=np.arctan2(delta_in[0], delta_in[1]),
                                 #road = road,
                                 )

                #graph.add_edge(id_tonode, id_fromnode,   id_edge_osm= id_edge_osm)
                #digraph.add_edge(id_fromnode, id_tonode )

        # print '  graph.nodes()',graph.nodes()
        for id_fromnode, tonodes in digraph.adj.items():
            print '  investigate id_fromnode', id_fromnode
            # dictionary with id_nnode as key and outgoing angle as value
            # if id_nnode is negative then the direction if from
            #  incoming edge from node -id_nnode
            directiondata = []
            for id_tonode, edge in tonodes.items():
                # print '    id_tonode',id_tonode
                directiondata.append((edge['angle_out'], id_tonode, edge['id_edge_sumo']))

            for id_node1, id_node2, edge in digraph.in_edges(id_fromnode, data=True):
                # print '    id_node1, id_node2',id_node1, id_node2
                directiondata.append((edge['angle_in'], -id_node1, edge['id_edge_sumo']))

            directiondata.sort()
            dirind_to_node = OrderedDict()
            node_to_dirind = OrderedDict()
            i = 0
            for angle, id_node, id_edge_sumo in directiondata:
                print '      ', i, 'id_node', id_node, 'angle', angle, 'id_edge_sumo', id_edge_sumo
                dirind_to_node[i] = id_node
                node_to_dirind[id_node] = i
                i += 1
            #
            digraph.node[id_fromnode]['dirind_to_node'] = dirind_to_node
            digraph.node[id_fromnode]['node_to_dirind'] = node_to_dirind

        # detect circular structures
        for id_node in list(digraph.nodes_iter()):

            node = digraph.node[id_node]
            print '\n  find loops for node', id_node  # ,node['dirind_to_node']
            for ind, id_nbnode in node['dirind_to_node'].iteritems():

                if id_nbnode < 0:
                    # incoming
                    ids_node = [id_node, -id_nbnode]
                    ids_nodepairs = [(-id_nbnode, id_node)]
                    # else:
                    #    # outgoing
                    #    ids_nodepairs = [(id_node, id_nbnode)]
                    #    ids_node = [id_node, id_nbnode]

                    ids_nodepairs = get_loop(digraph, ids_node, ids_nodepairs, n_node_max=self.n_node_sidewalkremove)

                    if len(ids_nodepairs) > 2:

                        # coords of nodes.....
                        #inds_edge_main, dists,angles = self.get_closest_edge_dist(point, ind, n_best = 20, d_max = dist_min_detect_cross)
                        #    for ind_edge_main, dist, angle in zip(inds_edge_main, dists,angles):
                        #        id_edge_osm_main,is_opp, nr = self.get_id_edge_osm_from_id_sumo(ids_edge_sumo[ind_edge_main])

                        for id_node, ids_nodepair in zip(ids_node[1:], ids_nodepairs):
                            id_fromnode, id_tonone = ids_nodepair
                            edge = digraph[id_fromnode][id_tonone]
                            id_sumo = edge['id_edge_sumo']
                            if id_tonone == id_node:
                                are_remove_sidewalk[id_sumo] = 1  # remove sidewalk from left

                            else:
                                if are_remove_sidewalk.has_key(id_sumo):
                                    pass
                                    # if are_remove_sidewalk[id_sumo] == 1:
                                    #    # let remove from left
                                    #    pass
                                    # else:
                                    #    # remove from right
                                    #    are_remove_sidewalk[id_sumo] = 2# remove sidewalk from right
                                else:
                                    are_remove_sidewalk[id_sumo] = 2  # remove sidewalk from right

                            print '   found edge', id_sumo, id_fromnode, id_tonone, id_node, 'is_remove_sidewalk', are_remove_sidewalk[id_sumo]

    def configure_roundabouts(self):
        print 'configure_roundabouts'
        nodes = self._net.nodes
        roundabouts = []
        edges_osm = self.edges_osm
        ids_edge_osm = self._ids_edge_osm
        ids_edge_sumo = self._ids_edge_sumo
        ids_tonode = self._ids_tonode
        ids_fromnode = self._ids_fromnode
        shapes = self._edgeshapes
        are_remove_sidewalk = self.are_remove_sidewalk
        alpha_crit = 90.0*np.pi/180.0

        graph = nx.Graph()
        digraph = nx.DiGraph()
        #junctionnet = {}
        for ind, id_edge_osm, shape, id_fromnode, id_tonode in zip(np.arange(len(ids_edge_osm)), ids_edge_osm, self._edgeshapes, ids_fromnode, ids_tonode):
            road = edges_osm[id_edge_osm]
            if road.is_roundabout():  # & (len(shape)>2):
                graph.add_edge(id_fromnode, id_tonode, ind=ind)
                digraph.add_edge(id_fromnode, id_tonode, ind=ind)

        # print '  graph.nodes()',graph.nodes()
        #ids_node_junctions = [[]]

        for ids_node_junction_list in nx.cycle_basis(graph):

            if len(ids_node_junction_list) > 3:  # at least 4 nodes for roundabout
                # print '    ids_node_junction_list',ids_node_junction_list
                ids_node_junction = np.array(ids_node_junction_list, dtype=np.int32)

                coords = nodes.coords[ids_node_junction]
                center = np.mean(coords, 0)
                deltas = coords-center
                angles = np.arctan2(deltas[:, 0], deltas[:, 1])
                inds = np.argsort(angles, axis=0)[::-1]  # reverse order
                # print '    inds',inds
                # print '    angels',angles[inds]
                # print '    ids_node_junction',ids_node_junction[inds]
                # print '    len(graph)',len(graph)
                ids_node_ok = list(ids_node_junction[inds])
                ids_node_ok.append(ids_node_ok[0])
                #n_node = len(ids_node_ok)
                # for ind in xrange(len(ids_node_ok)):
                #    if digraph.has_edge()

                ids_rbsedges_sumo = []
                for id_fromnode, id_tonode in zip(ids_node_ok[:-1], ids_node_ok[1:]):

                    ind = graph[id_tonode][id_fromnode]['ind']
                    if not digraph.has_edge(id_fromnode, id_tonode):
                        # print '  flip',ids_edge_sumo[ind]
                        # print '    shapes',shapes[ind]
                        # print '    shapes',shapes[ind][::-1]
                        ids_tonode[ind] = id_tonode
                        ids_fromnode[ind] = id_fromnode
                        shapes[ind][:] = shapes[ind][::-1]

                    ids_rbsedges_sumo.append(ids_edge_sumo[ind])
                    are_remove_sidewalk[ids_edge_sumo[ind]] = 1  # remove left sidewalk

                roundabouts.append((ids_rbsedges_sumo, ids_node_ok))

        #data = np.concatenate((angles.reshape(-1,1), ids_node_junction).reshape(-1,1),0)
        # graph.remove_nodes_from(ids_node_junction)
        self._roundabouts = roundabouts

    def get_id_osm_maindir(self, id_osm):
        if id_osm[0] == '-':
            return id_osm[1:]
        else:
            return id_osm

    def check_consistency(self):
        print 'check_consistency'
        nodelookup = {}

        for ind, id_fromnode,  id_tonode, id_edge_sumo in zip(np.arange(len(self._ids_edge_sumo)), self._ids_fromnode, self._ids_tonode, self._ids_edge_sumo):
            ids_fromtonode = (id_fromnode, id_tonode)
            if nodelookup.has_key(ids_fromtonode):
                print '  WARNING: %s double edge to %s detected' % (id_edge_sumo, nodelookup[ids_fromtonode]), 'from', id_fromnode, 'to', id_tonode, 'id_edge_sumo', id_edge_sumo, nodelookup[ids_fromtonode] == id_edge_sumo
                if nodelookup[ids_fromtonode] == id_edge_sumo:
                    # meand 2 edges with identical ID
                    self._are_edge_valid[ind] = False

            nodelookup[ids_fromtonode] = id_edge_sumo

    def configure_footpath(self):

        are_edge_valid = self._are_edge_valid
        edges_osm = self.edges_osm
        ids_edge_osm = self._ids_edge_osm
        ids_edge_sumo = self._ids_edge_sumo
        footpath_condig = ('footway', 'path', 'pedestrian')
        footpath_nomerge = ('footway', 'path', 'pedestrian', 'stairs', 'steps',
                            'platform', 'primary_link', 'secondary_link')
        footpath_merge_cross = ('cycleway',)
        prio_max = 7
        dist_min_detect = 15.0
        dist_min_detect_cross = 3.0
        dist_min_remove = 0.0

        n_best = 10

        length_max_cross = 30.0

        alpha_crit = 30.0*np.pi/180.0
        alpha_crit_cross = 30.0*np.pi/180.0
        print 'configure_footpath alpha_crit', alpha_crit
        #self.id_mode = ID_MODE_PED
        # highway=footway|path
        #self.highway = self._osmattrs.get('highway','footway')

        # Use footway=crossing for places where the footway crosses a street.
        # Use footway=sidewalk
        for ind, id_edge_osm, shape, length in zip(np.arange(len(ids_edge_osm)), ids_edge_osm, self._edgeshapes, self._length):
            road = edges_osm[id_edge_osm]

            if road.highway in footpath_condig:
                print '  check edge', ids_edge_sumo[ind], road.footway, (road.get_osmattr('tunnel') != 'yes'), (length > dist_min_remove), length
                #(road.footway != 'crossing') (road.footway != 'sidewalk')
                ids_osm_main = set()
                is_remove = True
                ip = 0

                if 0:  # (road.footway == 'sidewalk'):
                    # spacial sidewal merge. Sidewalks must be merged
                    # to a main road
                    for i in range(0, len(shape)):
                        point = shape[i]
                        is_foundmatch = False
                        inds_edge_main, dists, angles = self.get_closest_edge_dist(
                            point, ind, n_best=20, d_max=dist_min_detect)
                        #matchdata = {}
                        print '   check sidewalk point (%d/%d)' % (i+1, len(shape))
                        for ind_edge_main, dist, angle in zip(inds_edge_main, dists, angles):
                            id_edge_osm_main, is_opp, nr = self.get_id_edge_osm_from_id_sumo(
                                ids_edge_sumo[ind_edge_main])
                            road_main = edges_osm[id_edge_osm_main]
                            prio = road_main.get_priority(is_opp)
                            print '    check medge %s d=%.1fm al=%.1f' % (ids_edge_sumo[ind_edge_main], dist, angle/np.pi*180), prio, road_main.highway, (angle < alpha_crit), (dist < 2*dist_min_detect), (road_main.highway not in footpath_nomerge)
                            # print '   p1',shape[i-1],'pc',point,'p2',shape[i]
                            if (angle < 2*alpha_crit) & (dist < dist_min_detect) & (road_main.highway not in footpath_nomerge):
                                print '     add edge main edge %s' % ids_edge_sumo[ind_edge_main], (id_edge_osm_main, is_opp)
                                ids_osm_main.add((id_edge_osm_main, is_opp))
                                is_foundmatch = True
                                break

                        is_remove = len(ids_osm_main) > 0

                elif 0:  # (road.footway != 'crossing')|(length > length_max_cross):
                    # normal footpath merge

                    if (road.get_osmattr('tunnel') != 'yes') & (length > dist_min_remove):

                        for i in range(1, len(shape)):
                            point = shape[i-1]+0.5*(shape[i]-shape[i-1])
                            is_foundmatch = False
                            inds_edge_main, dists, angles = self.get_closest_edge_dist(
                                point, ind, n_best=20, d_max=dist_min_detect)
                            print '  check footpath point (%d/%d)' % (i+1, len(shape))
                            for ind_edge_main, dist, angle in zip(inds_edge_main, dists, angles):
                                id_edge_osm_main, is_opp, nr = self.get_id_edge_osm_from_id_sumo(
                                    ids_edge_sumo[ind_edge_main])
                                road_main = edges_osm[id_edge_osm_main]
                                prio = road_main.get_priority(is_opp)
                                print '   check medge %s d=%.1fm al=%.1f,%.2f' % (ids_edge_sumo[ind_edge_main], dist, angle/np.pi*180, angle), prio, road_main.highway, (angle < alpha_crit), (dist < dist_min_detect), (road_main.highway not in footpath_nomerge)
                                # print '   p1',shape[i-1],'pc',point,'p2',shape[i]
                                if (prio <= prio_max) & (angle < alpha_crit) & (dist < dist_min_detect) & (road_main.highway not in footpath_nomerge):
                                    print '       add edge main edge %s' % ids_edge_sumo[ind_edge_main], (id_edge_osm_main, is_opp)
                                    ids_osm_main.add((id_edge_osm_main, is_opp))
                                    is_foundmatch = True
                                    break

                            if not is_foundmatch:
                                ids_osm_main = set()
                                is_remove = False
                                break
                    else:
                        is_remove = False

                else:
                    # special merge at crossings
                    if (road.get_osmattr('tunnel') != 'yes'):

                        for i in range(1, len(shape)):
                            point = shape[i-1]+0.5*(shape[i]-shape[i-1])
                            is_foundmatch = False
                            inds_edge_main, dists, angles = self.get_closest_edge_dist(
                                point, ind, n_best=20, d_max=dist_min_detect_cross)
                            print '  check crossing point (%d/%d)' % (i+1, len(shape))
                            for ind_edge_main, dist, angle in zip(inds_edge_main, dists, angles):
                                id_edge_osm_main, is_opp, nr = self.get_id_edge_osm_from_id_sumo(
                                    ids_edge_sumo[ind_edge_main])
                                road_main = edges_osm[id_edge_osm_main]
                                prio = road_main.get_priority(is_opp)
                                print '   check medge %s d=%.1fm al=%.1f' % (ids_edge_sumo[ind_edge_main], dist, angle/np.pi*180), prio, road_main.highway, (angle < alpha_crit_cross), (dist < dist_min_detect_cross), (road_main.highway in footpath_merge_cross)
                                # print '   p1',shape[i-1],'pc',point,'p2',shape[i]
                                # if (prio<=prio_max)&(((angle<alpha_crit_cross)&(dist < dist_min_detect))|(dist < dist_min_detect_cross))& (road_main.highway in footpath_merge_cross):
                                if (prio <= prio_max) & (angle < alpha_crit_cross) & (dist < dist_min_detect_cross) & (road_main.highway in footpath_merge_cross):
                                    print '       add edge main edge %s' % ids_edge_sumo[ind_edge_main], (id_edge_osm_main, is_opp)
                                    ids_osm_main.add((id_edge_osm_main, is_opp))
                                    is_foundmatch = True
                                    break

                            if not is_foundmatch:
                                ids_osm_main = set()
                                is_remove = False
                                break

                    else:
                        is_remove = False

                if is_remove:
                    # footpath can be matched to main roads

                    for id_osm_main, is_opp in ids_osm_main:
                        print '    found', id_edge_osm, 'match with', id_osm_main, 'is_opp', is_opp
                        edges_osm[id_osm_main].add_sidewalk(road, is_opp=is_opp)

                    # make sure footpath is invalid
                    print '  eliminate edge', ids_edge_sumo[ind]
                    are_edge_valid[ind] = False

    def _get_osmid_from_attr(self, attrs):
        id_osm_str = attrs['osmid']

        if type(id_osm_str) in cm.STRINGTYPES:
            if id_osm_str.count(';') > 0:
                return int(id_osm_str.split(';')[0])
            else:
                return int(id_osm_str)
        else:
            return id_osm_str

    def _get_osmid_from_nxattr(self, attrs):
        id_osm_data = attrs['osmid']

        if hasattr(id_osm_data, '__iter__'):
            return str(id_osm_data[0])
        else:
            return str(id_osm_data)

    def is_reverse(self, id_osm, id_fromnode, id_tonode):
        """
        Method to detect whether osm edge got reversed in osmnx 
        with respect to OSM data.
        """

        ids_osmnode = self.edges_osm[id_osm].ids_osmnode
        # print 'is_revese id_fromnode_sumo, id_tonode_sumo',id_fromnode, id_tonode,ids_osmnode
        if id_fromnode in ids_osmnode:
            ind_from = ids_osmnode.index(id_fromnode)

        else:
            ind_from = -np.inf

        if id_tonode in ids_osmnode:
            ind_to = ids_osmnode.index(id_tonode)
        else:
            ind_to = np.inf
        # print '  ind_from',ind_from,'ind_to',ind_to,'return', ind_from > ind_to
        return ind_from > ind_to

    def import_nx(self, graphx, projparams=None, offset=None, highway_bidir=['rail', ]):
        """
        Import an networkx graph into net
        """
        print 'import_nx'
        net = self._net
        edges_osm = self.edges_osm
        nodes_osm = self.nodes_osm
        idmap = self._map_id_edge_sumo_to_doubles
        get_id_mode = net.modes.get_id_mode

        if not hasattr(graphx, 'adj'):
            graphx.adj = graphx._adj

        if not hasattr(graphx, 'node'):
            graphx.node = graphx._node

        # initialize main vectors
        #n_nodes = len(graphx.adj)
        #ids_nodes_sumo = np.zeros(n_nodes, dtype = np.object)
        #ids_nodes_sumo = np.array(graphx.adj.keys(), dtype = np.int32)
        gxnodes = graphx.node
        nodes = net.nodes
        edges = net.edges
        n_node = len(graphx.adj)
        ids_node = net.nodes.add_rows(n_node)

        id_sumonodetypes = nodes.types.choices
        id_nodetype_default = id_sumonodetypes['priority']
        id_nodetype_tl = id_sumonodetypes['traffic_light']
        nodetypemap = {'motorway_junction': id_sumonodetypes['zipper'],
                       'traffic_signals': id_sumonodetypes['traffic_light'],
                       'crossing': id_sumonodetypes['priority'],
                       'turning_circle': id_sumonodetypes['dead_end'],
                       'turning_loop': id_sumonodetypes['dead_end'],
                       'miniroundabout': id_sumonodetypes['priority'],
                       'stop': id_sumonodetypes['allway_stop'],
                       }

        lonlats_node = np.zeros((n_node, 2), dtype=np.float32)
        ids_node_sumo = np.zeros(n_node, dtype=np.object)
        nodetypes = np.zeros(n_node, dtype=np.int32)
        elevation = np.zeros(n_node, dtype=np.float32)
        #nodetype = np.zeros(n_node, dtype = np.object)

        # for id_sumo, nodedict in zip(nodes.ids_sumo[ids_node],:

        #ids_node_sumo = np.zeros(n_node, dtype = np.int32)

        i_node = -1
        for id_fromnode, id_fromnode_sumo, outgoingdicts in zip(ids_node, graphx.adj.keys(), graphx.adj.values()):
            i_node += 1
            #ids_node_sumo[i_node] = id_fromnode_sumo
            print '  ', id_fromnode, id_fromnode_sumo, gxnodes[id_fromnode_sumo]  # nodes.ids_sumo[id_fromnode]
            lonlats_node[i_node] = [gxnodes[id_fromnode_sumo]['x'], gxnodes[id_fromnode_sumo]['y']]
            ids_node_sumo[i_node] = str(id_fromnode_sumo)
            elevation[i_node] = float(gxnodes[id_fromnode_sumo].get('elevation', 0.0))
            nxnodetype = gxnodes[id_fromnode_sumo].get('highway', 'default')
            # if nxnodetype == 'crossing':...
            # if nxnodetype = 'bus_stop'
            # TODO:!!! pay attention to zipper nodes and check that there
            # are only 2 incoming edges
            nodetypes[i_node] = nodetypemap.get(nxnodetype, id_nodetype_default)

        nodes.ids_sumo[ids_node] = ids_node_sumo
        nodes.types[ids_node] = nodetypes
        nodes.types_tl[ids_node[nodetypes == id_nodetype_tl]] = nodes.types_tl.choices['static']

        if projparams is None:
            print '  take first node to estimate projection', (lonlats_node[0][0], lonlats_node[0][1])
            projparams = guess_utm_from_coord([lonlats_node[0][0], lonlats_node[0][1]])
        print '  projparams', projparams
        proj = pyproj.Proj(projparams)
        net.set_projparams(projparams)

        xx, yy = proj(lonlats_node[:, 0], lonlats_node[:, 1])
        zz = elevation
        if offset == None:
            offset = np.array([-np.min(xx), -np.min(yy)], dtype=np.float32)
            offset3d = np.array([-np.min(xx), -np.min(yy), 0.0], dtype=np.float32)
        net.set_offset(offset)

        # Format of Boundary box [MinX, MinY ,MaxX, MaxY ]
        net.set_boundaries([0, 0, np.max(xx)+offset[0], np.max(yy)+offset[1]],
                           [np.min(lonlats_node[:, 0]), np.min(lonlats_node[:, 1]), np.max(lonlats_node[:, 0]), np.max(lonlats_node[:, 1])])

        # print '  nodes.coords[ids_node].shape', nodes.coords[ids_node].shape
        # print '  (xx+offset[0]).shape', (xx+offset[0]).shape

        #nodes.coords[ids_node][:,0] = xx+offset[0]
        #nodes.coords[ids_node][:,1] = yy+offset[1]

        # print '  xx',np.array(xx, dtype = np.float32).reshape(-1,1).shape
        # print '  yy',np.array(yy, dtype = np.float32).reshape(-1,1).shape
        # print '  zz.reshape(-1,1)',zz.reshape(-1,1).shape
        nodes.coords[ids_node] = np.concatenate((np.array(xx, dtype=np.float32).reshape(-1, 1),
                                                 np.array(yy, dtype=np.float32).reshape(-1, 1),
                                                 zz.reshape(-1, 1)), 1) + offset3d

        ids_edge_sumo = []  # np.zeros(n_edge, dtype = np.object)
        ids_edge_osm = []  # np.zeros(n_edge, dtype = np.int32)
        inds_edge_opp = []  # -1*np.ones(n_edge, dtype = np.int32)

        ids_fromnode = []  # np.zeros(n_edge, dtype = np.int32)
        ids_tonode = []  # np.zeros(n_edge, dtype = np.int32)
        #lengths = np.zeros(n_edge, dtype = np.float32)
        shapes = []  # np.zeros(n_edge, dtype = np.object)
        #names = np.zeros(n_edge, dtype = np.object)
        # types_spread = np.zeros(n_edge, dtype = np.object)  # 0=center, 1 = right
        get_id_node = nodes.ids_sumo.get_id_from_index
        i_edge = -1
        i_node = -1

        # dictionary with id_osm as key and (id_fromnode,id_tonode) as value
        # this is to prevent, that a return way with the same id_osm
        # is being created
        self._edgelookup = {}

        # debug
        self._nodelookup = {}

        for id_fromnode, id_fromnode_sumo, outgoingdicts in zip(ids_node, graphx.adj.keys(), graphx.adj.values()):
            i_node += 1
            # print '  id_fromnode_sumo',i_node, id_fromnode, id_fromnode_sumo
            for id_tonode_sumo, edgedatalist in outgoingdicts.iteritems():

                edgeattrs = edgedatalist[0]

                id_osm = self._get_osmid_from_nxattr(edgeattrs)

                if self.is_edge_eligible(id_osm, id_fromnode_sumo, id_tonode_sumo):

                    id_tonode = get_id_node(str(id_tonode_sumo))

                    if edgeattrs.has_key('geometry'):
                        # print '    geometry',edgeattrs['geometry']
                        lonlats = edgeattrs['geometry'].xy
                        xx, yy = proj(lonlats[0], lonlats[1])
                        zz = np.zeros((len(xx), 1),  dtype=np.float32)
                        # print '    xx',xx
                        # print '    yy',yy
                        # print '    zz',zz
                        shape1 = np.concatenate((np.array(xx, dtype=np.float32).reshape(-1, 1),
                                                 np.array(yy, dtype=np.float32).reshape(-1, 1),
                                                 zz), 1) + offset3d

                        # elevation interpolation
                        # print '  shape1',shape1
                        # print '  dists',np.sqrt(np.sum((shape1[1:,:]-shape1[:-1,:])**2,1))
                        cumlength = np.cumsum(np.sqrt(np.sum((shape1[1:, :]-shape1[:-1, :])**2, 1)))
                        # print '  cumlength',cumlength
                        length = cumlength[-1]
                        vfrom, vto = nodes.coords[[id_fromnode, id_tonode]]
                        height_delta = vto[2]-vfrom[2]
                        height_offset = vfrom[2]
                        height = height_offset+cumlength/length*height_delta

                        # print '  cumlength.shape',cumlength.shape
                        # print '  height_offset',height_offset
                        # print '  height_delta',height_delta

                        # print '  height',height.shape,height
                        shape1[0, 2] = height_offset
                        shape1[1:, 2] = height

                    else:
                        shape1 = nodes.coords[[id_fromnode, id_tonode]]

                    #
                    # make foreward edge

                    # print '    shape=\n',list(shape)
                    # print '    shape=\n',list(shape)
                    i_edge += 1
                    ids_edge_osm.append(id_osm)
                    #edgenum = get_edgenumber(id_osm)
                    ids_edge_sumo.append(self.get_id_edge_sumo(id_osm))

                    road = edges_osm[id_osm]

                    is_oneway = road.is_oneway()
                    print '    check id_osm', id_osm, 'is_oneway', is_oneway, "id_fromnode_sumo %s id_tonode_sumo %s" % (id_fromnode_sumo, id_tonode_sumo)
                    if is_oneway:
                        is_reverse = False
                        shapes.append(list(shape1))
                        ids_fromnode.append(id_fromnode)
                        ids_tonode.append(id_tonode)

                    else:
                        is_reverse = self.is_reverse(id_osm, id_fromnode_sumo, id_tonode_sumo)
                        print 'is_revese', is_reverse, edges_osm[id_osm].ids_osmnode

                        # if (not is_oneway) & is_reverse:#self.is_reverse(id_osm, id_fromnode_sumo, id_tonode_sumo):
                        if is_reverse:
                            #shape1 = shape1[::-1]
                            shapes.append(list(shape1)[::-1])
                            ids_fromnode.append(id_tonode)
                            ids_tonode.append(id_fromnode)
                            print '      created edge %s from' % ids_edge_sumo[-1], id_tonode_sumo, 'to', id_fromnode_sumo
                            #id_node_temp = id_fromnode
                            #id_fromnode = id_tonode
                            #id_tonode = id_node_temp
                        else:
                            shapes.append(list(shape1))
                            ids_fromnode.append(id_fromnode)
                            ids_tonode.append(id_tonode)
                            print '      created edge %s from' % ids_edge_sumo[-1], id_fromnode_sumo, 'to', id_tonode_sumo
                            # ids_fromnode.append(id_fromnode)
                            # ids_tonode.append(id_tonode)

                    # print '    id_osm,id(road)', id_osm, id(road), ids_edge_osm[i_edge]

                    if not is_oneway:
                        # set edge for opposite direction
                        inds_edge_opp.append(i_edge + 1)  # index of opposite edge
                        i_edge += 1
                        if is_reverse:
                            shapes.append(list(shape1))
                            ids_fromnode.append(id_fromnode)
                            ids_tonode.append(id_tonode)
                        else:
                            shapes.append(list(shape1)[::-1])
                            ids_fromnode.append(id_tonode)
                            ids_tonode.append(id_fromnode)

                        ids_edge_sumo.append(self.get_id_edge_sumo('-'+id_osm))
                        ids_edge_osm.append(id_osm)
                        inds_edge_opp.append(-2)  # mark that this is the opposite edge
                        print '      created reverse ids_edge_sumo', ids_edge_sumo[i_edge], 'from', ids_fromnode[-1], 'to', ids_tonode[-1]

                    else:
                        inds_edge_opp.append(-1)

        self._are_edge_valid = np.ones(len(ids_edge_sumo), dtype=np.bool)
        are_remove_sidewalk = {}  # np.zeros(len(self._are_edge_valid), dtype = np.int32)
        self.are_remove_sidewalk = are_remove_sidewalk

        # print '  inds_valid before',inds_valid
        self._edgeshapes = np.array(shapes, dtype=np.object)
        #ids_edge_osm_to_inds = {}
        # for id_edge_osm, ind in zip(ids_edge_osm, np.arange(len(ids_edge_osm), dtype = np.int32)):
        #    ids_edge_osm_to_inds[id_edge_osm] = ind
        #self._ids_edge_osm_to_inds = ids_edge_osm_to_inds
        self._ids_edge_osm = np.array(ids_edge_osm, dtype=np.object)
        self._ids_edge_sumo = np.array(ids_edge_sumo, dtype=np.object)
        self._length = get_length_polypoints(self._edgeshapes)

        self._ids_fromnode = np.array(ids_fromnode, dtype=np.int32)
        self._ids_tonode = np.array(ids_tonode, dtype=np.int32)
        self.make_segment_edge_map()

        self.check_consistency()
        self.configure_roundabouts()

        if self.is_simplify_footpath:
            self.configure_sidewalks()
            self.configure_footpath()

        inds_valid = np.flatnonzero(self._are_edge_valid)
        n_edge = len(inds_valid)
        #ids_edge_sumo_remove_sidewalk = self._ids_edge_sumo[are_remove_sidewalk]

        # print '  n_edge',n_edge,'of',len(ids_edge_sumo)
        # print '  inds_valid',inds_valid
        # print '  shapes',  shapes
        ids_edges = edges.multimake(self._ids_edge_sumo[inds_valid],
                                    ids_fromnode=self._ids_fromnode[inds_valid],
                                    ids_tonode=self._ids_tonode[inds_valid],
                                    #lengths = lengths,
                                    shapes=self._edgeshapes[inds_valid],
                                    #names = names,
                                    )

        for id_edge, id_fromnode, id_tonode in zip(ids_edges, edges.ids_fromnode[ids_edges], edges.ids_tonode[ids_edges]):
            nodes.add_outgoing(id_fromnode, id_edge)
            nodes.add_incoming(id_tonode, id_edge)

        # check zipper nodes
        id_zipper = id_sumonodetypes['zipper']
        for id_node, nodetype in zip(ids_node, nodes.types[ids_node]):
            if nodetype == id_zipper:
                ids_incoming = nodes.ids_incoming[id_node]
                if ids_incoming is not None:
                    if len(ids_incoming) > 2:
                        nodes.types[id_node] = id_nodetype_default
            else:
                nodes.types[id_node] = id_nodetype_default

        roundabouts = net.roundabouts
        for ids_rbedge_sumo, ids_rbnode in self._roundabouts:
            # print '    ',
            roundabouts.make(ids_edge=edges.ids_sumo.get_ids_from_indices(ids_rbedge_sumo),
                             ids_node=ids_rbnode,
                             )

        # print '  shapes[0:9]',shapes[0:9]
        # print '  edges.shapes[1:10]',edges.shapes[1:10]
        # print '  edges.shapes.get_value()[0:9]',edges.shapes.get_value()[0:9]
        #net.edges.update(is_update_lanes = False)
        # enrich edges with osmattributes
        # print '  ids_edge_osm',ids_edge_osm
        inds = np.arange(len(ids_edges))
        for id_edge, id_edge_osm, ind_edge_opp in zip(
            ids_edges, self._ids_edge_osm[inds_valid],
            np.array(inds_edge_opp, dtype=np.int32)[inds_valid],
        ):

            id_sumo = edges.ids_sumo[id_edge]

            if id_sumo[0] > '-':  # it is not an opposite edge
                road = edges_osm[id_edge_osm]

                print '  configure way id_edge_osm', id_edge_osm, id_sumo, are_remove_sidewalk.get(id_sumo, 0)
                road.configure_edge(id_edge, net, is_remove_sidewalk=are_remove_sidewalk.get(id_sumo, 0))
                #id_edge_opp = self.get_id_edge_opp
                id_sumo_opp = '-'+id_sumo
                if edges.ids_sumo.has_index(id_sumo_opp):
                    print '  configure opposite way id_edge_osm', id_edge_osm, id_sumo_opp, are_remove_sidewalk.get(id_sumo_opp, 0)
                    road.configure_edge(edges.ids_sumo.get_id_from_index(id_sumo_opp),
                                        net, is_opp=True,
                                        is_remove_sidewalk=are_remove_sidewalk.get(id_sumo_opp, 0))
