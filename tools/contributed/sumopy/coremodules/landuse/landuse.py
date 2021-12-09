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

# @file    landuse.py
# @author  Joerg Schweizer
# @date   2012

import maps
from agilepy.lib_base.processes import Process, CmlMixin
from collections import OrderedDict
from numpy import random
import numpy as np
from coremodules.misc.shapeformat import ShapefileImporter
from coremodules.network.network import SumoIdsConf, MODES
from agilepy.lib_base.misc import string_to_float
from agilepy.lib_base.geometry import *
import agilepy.lib_base.xmlman as xm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.classman as cm
from coremodules.modules_common import *
import os
import sys
import time
import shutil
from xml.sax import saxutils, parse, handler
if __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    SUMOPYDIR = os.path.join(APPDIR, '..', '..')
    sys.path.append(SUMOPYDIR)
try:
    import pyproj
except:
    from mpl_toolkits.basemap import pyproj

#from coremodules.network.routing import get_mincostroute_edge2edges

try:
    from shapely.geometry import MultiPoint, Polygon
    IS_SHAPELY = True
except:
    IS_SHAPELY = False


def clean_osm(filepath_in, filepath_out):
    """
    Clean osm file from strange characters that compromize importing.
    """
    #
    substitutes = {"&quot;": "'", "&": "+"}
    fd_in = open(filepath_in, 'r')
    fd_out = open(filepath_out, 'w')
    for line in fd_in.readlines():
        for oldstr, newstr in substitutes.iteritems():
            line = line.replace(oldstr, newstr)
        fd_out.write(line)
    fd_in.close()
    fd_out.close()


class LanduseTypes(am.ArrayObjman):
    def __init__(self, parent, is_add_default=True, **kwargs):

        self._init_objman(ident='landusetypes',
                          parent=parent,
                          name='Landuse types',
                          info='Table with information on landuse types',
                          **kwargs)

        self._init_attributes()
        if is_add_default:
            self.add_types_default()

    def _init_attributes(self):
        # landuse types table
        self.add_col(am.ArrayConf('typekeys', '',
                                  is_index=True,
                                  dtype='object',  # actually a string of variable length
                                  perm='r',
                                  name='Type',
                                  info='Type of facility. Must be unique, because used as key.',
                                  ))

        # self.add_col(am.ArrayConf( 'osmid', '',
        #                                dtype = 'object',# actually a string of variable length
        #                                perm='rw',
        #                                name = 'Name',
        #                                info = 'Name of facility type used as reference in OSM.',
        #                                ))

        self.add_col(am.ArrayConf('colors', np.ones(4, np.float32),
                                  dtype=np.float32,
                                  metatype='color',
                                  perm='rw',
                                  name='Colors',
                                  info="Color corrispondig to landuse type as RGBA tuple with values from 0.0 to 1.0",
                                  xmltag='color',
                                  ))

        self.add_col(am.ArrayConf('descriptions', '',
                                  dtype='object',  # actually a string of variable length
                                  perm='r',
                                  name='Info',
                                  info='Information about this landuse.',
                                  ))

        self.add_col(am.ArrayConf('are_area', False,
                                  perm='rw',
                                  name='is area',
                                  info='True if this is an area of particular use, instead of a single facility. An area can include several facilities. If False, then it is dealt with a single facility.',
                                  ))

        self.add_col(am.ListArrayConf('osmfilters',
                                      perm='r',
                                      name='OSM filter',
                                      info='List of openstreetmap filters that allow to identify this facility type.',
                                      ))

        # this will remove all previous and setnw
        if len(self) > 0:
            self.clear()
            self.add_types_default()

    def get_landusetype_facility_from_area(self, id_landuse_area):
        # get typekey
        typekey_area = self.typekeys[id_landuse_area]
        typekey_info = typekey_area.split('_')
        if len(typekey_info) == 2:
            if typekey_info[1] == 'area':
                return self.typekeys.get_id_from_index(typekey_info[0])
            else:
                # no area, return unchanged
                return id_landuse_area
        else:
            # no area, return unchanged
            return id_landuse_area

    def format_ids(self, ids):
        return ','.join(self.typekeys[ids])

    def get_id_from_formatted(self, idstr):
        return self.typekeys.get_id_from_index(idstr)

    def get_ids_from_formatted(self, idstrs):
        return self.typekeys.get_ids_from_indices_save(idstrs.split(','))

    def add_types_default(self):
        # default types
        self.add_row(typekeys='leisure',
                     descriptions='Facilities which offer leasure type activities',
                     osmfilters=['historic.church', 'building.church', 'sport', 'sport.*'],
                     colors=(0.2, 0.5, 0.3, 0.7)
                     )
        # leisure.nature_reserve is actually an area so remove 'leisure.*'

        self.add_row(typekeys='commercial',
                     descriptions='Facility with trade, offices, banks, shopping opportunitties, etc.',
                     osmfilters=['building.hospital', 'building.retail', 'building.shop',
                                 'building.restaurant', 'building.cafe', 'restaurant', 'shop.*', 'building.commercial', ],
                     colors=(0.6171875,  0.6171875,  0.875, 0.7),
                     )

        self.add_row(typekeys='industrial',
                     descriptions='industrial production facilities.',
                     osmfilters=['building.industrial'],
                     colors=(0.89453125,  0.65625,  0.63671875, 0.7),
                     )

        self.add_row(typekeys='parking',
                     descriptions='Areas reserved for car parking. This can be either area or building.',
                     osmfilters=['building.parking', ],
                     colors=(0.52734375,  0.875,  0.875, 0.7),
                     )

        self.add_row(typekeys='residential',
                     descriptions='Facilities with residential use',
                     osmfilters=['building.house', 'building.apartments', 'building.*', 'building'],
                     colors=(0.921875,  0.78125,  0.4375, 0.7),
                     )

        self.add_row(typekeys='mixed',
                     descriptions='Facilities with mixed land use, which cannot be clearly assigned to one of the other landuse types.',
                     osmfilters=[],
                     colors=(0.5, 0.9, 0.5, 0.7),
                     )

        self.add_row(typekeys='sink',
                     descriptions='Areas where vehicles disappear (evaporate). These zones are used for turn-flow demand models in order to avoid the creation of routes with loops.',
                     osmfilters=[],
                     colors=(0.5,  0.0,  0.1, 1.0),
                     )
        self.add_row(typekeys='education',
                     descriptions='Educational facilities such as schools, universities',
                     # with amenity and landuse, we should look inside and assign all buildings
                     osmfilters=['building.school', 'building.university'],
                     colors=(0.89453125,  0.65625,  0.89453125, 0.7),
                     )

        self.add_row(typekeys='leisure_area',
                     descriptions='Areas on which leasure type activitiesbare offered',
                     osmfilters=['leisure.park', 'park', 'amenity.park',
                                 'landuse.park', 'amenity.sport', 'landuse.sport'],
                     are_area=True,
                     colors=(0.2, 0.5, 0.3, 0.7)
                     )

        self.add_row(typekeys='industrial_area',
                     descriptions='Area with industrial production facilities.',
                     osmfilters=['amenity.industrial', 'landuse.industrial', ],
                     are_area=True,
                     colors=(0.89453125,  0.65625,  0.63671875, 0.7),
                     )

        self.add_row(typekeys='commercial_area',
                     descriptions='Areas with trade, offices, banks, shopping opportunitties, or other customer services.',
                     osmfilters=['amenity.hospital', 'amenity.commercial', 'landuse.commercial', ],
                     colors=(0.6171875,  0.6171875,  0.875, 0.7),
                     are_area=True,
                     )

        self.add_row(typekeys='residential_area',
                     descriptions='Areas with residential facilities.',
                     osmfilters=['amenity.residential', 'landuse.residential'],
                     colors=(0.921875,  0.78125,  0.4375, 0.7),
                     are_area=True,
                     )

        self.add_row(typekeys='mixed_area',
                     descriptions='Facilities with mixed land use, which cannot be clearly assigned to one of the other landuse types.',
                     osmfilters=[],
                     colors=(0.5, 0.9, 0.5, 0.7),
                     are_area=True,
                     )

        self.add_row(typekeys='education_area',
                     descriptions='Educational facilities such as schools, universities',
                     osmfilters=['landuse.university', 'landuse.school', 'amenity.school', 'amenity.university'],
                     colors=(0.89453125,  0.65625,  0.89453125, 0.7),
                     are_area=True,
                     )

        self.add_row(typekeys='parking_area',
                     descriptions='Areas reserved for car parking. This can be either area or building.',
                     osmfilters=['amenity.parking'],
                     colors=(0.52734375,  0.875,  0.875, 0.7),
                     are_area=True,
                     )


