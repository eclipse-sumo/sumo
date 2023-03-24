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

# @file    shapeformat.py
# @author  Joerg Schweizer
# @date   2012


import time
import os
import types
import re
import numpy as np
from xml.sax import saxutils, parse, handler

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.processes import Process  # ,CmlMixin,ff,call
#from coremodules.scenario import scenario
from agilepy.lib_base.misc import get_inversemap
import shapefile


try:
    try:
        import pyproj
        IS_PROJ = True
    except:
        from mpl_toolkits.basemap import pyproj
        IS_PROJ = True

except:
    IS_PROJ = False
    print 'Import error: in order to run the traces plugin please install the following modules:'
    print '   mpl_toolkits.basemap and shapely'
    print 'Please install these modules if you want to use it.'
    print __doc__
    raise


try:
    from osgeo import osr
    IS_GDAL = True
except:
    IS_GDAL = False
    print 'WARNING: GDAL module is not installed.'


# Value Shape Type
SHAPETYPES = {
    'Null Shape': 0,
    'Point': 1,
    'PolyLine': 3,
    'Polygon': 5,
    'MultiPoint': 8,
    'PointZ': 11,
    'PolygonZ': 13,
    'MultiPointZ': 15,
    'PointM': 21,
    'PolyLineM': 23,
    'PolygonM': 25,
    'MultiPointM': 28,
    'MultiPatch': 31,
}
# 0 Null Shape
# 1 Point
# 3 PolyLine
# 5 Polygon
# 8 MultiPoint
# 11 PointZ
# 13 PolyLineZ
# 15 PolygonZ
# 18 MultiPointZ
# 21 PointM
# 23 PolyLineM
# 25 PolygonM
# 28 MultiPointM
# 31 MultiPatch

VEHICLECLASSCODE = {
    "": 'X',  # All
    "private": 'I',
    "emergency": 'E',
    "authority": 'I',
    "army": 'I',
    "vip": 'I',
    "passenger": 'I',
    "hov": 'I',
    "taxi": 'P',
    "bus": 'P',
    "coach": 'P',
    "delivery": 'I',
                "truck": 'I',
                "trailer": 'I',
                "tram": 'P',
                "rail_urban": 'P',
                "rail": 'P',
                "rail_electric": 'P',
                "motorcycle": 'I',
                "moped": 'I',
                "bicycle": 'B',
                "pedestrian": 'F',
                "evehicle": 'I',
                "custom1": 'P',
                "custom2": 'I',
}


def guess_utm_from_coord(coord):
    """
    Returns UTM projection parameters from  an example point 
    with coord = (lat,lon)
    """
    zone = get_zone(coord)
    return "+proj=utm +zone=%d +ellps=WGS84 +datum=WGS84 +units=m +no_defs" % zone


def get_zone(coordinates):
    if 56 <= coordinates[1] < 64 and 3 <= coordinates[0] < 12:
        return 32
    if 72 <= coordinates[1] < 84 and 0 <= coordinates[0] < 42:
        if coordinates[0] < 9:
            return 31
        elif coordinates[0] < 21:
            return 33
        elif coordinates[0] < 33:
            return 35
        return 37
    return int((coordinates[0] + 180) / 6) + 1


def get_letter(coordinates):
    return 'CDEFGHJKLMNPQRSTUVWXX'[int((coordinates[1] + 80) / 8)]


def get_shapeproj(projparams):
    """
    Returns text for shape .prj file
    This makes use of links like 
    http://spatialreference.org/ref/epsg/4326/prj/
    http://spatialreference.org/ref/epsg/wgs-84-utm-zone-32n/prj/
    +proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs
    +init=EPSG:23032
    """
    import urllib
    # print 'get_shapeproj',projparams
    params = {}
    for elem in projparams.split('+'):
        if elem.find('=') > 0:
            attr, val = elem.split('=')
            params[attr.strip()] = val.strip()
    # print 'params',params

    if params.has_key('init'):
        if params['init'].lower().find('epsg') >= 0:
            epgs, number = params['init'].lower().split(':')
            # print   epgs,number
            html = 'http://spatialreference.org/ref/epsg/%s/prj/' % number

    elif params.has_key('datum'):
        if params['datum'].lower().find('wgs') >= 0:
            number = params['datum'][3:]
            # print 'wgs', params['zone']+'n'
            html = 'http://spatialreference.org/ref/epsg/wgs-%s-%s-zone-%sn/prj/' % (
                number, params['proj'], params['zone'])

    # print 'html=',html
    f = urllib.urlopen(html)
    return (f.read())


def get_proj4_from_shapefile(filepath):

    parts = os.path.basename(filepath).split('.')
    basename = ''
    for part in parts[:-1]:
        basename += part

    dirname = os.path.dirname(filepath)

    shapefilepath = os.path.join(dirname, basename)

    projfilepath = shapefilepath+'.prj'
    proj4 = ''
    if not os.path.isfile(projfilepath):
        projfilepath = shapefilepath+'.PRJ'

    if os.path.isfile(projfilepath):
        prj_file = open(projfilepath, 'r')
        prj_txt = prj_file.read()

        if IS_GDAL:
            srs = osr.SpatialReference()
            if srs.ImportFromWkt(prj_txt):
                raise ValueError("Error importing PRJ information from: %s" % shapeprojpath)
            # srs.ImportFromESRI([prj_txt])
            srs.AutoIdentifyEPSG()
            # print 'Shape prj is: %s' % prj_txt
            # print 'WKT is: %s' % srs.ExportToWkt()
            # print 'Proj4 is: %s' % srs.ExportToProj4()

            # print 'EPSG is: %s' % srs.GetAuthorityCode(None)
            proj4 = srs.ExportToProj4()

        else:
            res = re.search(r'(GCS)_([a-zA-Z]+)_([0-9]+)', prj_txt)
            if res is not None:
                groups = res.groups()
                if len(groups) == 3:
                    gcs, proj, date = groups
                    proj4 = '+proj=longlat +datum=%s%s +no_defs' % (proj, date[2:])
    else:
        print 'WARNING: found no prj file', projfilepath
    return proj4
    # return "+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
    # return '+proj=longlat +datum=WGS84 +ellps=WGS84 +a=6378137.0 +f=298.257223563 +pm=0.0  +no_defs'


def get_shapefile(filepath):
    """
    Returns shapefile handler and proj4 parameter string
    of shape file with given path.
    """
    parts = os.path.basename(filepath).split('.')
    basename = ''
    for part in parts[:-1]:
        basename += part

    dirname = os.path.dirname(filepath)

    shapefilepath = os.path.join(dirname, basename)

    print 'import_shapefile *%s*' % (shapefilepath), type(str(shapefilepath))
    sf = shapefile.Reader(str(shapefilepath))

    return sf


def get_fieldinfo(field):
    """
    Returns attrname,default,dtype,digits_fraction
    """

    attrname, ftype, flen, fdigit = field
    attrname = attrname.strip()

    dtype = 'object'
    default = ''
    digits_fraction = None

    if ftype == 'C':
        dtype = 'object'
        default = ''

    elif ftype == 'N':
        if fdigit > 0:
            digits_fraction = fdigit
            dtype = 'float32'
            default = 0.0
        else:
            dtype = 'int32'
            default = 0

    return attrname, default, dtype, digits_fraction


class ShapefileImporter(Process):
    def __init__(self,  ident='shapefileimporter', parent=None,
                 name='Shape file importer',
                 filepath='',
                 coordsattr='',
                 attrnames_to_shapeattrs={},
                 info='Import of shape files in parent datastructure.',
                 logger=None,
                 **kwargs):

        print 'ShapefileImporter.__init__', filepath  # ,projparams_target_default, projparams_shape
        self._init_common(ident,
                          parent=parent,
                          name=name,
                          logger=logger,
                          info=info,
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.filepath = attrsman.add(cm.AttrConf('filepath', filepath,
                                                 groupnames=['options'],
                                                 perm='rw',
                                                 name='Shape file',
                                                 wildcards='Shape file (*.shp)|*.shp|All files (*.*)|*.*',
                                                 metatype='filepath',
                                                 info="""File path of shape file. Note that only the file with the extention ".shp" needs to be selected. Attention: all file extensions must be in small letters, for example .shp .dbf, shx, etc""",
                                                 ))

        # print 'self.filepath',self.filepath
        attrsman_parent = parent.get_attrsman()
        self._coordsconfig = attrsman_parent.get_config(coordsattr)

        for attrname, shapeattr_default in attrnames_to_shapeattrs.iteritems():
            config = attrsman_parent.get_config(attrname)
            fieldname = 'fieldname_'+attrname
            setattr(self, fieldname,
                    attrsman.add(cm.AttrConf(fieldname, shapeattr_default,
                                             groupnames=['options', 'field names'],
                                             perm='rw',
                                             attrname_orig=attrname,
                                             name='Field for '+config.get_name(),
                                             info='Field name for the following attribute: '+config.get_info(),
                                             )))

        self.projparams_shape = attrsman.add(cm.AttrConf('projparams_shape', kwargs.get("projparams_shape", ''),
                                                         groupnames=['options', ],
                                                         perm='rw',
                                                         name='Shape projection',
                                                         info='Projection4 parameters of shape data. If blank then shape file projection is used or if not present, projection will be guessed from shape coordinates.',
                                                         ))

        # self.projparams_target = attrsman.add(cm.AttrConf(  'projparams_target', kwargs.get("projparams_target",''),
        #                                                groupnames = ['options',],
        #                                                perm='rw',
        #                                                name = 'Target projection',
        #                                                info = 'Projection4 parameters of target, where the coordinates are imported. These are typically the scenario coordinates.',
        #                                                ))

        self._projparams_target = kwargs.get("projparams_target", '')
        self.is_use_shapeproj = attrsman.add(cm.AttrConf('is_use_shapeproj', kwargs.get("is_use_shapeproj", False),
                                                         groupnames=['options', ],
                                                         perm='rw',
                                                         name='Use shapefile projection?',
                                                         info='If selected, projection in shape file will be used to interpret projection. If not selected, target projection will be used.',
                                                         ))

        self.is_use_targetproj = attrsman.add(cm.AttrConf('is_use_targetproj', kwargs.get("is_use_targetproj", True),
                                                          groupnames=['options', ],
                                                          perm='rw',
                                                          name='Use target projection?',
                                                          info='If selected, target will be used to interpret projection.',
                                                          ))

        self.is_probe_offset = attrsman.add(cm.AttrConf('is_probe_offset', kwargs.get("is_probe_offset", False),
                                                        groupnames=['options', ],
                                                        perm='rw',
                                                        name='Probe offset?',
                                                        info="""With probe offset, a specific coordinate from the shap-map and the target-map will be used to calculate the offset. 
                                                                  This requires the coordinates of a dedicated point of the shape file and of the target.
                                                                  This method can be used if there is an unknon offset in the shape map coordinate system.
                                                                  """,
                                                        ))

        self.x_probe_shape = attrsman.add(cm.AttrConf('x_probe_shape', kwargs.get("x_probe_shape", 0.0),
                                                      groupnames=['options', ],
                                                      perm='rw',
                                                      name='Probed x coord of shape',
                                                      unit='m',
                                                      info='Probed x coord of shape-map.',
                                                      ))

        self.y_probe_shape = attrsman.add(cm.AttrConf('y_probe_shape', kwargs.get("y_probe_shape", 0.0),
                                                      groupnames=['options', ],
                                                      perm='rw',
                                                      name='Probed y coord shape',
                                                      unit='m',
                                                      info='Probed y coord of shape-map.',
                                                      ))

        self.x_probe_target = attrsman.add(cm.AttrConf('x_probe_target', kwargs.get("x_probe_target", 0.0),
                                                       groupnames=['options', ],
                                                       perm='rw',
                                                       name='Probed x coord of target',
                                                       unit='m',
                                                       info='Probed x coord of target-map.',
                                                       ))

        self.y_probe_target = attrsman.add(cm.AttrConf('y_probe_target', kwargs.get("y_probe_target", 0.0),
                                                       groupnames=['options', ],
                                                       perm='rw',
                                                       name='Probed y coord target',
                                                       unit='m',
                                                       info='Probed y coord of target-map.',
                                                       ))

        self.is_autoffset = attrsman.add(cm.AttrConf('is_autoffset', kwargs.get("is_autoffset", False),
                                                     groupnames=['options', ],
                                                     perm='rw',
                                                     name='Auto offset?',
                                                     info='If selected, offset will be determined automatically.',
                                                     ))

        # self.offset = attrsman.add(cm.AttrConf(  'offset', kwargs.get("offset",np.array([0.0,0.0,0.0],dtype = np.float32)),
        #                                                groupnames = ['options',],
        #                                                perm='r',
        #                                                name = 'Offset',
        #                                                unit = 'm',
        #                                                info = 'Network offset in WEP coordinates',
        #                                                ))

        self._offset = kwargs.get("offset", np.array([0.0, 0.0, 0.0], dtype=np.float32))

        self.is_limit_to_boundaries = attrsman.add(cm.AttrConf('is_limit_to_boundaries', kwargs.get("is_limit_to_boundaries", True),
                                                               groupnames=['options', ],
                                                               perm='rw',
                                                               name='Limit to boundaries?',
                                                               info='Import only shapes that fit into the given boundaries.',
                                                               ))

        # self.boundaries = attrsman.add(cm.AttrConf(  'boundaries',kwargs.get('boundaries',np.array([0.0,0.0,0.0,0.0],dtype = np.float32)) ,
        #                                groupnames = ['options',],
        #                                perm='r',
        #                                name = 'Boundaries',
        #                                unit = 'm',
        #                                info = 'Area limiting boundaries',
        #                                ))
        self.boundaries = kwargs.get('boundaries', np.array([0.0, 0.0, 0.0, 0.0], dtype=np.float32))
        # if map_attrconfig2shapeattr is None:
        #    # generate attrconfs with group 'options'
        #    # and default attrconfmap
        #    self._map_attrconfig2shapeattr = {}
        #
        # else:
        #    self._map_attrconfig2shapeattr = map_attrconfig2shapeattr

    def inboundaries(self, points, x_border=0.0, y_border=0.0):
        """
        Tests if the given points are in
        the  boundaries.

        Returns a binary vector with one element for each point
        If an element is True then the corrisponding 
        point in within the networks bounding box. 
        Otherwise the point is outside.
        Elevation is ignored.
        Format of points:
         [[x1,y1,z1],[x2,y2,z2],...]



        """
        # print 'inboundaries'
        # print '  self',self.boundaries
        # print '  points',type(points),points
        # print ' return',( (self.boundaries[2] >= BB[0]) & (self.boundaries[0] <= BB[2]) &
        #     (self.boundaries[3] >= BB[1]) & (self.boundaries[1] <= BB[3]) )

        return ((self.boundaries[2]-x_border >= points[:, 0]) & (self.boundaries[0]+x_border <= points[:, 0]) &
                (self.boundaries[3]-y_border >= points[:, 1]) & (self.boundaries[1]+y_border <= points[:, 1]))

    def make_fieldinfo(self):

        self._fieldinfo = {}
        fields = self._sf.fields
        #records = sf.records()
        for ind_field, field in zip(xrange(1, len(fields)), fields[1:]):
            attrname, default, dtype, digits_fraction = get_fieldinfo(field)
            self._fieldinfo[attrname] = (ind_field-1, default, dtype, digits_fraction)

    def get_projections(self):
        print 'get_projections IS_PROJ', IS_PROJ
        print 'self.projparams_shape', self.projparams_shape, 'self._projparams_target', self._projparams_target

        proj_shape = None
        proj_target = None

        if self.is_use_shapeproj & (self.projparams_shape == ''):
            print '   no shape projection given'
            self.projparams_shape = get_proj4_from_shapefile(self.filepath)
            print '     from prj file projparams_shape', self.projparams_shape
            if self.projparams_shape == '':
                # no results from shapefile info, let's try to guess
                self.projparams_shape = self.guess_shapeproj()
                print '     from guessing projparams_shape', self.projparams_shape

        # if self.is_guess_targetproj:
        #        self.projparams_target = self.guess_targetproj()
        # print 'get_projections'
        # print '  projparams_shape',self.projparams_shape
        # print '  projparams_target',self.projparams_target
        if IS_PROJ:
            if self.is_use_shapeproj:

                try:
                    print '  use projparams_shape =*%s*' % self.projparams_shape, type(str(self.projparams_shape)), pyproj.Proj(str(self.projparams_shape))
                    proj_shape = pyproj.Proj(str(self.projparams_shape))
                except:
                    proj_shape = None

            if self.is_use_targetproj:
                try:
                    print '  use projparams_target =*%s*' % self._projparams_target, type(str(self._projparams_target)), pyproj.Proj(str(self._projparams_target))
                    proj_target = pyproj.Proj(str(self._projparams_target))
                except:
                    proj_target = None

            return proj_shape, proj_target, self._offset

        else:
            # no projection available
            return None, None, self.offset

    def guess_shapeproj(self):
        shapes = self._sf.shapes()
        #shapetype = shapes[3].shapeType
        shape_rec = self._sf.shapeRecord(0)
        points = shape_rec.shape.points
        return guess_utm_from_coord(points[0])

    def _get_attrconfs_shapeinds(self):
        attrsman_parent = self.parent.get_attrsman()
        attrconfs = []
        shapeinds = []

        for fieldconf in self.get_attrsman().get_group('field names'):

            shapeattrname = fieldconf.get_value()
            attrconf = attrsman_parent.get_config(fieldconf.attrname_orig)

            if self._fieldinfo.has_key(shapeattrname):
                attrconfs.append(attrconf)
                shapeinds.append(self._fieldinfo[shapeattrname][0])

        # for attrconf, shapeattrname in self._map_attrconfig2shapeattr.iteritems():
        #    attrconfs.append(attrconf)
        #    shapeinds.append(self._fieldinfo[shapeattrname][0])
        return attrconfs, shapeinds

    def is_ready(self):
        return True

    def import_shapes(self):
        print 'import_shapes'

        shapes = self._sf.shapes()
        shapetype = shapes[3].shapeType
        records = self._sf.records()
        n_records = len(records)
        if n_records == 0:
            return [], 0

        proj_shape, proj_target, offset_target = self.get_projections()

        # print '  proj_shape',proj_shape,'proj_target',proj_target
        if self.is_use_shapeproj & (proj_shape is None):
            print 'WARNING: import_shapes, no shape projection'
            return [], 0
        if self.is_use_targetproj & (proj_target is None):
            print 'WARNING: import_shapes, no target projection'
            return [], 0

        offset = np.array([0.0, 0.0, 0.0], dtype=np.float32)

        if self.is_probe_offset:
            offset[0:2] = (+self.x_probe_target-self.x_probe_shape, self.y_probe_target-self.y_probe_shape)

        elif not self.is_autoffset:
            offset[0:2] = offset_target

        # print '  x_probe_target',self.x_probe_target,'x_probe_shape',self.x_probe_shape
        # print '  y_probe_target',self.y_probe_target,'y_probe_shape',self.y_probe_shape

        # print '  offset',offset
        # print '  offset_target',offset_target
        coordsconfig = self._coordsconfig
        ids = coordsconfig.get_manager().add_rows(n_records)
        print '  n_records', n_records, shapetype

        if (shapetype == 3) | (shapetype == 5):  # poliline = 3, polygon = 5
            for ind_rec, id_attr in zip(xrange(n_records), ids):
                # print '  ind_rec',ind_rec,'id_attr',id_attr
                shape_rec = self._sf.shapeRecord(ind_rec)
                points = shape_rec.shape.points
                # print '  points',points
                n_points = len(points)
                shape = np.zeros((n_points, 3), dtype=np.float32) + offset
                if self.is_use_targetproj & self.is_use_shapeproj:
                    for ind, point in zip(xrange(n_points), points):
                        shape[ind, 0:2] += np.array(pyproj.transform(proj_shape, proj_target, point[0], point[1]))

                elif self.is_use_targetproj:
                    for ind, point in zip(xrange(n_points), points):
                        shape[ind, 0:2] += proj_target(point[0], point[1])

                elif self.is_use_shapeproj:
                    for ind, point in zip(xrange(n_points), points):
                        shape[ind, 0:2] += proj_shape(point[0], point[1])

                else:  # no projection
                    for ind, point in zip(xrange(n_points), points):
                        shape[ind, 0:2] += (point[0], point[1])

                coordsconfig[id_attr] = list(shape)
                # print '  coords=',coordsconfig[id_attr]

        elif shapetype == 1:
            for ind_rec, id_attr in zip(xrange(n_records), ids):
                # print '  ind_rec',ind_rec,id_attr
                shape_rec = self._sf.shapeRecord(ind_rec)
                points = shape_rec.shape.points
                # print '  points',points
                n_points = len(points)
                vert = offset.copy()
                point = points[0]
                if self.is_use_targetproj & self.is_use_shapeproj:
                    x, y = pyproj.transform(proj_shape, proj_target, point[0], point[1])
                    # print '  x,y',x,y
                    vert[0:2] += [x, y]
                    #shape[0:2] = pyproj.transform(proj_shape, proj_target ,point[0], point[1])
                elif self.is_use_targetproj:
                    x, y = proj_target(point[0], point[1])
                    vert[0:2] += [x, y]

                elif self.is_use_shapeproj:
                    x, y = proj_shape(point[0], point[1])
                    # print '  x,y',x,y
                    vert[0:2] += [x, y]
                    #vert[0:2] = proj_shape(point[0], point[1])
                else:  # no projection
                    vert[0:2] += [point[0], point[1]]

                # print ' vert',vert
                coordsconfig[id_attr] = vert

            if self.is_autoffset:
                coords = coordsconfig.get_value()

                x_min, y_min, z_min = np.min(coords, 0)
                # print '  x_min,y_min,z_min',x_min,y_min,z_min

                coordsconfig.set_value(coords-np.min(coords, 0))

            # print '  coords=',coordsconfig.get_value()
        return ids, shapetype

    def do(self):
        print self.ident+'.do'
        #fields = self._sf.fields
        #records = self._sf.records()

        if os.path.isfile(self.filepath):
            self._sf = get_shapefile(self.filepath)

        else:
            return 'WARNING: shape file not found', self.filepath
            return False

        self.make_fieldinfo()

        attrconfs, shapeinds = self._get_attrconfs_shapeinds()

        # import shape info
        ids, shapetype = self.import_shapes()

        if len(ids) == 0:
            print 'WARNING: import_shapes failed'
            return False

        n_attrs = len(attrconfs)
        n_records = len(ids)

        # import no attributes from table
        if (n_attrs == 0) | (n_records == 0):
            print 'WARNING: successfully imported no data'
            return True
        shaperecords = self._sf.shapeRecord

        # print '  attrconfs',attrconfs
        # print '  shapeinds',shapeinds
        # print '  fieldinfo',self._fieldinfo

        self.import_data(shaperecords, ids, attrconfs, shapeinds)

        # check if shapes are inside network boundary
        if self.is_limit_to_boundaries:

            if shapetype == 1:  # shape are points
                ids_outside = ids[np.logical_not(self.inboundaries(self._coordsconfig[ids]))]

            else:
                # polygon type shape
                ids_outside = []
                for id_shape, shape in zip(ids, self._coordsconfig[ids]):
                    # print ' before bcheck:shape',shape
                    if not np.any(self.inboundaries(np.array(shape, dtype=np.float32))):
                        ids_outside.append(id_shape)

            print '  ids_outside', ids_outside
            self.parent.del_rows(ids_outside)

        return True

    def import_data(self, shaperecords, ids, attrconfs, shapeinds):
        print 'import_data'
        n_records = len(ids)
        objecttype = np.dtype(np.object)  # np.dtype(np.zeros(1,dtype = np.object))
        values_invalid = ['NULL', '\n']
        for ind_rec, id_attr in zip(xrange(n_records), ids):
            shape_rec = shaperecords(ind_rec)

            # print '  shape_rec',id_attr,shape_rec.record
            # use first field as id, but will also be a regular attribute
            #id_egde = shape_rec.record[0]
            #attrrow = {}
            # print '\n id_egde',id_egde
            for ind_sfield, attrconf in zip(shapeinds, attrconfs):
                # for i,field in zip(xrange(n),fields[1:]):
                val = shape_rec.record[ind_sfield]
                # print '    ind_sfield',ind_sfield,'attrname',attrconf.attrname,'type',type(val),'>>%s<<'%(repr(val)) ,attrconf.get_value().dtype,attrconf.get_value().dtype is objecttype
                if val not in values_invalid:
                    if attrconf.get_value().dtype is objecttype:
                        # print '    if destination is object then force to string'
                        if type(val) in cm.NUMERICTYPES:
                            # this is to avoid floats as object type
                            attrconf[id_attr] = str(int(val))
                        else:
                            attrconf[id_attr] = str(val)
                        # print '      type=',type(attrconf[id_attr])
                    else:
                        attrconf[id_attr] = val

        return True


class Shapedata(am.ArrayObjman):
    """
    Contains data and methods for handling shapefiles.
    https://code.google.com/p/pyshp/wiki/PyShpDocs
    http://www.esri.com/library/whitepapers/pdfs/shapefile.pdf
    """

    def __init__(self, ident='shapedata', parent=None, name='Shapefile data',
                 filepath="",
                 projparams_shape="+init=EPSG:23032",
                 offset=[0.0, 0.0],
                 shapetype=3,
                 log=None, **kwargs):

        self._init_objman(ident=ident,
                          parent=parent,
                          name=name,
                          version=0.0,
                          **kwargs)

        self.add(cm.AttrConf('filepath', filepath,
                             groupnames=['params', ],
                             perm='rw',
                             name='Shape file',
                             wildcards='Shape files (*.shp)|*.shp',
                             metatype='filepath',
                             info="""Shape file path.""",
                             ))

        self.add(cm.AttrConf('_projparams', projparams_shape,
                             groupnames=['params', ],
                             perm='rw',
                             name='Projection',
                             info='Projection parameters'
                             ))

        self.add(cm.AttrConf('_offset', offset,
                             groupnames=['params', ],
                             perm='r',
                             name='Offset',
                             info='Offset in WEP coordinates'
                             ))

        self.add(cm.AttrConf('shapetype', shapetype,
                             groupnames=['params', ],
                             choices=SHAPETYPES,
                             perm='rw',
                             name='Shape Type',
                             info='Shape  type.'
                             ))

        # todo: make these init dependent on shapetype
        if shapetype == 1:  # 'Point':1,

            self.add_col(am.ArrayConf('coords',  np.zeros(3, dtype=np.float32),
                                      dtype=np.float32,
                                      groupnames=['_private'],
                                      perm='rw',
                                      name='Coords',
                                      unit='m',
                                      info='Point coordinates.',
                                      ))
        else:
            self.add_col(am.ListArrayConf('shapes',
                                          groupnames=['_private'],
                                          perm='rw',
                                          name='Shape',
                                          unit='deg',
                                          info='Shape is a list of coordinates.',
                                          #is_plugin = True,
                                          xmltag='shape',
                                          ))

        self._log = log

        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        """
        To be overridden
        """
        pass

    def _init_constants(self):
        """
        To be overridden
        """
        pass

    def adjust_fieldlength(self):
        # print 'adjust_fieldlength'
        for attrconf in self.get_group('shapeattrs'):

            attrname, ftype, flen, fdigit = attrconf.field

            fftype = ftype.lower()
            len_max = 0
            # print '    ',attrname,attrconf.field,fftype,attrconf.get_attr()
            if fftype == 'c':
                for s in attrconf.get_value():
                    # print '    s=',s,type(s)
                    if len(s) > len_max:
                        len_max = len(s)
            elif fftype == 'n':
                for val in attrconf.get_value():
                    l = len(str(int(val)))
                    if l > len_max:
                        len_max = l
                if fdigit > 0:
                    len_max += 1+fdigit  # includes length of "." and digits

            attrconf.field = (attrname, ftype, len_max, fdigit)

    def get_basefilepath(self, filepath):

        parts = os.path.basename(filepath).split('.')
        basename = ''
        for part in parts[:-1]:
            basename += part

        dirname = os.path.dirname(filepath)
        #shapefilepath = os.path.join(dirname,basename)
        return os.path.join(dirname, basename)

    def export_shapefile(self, filepath=None):
        """
        Exports shape data into given file.
        If no file is given, the the default file path will be selected.
        """
        # https://code.google.com/p/pyshp/
        print '\nexport_shapefile', filepath
        #proj_target, offset_target = self.parent.get_proj_offset()
        if len(self) == 0:
            return False

        if filepath is None:
            filepath = self.filepath.get_value()
        else:
            self.filepath.set_value(filepath)

        basefilepath = self.get_basefilepath(filepath)
        shapetype = self.shapetype.get_value()
        w = shapefile.Writer()
        w.shapeType = shapetype
        w.autoBalance = 1
        shapes = []
        # print '  shapeattrs:',self.get_group('shapeattrs')
        attrnames = []
        for attrconf in self.get_group('shapeattrs'):
            # print '    ',attrconf.field
            w.field(*attrconf.field)
            attrnames.append(attrconf)

        # for key in self.get_keys():
        ids_shape = self.get_ids()
        offset = self._offset.get_value()

        # print '  offset',offset
        if shapetype == 1:  # 'Point':1,
            # single points need a scpecial treatment
            for id_shape, coord in zip(ids_shape, self.coords[ids_shape]):
                # print '  id_shape',id_shape,coord
                # shapes.append(self.shapes.get(key).tolist())
                # print '    shape',(np.array(shape, dtype = np.float32)[:,:2] - offset).tolist()
                w.poly(shapeType=shapetype, parts=[[coord[:2] - offset], ])
                #id = self.get_id_from_key(key)
                attrs = {}
                for attrconf in self.get_group('shapeattrs'):
                    val = attrconf[id_shape]
                    if attrconf.field[1] == 'N':
                        if val == np.nan:
                            val = 'NONE'
                    attrs[attrconf.attrname] = val
                # print '  record',key,' attrs=', attrs
                w.record(**attrs)
        else:

            for id_shape, shape in zip(ids_shape, self.shapes[ids_shape]):
                # print '  id_shape',id_shape, np.array(shape, dtype = np.float32)
                # shapes.append(self.shapes.get(key).tolist())
                # print '    shape',(np.array(shape, dtype = np.float32)[:,:2] - offset).tolist()
                w.poly(shapeType=shapetype, parts=[(np.array(shape, dtype=np.float32)[:, :2] - offset).tolist(), ])
                #id = self.get_id_from_key(key)
                attrs = {}
                for attrconf in self.get_group('shapeattrs'):
                    val = attrconf[id_shape]
                    if attrconf.field[1] == 'N':
                        if val == np.nan:
                            val = 'NONE'
                    attrs[attrconf.attrname] = val
                # print '  record',key,' attrs=', attrs
                w.record(**attrs)
        # a null shape: w.null()

        #w.poly(shapeType=3, parts=shapes)
        # try:
        w.save(filepath)
        # except:
        #    print 'WARNING in export_shapefile:\n no shapefile written. Probably fields are not large enough.'
        #    return

        try:
            shapeproj = get_shapeproj(self._projparams.get_value())
            # create the PRJ file
            prjfile = open("%s.prj" % basefilepath, "w")
            prjfile.write(shapeproj)
            prjfile.close()
            return True
        except:
            print 'WARNING in export_shapefile:\n no projection file written (probably no Internet connection).'
            print 'Open shapefile with projection: %s.' % self._projparams.get_value()
            # raise
            return False

    def add_field(self, field):
        """
        Configures field array and returns fieldname
        """
        # print 'add_field',field,'len=',len(field)
        attrname, ftype, flen, fdigit = field
        attrname = attrname.strip()

        dtype = 'object'
        digits_fraction = None

        if ftype == 'C':
            dtype = 'object'
            default = ''

        elif ftype == 'N':
            if fdigit > 0:
                digits_fraction = fdigit
                dtype = 'float32'
                default = 0.0
            else:
                dtype = 'int32'
                default = 0

        self.add_col(am.ArrayConf(attrname, default,
                                  dtype=dtype,
                                  groupnames=['shapeattrs'],
                                  perm='rw',
                                  name=attrname,
                                  field=field,
                                  info='Shape attribute: '+attrname,
                                  ))
        return attrname

    def import_shapefile(self, filepath=None, projparams=None, projparams_target=None, offset_target=(0.0, 0.0)):
        #from mpl_toolkits.basemap import pyproj
        if filepath is None:
            filepath = self.filepath.get_value()
        else:
            self.filepath.set_value(filepath)

        basefilepath = self.get_basefilepath(filepath)
        print 'import_shapefile *%s*' % (basefilepath), type(str(basefilepath))
        sf = shapefile.Reader(str(basefilepath))

        shapes = sf.shapes()
        self.shapetype = shapes[3].shapeType
        fields = sf.fields
        records = sf.records()

        if projparams == None:
            # here we could guess from shapes
            #projparams ="+init=EPSG:23032"
            shape_rec = sf.shapeRecord(0)
            points = shape_rec.shape.points
            projparams = guess_utm_from_coord(points[0])

        if projparams_target != None:
            proj_target = pyproj.Proj(projparams_target)
            self._projparams, self._offset = (proj_target, offset_target)
        else:
            proj_target = None
            self._projparams, self._offset = (projparams, offset_target)

        if IS_PROJ:
            proj_shape = pyproj.Proj(projparams)
        else:
            # no projection possible
            proj_target = None
            proj_shape = None

        attrnames = []
        for field in fields[1:]:
            attrnames.append(self.add_field(field))

        # print '  fields',len(fields),fields

        n = len(attrnames)
        for ind in xrange(len(records)):
            shape_rec = sf.shapeRecord(ind)

            # use first field as id, but will also be a regular attribute
            id_egde = shape_rec.record[0]
            attrrow = {}
            print '\n id_egde', id_egde
            for i, field in zip(xrange(n), fields[1:]):
                val = shape_rec.record[i]
                # print '  ',i,attrnames[i],'>>'+repr(val)+'<<', type(val)
                if field[1] == 'N':
                    if type(val) == types.StringType:
                        val = -1

                attrrow[attrnames[i]] = val

            shape = []
            if proj_target != None:

                for point in shape_rec.shape.points:
                    shape.append(np.array(pyproj.transform(proj_shape, proj_target, point[0], point[1]))+offset_target)
            else:
                for point in shape_rec.shape.points:
                    shape.append(np.array([point[0], point[1]])+offset_target)

            attrrow['shapes'] = np.array(shape, float)

            #self.set_row(id_egde, **attrrow)
            id = self.add_row(key=id_egde)

            for attrname, val in attrrow.iteritems():
                # print '  ',attrname,'>>'+repr(val)+'<<', type(val)
                getattr(self, attrname)[id] = val

        #if self._log: self._log.w('imported %d traces done.'%len(ids_trace))
        return True


def nodes_to_shapefile(net, filepath, dataname='nodeshapedata',
                       parent=None, log=None):
    """
    Export network nodes to shapefile.
    """
    nodes = net.nodes
    shapedata = Shapedata(parent, dataname, name='Node shape data',
                          filepath=filepath,
                          shapetype=SHAPETYPES['Point'],
                          projparams_shape=net.get_projparams(),
                          offset=net.get_offset(), log=log)

    #attrname, ftype, flen, fdigit = field
    attrlist = [
        ('id', 'id', 'ID_NODE', 'N', 12, 0),
        ('types', 'val', 'TYPE', 'C', 20, 0),
        # ('radii','val','RADIUS','N',5,3),
    ]

    map_nodetypes = get_inversemap(nodes.types.choices)
    nodetypes = np.zeros(max(map_nodetypes.keys())+1, dtype=np.object)
    nodetypes[map_nodetypes.keys()] = map_nodetypes.values()

    print 'nodes_to_shapefile', filepath

    for attr in attrlist:
        shapedata.add_field(attr[2:])

    ids_node = nodes.get_ids()

    ids_shape = shapedata.add_rows(len(nodes))
    # print '  shapedata.ID_ARC',shapedata.ID_ARC,'dir',dir(shapedata.ID_ARC)
    shapedata.ID_NODE[ids_shape] = ids_node
    shapedata.coords[ids_shape] = nodes.coords[ids_node]
    shapedata.TYPE[ids_shape] = nodetypes[nodes.types[ids_node]]

    # copy rest of attributes
    for netattrname, gettype, shapeattrname, x1, x2, x3 in attrlist:
        if netattrname not in ('id', 'types'):
            getattr(shapedata, shapeattrname)[ids_shape] = getattr(nodes, netattrname)[ids_node]

    shapedata.adjust_fieldlength()
    shapedata.export_shapefile()
    return True


def edges_to_shapefile(net, filepath, dataname='edgeshapedata',
                       is_access=True, parent=None, log=None):
    """
    Export network edges to shapefile.
    """
    edges = net.edges
    shapedata = Shapedata(parent, dataname, name='Edge shape data',
                          filepath=filepath,
                          shapetype=SHAPETYPES['PolyLine'],
                          projparams_shape=net.get_projparams(),
                          offset=net.get_offset(), log=log)

    #attrname, ftype, flen, fdigit = field
    attrlist = [
        ('id', 'id', 'ID_ARC', 'N', 12, 0),
        ('ids_fromnode', 'id', 'ID_FROMNOD', 'N', 12, 0),
        ('ids_tonode', 'id', 'ID_TONOD', 'N', 12, 0),
        ('lengths', 'val', 'LENGTH', 'N', 6, 3),
        ('widths', 'val', 'WIDTH', 'N', 3, 3),
        ('priorities', 'val', 'PRIO', 'N', 2, 0),
        ('nums_lanes', 'val', 'N_LANE', 'N', 2, 0),
        ('speeds_max', 'val', 'SPEED_MAX', 'N', 6, 3),
    ]

    print 'edges_to_shapefile', filepath

    for attr in attrlist:
        shapedata.add_field(attr[2:])

    if is_access:
        shapedata.add_field(('ACCESS', 'C', 64, 0))

    ids_edge = edges.get_ids()

    ids_shape = shapedata.add_rows(len(edges))
    # print '  shapedata.ID_ARC',shapedata.ID_ARC,'dir',dir(shapedata.ID_ARC)
    shapedata.ID_ARC[ids_shape] = ids_edge
    shapedata.shapes[ids_shape] = edges.shapes[ids_edge]

    for netattrname, gettype, shapeattrname, x1, x2, x3 in attrlist:
        if netattrname not in ('id', 'access'):
            getattr(shapedata, shapeattrname)[ids_shape] = getattr(edges, netattrname)[ids_edge]

    if is_access:
        accesses = np.zeros(len(ids_edge), dtype=object)
        accesses[:] = ''
        for mode, shapemode in VEHICLECLASSCODE.iteritems():
            # here we select this mode for access level 1 and 2
            #                     -1  0       1          2
            accessvec = np.array(['', 'X', shapemode, shapemode], dtype=np.unicode)

            if net.modes.has_modename(mode):
                accesslevels = edges.get_accesslevels(net.modes.get_id_mode(mode))
                accesses += accessvec[accesslevels[ids_edge]+1]

        for id_shape, access_str in zip(ids_shape, accesses):
            access = list(set(access_str))
            if len(access) == 0:
                # no dedicated access: all can go
                shapedata.ACCESS[id_shape] = 'X'
            else:
                access.sort()
                shapedata.ACCESS[id_shape] = ''.join(access)

    shapedata.adjust_fieldlength()
    shapedata.export_shapefile()
    return True


def facilities_to_shapefile(facilities, filepath, dataname='facilitydata',
                            is_access=True, parent=None, log=None):
    """
    Export network edges to shapefile.
    """
    net = facilities.get_net()
    shapedata = Shapedata(parent, dataname, name='Facilities shape data',
                          filepath=filepath,
                          shapetype=SHAPETYPES['PolyLine'],
                          projparams_shape=net.get_projparams(),
                          offset=net.get_offset(), log=log)

    #attrname, ftype, flen, fdigit = field
    attrlist = [
        ('id', 'id', 'ID_FACIL', 'N', 32, 0),
        ('ids_roadedge_closest', 'id', 'ID_ARC', 'N', 32, 0),
        ('positions_roadedge_closest', 'id', 'POS_ARC', 'N', 12, 5),
        ('osmkeys', 'val', 'OSMKEY', 'C', 32, 0),
    ]

    print 'facilities_to_shapefile', filepath

    for attr in attrlist:
        shapedata.add_field(attr[2:])

    ids_fac = facilities.get_ids()

    ids_shape = shapedata.add_rows(len(ids_fac))
    # print '  shapedata.ID_ARC',shapedata.ID_ARC,'dir',dir(shapedata.ID_ARC)
    shapedata.ID_FACIL[ids_shape] = ids_fac
    shapedata.shapes[ids_shape] = facilities.shapes[ids_fac]

    for netattrname, gettype, shapeattrname, x1, x2, x3 in attrlist:
        if netattrname not in ('id',):
            getattr(shapedata, shapeattrname)[ids_shape] = getattr(facilities, netattrname)[ids_fac]

    shapedata.adjust_fieldlength()
    shapedata.export_shapefile()
    return True


def zones_to_shapefile(zones, filepath, dataname='zonedata',
                       is_access=True, parent=None, log=None):
    """
    Export network edges to shapefile.
    """
    net = zones.parent.facilities.get_net()
    shapedata = Shapedata(parent, dataname, name='Facilities shape data',
                          filepath=filepath,
                          shapetype=SHAPETYPES['PolyLine'],
                          projparams_shape=net.get_projparams(),
                          offset=net.get_offset(), log=log)

    #attrname, ftype, flen, fdigit = field
    attrlist = [
        ('id', 'id', 'ID_ZON', 'N', 32, 0),
        # ~ ('names_extended','id','ID_NAM','N',32,0),
        ('ids_landusetype', 'id', 'ID_LANTYPE', 'N', 12, 5),
        ('areas', 'id', 'AREAS', 'N', 12, 5),

        # ('n_edges','id','N_EDGES','N',12,5),
        # ('share_roads_surface','id','SH_ROAD_SURF','N',12,5),
        # ('share_exclusive_cyclingroads','id','SHARE_EXCL_BIKE','N',12,5),
        # ('av_priority_roads','id','AV_ROAD_PRI','N',12,5),
        # ('density_intersection','id','DENS_INTER','N',12,5),
        # ('density_trafficlight','id','DENS_TL','N',12,5),
        # ('share_facilities','id','SHARE_FAC','N',12,5),
        # ('share_residential','id','SHARE_RES','N',12,5),
        # ('share_commercial','id','SHARE_COM','N',12,5),
        # ('share_industrial','id','SHARE_IND','N',12,5),
        # ('share_leisure','id','SHARE_LEI','N',12,5),
        # ('entropies','id','ENTROPY','N',12,5),
    ]

    print 'zones_to_shapefile', filepath

    for attr in attrlist:
        shapedata.add_field(attr[2:])

    ids_zon = zones.get_ids()

    ids_shape = shapedata.add_rows(len(ids_zon))
    # print '  shapedata.ID_ARC',shapedata.ID_ARC,'dir',dir(shapedata.ID_ARC)
    shapedata.ID_ZON[ids_shape] = ids_zon
    shapedata.shapes[ids_shape] = zones.shapes[ids_zon]
    # ~ shapedata.ID_ZON[ids_shape] = ids_zon
    for netattrname, gettype, shapeattrname, x1, x2, x3 in attrlist:
        if netattrname not in ('id',):
            getattr(shapedata, shapeattrname)[ids_shape] = getattr(zones, netattrname)[ids_zon]

    # ~ shapedata.adjust_fieldlength()
    shapedata.export_shapefile()
    return True