class Zones(am.ArrayObjman):
    def __init__(self, parent, edges, **kwargs):
        self._init_objman(ident='zones', parent=parent,
                          name='Zones',
                          info='Traffic Zones which can be used for zone-to-zone traffic transport demand or to specify zones for traffic evaporation.',
                          is_plugin=True,
                          version=0.3,
                          **kwargs)

        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        # print 'Zones._init_attributes',hasattr(self,'are_evaporate'),'version',self.get_version()
        edges = self.parent.get_net().edges
        self.add_col(SumoIdsConf('Zone', name='Name', perm='rw', info='Unique short name or ID to identify zone.'))

        self.add_col(am.ArrayConf('names_extended', '',
                                  dtype=np.object,
                                  groupnames=['parameter'],
                                  perm='rw',
                                  name='Extended name',
                                  info='Extended, human readable name, no uniqueness required, not used as key.',
                                  ))

        self.add_col(am.IdsArrayConf('ids_landusetype',  self.parent.landusetypes,
                                     id_default=6,
                                     #choices = self.parent.landusetypes.typekeys.get_indexmap(),
                                     #choiceattrname = 'typekeys',
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Type',
                                     info='Zone type. This is actually the landuse type.',
                                     ))

        self.add_col(am.ArrayConf('coords',  np.zeros(3, dtype=np.float32),
                                  groupnames=['state'],
                                  perm='r',
                                  name='Coords',
                                  unit='m',
                                  info='Zone center coordinates.',
                                  is_plugin=True,
                                  ))

        self.add_col(am.ListArrayConf('shapes',
                                      groupnames=['_private'],
                                      perm='rw',
                                      name='Shape',
                                      unit='m',
                                      info='List of 3D Shape coordinates delimiting a zone.',
                                      is_plugin=True,
                                      ))

        self.add_col(am.ListArrayConf('areas',
                                      groupnames=['state'],
                                      perm='rw',
                                      name='Area',
                                      unit='km^2',
                                      info='Area of the zone in square kilometers.',
                                      is_plugin=True,
                                      ))

        # self.add_col(am.IdlistsArrayConf( 'ids_edges_orig', edges,
        #                                    groupnames = ['state'],
        #                                    name = 'IDs orig edges',
        #                                    info = 'List with IDs of network edges that can be used as origins for trips in this zone.',
        #                                    ))

        # self.add_col(am.IdlistsArrayConf( 'ids_edges_dest', edges,
        #                                    groupnames = ['state'],
        #                                    name = 'IDs dest edges',
        #                                    info = 'List with IDs of network edges that can be used as origins for trips in this zone.',
        #                                    ))

        if self.get_version() < 0.2:
            self.delete('priority_max')
            self.delete('speed_max')
            self.delete('n_edges_min_length')
            self.delete('n_edges_max_length')

        # self.add_col(am.ListArrayConf( 'probs_edges_orig',
        #                                groupnames = ['_private'],
        #                                #perm='rw',
        #                                name = 'edge probs origin',
        #                                info = 'Probabilities of edges to be at the origin of a trip departing from this zone.',
        #                                ))
        # self.add_col(am.ListArrayConf( 'probs_edges_dest',
        #                                groupnames = ['_private'],
        #                                #perm='rw',
        #                                name = 'edge probs dest',
        #                                info = 'Probabilities of edges to be a destination of a trip arriving at this zone.',
        #                                ))

        self.add_col(am.IdlistsArrayConf('ids_edges_inside', edges,
                                         groupnames=['state'],
                                         name='Edge IDs inside',
                                         info='List with IDs of network edges that are entirely inside each zone.',
                                         ))

        if self.get_version() < 0.3:
            ids = self.get_ids()
            self.ids_edges_inside[ids] = self.ids_edges_orig[ids]
            self.delete('ids_edges_dest')
            self.delete('ids_edges_orig')
            self.delete('probs_edges_orig')
            self.delete('probs_edges_dest')

        self.set_version(0.3)

    def _init_constants(self):
        self._proj = None
        self._offset = None
        self._mode_to_edges_inside = {}
        attrsman = self.get_attrsman()
        attrsman.do_not_save_attrs(['_proj', '_offset', '_mode_to_edges_inside'])
        # self.set_version(0.2)

    def make(self, zonename='',
             coord=np.zeros(3, dtype=np.float32),
             shape=[],
             id_landusetype=-1):
        """
        Add a zone
        """
        # print 'Zone.make',coord
        # print '  shape',type(shape),shape

        self.get_coords_from_shape(shape)
        id_zone = self.add_row(coords=self.get_coords_from_shape(shape),
                               shapes=shape, areas=find_area(shape)/1000000.,
                               ids_landusetype=id_landusetype,
                               )
        if zonename == '':
            self.ids_sumo[id_zone] = str(id_zone)
        else:
            self.ids_sumo[id_zone] = zonename

        self.identify_zoneedges(id_zone)
        # print '   shapes\n',self.shapes.value
        # print '   zone.shapes[id_zone]\n',self.shapes[id_zone]

        return id_zone

    def format_ids(self, ids):
        return ','.join(self.ids_sumo[ids])

    def get_id_from_formatted(self, idstr):
        return self.ids_sumo.get_id_from_index(idstr)

    def get_ids_from_formatted(self, idstrs):
        return self.ids_sumo.get_ids_from_indices_save(idstrs.split(','))

    def get_coords_from_shape(self, shape):
        # print 'get_coords_from_shape',np.array(shape),np.mean(np.array(shape),0)
        return np.mean(np.array(shape), 0)

    def del_element(self, id_zone):
        # print 'del_element',id_zone
        self.del_row(id_zone)

    def get_edges(self):
        return self.ids_edges_inside.get_linktab()

    def update_zones(self):
        """Update all dependent attributes of all zones"""
        ids = self.get_ids()
        for id_zone, shape in zip(ids, self.shapes[ids]):
            self.coords[id_zone] = self.get_coords_from_shape(shape)
            self.areas[id_zone] = find_area(shape)/1000000.0
            self.identify_zoneedges(id_zone)

    def refresh_zoneedges(self):
        for _id in self.get_ids():
            self.identify_zoneedges(_id)
            # self.make_egdeprobs(_id)
            self.coords[_id] = self.get_coords_from_shape(self.shapes[_id])

    def refresh_zonearea(self):
        """
        Update the area of each zone
        """
        ids_zone = self.get_ids()
        for id_zone in ids_zone:
            self.identify_zonearea(id_zone)

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        # self.zones.update_netoffset(deltaoffset)
        self.coords.value[:, :2] = self.coords.value[:, :2] + deltaoffset
        shapes = self.shapes.value
        for i in xrange(len(shapes)):
            s = np.array(shapes[i])
            s[:, :2] = s[:, :2] + deltaoffset
            shapes[i] = list(s)

    def identify_zonearea(self, id_zone):
        # print 'identify_zonearea',id_zone
        shape = self.shapes[id_zone]
        self.areas[id_zone] = find_area(shape)/1000000.0

    def identify_zoneedges(self, id_zone):
        print 'identify_zoneedges of id_zone', id_zone

        if len(self.shapes[id_zone]) >= 3:
            # must have at least 3 vertices to be an area

            inds_within = []
            ind = 0
            # print '  self.shapes[id_zone]',self.shapes[id_zone]
            if not IS_SHAPELY:
                polygon = np.array(self.shapes[id_zone])[:, :2]
                # print '  polygon',type(polygon)
                for polyline in self.get_edges().shapes.get_value():

                    # print '    np.array(polyline)[:,:2]',np.array(polyline)[:,:2],type(np.array(polyline)[:,:2])
                    if is_polyline_in_polygon(np.array(polyline)[:, :2], polygon):
                        inds_within.append(ind)
                    ind += 1
            else:
                polygon = Polygon(np.array(self.shapes[id_zone])[:, :2])
                for polyline in self.get_edges().shapes.get_value():
                    if MultiPoint(np.array(polyline)[:, :2]).within(polygon):
                        inds_within.append(ind)
                    ind += 1

            # print '  inds_within',inds_within

            # select and determine weights
            self.ids_edges_inside[id_zone] = self.get_edges().get_ids(inds_within)
            print '  found', len(self.ids_edges_inside[id_zone]), 'edges'
            if len(self.ids_edges_inside[id_zone]) == 0:
                print 'WARNING in identify_zoneedges: no edges found in zone', id_zone

    def get_zoneedges_by_mode_fast(self, id_zone, id_mode, speed_max=None,
                                   modeconst_excl=0.0, modeconst_mix=0.0,
                                   weights=None, fstar=None):
        """
        Returns a list of edge IDs which are connected and accessible by mode id_mode.
        Uses cashed results, if possible.

        Optionally weights and fstar can be provided to improve computational speed.
        Otherwise weights and fstar are calculated with the optional parameters
        modeconst_excl and modeconst_mix
        """

        # print 'get_zoneedges_by_mode_fast id_zone',id_zone,'id_mode',id_mode
        if not self._mode_to_edges_inside.has_key(id_mode):
            self._mode_to_edges_inside[id_mode] = {}

        if not self._mode_to_edges_inside[id_mode].has_key(id_zone):
            self._mode_to_edges_inside[id_mode][id_zone] = self.get_zoneedges_by_mode(
                id_zone, id_mode, weights=weights, fstar=fstar)
            # print '  found edges',len(self._mode_to_edges_inside[id_mode][id_zone])
        return self._mode_to_edges_inside[id_mode][id_zone]

    def get_zoneedges_by_mode(self, id_zone, id_mode, speed_max=None,
                              modeconst_excl=0.0, modeconst_mix=0.0,
                              weights=None, fstar=None):
        """
        Returns a list of edge IDs which are connected and accessible by mode id_mode.
        The algorithm tries to maximize the number of reachabe edges.

        Optionally weights and fstar can be provided to improve computational speed.
        Otherwise weights and fstar are calculated with the optional parameters
        modeconst_excl and modeconst_mix
        """
        # print 'get_zoneedges_by_mode id_zone',id_zone,'id_mode',id_mode

        return self.get_edges().get_max_connected(
            ids_edge=self.ids_edges_inside[id_zone],
            id_mode=id_mode, speed_max=speed_max,
            modeconst_excl=modeconst_excl,
            modeconst_mix=modeconst_mix,
            weights=weights, fstar=fstar,
            is_bidir=True)

    def get_egdeprobs(self, id_zone, n_edges_min_length, n_edges_max_length, priority_max, speed_max, is_normalize=True, is_dict=False):
        """
        OBSOLETE!!!
        Returns vectors ids_edge and edge_props
        where ids_edge are the edge IDs of edges in id_zone and edge_props are 
        the respective probabilities to start/end a trip

        if is_dict is True then a dictionary is returnd with edge IDs as key 
        and arrival/departure probability as value
        """
        # todo: rename ids_edges_orig to simply ids_edges
        ids_edge_orig = self.ids_edges_orig[id_zone]
        if ids_edge_orig is None:
            print 'WARNING: no edges in zone', id_zone, '. Run edge detection first.'
            if is_dict:
                return {}
            else:
                return [], []

        n_edges_orig = len(ids_edge_orig)

        if n_edges_orig > 0:
            weights = self.get_edgeweights(ids_edge_orig, n_edges_min_length,
                                           n_edges_max_length, priority_max, speed_max)
            if is_normalize:
                weights = weights/np.sum(weights)
            if is_dict:
                probs = {}
                for id_edge, prob in zip(ids_edge_orig, weights):
                    probs[id_edge] = prob
                return probs
            else:
                return ids_edge_orig, weights
        else:
            if is_dict:
                return {}
            else:
                return [], []

    def make_egdeprobs(self, id_zone, n_edges_min_length, n_edges_max_length, priority_max, speed_max,  is_normalize=True):
        """
        OBSOLETE!!! This funzione has moved to origin_to_destination
        Update  probs_edges_orig and probs_edges_dest distribution 
        one for departures and one for arrivals.
        """

        # print 'make_egdeprobs',id_zone
        #zones = self.zones.value
        #edgeweights_orig = {}
        #edgeweights_dest = {}

        # for id_zone in zones.get_ids():
        ids_edge_orig = self.ids_edges_orig[id_zone]
        ids_edge_dest = self.ids_edges_dest[id_zone]
        n_edges_orig = len(ids_edge_orig)
        n_edges_dest = len(ids_edge_dest)
        # da fare meglio...
        if n_edges_orig > 0:
            #self.probs_edges_orig[id_zone] = 1.0/float(n_edges_orig)*np.ones(n_edges_orig,np.float)
            weights = self.get_edgeweights(ids_edge_orig, n_edges_min_length,
                                           n_edges_max_length, priority_max, speed_max)
            if is_normalize:
                self.probs_edges_orig[id_zone] = weights/np.sum(weights)
            else:
                self.probs_edges_orig[id_zone] = weights
        else:
            self.probs_edges_orig[id_zone] = []

        if n_edges_dest > 0:
            #self.probs_edges_dest[id_zone] = 1.0/float(n_edges_dest)*np.ones(n_edges_dest,np.float)
            weights = self.get_edgeweights(ids_edge_dest, n_edges_min_length,
                                           n_edges_max_length, priority_max, speed_max)
            if is_normalize:
                self.probs_edges_dest[id_zone] = weights/np.sum(weights)
            else:
                self.probs_edges_dest[id_zone] = weights
        else:
            self.probs_edges_dest[id_zone] = []

    def get_edgeweights(self, ids_edge, n_edges_min_length, n_edges_max_length, priority_max, speed_max):
        # OBSOLETE!!! This funzione has moved to origin_to_destination
        # print 'get_edgeweights ids_edge',ids_edge
        edges = self.get_edges()
        n_edges = len(ids_edge)
        if (n_edges > n_edges_min_length) & (n_edges < n_edges_max_length):
            return edges.lengths[ids_edge]*((edges.priorities[ids_edge] < priority_max) & (edges.speeds_max[ids_edge] < speed_max))
        else:
            return np.ones(n_edges, dtype=np.float32)*((edges.priorities[ids_edge] < priority_max) & (edges.speeds_max[ids_edge] < speed_max))

    def write_kml(self, fd=None, indent=0):

        ids_zone = self.get_ids()
        for id_zone in ids_zone:
            fd.write(xm.begin('Placemark', indent + 2))
            fd.write((indent + 4)*' '+'<name>%s</name>\n' % self.names_extended[id_zone])
            fd.write(xm.begin('LineString', indent + 4))
            fd.write(xm.begin('coordinates', indent + 6))

            for point in self.shapes[id_zone]:

                projection = self.project(point[0], point[1])
                fd.write((indent+8)*' '+'%f,%f\n' % (projection[0], projection[1]))

            fd.write(xm.end('coordinates', indent + 6))
            fd.write(xm.end('LineString', indent + 4))
            fd.write(xm.end('Placemark', indent + 2))

    def write_xml(self, fd=None, indent=0):
        print 'Zones.write_xml'
        net = self.parent.parent.net
        ids_edge_sumo = net.edges.ids_sumo
        ids_zone = self.get_ids()
        fd.write(xm.begin('tazs', indent))
        for id_zone_sumo, shape, id_edges_orig, id_edges_dest, probs_edge_orig, probs_edge_dest in zip(
                self.ids_sumo[ids_zone], self.shapes[ids_zone],
                self.ids_edges_orig[ids_zone], self.ids_edges_dest[ids_zone],
                self.probs_edges_orig[ids_zone], self.probs_edges_dest[ids_zone]):

            if id_edges_orig is not None:
                if len(id_edges_orig) > 0:
                    # print '  id_zone_sumo',id_zone_sumo,'id_edges_orig',id_edges_orig,'probs_edge_orig',probs_edge_orig
                    #    <taz id="<TAZ_ID>">
                    #      <tazSource id="<EDGE_ID>" weight="<PROBABILITY_TO_USE>"/>
                    #      ... further source edges ...
                    #
                    #      <tazSink id="<EDGE_ID>" weight="<PROBABILITY_TO_USE>"/>
                    #      ... further destination edges ...
                    #   </taz>

                    fd.write(xm.start('taz', indent + 2))
                    fd.write(xm.num('id', id_zone_sumo))
                    fd.write(xm.mat('shape', shape))
                    fd.write(xm.stop())
                    # for id_edge_sumo, prob_edge in zip(ids_edge_sumo[id_edges_orig], probs_edge_orig):
                    #    fd.write(xm.start('tazSource',indent + 4))
                    #    fd.write(xm.num('id',id_edge_sumo))
                    #    fd.write(xm.num('weight',prob_edge))
                    #    fd.write(xm.stopit())
                    #
                    # for id_edge_sumo, prob_edge in zip(ids_edge_sumo[id_edges_dest], probs_edge_dest):
                    #    fd.write(xm.start('tazSink',indent + 4))
                    #    fd.write(xm.num('id',id_edge_sumo))
                    #    fd.write(xm.num('weight',prob_edge))
                    #    fd.write(xm.stopit())

                    fd.write(xm.end('taz', indent + 2))

        fd.write(xm.end('tazs', indent))

    def project(self, x, y):
        if self._proj is None:
            self._proj, self._offset = self.get_proj_and_offset()
        lons, lats = self._proj(x-self._offset[0], y-self._offset[1], inverse=True)
        return np.transpose(np.concatenate(([lons], [lats]), axis=0))

    def get_proj_and_offset(self):
        if self._proj is None:
            net = self.parent.parent.net
            proj_params = str(net.get_projparams())
            # try:
            self._proj = pyproj.Proj(proj_params)
            # except:
            #    proj_params ="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
            #    self._proj = pyproj.Proj(self.proj_params)

            self._offset = net.get_offset()

            return self._proj, self._offset

    def get_zonefilepath(self):
        return self.parent.parent.get_rootfilepath()+'.taz'

    def export_kml(self, filepath=None, encoding='UTF-8'):
        """
        Export zones to Google kml file formate.
        """
        print 'export_sumoxml', filepath, len(self)
        if len(self) == 0:
            return None

        if filepath is None:
            filepath = self.get_zonefilepath()+'.kml'

        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in write_obj_to_xml: could not open', filepath
            return False
        #xmltag, xmltag_item, attrname_id = self.xmltag
        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        fd.write('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
        indent = 0
        #fd.write(xm.begin('routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"',indent))

        fd.write(xm.begin('Document', indent=0))

        self.write_kml(fd, indent=0)

        fd.write(xm.end('Document', indent=0))
        fd.write(xm.end('kml', indent=0))
        fd.close()
        return filepath

    def export_sumoxml(self, filepath=None, encoding='UTF-8'):
        """
        Export zones to SUMO xml file formate.
        """
        print 'export_sumoxml', filepath, len(self)
        if len(self) == 0:
            return None

        if filepath is None:
            filepath = self.get_zonefilepath()+'.xml'

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


class ZonesFromShapeImporter(ShapefileImporter):
    def __init__(self, ident='zonesfromshapeimporter', zones=None,
                 name='Zones from Shapefile importer',
                 shapeattr_ids_zone='ID',
                 shapeattr_names_zone='',
                 shapeattr_names_landuse='',
                 filepath='', logger=None, **kwargs):

        net = zones.parent.parent.net
        ShapefileImporter.__init__(self, ident, parent=zones,
                                   name=name,
                                   filepath=filepath,
                                   coordsattr='shapes',
                                   attrnames_to_shapeattrs={'ids_sumo': shapeattr_ids_zone,
                                                            'names_extended': shapeattr_names_zone,
                                                            'ids_landusetype': shapeattr_names_landuse},
                                   projparams_target=net.get_projparams(),
                                   offset=net.get_offset(),
                                   boundaries=net.get_boundaries(is_netboundaries=True),
                                   logger=logger,
                                   **kwargs)

    def do(self):
        ShapefileImporter.do(self)

        # update zones
        self.parent.update_zones()


class FacilityTypeMixin(cm.BaseObjman):
    def __init__(self, ident, parent,
                 name='Facility Type Mixin',
                 info='Provides methods to handle specific facility functions.',
                 **kwargs):
        """
        To be overridden.
        """
        # attention parent is the Strategies table
        self._init_objman(ident, parent, **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))
        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        self._init_attributes_common()

    def _init_attributes_common(self):
        # print 'StrategyMixin._init_attributes'
        attrsman = self.get_attrsman()
        landusetypes = self.get_landuse().landusetypes
        self.ids_landusetype = attrsman.add(cm.AttrConf('ids_landusetype',
                                                        [landusetypes.get_id_from_formatted('residential'), ],
                                                        groupnames=['parameters'],
                                                        perm='rw',
                                                        #choices = landusetypes.typekeys.get_indexmap(),
                                                        name='landuse',
                                                        info='Default landuse type of this facility.',
                                                        ))

        self.osmkey = attrsman.add(cm.AttrConf('osmkey', 'building.yes',
                                               groupnames=['parameters'],
                                               perm='rw',
                                               name='OSM key',
                                               info='Default Open Street Map key for this facility.',
                                               ))

        self.length_min = attrsman.add(cm.AttrConf('length_min', 15.0,
                                                   groupnames=['parameters'],
                                                   perm='rw',
                                                   name='Min. length',
                                                   unit='m',
                                                   info='Minimum length of entire property.',
                                                   ))

        self.length_max = attrsman.add(cm.AttrConf('length_max', 100.0,
                                                   groupnames=['parameters'],
                                                   perm='rw',
                                                   name='Max. length',
                                                   unit='m',
                                                   info='Maximum length of entire property.',
                                                   ))
        self.width_min = attrsman.add(cm.AttrConf('width_min', 20.0,
                                                  groupnames=['parameters'],
                                                  perm='rw',
                                                  name='Min. width',
                                                  unit='m',
                                                  info='Minimum width of entire property.',
                                                  ))

        self.width_max = attrsman.add(cm.AttrConf('width_max', 80.0,
                                                  groupnames=['parameters'],
                                                  perm='rw',
                                                  name='Max. width',
                                                  unit='m',
                                                  info='Maximum width of entire property.',
                                                  ))

        self.height_min = attrsman.add(cm.AttrConf('height_min', 15.0,
                                                   groupnames=['parameters'],
                                                   perm='rw',
                                                   name='Min. height',
                                                   unit='m',
                                                   info='Minimum height of facility.',
                                                   ))

        self.height_max = attrsman.add(cm.AttrConf('height_max', 35.0,
                                                   groupnames=['parameters'],
                                                   perm='rw',
                                                   name='Max. height',
                                                   unit='m',
                                                   info='Maximum height of facility.',
                                                   ))

        self.dist_road_min = attrsman.add(cm.AttrConf('dist_road_min', 1.0,
                                                      groupnames=['parameters'],
                                                      perm='rw',
                                                      name='Min. dist road',
                                                      unit='m',
                                                      info='Minimum distance from road.',
                                                      ))

        self.dist_road_max = attrsman.add(cm.AttrConf('dist_road_max', 5.0,
                                                      groupnames=['parameters'],
                                                      perm='rw',
                                                      name='Max. dist road',
                                                      info='Maximum distance from road.',
                                                      ))

        self.dist_prop_min = attrsman.add(cm.AttrConf('dist_prop_min', 1.0,
                                                      groupnames=['parameters'],
                                                      perm='rw',
                                                      name='Min. prop dist',
                                                      unit='m',
                                                      info='Minimum distance to other properties.',
                                                      ))

        self.dist_prop_max = attrsman.add(cm.AttrConf('dist_prop_max', 4.0,
                                                      groupnames=['parameters'],
                                                      perm='rw',
                                                      name='Max. prop dist',
                                                      info='Maximum distance to other properties.',
                                                      ))

        self.shape_default = attrsman.add(cm.AttrConf('shape_default', [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [1.0, 1.0, 0.0], [0.0, 1.0, 0.0]],
                                                      groupnames=['parameters'],
                                                      perm='r',
                                                      name='Defaut shape',
                                                      info='Basic facility shape.',
                                                      ))

        self.unitvolume_default = attrsman.add(cm.AttrConf('unitvolume_default',
                                                           default=100.0,
                                                           dtype=np.float32,
                                                           perm='r',
                                                           name='Unit volume',
                                                           info='Default value of the volume necessary to store one person or container. Volume used to calculate capacity.',
                                                           ))

    def _init_constants(self):

        # self.get_attrsman().do_not_save_attrs([
        #                '_id_mode_bike','_id_mode_auto','_id_mode_moto',
        #                        ])
        pass

    def get_id_type(self):
        # print 'get_id_type from ',self.parent.get_ident_abs()
        # print '  names',self.parent.names.get_value()
        return self.parent.names.get_id_from_index(self.get_ident())

    def get_facilities(self):
        return self.parent.parent

    def get_landuse(self):
        return self.parent.parent.parent

    def get_scenario(self):
        return self.parent.parent.get_scenario()

    def get_shape0(self, length_fac, width_fac, height_max, capacity):
        """
        Returns facility shape in origin coordinate system
        and height as a function of the capacity and available space
        """
        # One could do some random operations on the default shape
        # here just stretch default shape to fit area
        shape = np.array(self.shape_default, dtype=np.float32) * [length_fac, width_fac, 0.0]
        # y-axes must be flipped so that hoses grow
        # to the right side of the road
        shape[:, 1] *= -1
        # Calculates height of the building in function of the
        # required capacity and available space.

        # here just use a random number
        # to be overridden
        height = min(random.uniform(self.height_min, self.height_max), height_max)

        return shape, height

    def generate(self,  offset=[0.0, 0.0, 0.0],
                 length=10.0,
                 width=10.0,
                 angle=0.0,
                 pos_edge=0.0,
                 capacity=None,
                 id_landusetype=None,
                 height_max=30.0,
                 id_edge=None,
                 width_edge=3.0,
                 ):
        n_shape = len(self.shape_default)
        shape_fac = np.zeros((n_shape, 3), dtype=np.float32)

        # determine effecive land area
        dist_edge = random.uniform(self.dist_road_min, self.dist_road_max) + width_edge
        width_fac = width-dist_edge

        dist_prop = random.uniform(self.dist_prop_min, self.dist_prop_max)
        length_fac = length-2*self.dist_prop_max

        # do offset
        dxn = np.cos(angle-np.pi/2)
        dyn = np.sin(angle-np.pi/2)

        offset_fac = offset\
            + np.array([dxn, dyn, 0.0], dtype=np.float32)*dist_edge\
            + np.array([dyn, dxn, 0.0], dtype=np.float32)*dist_prop

        shape, height_fac = self.get_shape0(length_fac, width_fac, height_max, capacity)

        # transform in to the right place
        shape_fac[:, 0] = shape[:, 0]*np.cos(angle) - shape[:, 1]*np.sin(angle)
        shape_fac[:, 1] = shape[:, 0]*np.sin(angle) + shape[:, 1]*np.cos(angle)
        shape_fac += offset_fac

        if id_landusetype is None:
            id_landusetype = self.ids_landusetype[0]

        id_fac = self.get_facilities().make(id_landusetype=id_landusetype,
                                            id_zone=None,
                                            id_facilitytype=self.get_id_type(),
                                            osmkey=self.osmkey,
                                            area=None,
                                            height=height_fac,
                                            centroid=None,
                                            shape=list(shape_fac),
                                            id_roadedge_closest=id_edge,
                                            position_roadedge_closest=pos_edge + 0.5 * length,
                                            )


class FacilityTypeHouse(FacilityTypeMixin):
    def __init__(self, ident, parent,
                 name='Single House',
                 info='Parameters and methods for residential house.',
                 **kwargs):

        self._init_objman(ident, parent, name=name, info=info, **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))

        self._init_attributes()
        self._init_constants()
        # specific init


class FacilityTypes(am.ArrayObjman):
    def __init__(self, ident, facilities, is_add_default=True, **kwargs):
        self._init_objman(ident=ident,
                          parent=facilities,
                          name='Facility types',
                          info='Table holding facility type specific parameters and an object with methods ',
                          **kwargs)

        self._init_attributes()
        if is_add_default:
            self.add_default()

    def _init_attributes(self):
        # landuse types table
        self.add_col(am.ArrayConf('names',
                                  default='',
                                  dtype='object',
                                  perm='r',
                                  is_index=True,
                                  name='Short name',
                                  info='Strategy name. Must be unique, used as index.',
                                  ))

        self.add_col(am.ArrayConf('unitvolumes',
                                  default=100.0,
                                  dtype=np.float32,
                                  perm='rw',
                                  name='Unit volume',
                                  info='The volume necessary to store one person or container. Volume used to calculate capacity.',
                                  ))

        self.add_col(cm.ObjsConf('typeobjects',
                                 #groupnames = ['state'],
                                 name='Type objects',
                                 info='Facility type object.',
                                 ))

        self.add_default()

    def format_ids(self, ids):
        return ','.join(self.names[ids])

    def get_id_from_formatted(self, idstr):
        return self.names.get_id_from_index(idstr)

    def get_ids_from_formatted(self, idstrs):
        return self.names.get_ids_from_indices_save(idstrs.split(','))

    def add_default(self):
        self.clear()
        self.add_type('house', FacilityTypeHouse)

    def get_typeobj(self, id_type):
        return self.typeobjects[id_type]

    def add_type(self, ident, TypeClass, **kwargs):
        # print 'add_strategy', ident
        if not self.names.has_index(ident):
            factypeobj = TypeClass(ident, self)
            id_type = self.add_row(names=ident,
                                   typeobjects=factypeobj,
                                   unitvolumes=factypeobj.unitvolume_default
                                   )
            return id_type
        else:
            return self.get_id_from_formatted(id_type)


class Facilities(am.ArrayObjman):
    def __init__(self, landuse, landusetypes, zones, net=None, **kwargs):
        # print 'Facilities.__init__',hasattr(self,'lanes')
        self._init_objman(ident='facilities',
                          parent=landuse,
                          name='Facilities',
                          info='Information on buildings, their type of usage and access to the transport network.',
                          xmltag=('polys', 'poly', 'ids_sumo'),
                          is_plugin=True,
                          **kwargs)

        self._init_attributes()
        self._init_constants()

        if net is not None:
            self.add_col(am.IdsArrayConf('ids_roadedge_closest', net.edges,
                                         groupnames=['landuse'],
                                         name='Road edge ID',
                                         info='ID of road edge which is closest to this facility.',
                                         ))

            self.add_col(am.ArrayConf('positions_roadedge_closest', 0.0,
                                      dtype=np.float32,
                                      groupnames=['landuse'],
                                      perm='r',
                                      name='Road edge pos',
                                      unit='m',
                                      info='Position on road edge which is closest to this facility',
                                      ))

            # self.ids_stop_closest = self.facilities.add(cm.ArrayConf( 'ids_stop_closest', None,
            #                        dtype = 'object',
            #                        name = 'ID stops',
            #                        perm='rw',
            #                        info = 'List of IDs of closest public transport stops.',
            #                        ))

    def _init_constants(self):
        self._proj = None
        self._offset = None

        attrsman = self.get_attrsman()
        attrsman.do_not_save_attrs(['_proj', '_offset'])

    def _init_attributes(self):
        landusetypes = self.parent.landusetypes
        zones = self.parent.zones
        self.add(cm.ObjConf(FacilityTypes('facilitytypes', self,)))

        self.add_col(SumoIdsConf('Facility', info='SUMO facility ID'))

        self.add_col(am.ArrayConf('names', '',
                                  dtype='object',  # actually a string of variable length
                                  perm='rw',
                                  name='Name',
                                  info='Name of facility.',
                                  is_plugin=True,
                                  ))

        self.add_col(am.IdsArrayConf('ids_landusetype', landusetypes,
                                     groupnames=['landuse'],
                                     perm='rw',
                                     name='ID landuse',
                                     info='ID of landuse.',
                                     is_plugin=True,
                                     ))

        self.add_col(am.IdsArrayConf('ids_zone', zones,
                                     groupnames=['landuse'],
                                     perm='r',
                                     name='ID zone',
                                     info='ID of traffic zone, where this facility is located.',
                                     ))
        facilitytypes = self.get_facilitytypes()
        if len(facilitytypes) > 0:
            id_default = facilitytypes.get_ids()[0]
        else:
            id_default = -1
        self.add_col(am.IdsArrayConf('ids_facilitytype', facilitytypes,
                                     id_default=id_default,
                                     groupnames=['landuse'],
                                     perm='rw',
                                     name='ID fac. type',
                                     info='ID of facility type (house, scycraper, factory, parking,...).',
                                     is_plugin=True,
                                     ))

        self.add_col(am.ArrayConf('osmkeys', 'building.yes',
                                  dtype='object',  # actually a string of variable length
                                  perm='rw',
                                  name='OSM key',
                                  info='OSM key of facility.',
                                  xmltag='type',
                                  is_plugin=True,
                                  ))

        self.add_col(am.ArrayConf('capacities', 0,
                                  dtype=np.int32,
                                  groupnames=['landuse'],
                                  perm='r',
                                  name='Capacity',
                                  info='Person capacity of this facility. For example maximum number of adulds living in a building or number of people working in a factory.',
                                  ))

        self.add_col(am.ArrayConf('areas', 0.0,
                                  dtype=np.float32,
                                  groupnames=['landuse'],
                                  perm='r',
                                  name='Area',
                                  unit='m^2',
                                  info='Area of this facility.',
                                  ))

        self.add_col(am.ArrayConf('heights', 10.0,
                                  dtype=np.float32,
                                  groupnames=['landuse'],
                                  perm='r',
                                  name='Height',
                                  unit='m',
                                  info='Height above ground of this facility.',
                                  ))

        self.add_col(am.ArrayConf('centroids',  np.zeros(3, dtype=np.float32),
                                  dtype=np.float32,
                                  groupnames=['state', '_private'],
                                  perm='r',
                                  name='Center',
                                  unit='m',
                                  info='Center coordinates of this facility.',
                                  ))

        self.add_col(am.ListArrayConf('shapes',
                                      groupnames=['_private'],
                                      perm='rw',
                                      name='Shape',
                                      unit='m',
                                      info='List of 3D Shape coordinates of facility.',
                                      xmltag='shape',
                                      is_plugin=True,
                                      ))

        # self.add_col(TabIdsArrayConf( 'ftypes',
        #                            name = 'Types',
        #                            info = 'Draw obj and ids',
        #                            ))

        if self.plugin is None:
            self.init_plugin(True)
            self.shapes.init_plugin(True)
            self.osmkeys.init_plugin(True)
            self.ids_landusetype.init_plugin(True)
        # configure only if net is initialized

    def make(self, id_sumo=None,
             id_landusetype=None,
             id_zone=None,
             id_facilitytype=None,
             osmkey=None,
             area=None,
             capacity=None,
             height=None,
             centroid=None,
             shape=[],
             id_roadedge_closest=None,
             position_roadedge_closest=None,
             ):
        """
        Adds a facilities 
        """
        id_fac = self.suggest_id()
        if id_sumo is None:
            id_sumo = str(id_fac)

        # stuff with landusetype must be done later
        id_fac = self.add_row(_id=id_fac,
                              ids_sumo=id_sumo,
                              ids_landusetype=id_landusetype,
                              ids_zone=id_zone,
                              ids_facilitytype=id_facilitytype,
                              osmkeys=osmkey,
                              areas=area,
                              capacities=capacity,
                              heights=height,
                              centroids=centroid,
                              shapes=shape,
                              ids_roadedge_closest=id_roadedge_closest,
                              positions_roadedge_closest=position_roadedge_closest,
                              )
        # do area calcs and other
        if area is None:
            self.update_area(id_fac)

        if capacity is None:
            self.update_capacity(id_fac)

        if centroid is None:
            self.update_centroid(id_fac)
        return id_fac

    def generate(self, facilitytype, **kwargs):
        """
        Generates a facility. The generation of the facility will be
        performed by the faciliy type instance.
        """
        return facilitytype.generate(**kwargs)

    def format_ids(self, ids):
        return ','.join(self.ids_sumo[ids])

    def get_id_from_formatted(self, idstr):
        return self.ids_sumo.get_id_from_index(idstr)

    def get_ids_from_formatted(self, idstrs):
        return self.ids_sumo.get_ids_from_indices_save(idstrs.split(','))

    def del_element(self, id_fac):
        # print 'del_element',id_zone
        self.del_row(id_fac)

    def write_xml(self, fd, indent=0, is_print_begin_end=True, delta=np.zeros(3, dtype=np.float32)):
        xmltag, xmltag_item, attrname_id = self.xmltag
        layer_default = -1
        fill_default = 1
        ids_landusetype = self.ids_landusetype
        landusecolors = self.get_landusetypes().colors

        if is_print_begin_end:
            fd.write(xm.begin(xmltag, indent))

        attrsconfigs_write = [self.ids_sumo, self.osmkeys]
        xmltag_shape = self.shapes.xmltag
        sep_shape = self.shapes.xmlsep
        for _id in self.get_ids():
            fd.write(xm.start(xmltag_item, indent+2))
            for attrsconfig in attrsconfigs_write:
                attrsconfig.write_xml(fd, _id)

            fd.write(xm.mat(xmltag_shape, self.shapes[_id] - delta))

            landusecolors.write_xml(fd, ids_landusetype[_id])
            fd.write(xm.num('layer', layer_default))
            fd.write(xm.num('fill', fill_default))

            fd.write(xm.stopit())

        if is_print_begin_end:
            fd.write(xm.end(xmltag, indent))

    def get_landusetypes(self):
        return self.ids_landusetype.get_linktab()

    def get_facilitytypes(self):
        return self.facilitytypes.get_value()

    def get_net(self):
        # print 'get_net',self.ids_edge_closest_road.get_linktab(),self.ids_edge_closest_road.get_linktab().parent
        return self.ids_roadedge_closest.get_linktab().parent

    def get_scenario(self):
        return self.ids_roadedge_closest.get_linktab().parent.parent

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        # self.zones.update_netoffset(deltaoffset)
        self.centroids.value[:, :2] = self.centroids.value[:, :2] + deltaoffset
        shapes = self.shapes.value
        for i in xrange(len(shapes)):
            s = np.array(shapes[i])
            s[:, :2] = s[:, :2] + deltaoffset
            shapes[i] = list(s)

    def get_edges(self):
        return self.ids_roadedge_closest.get_linktab()

    def identify_taz(self):
        """
        Identifies id of traffic assignment zone for each facility.
        Note that not all facilities are within such a zone.
        """
        zones = self.ids_zone.get_linktab()
        # self.get_demand().get_districts()
        for id_fac in self.get_ids():
            for id_zone in zones.get_ids():
                if is_polyline_in_polygon(self.shapes[id_fac], zones.shapes[id_zone]):
                    self.ids_zone[id_fac] = id_zone
                    break

    def get_departure_probabilities(self):
        """
        Returns a dictionary, where probabilities[id_zone]
        is a vector of departure probabilities for each facility
        of zone id_zone.
        """
        zones = self.ids_zone.get_linktab()
        # print 'get_departure_probabilities in n_zones',len(zones)
        probabilities = {}

        inds_fac = self.get_inds()
        for id_zone in zones.get_ids():
            # probabilities[id_zone]={}
            # for id_landusetype in  set(self.ids_landusetype.value):
            # print '  id_zone',id_zone
            # print '  ids_landusetype',self.ids_landusetype.value[inds_fac]
            # print '  ids_zone',self.ids_zone.value[inds_fac]
            # print ''
            util = self.capacities.value[inds_fac].astype(np.float32)*(self.ids_zone.value[inds_fac] == id_zone)
            util_tot = np.sum(util)
            # print '\n\n  [id_taz][ftype]',id_taz,ftype,util_tot,np.sum(util/np.sum(util))
            # print '  self.type==ftype',self.type==ftype
            # print '  self.id_taz==id_taz',self.id_taz==id_taz
            # print '  util',util
            if util_tot > 0.0:
                probabilities[id_zone] = util/util_tot
            else:
                probabilities[id_zone] = util  # all zero prob

        return probabilities, self.get_ids(inds_fac)

    def get_departure_probabilities_landuse2(self, ids_landusetype):
        """
        Returns a dictionary, where probabilities[id_zone]
        is a vector of departure probabilities for each facility
        of zone id_zone.
        Probabilities are proportional to the capacity attribute of the facility.

        Only facilities with one of the landuse given in ids_landusetype
        have non-zero probabilities.

        The ids_fac is an array that contains the facility ids in correspondence
        to the probability vector.
        """
        print 'get_departure_probabilities_landuse2 ids_landusetype', ids_landusetype
        probabilities = {}
        zones = self.ids_zone.get_linktab()
        inds_fac = self.get_inds()
        n_frac = len(inds_fac)
        for id_zone in zones.get_ids():
            # probabilities[id_zone]={}
            utils = np.zeros(n_frac, dtype=np.float32)
            util_tot = 0.0
            for id_landusetype in ids_landusetype:
                # print '    id_zone,id_landusetype',id_zone,id_landusetype
                # print '  ids_landusetype',self.ids_landusetype.value[inds_fac]
                # print '  ids_zone',self.ids_zone.value[inds_fac]
                # print ''
                utils_new = self.capacities.value[inds_fac].astype(
                    np.float32)*np.logical_and((self.ids_landusetype.value[inds_fac] == id_landusetype), (self.ids_zone.value[inds_fac] == id_zone))
                utils += utils_new
                util_tot += np.sum(utils_new)
                # print '\n\n  [id_taz][ftype]',id_taz,ftype,util_tot,np.sum(util/np.sum(util))
                # print '  self.type==ftype',self.type==ftype
                # print '  self.id_taz==id_taz',self.id_taz==id_taz
                # print '      util',np.sum(utils)

            if util_tot > 0.0:
                probabilities[id_zone] = utils/util_tot
            else:
                probabilities[id_zone] = utils  # all zero prob

            if 0:  # debug
                print '    sum(probs)', np.sum(probabilities[id_zone])
                if np.sum(probabilities[id_zone]) > 0:
                    ids_fac = self.get_ids(inds_fac)
                    for id_fac, id_landusetype, id_thiszone, prob in zip(ids_fac, self.ids_landusetype[ids_fac], self.ids_zone[ids_fac], probabilities[id_zone]):
                        if (id_thiszone == id_zone):  # & (id_landusetype in ids_landusetype):
                            if prob > 0:
                                print '        id_fac', id_fac, 'id_landusetype', id_landusetype, 'prob', prob

        return probabilities, self.get_ids(inds_fac)

    def get_departure_probabilities_landuse(self):
        """
        Returns the dictionnary of dictionaries with departure (or arrival)
        probabilities where probabilities[id_zone][id_landusetype]
        is a probability distribution vector  giving for each facility the
        probability to depart/arrive in zone id_zone with facility type ftype.

        The ids_fac is an array that contains the facility ids in correspondence
        to the probability vector.
        """
        print 'get_departure_probabilities_landuse'
        probabilities = {}
        zones = self.ids_zone.get_linktab()
        inds_fac = self.get_inds()
        for id_zone in zones.get_ids():
            probabilities[id_zone] = {}
            for id_landusetype in set(self.ids_landusetype.value):
                print '  id_zone,id_landusetype', id_zone, id_landusetype
                # print '  ids_landusetype',self.ids_landusetype.value[inds_fac]
                # print '  ids_zone',self.ids_zone.value[inds_fac]
                # print ''
                util = self.capacities.value[inds_fac].astype(
                    np.float32)*((self.ids_landusetype.value[inds_fac] == id_landusetype) & (self.ids_zone.value[inds_fac] == id_zone))
                util_tot = np.sum(util)
                # print '\n\n  [id_taz][ftype]',id_taz,ftype,util_tot,np.sum(util/np.sum(util))
                # print '  self.type==ftype',self.type==ftype
                # print '  self.id_taz==id_taz',self.id_taz==id_taz
                # print '  util',util
                if util_tot > 0.0:
                    probabilities[id_zone][id_landusetype] = util/util_tot
                else:
                    probabilities[id_zone][id_landusetype] = util  # all zero prob

        return probabilities, self.get_ids(inds_fac)

    def update(self, ids=None):
        # print 'update',ids
        if ids is None:
            ids = self.get_ids()

        for _id in ids:
            # print '  self.centroids[_id]',self.centroids[_id]
            # print '  self.shapes[_id]',self.shapes[_id],np.mean(self.shapes[_id],0)
            self.update_centroid(_id)
            #self.areas[_id] = find_area(np.array(self.shapes[_id],float)[:,:2])
            self.update_area(_id)
            #self.areas[_id] = get_polygonarea_fast(np.array(self.shapes[_id],float)[:,0], np.array(self.shapes[_id],float)[:,1])

        self.identify_landuse_from_area(ids)
        self.update_capacities(ids)
        # self.identify_closest_edge(ids)

    def get_ids_area(self, ids=None):
        if ids is None:
            ids = self.get_ids()
        return ids[self.get_landusetypes().are_area[self.ids_landusetype[ids]]]

    def get_ids_building(self, ids=None):
        """Returns all building type of facilities"""
        # print 'get_ids_building'
        if ids is None:
            ids = self.get_ids()
        # debug
        #landusetypes =  self.get_landusetypes()
        # for id_fac in ids[self.get_landusetypes().are_area[self.ids_landusetype[ids]] == False]:
        #    id_landusetype = self.ids_landusetype[id_fac]
        #    print '  id_fac',id_fac,id_landusetype,'is_area',landusetypes.are_area[id_landusetype]

        return ids[self.get_landusetypes().are_area[self.ids_landusetype[ids]] == False]

    def identify_landuse_from_area(self, ids_fac=None):
        """Determines the landuse of facilities from the landuse of areas in which their are located"""
        print 'identify_landuse_from_area', ids_fac
        # TODO:
        landusetypes = self.get_landusetypes()
        ids_area = self.get_ids_area(ids_fac)
        ids_build = self.get_ids_building(ids_fac)

        for id_area, shape, id_landuse in zip(ids_area, self.shapes[ids_area], self.ids_landusetype[ids_area]):
            id_landusetype_fac = landusetypes.get_landusetype_facility_from_area(self.ids_landusetype[id_area])
            for id_fac, osmkey, coord in zip(ids_build, self.osmkeys[ids_build], self.centroids[ids_build]):

                if osmkey == 'building.yes':
                    if is_point_in_polygon(coord[:2], np.array(shape, dtype=np.float32)[:, :2], is_use_shapely=IS_SHAPELY):
                        print '  found id_fac', id_fac, osmkey, 'in id_area', id_area
                        print '    id_landusetype', self.ids_landusetype[id_fac], 'is_area', landusetypes.are_area[self.ids_landusetype[id_fac]], '->', id_landusetype_fac
                        self.ids_landusetype[id_fac] = id_landusetype_fac

    def update_centroid(self, _id):
        self.centroids[_id] = np.mean(self.shapes[_id], 0)

    def update_area(self, _id):
        if IS_SHAPELY:
            self.areas[_id] = find_area(self.shapes[_id])
        else:
            self.areas[_id] = get_polygonarea_fast(np.array(self.shapes[_id], float)[
                                                   :, 0], np.array(self.shapes[_id], float)[:, 1])

    def update_capacity(self, id_fac):
        self.update_capacities([id_fac])

    def update_capacities(self, ids):
        ids_fac = self.get_ids_building(ids)
        ids_area = self.get_ids_area(ids)
        volumes_unit = self.get_facilitytypes().unitvolumes[self.ids_facilitytype[ids_fac]]

        self.capacities[ids_fac] = self.areas[ids_fac]*self.heights[ids_fac]/volumes_unit

        # here we assume that pure areas do not have capacities
        # this will prevent that activities are assigned to areas
        # instead of facilities (buildings)
        # TODO: problem is for example parks with no buildings
        # fixed: parks etc. are areas
        self.capacities[ids_area] = 0.0

    def get_dists(self, ids_fac_from, ids_fac_to):
        """
        Returns centroid to centroid distance from facilities in vector
        ids_fac_from to facilities in vector ids_fac_to.
        """

        return np.sqrt(np.sum((self.centroids[ids_fac_to]-self.centroids[ids_fac_from])**2))

    def identify_closest_edge(self, ids=None, priority_max=5, has_sidewalk=True, n_best=10):
        """
        Identifies edge ID and position on this edge that 
        is closest to the centoid of each facility and the satisfies certain
        conditions.
        """
        print 'identify_closest_edge'
        edges = self.get_edges()
        id_ped = self.get_net().modes.get_id_mode('pedestrian')
        # select edges...if (edges.priorities[id_edge]<=priority_max) & edges.has_sidewalk(id_edge):

        # ids_edge = edges.select_ids((edges.priorities.get_value()<priority_max)\
        #                             & (edges.widths_sidewalk.get_value()>0.0))
        accesslevels = edges.get_accesslevels(id_ped)
        # edges.make_segment_edge_map()

        for id_fac in self.get_ids():
            ids_edge, dists = edges.get_closest_edge(self.centroids[id_fac], n_best=n_best, accesslevels=accesslevels)

            if len(ids_edge) > 0:
                id_edge = ids_edge[0]

                # determin position on edeg where edge is closest to centroid
                # TODO: solve this faster with precalculated maps!!
                xc, yc, zc = self.centroids[id_fac]
                shape = edges.shapes[id_edge]
                n_segs = len(shape)

                d_min = 10.0**8
                x_min = 0.0
                y_min = 0.0
                j_min = 0
                p_min = 0.0
                pos = 0.0
                x1, y1, z1 = shape[0]
                edgelength = edges.lengths[id_edge]
                for j in xrange(1, n_segs):
                    x2, y2, z2 = shape[j]
                    d, xp, yp = shortest_dist(x1, y1, x2, y2, xc, yc)
                    # print '    x1,y1=(%d,%d)'%(x1,y1),',x2,y2=(%d,%d)'%(x2,y2),',xc,yc=(%d,%d)'%(xc,yc)
                    # print '    d,x,y=(%d,%d,%d)'%shortest_dist(x1,y1, x2,y2, xc,yc)
                    if d < d_min:
                        d_min = d
                        # print '    **d_min=',d_min,[xp,yp]
                        x_min = xp
                        y_min = yp
                        j_min = j
                        p_min = pos
                    # print '    pos',pos,[x2-x1,y2-y1],'p_min',p_min
                    pos += np.linalg.norm([x2-x1, y2-y1])
                    x1, y1 = x2, y2

                x1, y1, z1 = shape[j_min-1]
                pos_min = p_min+np.linalg.norm([x_min-x1, y_min-y1])
                # print '  k=%d,d_min=%d, x1,y1=(%d,%d),xmin,ymin=(%d,%d),xc,yc=(%d,%d)'%(k,d_min,x1,y1,x_min,y_min,xc,yc)
                # print '  pos=%d,p_min=%d,pos_min=%d'%(pos,p_min,pos_min)

                if pos_min > edgelength:
                    pos_min = edgelength

                if pos_min < 0:
                    pos_min = 0
                # print '  id_fac,id_edge',id_fac,id_edge,pos_min
                self.ids_roadedge_closest[id_fac] = id_edge
                self.positions_roadedge_closest[id_fac] = pos_min

    def set_shape(self, id_fac, shape):
        # print 'set_shape',id_fac,shape
        self.shapes[id_fac] = shape
        self.update([id_fac])
        #self.areas[id_fac] = find_area(shape[:,:2])
        #self.centroids[id_fac] =np.mean(shape,0)

    def add_polys(self, ids_sumo=[], **kwargs):
        """
        Adds a facilities as used on sumo poly xml info
        """
        # stuff with landusetype must be done later
        return self.add_rows(n=len(ids_sumo),    ids_sumo=ids_sumo, **kwargs)

    def add_poly(self, id_sumo, id_landusetype=None, osmkey=None, shape=np.array([], np.float32)):
        """
        Adds a facility as used on sumo poly xml info
        """
        # print 'add_poly',id_sumo,id_landusetype,osmkey

        landusetypes = self.get_landusetypes()

        if id_landusetype is None:
            # make default landuse
            id_landusetype = landusetypes.typekeys.get_id_from_index('residential')

        if osmkey is None:
            # use first filter as key
            osmkey = landusetypes.osmfilters[id_landusetype][0]

        id_fac = self.add_row(ids_sumo=id_sumo,
                              ids_landusetype=id_landusetype,
                              osmkeys=osmkey,
                              )
        self.set_shape(id_fac, shape)
        return id_fac

    def clear(self):
        # self.reset()
        self.clear_rows()

    def set_shapes(self, ids, vertices):
        self.shapes[ids] = vertices
        if not hasattr(ids, '__iter__'):
            ids = [ids]
        self.update(ids)

    def import_poly(self, polyfilepath, is_remove_xmlfiles=False, is_clear=True, **kwargs):
        print 'import_poly from %s ' % (polyfilepath,)
        if is_clear:
            self.clear()
        # let's read first the offset information, which are in the
        # comment area
        fd = open(polyfilepath, 'r')
        is_comment = False
        is_processing = False
        offset = self.get_net().get_offset()  # default is offset from net
        # print '  offset,offset_delta',offset,type(offset)
        #offset = np.array([0,0],float)
        for line in fd.readlines():
            if line.find('<!--') >= 0:
                is_comment = True
            if is_comment & (line.find('<processing') >= 0):
                is_processing = True
            if is_processing & (line.find('<offset.x') >= 0):
                offset[0] = float(xm.read_keyvalue(line, 'value'))
            elif is_processing & (line.find('<offset.y') >= 0):
                offset[1] = float(xm.read_keyvalue(line, 'value'))
                break
        fd.close()
        offset_delta = offset - self.get_net().get_offset()

        exectime_start = time.clock()

        counter = SumoPolyCounter()
        parse(polyfilepath, counter)
        fastreader = SumoPolyReader(self, counter, offset_delta, **kwargs)
        parse(polyfilepath, fastreader)
        fastreader.finish()

        # update ids_landuse...
        # self.update()

        # timeit
        print '  exec time=', time.clock() - exectime_start

        # print '  self.shapes',self.shapes.value

    def write_kml(self, fd=None, indent=0):
        #  <busStop id="busstop4" lane="1/0to2/0_0" startPos="20" endPos="40" lines="100 101"/>

        ids_fac = self.get_ids()
        for id_fac in ids_fac:
            fd.write(xm.begin('Placemark', indent + 2))
            fd.write((indent+4)*' '+'<name>%s</name>\n' % id_fac)
            fd.write(xm.begin('Polygon', indent + 4))
            fd.write((indent+6)*' '+'<extrude>1</extrude>\n' % id_fac)
            fd.write((indent+6)*' '+'<altitudeMode>relativeToGround</altitudeMode>\n' % id_fac)
            fd.write(xm.begin('outerBoundaryIs', indent + 6))

            fd.write(xm.begin('LinearRing', indent + 8))
            fd.write(xm.begin('coordinates', indent + 10))

            for point in self.shapes[id_fac]:

                projection = self.project(point[0], point[1])
                fd.write((indent+12)*' '+'%f,%f,%f\n' % (projection[0], projection[1], self.heights[id_fac]))

            fd.write(xm.end('coordinates', indent + 10))
            fd.write(xm.end('LinearRing', indent + 8))
            fd.write(xm.end('outerBoundaryIs', indent + 6))
            fd.write(xm.end('Polygon', indent + 4))
            fd.write(xm.end('Placemark', indent + 2))

    def project(self, x, y):
        if self._proj is None:
            self._proj, self._offset = self.get_proj_and_offset()
        lons, lats = self._proj(x-self._offset[0], y-self._offset[1], inverse=True)
        return np.transpose(np.concatenate(([lons], [lats]), axis=0))

    def get_proj_and_offset(self):
        if self._proj is None:
            net = self.parent.parent.net
            proj_params = str(net.get_projparams())
            # try:
            self._proj = pyproj.Proj(proj_params)
            # except:
            #    proj_params ="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
            #    self._proj = pyproj.Proj(self.proj_params)

            self._offset = net.get_offset()

            return self._proj, self._offset

    def get_facfilepath(self):
        return self.parent.parent.get_rootfilepath()+'.fac.kml'

    def export_sumokml(self, filepath=None, encoding='UTF-8'):
        """
        Export stops to SUMO stop xml file.
        """
        print 'export_sumoxml', filepath, len(self)
        if len(self) == 0:
            return None

        if filepath is None:
            filepath = self.get_facfilepath()

        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in write_obj_to_xml: could not open', filepath
            return False
        #xmltag, xmltag_item, attrname_id = self.xmltag
        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        fd.write('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
        indent = 0
        #fd.write(xm.begin('routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"',indent))

        fd.write(xm.begin('Document', indent=0))

        self.write_kml(fd, indent=0)

        fd.write(xm.end('Document', indent=0))
        fd.write(xm.end('kml', indent=0))
        fd.close()
        return filepath


class SumoPolyCounter(handler.ContentHandler):
    """Counts facilities from poly.xml file into facility structure"""

    def __init__(self):
        self.n_fac = 0

    def startElement(self, name, attrs):
        # print 'startElement',name,len(attrs)
        if name == 'poly':
            self.n_fac += 1


class SumoPolyReader(handler.ContentHandler):
    """Reads facilities from poly.xml file into facility structure"""

    def __init__(self, facilities, counter, offset_delta, type_default='building.yes', height_default=7.0):

        self._type_default = type_default
        self._height_default = height_default
        self._facilities = facilities
        self._ids_landusetype_all = self._facilities.get_landusetypes().get_ids()
        self._osmfilters = self._facilities.get_landusetypes().osmfilters

        self._ind_fac = -1
        self.ids_sumo = np.zeros(counter.n_fac, np.object)
        self.ids_landusetype = -1*np.ones(counter.n_fac, np.int32)
        self.osmkeys = np.zeros(counter.n_fac, np.object)
        self.names = np.zeros(counter.n_fac, np.object)
        self.shape = np.zeros(counter.n_fac, np.object)
        self.areas = np.zeros(counter.n_fac, np.float32)
        self.heights = self._height_default * np.ones(counter.n_fac, np.float32)
        self.centroids = np.zeros((counter.n_fac, 3), np.float32)

        #self._id_facility = None
        self._offset_delta = offset_delta

    def startElement(self, name, attrs):

        # print 'startElement', name, len(attrs)
        if name == 'poly':
            self._ind_fac += 1
            i = self._ind_fac

            osmkey = attrs.get('type', self._type_default)
            # print '  id_sumo=',attrs['id'],osmkey
            id_landuse = self.get_landuse(osmkey)
            if id_landuse >= 0:  # land use is interesting
                shape = xm.process_shape(attrs.get('shape', ''), offset=self._offset_delta)
                shapearray = np.array(shape, np.float32)
                # print '  shapearray',shapearray
                self.ids_sumo[i] = attrs['id']
                self.ids_landusetype[i] = id_landuse
                self.osmkeys[i] = osmkey
                self.shape[i] = shape
                self.areas[i] = find_area(shapearray[:, :2])
                self.centroids[i] = np.mean(shapearray, 0)
                self.names[i] = ''

                # print '    control id_sumo',self.ids_sumo[i]

        elif name == 'param':
            i = self._ind_fac
            if attrs['key'] == 'height':
                self.heights[i] = string_to_float(attrs['value'])
            elif attrs['key'] == 'name':
                self.names[i] = attrs['value']

            # color info in this file no longer used as it is defined in
            # facility types table
            # color = np.array(xm.parse_color(attrs['color']))*0.8,# make em darker!!

    def get_landuse(self, osmkey):
        keyvec = osmkey.split('.')
        len_keyvec = len(keyvec)
        print 'get_landuse', len_keyvec, keyvec
        #is_match = False
        for id_landusetype in self._ids_landusetype_all:
            # print '    id_landusetype',id_landusetype
            # if fkeys==('building.industrial'): print ' check',facilitytype

            # first filter only exact matches before wildcards
            for osmfilter in self._osmfilters[id_landusetype]:
                # print '       ?osmfiltervec',osmfilter,osmkey==osmfilter
                if osmkey == osmfilter:  # exact match of filter
                    print '      exact', osmkey
                    return id_landusetype

        # now check for wildcards
        for id_landusetype in self._ids_landusetype_all:
            # print '    *id_landusetype',id_landusetype

            for osmfilter in self._osmfilters[id_landusetype]:
                osmfiltervec = osmfilter.split('.')
                # print '       ?osmfiltervec',osmfiltervec,(len(osmfiltervec)==2)&(len_keyvec==2)
                if (len(osmfiltervec) == 2) & (len_keyvec == 2):
                    if osmfiltervec[0] == keyvec[0]:
                        if osmfiltervec[1] == '*':
                            print '      *', keyvec[0]
                            return id_landusetype
        return -1

    def finish(self):

        # print 'write_to_net'
        inds_valid = np.flatnonzero(self.ids_landusetype >= 0)
        ids_fac = self._facilities.add_polys(
            ids_sumo=self.ids_sumo[inds_valid],
            ids_landusetype=self.ids_landusetype[inds_valid],
            osmkeys=self.osmkeys[inds_valid],
            shapes=self.shape[inds_valid],
            areas=self.areas[inds_valid],
            centroids=self.centroids[inds_valid],
            heights=self.heights[inds_valid],
            names=self.names[inds_valid]
        )

    # def characters(self, content):
    #    if self._id is not None:
    #        self._currentShape = self._currentShape + content

    # def endElement(self, name):
    #    pass


class Parking(am.ArrayObjman):
    def __init__(self, landuse, lanes, **kwargs):
        # print 'Parking.__init__',lanes,hasattr(self,'lanes')
        self._init_objman(ident='parking', parent=landuse,
                          name='Parking',
                          info='Information on private car parking.',
                          #is_plugin = True,
                          # **kwargs
                          )

        self._init_attributes(lanes)

    def _init_attributes(self, lanes=None):
        # print 'Parkin._init_attributes',lanes,hasattr(self,'lanes'),hasattr(self,'ids_lane')
        if lanes is None:
            # upgrade call
            # lanes exists already as link
            lanes = self.get_lanes()

        # print '  lanes',lanes
        # --------------------------------------------------------------------
        # misc params...

        # these are options for assignment procedure!!
        # self.add(AttrConf(  'length_noparking', kwargs.get('length_noparking',20.0),
        #                                groupnames = ['options'],
        #                                perm='wr',
        #                                unit = 'm',
        #                                name = 'Min Length',
        #                                info = 'Minimum edge length for assigning on-road parking space.' ,
        #                                #xmltag = 'pos',
        #                                ))
        #
        # self.add(AttrConf(  'length_space', kwargs.get('length_space',20.0),
        #                                groupnames = ['options'],
        #                                perm='wr',
        #                                unit = 'm',
        #                                name = 'Lot length',
        #                                info = 'Length of a standard parking lot.' ,
        #                                #xmltag = 'pos',
        #                                ))

        self.add_col(am.IdsArrayConf('ids_lane', lanes,
                                     name='ID Lane',
                                     info='ID of lane for this parking position. ',
                                     ))

        self.add_col(am.ArrayConf('positions', 0.0,
                                  dtype=np.float32,
                                  perm='r',
                                  name='Pos',
                                  unit='m',
                                  info="Position on lane for this parking.",
                                  ))

        self.add_col(am.ArrayConf('lengths', 0.0,
                                  dtype=np.float32,
                                  #choices = OPTIONMAP_POS_DEPARTURE,
                                  perm='r',
                                  name='Length',
                                  unit='m',
                                  info="Length of parking lot in edge direction.",
                                  ))

        self.add_col(am.ArrayConf('angles', 0.0,
                                  dtype=np.float32,
                                  perm='rw',
                                  name='Angle',
                                  unit='deg',
                                  info="Parking angle with respect to lane direction.",
                                  ))

        self.add_col(am.ArrayConf('vertices',  np.zeros((2, 3), dtype=np.float32),
                                  dtype=np.float32,
                                  groupnames=['_private'],
                                  perm='r',
                                  name='Coords',
                                  unit='m',
                                  info="Start and end vertices of right side of parking space.",
                                  ))

        self.add_col(am.ArrayConf('numbers_booking', 0,  # ???
                                  dtype=np.int32,
                                  perm='r',
                                  name='booked',
                                  info="Number of vehicles booked for this parking.",
                                  ))

        # self.add_col(am.ArrayConf(  'durations', 0.0,# ???
        #                                dtype=np.float32,
        #                                perm='r',
        #                                name = 'Parking duration',
        #                                unit = 's',
        #                                info = "Default duration of car parking.",
        #                                ))

        self.add(cm.ObjConf(lanes, is_child=False, groups=['_private']))

        self.add(cm.ObjConf(lanes.parent.edges, is_child=False, groups=['_private']))

    def get_edges(self):
        return self.edges.get_value()

    def get_lanes(self):
        return self.lanes.get_value()

    def link_vehiclefleet(self, vehicles):
        """
        Links to table with vehicle info.
        """
        self.add_col(am.IdsArrayConf('ids_bookedveh', vehicles,
                                     name='ID booked veh',
                                     info='ID of vehicle which has booked this parking position.',
                                     ))

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust their coordinates.
        """
        pass

    def get_parkinglane_from_edge(self, id_edge, id_mode, length_min=15.0, priority_max=8, n_freelanes_min=1):
        """
        Check if edge can have on-road parking.
        Returns lane ID if parking is possible and -1 otherwise.
        """
        edges = self.edges.get_value()
        lanes = self.lanes.get_value()
        id_mode_ped = edges.parent.modes.get_id_mode('pedestrian')
        # check access
        ids_lane = edges.ids_lanes[id_edge]
        edgelength = edges.lengths[id_edge]

        if (len(ids_lane) >= 2):
            if (lanes.get_accesslevel([ids_lane[0]], id_mode_ped) > -1) & (lanes.get_accesslevel([ids_lane[1]], id_mode) > -1):
                # check size
                # laneindex =
                # print 'get_parkinglane_from_edge',id_edge, id_mode,priority_max,length_min
                # print '   check',(edges.priorities[id_edge]<=priority_max),(edges.lengths[id_edge]>length_min),(edges.widths_sidewalk[id_edge]>-1)

                if (edges.priorities[id_edge] <= priority_max) & (edgelength > length_min) & (edges.widths_sidewalk[id_edge] > 0):

                    laneindex = 1
                    # print '  found',laneindex,edges.nums_lanes[id_edge]-laneindex > n_freelanes_min
                    if (len(ids_lane)-laneindex >= n_freelanes_min):
                        return ids_lane[laneindex]
                    else:
                        return -1
                else:
                    return -1
            else:
                return -1

        return -1  # no parking possible by default

    def get_edge_pos_parking(self, id_parking):
        lanes = self.lanes.get_value()
        return lanes.ids_edge[self.ids_lane[id_parking]], self.positions[id_parking]

    # def get_edge_pos_parking(self, id_parking):
    #    """
    #    Retuens edge and position of parking with id_parking
    #    """
    #    ind = self.parking.get_ind(id_parking)
    #
    #    return self.edges.get_value()(self.id_edge_parking[ind]),self.pos_edge_parking[ind]

    def make_parking(self, id_mode=MODES['passenger'],
                     length_min=42.0, length_noparking=15.0,
                     length_lot=6.0, angle=0.0,
                     is_clear=True,
                     logger=None,
                     **kwargs):
        print 'make_parking'
        if is_clear:
            self.clear()
        edges = self.edges.get_value()
        lanes = self.lanes.get_value()
        n_parking = 0
        ids_parking = []
        ids_lane_current = self.ids_lane.get_value().copy()
        ii = 0.0
        n_edges = len(edges.get_ids())
        for id_edge in edges.get_ids():
            ii += 1
            if logger is not None:
                logger.progress(int(ii/n_edges*100))
            # check if edge is suitable...
            # print '  id_edge,length,n_lanes,',id_edge
            id_lane = self.get_parkinglane_from_edge(id_edge, id_mode, length_min, **kwargs)

            is_eligible = id_lane >= 0
            if not is_clear:
                if id_lane not in ids_lane_current:
                    is_eligible = False

            if is_eligible:
                n_spaces = int((edges.lengths[id_edge]-2*length_noparking)/length_lot)
                # print '    create',id_edge,lanes.indexes[id_lane],edges.lengths[id_edge],n_spaces
                # print '  delta',lanes.shapes[id_lane][0]-lanes.shapes[id_lane][-1]
                pos_offset = length_noparking
                pos = pos_offset
                if n_spaces > 0:
                    for i in xrange(n_spaces):
                        #id_park = self.suggest_id()
                        # print '    pos=',pos,pos/edges.lengths[id_edge]

                        # print '    vertices',get_vec_on_polyline_from_pos(lanes.shapes[id_lane],pos, length_lot, angle = angle)
                        n_parking += 1

                        id_park = self.add_row(ids_lane=id_lane,
                                               positions=pos,
                                               lengths=length_lot,
                                               angles=angle,
                                               vertices=get_vec_on_polyline_from_pos(
                                                   lanes.shapes[id_lane], pos, length_lot-0.5, angle=angle)
                                               )
                        # print '    created id_park,pos', id_park,pos#,get_coord_on_polyline_from_pos(lanes.shapes[id_lane],pos),lanes.shapes[id_lane]
                        ids_parking.append(id_park)
                        pos = pos_offset+(i+1)*length_lot

        print '  created %d parking spaces' % n_parking
        return ids_parking

    def clear_booking(self):
        self.numbers_booking.reset()
        if hasattr(self, 'ids_bookedveh'):
            self.ids_bookedveh.reset()

    def get_closest_parking(self, id_veh, coord, c_spread=2.0):
        """
        Returns  parking space for  id_veh as close as possible to coord.
        """

        #inds_person = self.persons.get_inds(ids_person)
        print 'get_closest_parking'
        ind_parking_closest = self.get_inds()[np.argmin(
            np.sum((coord-self.vertices.value[:, 1, :])**2, 1) + c_spread*lengths*self.lengths.get_value())]
        self.numbers_booking.get_value()[ind_parking_closest] += 1
        return self.get_ids(ind_parking_closest), ind_parking_closest

    def get_closest_parkings(self, ids_veh, ids_mode, coords, dists_walk_max, c_spread=2.0,
                             n_retrials=20, id_mode_fallback=None):
        """
        Returns  parking space for each vehicle in ids_veh as close as possible to coords.
        """

        # Used by virtualpop

        lanes = self.ids_lane.get_linktab()
        ids_lane = self.ids_lane.get_value()
        #inds_person = self.persons.get_inds(ids_person)
        n = len(ids_veh)

        # print 'get_closest_parking',n,len(self),'n_retrials',n_retrials
        if len(self) == 0:
            print 'WARNING in get_closest_parkings: there is no parking.'
            return [], []

        #parking = self.get_landuse().parking
        #inds_parking = parking.get_inds()
        coord_parking = self.vertices.value[:, 1, :]
        # print '  coord_parking',coord_parking
        numbers_booking = self.numbers_booking.get_value()
        lengths = self.lengths.get_value()
        inds_vehparking = np.zeros(n, int)
        are_fallback = np.zeros(n, bool)

        #inds_parking_avail = np.flatnonzero( self.ids_bookedveh.value == -1).tolist()
        inds_parking_avail = self.get_inds().copy()

        #ids_veh = np.zeros(n,object)
        i = 0
        for id_veh, id_mode, coord, dist_walk_max_sq in zip(ids_veh, ids_mode, coords, dists_walk_max**2):
            # print '  search parking for id_veh',id_veh
            # print '    landuse.id_bookedveh_parking',landuse.id_bookedveh_parking
            #

            # print '    inds_parking_avail',inds_parking_avail
            # print '    dists',np.sum((coord-coord_parking[inds_parking_avail])**2,1),np.argmin(np.sum((coord-coord_parking[inds_parking_avail])**2,1))
            is_fallback = False
            penalties_inaccessible = np.ones(len(numbers_booking), dtype=np.float32)
            n_search = n_retrials
            is_search = True
            while (n_search > 0) & is_search:
                dists = np.sum((coord-coord_parking)**2, 1)
                ind_parking_closest = inds_parking_avail[np.argmin(
                    dists + c_spread*lengths*numbers_booking*penalties_inaccessible)]
                # print '    ind_parking_closest,n_avail',ind_parking_closest,len(inds_parking_avail),'n_search',n_search,'is_search',is_search
                is_search = (not (lanes.get_accesslevel([ids_lane[ind_parking_closest]], id_mode) >= 0))\
                    | (dists[ind_parking_closest] > dist_walk_max_sq)
                penalties_inaccessible[ind_parking_closest] = np.inf  # prevent reselection of this parking
                n_search -= 1
                # print '    n_search',n_search,'is_search',is_search

            # print '  done with id_mode is_search',is_search
            if is_search & (id_mode_fallback is not None):
                # search mode means no parking has bee found for ordinary mode
                # now try with fallback mode

                is_fallback = True
                penalties_inaccessible = np.ones(len(numbers_booking), dtype=np.float32)
                n_search = n_retrials
                while (n_search > 0) & is_search:
                    dists = np.sum((coord-coord_parking)**2, 1)
                    ind_parking_closest = inds_parking_avail[np.argmin(
                        dists + c_spread*lengths*numbers_booking*penalties_inaccessible)]
                    # print '  ind_parking_closest,n_avail',ind_parking_closest,len(inds_parking_avail)
                    is_search = (not (lanes.get_accesslevel([ids_lane[ind_parking_closest]], id_mode_fallback) >= 0))\
                        | (dists[ind_parking_closest] > dist_walk_max_sq)
                    penalties_inaccessible[ind_parking_closest] = np.inf  # prevent reselection of this parking
                    n_search -= 1
                    # print '    fallback n_search',n_search,'is_search',is_search

            if is_search:
                print 'WARNING: inaccessible parking for id_veh', id_veh, 'is_fallback', is_fallback
                print '  dist=%.1f' % (np.sqrt(dists[ind_parking_closest])), 'id_lane', ids_lane[ind_parking_closest], 'al', lanes.get_accesslevel([ids_lane[ind_parking_closest]], id_mode_fallback)

            inds_vehparking[i] = ind_parking_closest
            are_fallback[i] = is_fallback
            # print '    coords_veh',coord
            # print '    coord_park',coord_parking[ind_parking_closest]
            numbers_booking[ind_parking_closest] += 1

            #id_parking = self.get_ids([ind_parking_closest])
            #id_edge, pos = self.get_edge_pos_parking(id_parking)
            # print '    id_veh=%s,id_parking_closest=%s, dist =%.2fm'%(id_veh,id_parking,np.sqrt(np.sum((coord-coord_parking[ind_parking_closest])**2)))
            # ids_bookedveh[ind_parking_closest]=id_veh # occupy parking
            # print '    id_edge, pos',id_edge, pos
            # inds_parking_avail.remove(ind_parking_closest)
            i += 1

        # print '  inds_vehparking',  inds_vehparking
        # print '  ids_vehparking',  self.get_ids(inds_vehparking)
        # print '  ids_veh',ids_veh
        #self.ids_bookedveh.value[inds_vehparking] = ids_veh
        # self.ids_bookedveh.[ids_parking] =ids_bookedveh
        return self.get_ids(inds_vehparking), are_fallback

    def assign_parking(self, ids_veh, coords, is_overbook=False):
        """
        Assigns a parking space to each vehicle as close as possible to coords.
        Only one vehicle can be assigned to a parking space.
        """

        #inds_person = self.persons.get_inds(ids_person)
        n = len(ids_veh)
        # print 'assign_parking',n

        #parking = self.get_landuse().parking
        #inds_parking = parking.get_inds()
        coord_parking = self.vertices.value[:, 1, :]

        inds_vehparking = np.zeros(n, int)

        inds_parking_avail = np.flatnonzero(self.ids_bookedveh.value == -1).tolist()

        #ids_veh = np.zeros(n,object)
        i = 0
        for id_veh, coord in zip(ids_veh, coords):
            # print '\n  id_veh,coord',id_veh,coord
            # print '    landuse.id_bookedveh_parking',landuse.id_bookedveh_parking
            #

            # print '    inds_parking_avail',inds_parking_avail
            # print '    dists',np.sum((coord-coord_parking[inds_parking_avail])**2,1),np.argmin(np.sum((coord-coord_parking[inds_parking_avail])**2,1))
            ind_parking_closest = inds_parking_avail[np.argmin(np.sum((coord-coord_parking[inds_parking_avail])**2, 1))]
            # print '  ind_parking_closest,n_avail',ind_parking_closest,len(inds_parking_avail)
            inds_vehparking[i] = ind_parking_closest
            # print '  id_veh=%s,id_parking_closest=%s, dist =%.2fm'%(id_veh,self.get_ids([ind_parking_closest]),np.sqrt(np.sum((coord-coord_parking[ind_parking_closest])**2)))
            # ids_bookedveh[ind_parking_closest]=id_veh # occupy parking

            inds_parking_avail.remove(ind_parking_closest)
            i += 1

        # print '  inds_vehparking',  inds_vehparking
        # print '  ids_vehparking',  self.get_ids(inds_vehparking)
        # print '  ids_veh',ids_veh
        self.ids_bookedveh.value[inds_vehparking] = ids_veh
        # self.ids_bookedveh.[ids_parking] =ids_bookedveh
        return self.get_ids(inds_vehparking), inds_vehparking


class Landuse(cm.BaseObjman):
    def __init__(self, scenario=None, net=None,  **kwargs):
        self._init_objman(ident='landuse', parent=scenario, name='Landuse', **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))

        if scenario is not None:
            net = scenario.net
        # self.net = attrsman.add(   cm.ObjConf( net, is_child = False ) )# link only

        self.landusetypes = attrsman.add(cm.ObjConf(LanduseTypes(self)))
        self.zones = attrsman.add(cm.ObjConf(Zones(self, net.edges)))
        self.facilities = attrsman.add(cm.ObjConf(Facilities(self, self.landusetypes, self.zones, net=net)))
        self.parking = attrsman.add(cm.ObjConf(Parking(self, net.lanes)))
        self.maps = attrsman.add(cm.ObjConf(maps.Maps(self)))

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        self.zones.update_netoffset(deltaoffset)
        self.facilities.update_netoffset(deltaoffset)
        self.parking.update_netoffset(deltaoffset)
        self.maps.update_netoffset(deltaoffset)

    def get_net(self):
        # parent of landuse must be scenario
        if self.parent is not None:
            return self.parent.net
        else:
            return None

    def export_polyxml(self, filepath=None, encoding='UTF-8', delta=np.zeros(3, dtype=np.float32)):
        """
        Export landuse facilities to SUMO poly.xml file.
        """
        if len(self.facilities) == 0:
            return None

        if filepath is None:
            if self.parent is not None:
                filepath = self.get_filepath()
            else:
                filepath = os.path.join(os.getcwd(), 'landuse.poly.xml')

        print 'export_polyxml', filepath
        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_poly_xml: could not open', filepath
            return None

        #xmltag, xmltag_item, attrname_id = self.xmltag
        xmltag_poly = 'additional'
        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        fd.write(xm.begin(xmltag_poly))
        indent = 2

        fd.write(xm.start('location', indent+2))
        # print '  groups:',self.parent.net.get_attrsman().get_groups()
        for attrconfig in self.parent.net.get_attrsman().get_group('location'):
            # print '    locationconfig',attrconfig.attrname
            if attrconfig.attrname == '_boundaries':
                delta_bb = np.zeros(4, dtype=np.float32)
                delta_bb[0:2] = delta[:2]
                delta_bb[2:4] = delta[:2]
                fd.write(xm.arr('convBoundary', attrconfig.get_value()-delta_bb))
            else:
                attrconfig.write_xml(fd)
        fd.write(xm.stopit())

        self.facilities.write_xml(fd, indent=indent+2, is_print_begin_end=False, delta=delta)

        fd.write(xm.end(xmltag_poly))
        fd.close()
        return filepath

    def get_filepath(self):
        return self.parent.get_rootfilepath() + '.poly.xml'

    def import_polyxml(self, rootname=None, dirname='', filepath=None, is_clear=True, **kwargs):
        if filepath is None:
            if rootname is not None:
                filepath = os.path.join(dirname, rootname+'.poly.xml')
            else:
                filepath = self.get_filepath()

        if os.path.isfile(filepath):
            self.facilities.import_poly(filepath, is_clear=is_clear, **kwargs)

        else:
            self.get_logger().w('import_xml: files not found:'+filepath, key='message')

        #
        # here may be other relevant imports
        #


class FacilityGenerator(Process):
    def __init__(self, ident='facilitygenerator', facilities=None,  logger=None, **kwargs):
        print 'FacilityGenerator.__init__'

        # TODO: let this be independent, link to it or child??

        self._init_common(ident,
                          parent=facilities,
                          name='Facility Generator',
                          logger=logger,
                          info='Generates facilities (buildigs, factories, parks, etc.) in a given street network.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        # make for each possible pattern a field for prob

        self.edgelength_min = attrsman.add(cm.AttrConf('edgelength_min', kwargs.get('edgelength_min', 50.0),
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Minimum edge length',
                                                       unit='m',
                                                       info="""Minimum edge length for which houses are generated.""",
                                                       ))

        self.priority_max = attrsman.add(cm.AttrConf('priority_max', kwargs.get('priority_max', 7),
                                                     groupnames=['options'],
                                                     perm='rw',
                                                     name='Max. priority',
                                                     info="""Maximum edge priority where facilities will be created.""",
                                                     ))

        self.height_max = attrsman.add(cm.AttrConf('height_max', kwargs.get('height_max', 20.0),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   unit='m',
                                                   name='Max facility height',
                                                   info="""Maximum height of facilities.""",
                                                   ))

        self.capacity_max = attrsman.add(cm.AttrConf('capacity_max', kwargs.get('capacity_max', 1000),
                                                     groupnames=['options'],
                                                     perm='rw',
                                                     name='Max. facility capacity',
                                                     info="""Maximum capacity of a facility. Capacity is the number of adulds living in a house or working in a factory.""",
                                                     ))

        self.n_retry = attrsman.add(cm.AttrConf('n_retry', kwargs.get('n_retry', 5),
                                                groupnames=['options'],
                                                perm='rw',
                                                name='Retry number',
                                                info="""Number of times the algorithm is trying to fit a facility in a road-gap.""",
                                                ))

        # self.id_facilitytype = attrsman.add(cm.AttrConf( 'id_facilitytype',kwargs.get('id_facilitytype',1),
        #                    groupnames = ['options'],
        #                    perm='rw',
        #                    choices = self.parent.facilities.facilitytypes.get_value().names.get_indexmap(),
        #                    name = 'Facility type',
        #                    info = """Facility type to be generated.""",
        #                    ))

    def do(self):
        print self.get_name()+'.do'
        # links
        facilities = self.parent
        net = facilities.parent.get_net()
        edges = net.edges
        nodes = net.nodes
        #self._edges = edges

        #self._segvertices = edges.get_segvertices_xy()
        x1, y1, x2, y2 = edges.get_segvertices_xy()

        logger = self.get_logger()

        ids_edge = edges.select_ids((edges.widths_sidewalk.get_value() > 0)
                                    & (edges.lengths.get_value() > self.edgelength_min)
                                    & (edges.priorities.get_value() < self.priority_max)
                                    )
        facilitytypes = facilities.facilitytypes.get_value()

        # here we can make a selection
        facilitytypeobjs = facilitytypes.typeobjects[facilitytypes.get_ids()]

        # print '  facilitytypes, facilitytypeobjs',facilitytypes,facilitytypeobjs
        n_factypes = len(facilitytypes)
        n_fac = 0
        n_edges = len(ids_edge)
        #logger.w('Add facilities to %d edges')
        # print '  eligible edges =',ids_edge
        i = 0.0
        for id_edge, edgelength, id_fromnode, id_tonode, shape, edgewidth\
                in zip(ids_edge, edges.lengths[ids_edge],
                       edges.ids_fromnode[ids_edge],
                       edges.ids_tonode[ids_edge],
                       edges.shapes[ids_edge],
                       edges.widths[ids_edge],
                       ):
            pos = 5.0
            # print '  Build at edge',id_edge,edgelength
            #logger.w('Add facilities to %d edges')
            i += 1
            logger.progress(i/n_edges*100)

            # identify opposite edge, which needs to be excluded
            # from bulding overlapping check
            if (nodes.ids_incoming[id_fromnode] is not None)\
                    & (nodes.ids_outgoing[id_tonode] is not None):
                ids_incoming_fomnode = set(nodes.ids_incoming[id_fromnode])
                ids_outgoing_tonode = set(nodes.ids_outgoing[id_tonode])

                id_edge_opp_set = ids_incoming_fomnode.intersection(ids_outgoing_tonode)
                if len(id_edge_opp_set) > 0:
                    id_edge_opp = id_edge_opp_set.pop()
                    inds_seg_opp = edges.get_inds_seg_from_id_edge(id_edge_opp)
                else:
                    # print '   no edge in opposite direction'
                    id_edge_opp = -1
                    inds_seg_opp = None
            else:
                id_edge_opp = -1
                inds_seg_opp = None
            #ids_tonode_outgoing = edges.ids_tonode[nodes.ids_outgoing[id_tonode]]
            # net.get_ids_edge_from_inds_seg(inds_seg)
            # net.get_inds_seg_from_id_edge(id_edge)

            while pos < edgelength:
                facilitytype = facilitytypeobjs[0]  # could be according to statistics
                # print '    next position',pos
                n_trials = self.n_retry
                is_success = False
                while (n_trials > 0) & (not is_success):
                    length_fac = random.uniform(facilitytype.length_min, facilitytype.length_max)
                    width_fac = random.uniform(facilitytype.width_min, facilitytype.width_max)

                    # fix from to positions
                    pos11 = pos
                    pos21 = pos + length_fac
                    if pos21 < edgelength:
                        # print '      try place',n_trials,facilitytype,'id_edge',id_edge,pos11,pos21,edgelength

                        coord11, angle = get_coord_angle_on_polyline_from_pos(shape, pos11)
                        dxn = np.cos(angle-np.pi/2)
                        dyn = np.sin(angle-np.pi/2)
                        coord12 = [coord11[0]+width_fac*dxn, coord11[1]+width_fac*dyn, coord11[2]]

                        coord21, angle = get_coord_angle_on_polyline_from_pos(shape, pos21)
                        dxn = np.cos(angle-np.pi/2)
                        dyn = np.sin(angle-np.pi/2)
                        coord22 = [coord21[0]+width_fac*dxn, coord21[1]+width_fac*dyn, coord21[2]]

                        id_edge1 = edges.get_ids_edge_from_inds_seg(self.get_segind_closest_edge(
                            coord12, x1, y1, x2, y2, inds_seg_exclude=inds_seg_opp))

                        #id_edge2 = edges.get_ids_edge_from_inds_seg(self.get_segind_closest_edge(coord22, x1,y1,x2,y2, inds_seg_exclude = inds_seg_opp))
                        # print '      id_edge,id_edge1,id_edge2',id_edge,id_edge1,id_edge2
                        # print '      shape =',np.array([coord11, coord12, coord22, coord21,], dtype = np.float32)
                        if id_edge1 == id_edge:
                            id_edge2 = edges.get_ids_edge_from_inds_seg(self.get_segind_closest_edge(
                                coord22, x1, y1, x2, y2, inds_seg_exclude=inds_seg_opp))

                            if id_edge2 == id_edge:
                                id_fac = facilities.generate(facilitytype,
                                                             offset=coord11,  # offset
                                                             length=length_fac,
                                                             width=width_fac,
                                                             #bbox = [coord11, coord12, coord22, coord21,],
                                                             id_landusetype=None,
                                                             angle=angle,
                                                             pos_edge=pos11,
                                                             capacity=self.capacity_max,  # could be function of dist to center/pop
                                                             height_max=self.height_max,  # could be function of dist to center
                                                             id_edge=id_edge,
                                                             width_edge=edgewidth,
                                                             )

                                if id_fac != -1:
                                    # print '    ****generation successful id_fac=',id_fac
                                    is_success = True
                                    n_fac += 1

                    n_trials -= 1

                pos = pos21
                # print '    update with pos',pos
                # generate a parallel shape with distance  width_fac
                #angles_perb = get_angles_perpendicular(shape)
                #dxn = np.cos(angles_perb)
                #dyn = np.sin(angles_perb)
                #shape2 = np.zeros(shape.shape, np.float32)
                #shape2[:,0] = dxn*width_fac + shape[:,0]
                #shape2[:,1] = dyn*width_fac + shape[:,1]
                #shape2[:,2] = shape[:,2]

                # check if positions on parallel shape are closest to
                # this edge or closer to another edge
        print '  Done, generated %d facilities' % n_fac
        return True

    def get_segind_closest_edge(self, p, x1, y1, x2, y2, inds_seg_exclude=None):
        d2 = get_dist_point_to_segs(p[0:2], x1, y1, x2, y2, is_ending=True)
        if inds_seg_exclude is not None:
            d2[inds_seg_exclude] = np.inf
        # print '  min(d2)=',np.min(d2),'argmin=',np.argmin(d2),self.get_ids(self._edgeinds[np.argmin(d2)])
        return np.argmin(d2)


class ParkingGenerator(Process):
    def __init__(self, ident='parkinggenerator', parking=None,  logger=None, **kwargs):
        print 'ParkingGenerator.__init__'

        # TODO: let this be independent, link to it or child??

        self._init_common(ident,
                          parent=parking,
                          name='On Road parking generator',
                          logger=logger,
                          info='Generates on road parking.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))
        scenario = parking.parent.parent

        self.id_mode = attrsman.add(cm.AttrConf('id_mode', kwargs.get('id_mode', MODES['passenger']),
                                                groupnames=['options'],
                                                choices=scenario.net.modes.names.get_indexmap(),
                                                perm='rw',
                                                name='Mode ID',
                                                info="""Mode of parked vehicles. This is to select lanes which must be accessible for this mode.""",
                                                ))

        self.length_min = attrsman.add(cm.AttrConf('length_min', kwargs.get('length_min', 42.0),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   unit='m',
                                                   name='Min. edge length',
                                                   info="""Minimum edge length in order to qualify for parking.""",
                                                   ))

        self.length_noparking = attrsman.add(cm.AttrConf('length_noparking', kwargs.get('length_noparking', 15.0),
                                                         groupnames=['options'],
                                                         perm='rw',
                                                         unit='m',
                                                         name='No parking length',
                                                         info="""Length from junction to the first or last parking on an edge.""",
                                                         ))

        self.length_lot = attrsman.add(cm.AttrConf('length_lot', kwargs.get('length_lot', 6.0),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   unit='m',
                                                   name='Lot length',
                                                   info="""Length of a single parking lot.""",
                                                   ))

        self.angle = attrsman.add(cm.AttrConf('angle', kwargs.get('angle', 0.0),
                                              groupnames=['options'],
                                              perm='rw',
                                              name='Angle',
                                              info="""Angle of parking with respect ti lane direction. Currently only 0.0 is possible.""",
                                              ))

        self.priority_max = attrsman.add(cm.AttrConf('priority_max', kwargs.get('priority_max', 7),
                                                     groupnames=['options'],
                                                     perm='rw',
                                                     name='Max. priority',
                                                     info="""Maximum edge priority where parkings will be created.""",
                                                     ))

        self.n_freelanes_min = attrsman.add(cm.AttrConf('n_freelanes_min', kwargs.get('n_freelanes_min', 1),
                                                        groupnames=['options'],
                                                        perm='rw',
                                                        name='Min. free lanes',
                                                        info="""Minimum number of free lanes on the edge. These is the minimum number of lanes excluding the parking lane.""",
                                                        ))
        self.is_clear = attrsman.add(cm.AttrConf('is_clear', kwargs.get('is_clear', True),
                                                 groupnames=['options'],
                                                 perm='rw',
                                                 name='Clear',
                                                 info="""Clear previous parking areas from ntework.""",
                                                 ))

    def do(self):
        print self.get_name()+'.do'
        # links
        # print '  self.id_mode',self.id_mode
        # print '  self.get_kwoptions()',self.get_kwoptions()
        logger = self.get_logger()

        self.parent.make_parking(logger=logger, **self.get_kwoptions())
        return True


class OsmPolyImporter(CmlMixin, Process):
    def __init__(self,  landuse=None,
                 osmfilepaths=None,
                 typefilepath=None,
                 polyfilepath=None,
                 projparams=None,
                 offset_x=None,
                 offset_y=None,
                 is_keep_full_type=True,
                 is_import_all_attributes=True,
                 is_use_name_for_id=False,
                 polytypefilepath='',
                 is_clean_osmfile=True,
                 is_merge=False,
                 logger=None, **kwargs):
        print 'OsmPolyImporter.__init__', landuse, landuse.parent.get_rootfilename()
        self._init_common('osmpolyimporter', name='OSM Poly import',
                          logger=logger,
                          info='Converts a OSM  file to a SUMO Poly file and read facilities into scenario.',
                          )
        if landuse is None:
            self._landuse = Landuse()
        else:
            self._landuse = landuse

        self.init_cml('polyconvert')  # pass main shell command

        if landuse.parent is not None:
            scenario = landuse.parent
            rootname = scenario.get_rootfilename()
            rootdirpath = scenario.get_workdirpath()
            if hasattr(scenario, 'net'):
                if projparams is None:
                    projparams = scenario.net.get_projparams()
                if (offset_x is None) & (offset_y is None):
                    offset_x, offset_y = scenario.net.get_offset()
        else:
            rootname = landuse.get_ident()
            rootdirpath = os.getcwd()

        if polyfilepath is None:
            polyfilepath = os.path.join(rootdirpath, rootname+'.poly.xml')

        if osmfilepaths is None:
            osmfilepaths = os.path.join(rootdirpath, rootname+'.osm.xml')

        if typefilepath is None:
            if 'SUMO_HOME' in os.environ:
                typemapdir = os.path.join(os.environ['SUMO_HOME'], 'data', 'typemap')
                typefilepath = os.path.join(typemapdir, 'osmPolyconvert.typ.xml')
            else:
                print("No poly typemaps found. Please declare environment variable 'SUMO_HOME'")
                typefilepath = ''

        attrsman = self.get_attrsman()

        self.rootdirpath = rootdirpath

        self.rootname = rootname

        self.add_option('osmfilepaths', osmfilepaths,
                        groupnames=['options'],
                        cml='--osm-files',
                        perm='rw',
                        name='OSM files',
                        wildcards='OSM XML files (*.osm)|*.osm*',
                        metatype='filepaths',
                        info='Openstreetmap files to be imported.',
                        )

        self.is_merge = attrsman.add(cm.AttrConf('is_merge', is_merge,
                                                 groupnames=['options'],
                                                 perm='rw',
                                                 name='Merge',
                                                 info='If set, imported polygons are merged with existing.',
                                                 ))

        self.is_clean_osmfile = attrsman.add(cm.AttrConf('is_clean_osmfile', is_clean_osmfile,
                                                         groupnames=['options'],
                                                         perm='rw',
                                                         name='Clean OSM files',
                                                         info='If set, OSM files are cleaned from strange characters prior to import (recommended).',
                                                         ))

        self.height_default = attrsman.add(cm.AttrConf('height_default', kwargs.get('height_default', 7.0),
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Default height',
                                                       info='Default height of facilities in case no height information is available.',
                                                       ))

        self.type_default = attrsman.add(cm.AttrConf('type_default', kwargs.get('type_default', 'building.yes'),
                                                     groupnames=['options'],
                                                     perm='rw',
                                                     name='Default facility type',
                                                     info='Default type of facilities in case no type information is available.',
                                                     ))

        self.add_option('polyfilepath', polyfilepath,
                        groupnames=['_private'],
                        cml='--output-file',
                        perm='r',
                        name='Poly file',
                        wildcards='Poly XML files (*.poly.xml)|*.poly.xml',
                        metatype='filepath',
                        info='SUMO Poly file in XML format.',
                        )

        self.add_option('typefilepath', typefilepath,
                        groupnames=['options'],
                        cml='--type-file',
                        perm='rw',
                        name='Type file',
                        wildcards='Typemap XML files (*.typ.xml)|*.typ.xml',
                        metatype='filepath',
                        info="""Typemap XML files. In these file, 
OSM building types are mapped to specific facility parameters, is not explicitely set by OSM attributes.""",
                        )

        # --net-file <FILE> 	Loads SUMO-network FILE as reference to offset and projection
        self.add_option('projparams', projparams,
                        groupnames=['options'],
                        cml='--proj',
                        perm='rw',
                        name='projection',
                        info='Uses STR as proj.4 definition for projection. Default is the projection of the network, better do not touch!',
                        is_enabled=lambda self: self.projparams is not None,
                        )

        self.add_option('offset_x', offset_x,
                        groupnames=['options', 'geometry'],
                        cml='--offset.x ',
                        perm='rw',
                        unit='m',
                        name='X-Offset',
                        info='Adds offset to net x-positions; default: 0.0',
                        is_enabled=lambda self: self.offset_x is not None,
                        )
        self.add_option('offset_y', offset_y,
                        groupnames=['options', 'geometry'],
                        cml='--offset.y ',
                        perm='rw',
                        unit='m',
                        name='Y-Offset',
                        info='Adds offset to net x-positions; default: 0.0',
                        is_enabled=lambda self: self.offset_y is not None,
                        )

        self.add_option('is_keep_full_type', is_keep_full_type,
                        groupnames=['options'],
                        cml='--osm.keep-full-type',
                        perm='rw',
                        name='keep full OSM type',
                        info='The type will be made of the key-value - pair.',
                        )

        self.add_option('is_import_all_attributes', is_keep_full_type,
                        groupnames=['options'],
                        cml='--all-attributes',
                        perm='rw',
                        name='import all attributes',
                        info='Imports all OSM attributes.',
                        )

        self.add_option('is_use_name_for_id', is_use_name_for_id,
                        groupnames=['options'],
                        cml='--osm.use-name',
                        perm='rw',
                        name='use OSM name for id',
                        info=' 	The OSM id (not internal ID) will be set from the given OSM name attribute.',
                        )

        self.add_option('polytypefilepath', polytypefilepath,
                        groupnames=[],  # ['_private'],#
                        cml='--type-file',
                        perm='rw',
                        name='Poly type file',
                        wildcards='Net XML files (*.xml)|*.xml',
                        metatype='filepath',
                        info='SUMO Poly type file in XML format.',
                        is_enabled=lambda self: self.polytypefilepath != '',
                        )

    def update_params(self):
        """
        Make all parameters consistent.
        example: used by import OSM to calculate/update number of tiles
        from process dialog
        """
        pass
        #self.workdirpath = os.path.dirname(self.netfilepath)
        #bn =  os.path.basename(self.netfilepath).split('.')
        # if len(bn)>0:
        #    self.rootname = bn[0]

    def do(self):
        self.update_params()
        cml = self.get_cml()

        if self.is_clean_osmfile:
            for path in self.osmfilepaths.split(','):
                path_temp = path+'.clean'
                clean_osm(path, path_temp)
                #shutil.copy (path_temp, path)
                shutil.move(path_temp, path)

        # print 'SumonetImporter.do',cml
        #import_xml(self, rootname, dirname, is_clean_nodes = True)
        self.run_cml(cml)
        if self.status == 'success':
            if os.path.isfile(self.polyfilepath):
                print '  OSM->poly.xml successful, start importing xml files'
                self._landuse.import_polyxml(self.rootname, self.rootdirpath,
                                             is_clear=not self.is_merge,
                                             type_default=self.type_default,
                                             height_default=self.height_default)
                print '  import poly in sumopy done.'
                return True
            return False
        else:
            return False

    def get_landuse(self):
        # used to het landuse in case landuse has been created
        return self._landuse


if __name__ == '__main__':
    ###############################################################################
    # print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    #from coremodules.scenario import scenario
    from coremodules.network import network
    logger = Logger()
    NETPATH = os.path.join(SUMOPYDIR, 'coremodules', 'network', 'testnet')
    net = network.Network(logger=logger)
    rootname = 'facsp2'
    net.import_xml(rootname, NETPATH)
    # net.read_sumonodes(os.path.join(NETPATH,'facsp2.nod.xml'))
    # net.read_sumoedges(os.path.join(NETPATH,'facsp2.edg.xml'))
    landuse = Landuse(net=net, logger=logger)

    # landuse.facilities.import_poly(os.path.join(NETPATH,'facsp2.poly.xml'))
    landuse.import_xml(rootname, NETPATH)
    objbrowser(landuse)
